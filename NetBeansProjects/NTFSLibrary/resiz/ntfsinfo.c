#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "debug.h"
#include "types.h"
#include "support.h"
#include "endians.h"
#include "bootsect.h"
#include "device.h"
#include "attrib.h"
#include "volume.h"
#include "mft.h"
#include "bitmap.h"
#include "inode.h"
#include "runlist.h"
#include "version.h"

#include "libresiz.h"

struct bitmap {
    s64 size;
    u8 *bm;
};

struct llcn_t {
    s64 lcn; /* last used LCN for a "special" file/attr type */
    s64 inode; /* inode using it */
};

typedef struct {
    ntfs_inode *ni; /* inode being processed */
    ntfs_attr_search_ctx *ctx; /* inode attribute being processed */
    s64 inuse; /* num of clusters in use */
    int multi_ref; /* num of clusters referenced many times */
    int outsider; /* num of clusters outside the volume */
    int show_outsider; /* controls showing the above information */
    int flags;
    struct bitmap lcn_bitmap;
} ntfsck_t;

typedef struct {
    ntfs_volume *vol;
    ntfs_inode *ni; /* inode being processed */
    s64 new_volume_size; /* in clusters; 0 = --info w/o --size */
    MFT_REF mref; /* mft reference */
    MFT_RECORD *mrec; /* mft record */
    ntfs_attr_search_ctx *ctx; /* inode attribute being processed */
    u64 relocations; /* num of clusters to relocate */
    s64 inuse; /* num of clusters in use */
    runlist mftmir_rl; /* $MFTMirr AT_DATA's new position */
    s64 mftmir_old; /* $MFTMirr AT_DATA's old LCN */
    int dirty_inode; /* some inode data got relocated */
    int shrink; /* shrink = 1, enlarge = 0 */
    s64 badclusters; /* num of physically dead clusters */
    VCN mft_highest_vcn; /* used for relocating the $MFT */
    struct bitmap lcn_bitmap;
    /* Temporary statistics until all case is supported */
    struct llcn_t last_mft;
    struct llcn_t last_mftmir;
    struct llcn_t last_multi_mft;
    struct llcn_t last_sparse;
    struct llcn_t last_compressed;
    struct llcn_t last_lcn;
    s64 last_unsupp; /* last unsupported cluster */
} ntfs_resize_t;

#define NTFS_MAX_CLUSTER_SIZE	(65536)
#define NTFS_MBYTE (1 << 20)

#define FREE_FAULT(alloc) {if (alloc) free(alloc); return -1;}
#define FREE_RESULT(alloc, result) {free(alloc); return result;}

#define rounded_up_division(a, b) (((a) + (b - 1)) / (b))

#if CLEAN_EXIT

static void close_inode_and_context(ntfs_attr_search_ctx *ctx) {
    ntfs_inode *ni;

    ni = ctx->base_ntfs_ino;
    if (!ni) ni = ctx->ntfs_ino;
    ntfs_attr_put_search_ctx(ctx);
    if (ni) ntfs_inode_close(ni);
}
#endif /* CLEAN_EXIT */

/**
 * mount_volume
 *
 * First perform some checks to determine if the volume is already mounted, or
 * is dirty (Windows wasn't shutdown properly).  If everything is OK, then mount
 * the volume (load the metadata into memory).
 */
static int mount_volume(ntfs_volume **pvol, char *volume) {
    unsigned long mntflag;
    ntfs_volume *vol = NULL;

    if (ntfs_check_if_mounted(volume, &mntflag))
        return ESTATUS;
    if (mntflag & NTFS_MF_MOUNTED)
        return EMOUNT;
    /*
     * Pass NTFS_MNT_FORENSIC so that the mount process does not modify the
     * volume at all.  We will do the logfile emptying and dirty setting
     * later if needed.
     */
    if (!(vol = ntfs_mount(volume, NTFS_MNT_RDONLY | NTFS_MNT_FORENSIC))) {
        int err = errno;
        if (err == EINVAL) return EVINVAL;
        else if (err == EIO) return EVIO;
    }
    if (NTFS_MAX_CLUSTER_SIZE < vol->cluster_size)
        return ELARGE;
    if (ntfs_version_is_supported(vol))
        return EVERSION;

    *pvol = vol;
    return 0;
}

static void umount_volume(ntfs_volume *vol) {
    ntfs_umount(vol, TRUE);
}

/**
 * compare_bitmaps
 *
 * Compare two bitmaps.  In this case, $Bitmap as read from the disk and
 * lcn_bitmap which we built from the MFT Records.
 */
static int compare_bitmaps(ntfs_volume *vol, struct bitmap *a) {
    s64 i, pos, count;
    int mismatch = 0;
    int backup_boot = 0;
    u8 bm[NTFS_BUF_SIZE];

    pos = 0;
    while (1) {
        count = ntfs_attr_pread(vol->lcnbmp_na, pos, NTFS_BUF_SIZE, bm);
        if (count == -1) return ECOMPARE;
        if (count == 0) {
            if (a->size > pos) return ESMALL;
            break;
        }

        for (i = 0; i < count; i++, pos++) {
            s64 cl; /* current cluster */

            if (a->size <= pos)
                goto done;

            if (a->bm[pos] == bm[i])
                continue;

            for (cl = pos * 8; cl < (pos + 1) * 8; cl++) {
                char bit;

                bit = ntfs_bit_get(a->bm, cl);
                if (bit == ntfs_bit_get(bm, i * 8 + cl % 8))
                    continue;

                if (!mismatch && !bit && !backup_boot &&
                        cl == vol->nr_clusters / 2) {
                    /* FIXME: call also boot sector check */
                    backup_boot = 1;
                    printf("Found backup boot sector in "
                            "the middle of the volume.\n");
                    continue;
                }
                if (++mismatch > 10) continue;
            }
        }
    }
done:
    if (mismatch) return ECHKCLUS;

    return 0;
}

/**
 * bitmap_file_data_fixup
 *
 * $Bitmap can overlap the end of the volume. Any bits in this region
 * must be set. This region also encompasses the backup boot sector.
 */
static void bitmap_file_data_fixup(s64 cluster, struct bitmap *bm) {
    for (; cluster < bm->size << 3; cluster++)
        ntfs_bit_set(bm->bm, (u64) cluster, 1);
}

/**
 * setup_lcn_bitmap
 *
 * Allocate a block of memory with one bit for each cluster of the disk.
 * All the bits are set to 0, except those representing the region beyond the
 * end of the disk.
 */
static int setup_lcn_bitmap(struct bitmap *bm, s64 nr_clusters) {
    /* Determine lcn bitmap byte size and allocate it. */
    bm->size = rounded_up_division(nr_clusters, 8);

    bm->bm = ntfs_calloc(bm->size);
    if (!bm->bm) return -1;

    bitmap_file_data_fixup(nr_clusters, bm);
    return 0;
}

static int inode_close(ntfs_inode *ni) {
    if (ntfs_inode_close(ni))
        return -1;
    return 0;
}

static ntfs_attr_search_ctx *attr_get_search_ctx(ntfs_inode *ni, MFT_RECORD *mrec) {
    return ntfs_attr_get_search_ctx(ni, mrec);
}

/**
 * build_lcn_usage_bitmap
 *
 * lcn_bitmap has one bit for each cluster on the disk.  Initially, lcn_bitmap
 * has no bits set.  As each attribute record is read the bits in lcn_bitmap are
 * checked to ensure that no other file already references that cluster.
 *
 * This serves as a rudimentary "chkdsk" operation.
 */
static int build_lcn_usage_bitmap(ntfs_volume *vol, ntfsck_t *fsck) {
    s64 inode;
    ATTR_RECORD *a;
    runlist *rl;
    int i, j;
    struct bitmap *lcn_bitmap = &fsck->lcn_bitmap;

    a = fsck->ctx->attr;
    inode = fsck->ni->mft_no;

    if (!a->non_resident)
        return 0;

    if (!(rl = ntfs_mapping_pairs_decompress(vol, a, NULL)))
        FREE_FAULT(rl)

        for (i = 0; rl[i].length; i++) {
            s64 lcn = rl[i].lcn;
            s64 lcn_length = rl[i].length;

            /* CHECKME: LCN_RL_NOT_MAPPED check isn't needed */
            if (lcn == LCN_HOLE || lcn == LCN_RL_NOT_MAPPED)
                continue;

            /* FIXME: ntfs_mapping_pairs_decompress should return error */
            if (lcn < 0 || lcn_length <= 0)
                return -1;

            for (j = 0; j < lcn_length; j++) {
                u64 k = (u64) lcn + j;

                if (k >= (u64) vol->nr_clusters) {
                    long long outsiders = lcn_length - j;
                    fsck->outsider += outsiders;
                    ++fsck->show_outsider;

                    break;
                }

                if (ntfs_bit_get_and_set(lcn_bitmap->bm, k, 1)) {
                    ++fsck->multi_ref;
                    continue;
                }
            }
            fsck->inuse += lcn_length;
        }
    free(rl);

    return 0;
}

/**
 * walk_attributes
 *
 * For a given MFT Record, iterate through all its attributes.  Any non-resident
 * data runs will be marked in lcn_bitmap.
 */
static int walk_attributes(ntfs_volume *vol, ntfsck_t *fsck) {
    if (!(fsck->ctx = attr_get_search_ctx(fsck->ni, NULL)))
        return -1;

    while (!ntfs_attrs_walk(fsck->ctx)) {
        if (fsck->ctx->attr->type == AT_END)
            break;
        if (build_lcn_usage_bitmap(vol, fsck))
            return EBITMAP;
    }

    ntfs_attr_put_search_ctx(fsck->ctx);
    return 0;
}

/**
 * walk_inodes
 *
 * Read each record in the MFT, skipping the unused ones, and build up a bitmap
 * from all the non-resident attributes.
 */
static int build_allocation_bitmap(ntfs_volume *vol, ntfsck_t *fsck) {
    s64 nr_mft_records, inode = 0;
    ntfs_inode *ni;
    int pb_flags = 0; /* progress bar flags */

    /* WARNING: don't modify the text, external tools grep for it */
    nr_mft_records = vol->mft_na->initialized_size >>
            vol->mft_record_size_bits;

    for (; inode < nr_mft_records; inode++) {
        if ((ni = ntfs_inode_open(vol, (MFT_REF) inode)) == NULL) {
            /* FIXME: continue only if it make sense, e.g.
               MFT record not in use based on $MFT bitmap */
            if (errno == EIO || errno == ENOENT)
                continue;
            return -1;
        }

        if (ni->mrec->base_mft_record)
            goto close_inode;

        fsck->ni = ni;
        if (walk_attributes(vol, fsck) != 0) {
            inode_close(ni);
            return -1;
        }
close_inode:
        if (inode_close(ni) != 0)
            return -1;
    }
    return 0;
}

static int check_cluster_allocation(ntfs_volume *vol, ntfsck_t *fsck) {
    memset(fsck, 0, sizeof (ntfsck_t));

    if (setup_lcn_bitmap(&fsck->lcn_bitmap, vol->nr_clusters) != 0)
        return ECOMPARE;
    if (build_allocation_bitmap(vol, fsck) != 0)
        return ECOMPARE;
    if (fsck->outsider || fsck->multi_ref)
        return ECHKCLUS;

    return compare_bitmaps(vol, &fsck->lcn_bitmap);
}

/**
 * lookup_data_attr
 *
 * Find the $DATA attribute (with or without a name) for the given MFT reference
 * (inode number).
 */
static int lookup_data_attr(ntfs_volume *vol,
        MFT_REF mref,
        const char *aname,
        ntfs_attr_search_ctx **ctx) {
    ntfs_inode *ni;
    ntfschar *ustr;
    int len = 0;

    if (!(ni = ntfs_inode_open(vol, mref)))
        return -1;

    if (!(*ctx = attr_get_search_ctx(ni, NULL)))
        return -1;

    if ((ustr = ntfs_str2ucs(aname, &len)) == NULL)
        return -1;

    if (ntfs_attr_lookup(AT_DATA, ustr, len, 0, 0, NULL, 0, *ctx))
        return -1;

    ntfs_ucsfree(ustr);
    return 0;
}

static int check_bad_sectors(ntfs_volume *vol) {
    ntfs_attr_search_ctx *ctx;
    ntfs_inode *base_ni;
    runlist *rl;
    s64 i, badclusters = 0;

    if (lookup_data_attr(vol, FILE_BadClus, "$Bad", &ctx))
        return ELOOKUP;

    base_ni = ctx->base_ntfs_ino;
    if (!base_ni)
        base_ni = ctx->ntfs_ino;

    if (NInoAttrList(base_ni)) return ESECTORS;
    if (!ctx->attr->non_resident) return ESECTORS;
    /*
     * FIXME: The below would be partial for non-base records in the
     * not yet supported multi-record case. Alternatively use audited
     * ntfs_attr_truncate after an umount & mount.
     */
    if (!(rl = ntfs_mapping_pairs_decompress(vol, ctx->attr, NULL)))
        return EDECOMP;

    for (i = 0; rl[i].length; i++) {
        /* CHECKME: LCN_RL_NOT_MAPPED check isn't needed */
        if (rl[i].lcn == LCN_HOLE || rl[i].lcn == LCN_RL_NOT_MAPPED)
            continue;

        badclusters += rl[i].length;
    }
    if (badclusters)FREE_RESULT(rl, ESECTORS)
        free(rl);

#if CLEAN_EXIT
    close_inode_and_context(ctx);
#else
    ntfs_attr_put_search_ctx(ctx);
#endif

    return badclusters;
}

static int collect_resize_constraints(ntfs_resize_t *resize, runlist *rl) {
    s64 inode, last_lcn;
    ATTR_FLAGS flags;
    ATTR_TYPES atype;
    struct llcn_t *llcn = NULL;
    int ret, supported = 0;

    last_lcn = rl->lcn + (rl->length - 1);

    inode = resize->ni->mft_no;
    flags = resize->ctx->attr->flags;
    atype = resize->ctx->attr->type;

    if ((ret = ntfs_inode_badclus_bad(inode, resize->ctx->attr)) != 0) {
        if (ret == -1) return -1;
        return 0;
    }

    if (inode == FILE_Bitmap) {
        llcn = &resize->last_lcn;
        if (atype == AT_DATA && NInoAttrList(resize->ni))
            return -1;

        supported = 1;

    } else if (inode == FILE_MFT) {
        llcn = &resize->last_mft;
        /*
         *  First run of $MFT AT_DATA isn't supported yet.
         */
        if (atype != AT_DATA || rl->vcn)
            supported = 1;

    } else if (NInoAttrList(resize->ni)) {
        llcn = &resize->last_multi_mft;

        if (inode != FILE_MFTMirr)
            supported = 1;

    } else if (flags & ATTR_IS_SPARSE) {
        llcn = &resize->last_sparse;
        supported = 1;

    } else if (flags & ATTR_IS_COMPRESSED) {
        llcn = &resize->last_compressed;
        supported = 1;

    } else if (inode == FILE_MFTMirr) {
        llcn = &resize->last_mftmir;
        supported = 1;

        /* Fragmented $MFTMirr DATA attribute isn't supported yet */
        if (atype == AT_DATA)
            if (rl[1].length != 0 || rl->vcn)
                supported = 0;
    } else {
        llcn = &resize->last_lcn;
        supported = 1;
    }

    if (llcn->lcn < last_lcn) {
        llcn->lcn = last_lcn;
        llcn->inode = inode;
    }

    if (supported) return 0;
    if (resize->last_unsupp < last_lcn)
        resize->last_unsupp = last_lcn;

    return 0;
}

static int collect_relocation_info(ntfs_resize_t *resize, runlist *rl) {
    s64 lcn, lcn_length, start, len, inode;
    s64 new_vol_size; /* (last LCN on the volume) + 1 */

    lcn = rl->lcn;
    lcn_length = rl->length;
    inode = resize->ni->mft_no;
    new_vol_size = resize->new_volume_size;

    if (lcn + lcn_length <= new_vol_size)
        return 0;

    if (inode == FILE_Bitmap && resize->ctx->attr->type == AT_DATA)
        return 0;

    start = lcn;
    len = lcn_length;

    if (lcn < new_vol_size) {
        start = new_vol_size;
        len = lcn_length - (new_vol_size - lcn);
    }

    resize->relocations += len;
    return 0;
}

static int build_resize_constraints(ntfs_resize_t *resize) {
    s64 i;
    runlist *rl;

    if (!resize->ctx->attr->non_resident)
        return 0;

    if (!(rl = ntfs_mapping_pairs_decompress(resize->vol,
            resize->ctx->attr, NULL)))
        return -1;

    for (i = 0; rl[i].length; i++) {
        /* CHECKME: LCN_RL_NOT_MAPPED check isn't needed */
        if (rl[i].lcn == LCN_HOLE || rl[i].lcn == LCN_RL_NOT_MAPPED)
            continue;

        if (collect_resize_constraints(resize, rl + i))
            FREE_FAULT(rl);
        if (resize->shrink) {
            if (collect_relocation_info(resize, rl + i))
                FREE_FAULT(rl);
        }
    }
    free(rl);
    return 0;
}

static int resize_constraints_by_attributes(ntfs_resize_t *resize) {
    if (!(resize->ctx = attr_get_search_ctx(resize->ni, NULL)))
        return -1;

    while (!ntfs_attrs_walk(resize->ctx)) {
        if (resize->ctx->attr->type == AT_END)
            break;
        if (build_resize_constraints(resize))
            return -1;
    }

    ntfs_attr_put_search_ctx(resize->ctx);
    return 0;
}

static int set_resize_constraints(ntfs_resize_t *resize) {
    s64 nr_mft_records, inode;
    ntfs_inode *ni;

    nr_mft_records = resize->vol->mft_na->initialized_size >>
            resize->vol->mft_record_size_bits;

    for (inode = 0; inode < nr_mft_records; inode++) {

        ni = ntfs_inode_open(resize->vol, (MFT_REF) inode);
        if (ni == NULL) {
            if (errno == EIO || errno == ENOENT)
                continue;
            return -1;
        }

        if (ni->mrec->base_mft_record)
            goto close_inode;

        resize->ni = ni;
        if (resize_constraints_by_attributes(resize))
            return -1;
close_inode:
        if (inode_close(ni) != 0) return -1;
    }
    return 0;
}

static void set_disk_usage_constraint(ntfs_resize_t *resize) {
    /* last lcn for a filled up volume (no empty space) */
    s64 last = resize->inuse - 1;

    if (resize->last_unsupp < last)
        resize->last_unsupp = last;
}

static int check_resize_constraints(ntfs_resize_t *resize) {
    s64 new_size = resize->new_volume_size;

    /* FIXME: resize.shrink true also if only -i is used */
    if (!resize->shrink) return 0;

    if (resize->inuse == resize->vol->nr_clusters)
        return -1;

    return 0;
}

static int advise_resize(int *pusiz, ntfs_volume *vol, s64 supp_lcn) {
    s64 new_b;
    /* Take the next supported cluster (free or relocatable)
       plus reserve a cluster for the backup boot sector */
    supp_lcn += 2;

    if (supp_lcn > vol->nr_clusters) return -1;

    new_b = supp_lcn * vol->cluster_size;
    *pusiz = rounded_up_division(new_b, NTFS_MBYTE);

    return 0;
}

int ntfsinfo(int *pusiz, char *volume) {
    ntfsck_t fsck;
    ntfs_resize_t resize;
    s64 device_size; /* in bytes */
    ntfs_volume *vol = NULL;
    int result = 0;

    if (!volume) return EPARAM;
    if ((result = mount_volume(&vol, volume)))
        return result;
    //	err_exit("Couldn't open volume '%s'!\n", opt.volume);

    device_size = ntfs_device_size_get(vol->dev, vol->sector_size);
    device_size *= vol->sector_size;
    if (device_size <= 0) return EDEVICE;
    if (device_size < vol->nr_clusters * vol->cluster_size)
        return EDEVICE;

    memset(&resize, 0, sizeof (resize));
    resize.vol = vol;
    /*
     * Checking and __reporting__ of bad sectors must be done before cluster
     * allocation check because chkdsk doesn't fix $Bitmap's w/ bad sectors
     * thus users would (were) quite confused why chkdsk doesn't work.
     */
    resize.badclusters = check_bad_sectors(vol);
    check_cluster_allocation(vol, &fsck);

    resize.inuse = fsck.inuse;
    resize.lcn_bitmap = fsck.lcn_bitmap;

    if (set_resize_constraints(&resize))
        FREE_RESULT(resize.lcn_bitmap.bm, ERECON)
        set_disk_usage_constraint(&resize);
    if (check_resize_constraints(&resize))
        FREE_RESULT(resize.lcn_bitmap.bm, ERECHK)
        if (advise_resize(pusiz, resize.vol, resize.last_unsupp))
            FREE_RESULT(resize.lcn_bitmap.bm, EFULL)

#if CLEAN_EXIT
            if (resize.lcn_bitmap.bm) free(resize.lcn_bitmap.bm);
#endif
    if (vol) umount_volume(vol);

    return 0;
}
