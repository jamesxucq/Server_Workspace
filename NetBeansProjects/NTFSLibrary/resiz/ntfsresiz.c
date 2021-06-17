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

/* FIXME: This, lcn_bitmap and pos from find_free_cluster() will make a cluster
   allocation related structure, attached to ntfs_resize_t */
static s64 max_free_cluster_range = 0;

#define NTFS_MAX_CLUSTER_SIZE	(65536)
#define NTFS_MBYTE (1 << 20)
//#define NTFS_GBYTE (1 << 30)

#define DIRTY_NONE		(0)
#define DIRTY_INODE		(1)
#define DIRTY_ATTRIB	(2)

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

static void rl_set(runlist *rl, VCN vcn, LCN lcn, s64 len) {
    rl->vcn = vcn;
    rl->lcn = lcn;
    rl->length = len;
}

static int rl_items(runlist *rl) {
    int i = 0;

    while (rl[i++].length);

    return i;
}

static int rl_expand(runlist **rl, const VCN last_vcn) {
    int len;
    runlist *p = *rl;

    len = rl_items(p) - 1;
    if (len <= 0) return -1;

    if (p[len].vcn > last_vcn) return -1;
    if (p[len - 1].lcn == LCN_HOLE) {
        p[len - 1].length += last_vcn - p[len].vcn;
        p[len].vcn = last_vcn;
    } else if (p[len - 1].lcn >= 0) {
        p = realloc(*rl, (++len + 1) * sizeof (runlist_element));
        if (!p) return -1;

        p[len - 1].lcn = LCN_HOLE;
        p[len - 1].length = last_vcn - p[len - 1].vcn;
        rl_set(p + len, last_vcn, LCN_ENOENT, 0LL);
        *rl = p;
    } else return -1;

    return 0;
}

static int rl_truncate(runlist **rl, const VCN last_vcn) {
    int len;
    VCN vcn;

    len = rl_items(*rl) - 1;
    if (len <= 0) return -1;

    vcn = (*rl)[len].vcn;

    if (vcn < last_vcn) {
        if (rl_expand(rl, last_vcn))
            return -1;
    } else if (vcn > last_vcn) {
        if (ntfs_rl_truncate(rl, last_vcn) == -1)
            return -1;
    }

    return 0;
}

static int rl_insert_at_run(runlist **rl, int run, runlist *ins) {
    int items, ins_items;
    int new_size, size_tail;
    runlist *rle;
    s64 vcn;

    items = rl_items(*rl);
    ins_items = rl_items(ins) - 1;
    new_size = ((items - 1) + ins_items) * sizeof (runlist_element);
    size_tail = (items - run - 1) * sizeof (runlist_element);

    if (!(*rl = (runlist *) realloc(*rl, new_size)))
        return -1;

    rle = *rl + run;

    memmove(rle + ins_items, rle + 1, size_tail);

    for (vcn = rle->vcn; ins->length; rle++, vcn += ins->length, ins++) {
        rl_set(rle, vcn, ins->lcn, ins->length);
        //		dump_run(rle);
    }

    return 0;

    /* FIXME: fast path if ins_items = 1 */
    //	(*rl + run)->lcn = ins->lcn;
}

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
    if (!(vol = ntfs_mount(volume, NTFS_MNT_FORENSIC))) {
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

static ntfs_attr_search_ctx *attr_get_search_ctx(ntfs_inode *ni, MFT_RECORD *mrec) {
    return ntfs_attr_get_search_ctx(ni, mrec);
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

static int inode_close(ntfs_inode *ni) {
    if (ntfs_inode_close(ni))
        return -1;
    return 0;
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
        return -1;

    for (i = 0; rl[i].length; i++) {
        s64 lcn = rl[i].lcn;
        s64 lcn_length = rl[i].length;

        /* CHECKME: LCN_RL_NOT_MAPPED check isn't needed */
        if (lcn == LCN_HOLE || lcn == LCN_RL_NOT_MAPPED)
            continue;

        /* FIXME: ntfs_mapping_pairs_decompress should return error */
        if (lcn < 0 || lcn_length <= 0)
            FREE_FAULT(rl)

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

        if (inode == FILE_MFTMirr) return -1;
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
    /* FIXME: reserve some extra space so Windows can boot ... */
    if (new_size < resize->inuse)
        return -1;
    if (new_size <= resize->last_unsupp)
        return -1;

    return 0;
}

/**
 * prepare_volume_fixup
 *
 * Set the volume's dirty flag and wipe the filesystem journal.  When Windows
 * boots it will automatically run chkdsk to check for any problems.  If the
 * read-only command line option was given, this function will do nothing.
 */
static int prepare_volume_fixup(ntfs_volume *vol) {
    vol->flags |= VOLUME_IS_DIRTY;
    if (ntfs_volume_write_flags(vol, vol->flags))
        return -1;
    NVolSetWasDirty(vol);
    if (vol->dev->d_ops->sync(vol->dev) == -1)
        return -1;
    if (ntfs_logfile_reset(vol))
        return -1;
    if (vol->dev->d_ops->sync(vol->dev) == -1)
        return -1;

    return 0;
}

/**
 * write_mft_record
 *
 * Write an MFT Record back to the disk.  If the read-only command line option
 * was given, this function will do nothing.
 */
static int write_mft_record(ntfs_volume *v, const MFT_REF mref, MFT_RECORD *buf) {
    if (ntfs_mft_record_write(v, mref, buf))
        return -1;

    //	if (v->dev->d_ops->sync(v->dev) == -1)
    //		perr_exit("Failed to sync device");

    return 0;
}

static int rl_split_run(runlist **rl, int run, s64 pos) {
    runlist *rl_new, *rle_new, *rle;
    int items, new_size, size_head, size_tail;
    s64 len_head, len_tail;

    items = rl_items(*rl);
    new_size = (items + 1) * sizeof (runlist_element);
    size_head = run * sizeof (runlist_element);
    size_tail = (items - run - 1) * sizeof (runlist_element);

    rl_new = ntfs_malloc(new_size);
    if (!rl_new) return -1;

    rle_new = rl_new + run;
    rle = *rl + run;

    memmove(rl_new, *rl, size_head);
    memmove(rle_new + 2, rle + 1, size_tail);

    len_tail = rle->length - (pos - rle->lcn);
    len_head = rle->length - len_tail;

    rl_set(rle_new, rle->vcn, rle->lcn, len_head);
    rl_set(rle_new + 1, rle->vcn + len_head, rle->lcn + len_head, len_tail);

    free(*rl);
    *rl = rl_new;

    return 0;
}

static void set_bitmap_range(struct bitmap *bm, s64 pos, s64 length, u8 bit) {
    while (length--)
        ntfs_bit_set(bm->bm, pos++, bit);
}

static void set_bitmap_clusters(struct bitmap *bm, runlist *rl, u8 bit) {
    for (; rl->length; rl++)
        set_bitmap_range(bm, rl->lcn, rl->length, bit);
}

static void release_bitmap_clusters(struct bitmap *bm, runlist *rl) {
    max_free_cluster_range = 0;
    set_bitmap_clusters(bm, rl, 0);
}

static void set_max_free_zone(s64 length, s64 end, runlist_element *rle) {
    if (length > rle->length) {
        rle->lcn = end - length;
        rle->length = length;
    }
}

static int find_free_cluster(struct bitmap *bm,
        runlist_element *rle,
        s64 nr_vol_clusters,
        int hint) {
    /* FIXME: get rid of this 'static' variable */
    static s64 pos = 0;
    s64 i, items = rle->length;
    s64 free_zone = 0;

    if (pos >= nr_vol_clusters)
        pos = 0;
    if (!max_free_cluster_range)
        max_free_cluster_range = nr_vol_clusters;
    rle->lcn = rle->length = 0;
    if (hint) pos = nr_vol_clusters / 2;
    i = pos;

    do {
        if (!ntfs_bit_get(bm->bm, i)) {
            if (++free_zone == items) {
                set_max_free_zone(free_zone, i + 1, rle);
                break;
            }
        } else {
            set_max_free_zone(free_zone, i, rle);
            free_zone = 0;
        }
        if (++i == nr_vol_clusters) {
            set_max_free_zone(free_zone, i, rle);
            i = free_zone = 0;
        }
        if (rle->length == max_free_cluster_range)
            break;
    } while (i != pos);

    if (i) set_max_free_zone(free_zone, i, rle);

    if (!rle->lcn) {
        errno = ENOSPC;
        return -1;
    }
    if (rle->length < items && rle->length < max_free_cluster_range)
        max_free_cluster_range = rle->length;

    pos = rle->lcn + items;
    if (pos == nr_vol_clusters) pos = 0;

    set_bitmap_range(bm, rle->lcn, rle->length, 1);
    return 0;
}

static int read_all(struct ntfs_device *dev, void *buf, int count) {
    int i;

    while (count > 0) {

        i = count;
        if (!NDevReadOnly(dev))
            i = dev->d_ops->read(dev, buf, count);

        if (i < 0) {
            if (errno != EAGAIN && errno != EINTR)
                return -1;
        } else if (i > 0) {
            count -= i;
            buf = i + (char *) buf;
        } else return -1;
    }
    return 0;
}

static int write_all(struct ntfs_device *dev, void *buf, int count) {
    int i;

    while (count > 0) {
        i = count;
        if (!NDevReadOnly(dev))
            i = dev->d_ops->write(dev, buf, count);

        if (i < 0) {
            if (errno != EAGAIN && errno != EINTR)
                return -1;
        } else {
            count -= i;
            buf = i + (char *) buf;
        }
    }
    return 0;
}

static runlist *alloc_cluster(struct bitmap *bm,
        s64 items,
        s64 nr_vol_clusters,
        int hint) {
    runlist_element rle;
    runlist *rl = NULL;
    int rl_size, runs = 0;
    s64 vcn = 0;

    if (items <= 0) {
        errno = EINVAL;
        return NULL;
    }

    while (items > 0) {
        if (runs) hint = 0;
        rle.length = items;
        if (find_free_cluster(bm, &rle, nr_vol_clusters, hint) == -1)
            return NULL;

        rl_size = (runs + 2) * sizeof (runlist_element);
        if (!(rl = (runlist *) realloc(rl, rl_size)))
            return NULL;

        rl_set(rl + runs, vcn, rle.lcn, rle.length);

        vcn += rle.length;
        items -= rle.length;
        runs++;
    }

    rl_set(rl + runs, vcn, -1LL, 0LL);
    //if (runs > 1) dump_runlist(rl);

    return rl;
}

static int lseek_to_cluster(ntfs_volume *vol, s64 lcn) {
    off_t pos;

    pos = (off_t) (lcn * vol->cluster_size);

    if (vol->dev->d_ops->seek(vol->dev, pos, SEEK_SET) == (off_t) - 1)
        return -1;

    return 0;
}

static int copy_clusters(ntfs_resize_t *resize, s64 dest, s64 src, s64 len) {
    s64 i;
    char buff[NTFS_MAX_CLUSTER_SIZE]; /* overflow checked at mount time */
    ntfs_volume *vol = resize->vol;

    for (i = 0; i < len; i++) {

        if (lseek_to_cluster(vol, src + i))
            return -1;
        if (read_all(vol->dev, buff, vol->cluster_size) == -1)
            return -1;
        if (lseek_to_cluster(vol, dest + i))
            return -1;
        if (write_all(vol->dev, buff, vol->cluster_size) == -1)
            return -1;

        resize->relocations++;
    }

    return 0;
}

static int relocate_clusters(ntfs_resize_t *r, runlist *dest_rl, s64 src_lcn) {
    /* collect_shrink_constraints() ensured $MFTMir DATA is one run */
    if (r->mref == FILE_MFTMirr && r->ctx->attr->type == AT_DATA) {
        if (!r->mftmir_old) {
            r->mftmir_rl.lcn = dest_rl->lcn;
            r->mftmir_rl.length = dest_rl->length;
            r->mftmir_old = src_lcn;
        } else return -1;
    }

    for (; dest_rl->length; src_lcn += dest_rl->length, dest_rl++) {
        if (copy_clusters(r, dest_rl->lcn, src_lcn, dest_rl->length))
            return -1;
    }

    return 0;
}

static int relocate_run(ntfs_resize_t *resize, runlist **rl, int run) {
    s64 lcn, lcn_length;
    s64 new_vol_size; /* (last LCN on the volume) + 1 */
    runlist *relocate_rl; /* relocate runlist to relocate_rl */
    int hint;

    lcn = (*rl + run)->lcn;
    lcn_length = (*rl + run)->length;
    new_vol_size = resize->new_volume_size;

    if (lcn + lcn_length <= new_vol_size)
        return 0;

    if (lcn < new_vol_size) {
        if (rl_split_run(rl, run, new_vol_size))
            return -1;
        return 0;
    }

    hint = (resize->mref == FILE_MFTMirr) ? 1 : 0;
    if (!(relocate_rl = alloc_cluster(&resize->lcn_bitmap, lcn_length,
            new_vol_size, hint)))
        return -1;

    /* FIXME: check $MFTMirr DATA isn't multi-run (or support it) */
    if (relocate_clusters(resize, relocate_rl, lcn))
        FREE_FAULT(relocate_rl)
    if (rl_insert_at_run(rl, run, relocate_rl))
        FREE_FAULT(relocate_rl)

    /* We don't release old clusters in the bitmap, that area isn't
       used by the allocator and will be truncated later on */
    free(relocate_rl);
    resize->dirty_inode = DIRTY_ATTRIB;

    return 0;
}

static int rl_fixup(runlist **rl) {
    runlist *tmp = *rl;

    if (tmp->lcn == LCN_RL_NOT_MAPPED) {
        s64 unmapped_len = tmp->length;

        if (!tmp->length) return -1;
        (*rl)++;
        for (tmp = *rl; tmp->length; tmp++)
            tmp->vcn -= unmapped_len;
    }

    for (tmp = *rl; tmp->length; tmp++) {
        if (tmp->lcn == LCN_RL_NOT_MAPPED) {
            if (tmp[1].length) return -1;
            tmp->lcn = LCN_ENOENT;
            tmp->length = 0;
        }
    }

    return 0;
}

static int replace_attribute_runlist(ntfs_volume *vol,
        ntfs_attr_search_ctx *ctx,
        runlist *rl) {
    int mp_size, l;
    void *mp;
    ATTR_RECORD *a = ctx->attr;

    if (rl_fixup(&rl)) return -1;
    if ((mp_size = ntfs_get_size_for_mapping_pairs(vol, rl, 0)) == -1)
        return -1;

    if (a->name_length) {
        u16 name_offs = le16_to_cpu(a->name_offset);
        u16 mp_offs = le16_to_cpu(a->mapping_pairs_offset);

        if (name_offs >= mp_offs) return -1;
    }

    /* CHECKME: don't trust mapping_pairs is always the last item in the
       attribute, instead check for the real size/space */
    l = (int) le32_to_cpu(a->length) - le16_to_cpu(a->mapping_pairs_offset);
    if (mp_size > l) {
        s64 remains_size;
        char *next_attr;

        mp_size = (mp_size + 7) & ~7;
        next_attr = (char *) a + le32_to_cpu(a->length);
        l = mp_size - l;

        remains_size = le32_to_cpu(ctx->mrec->bytes_in_use);
        remains_size -= (next_attr - (char *) ctx->mrec);

        if (le32_to_cpu(ctx->mrec->bytes_in_use) + l >
                le32_to_cpu(ctx->mrec->bytes_allocated))
            return -1;

        memmove(next_attr + l, next_attr, remains_size);
        ctx->mrec->bytes_in_use = cpu_to_le32(l +
                le32_to_cpu(ctx->mrec->bytes_in_use));
        a->length = cpu_to_le32(le32_to_cpu(a->length) + l);
    }

    mp = ntfs_calloc(mp_size);
    if (!mp) return -1;

    if (ntfs_mapping_pairs_build(vol, mp, mp_size, rl, 0, NULL))
        return -1;
    memmove((u8*) a + le16_to_cpu(a->mapping_pairs_offset), mp, mp_size);

    free(mp);
    return 0;
}

static int relocate_attribute(ntfs_resize_t *resize) {
    ATTR_RECORD *a;
    runlist *rl;
    int i;

    a = resize->ctx->attr;
    if (!a->non_resident) return 0;

    if (!(rl = ntfs_mapping_pairs_decompress(resize->vol, a, NULL)))
        return -1;

    for (i = 0; rl[i].length; i++) {
        s64 lcn = rl[i].lcn;
        s64 lcn_length = rl[i].length;

        if (lcn == LCN_HOLE || lcn == LCN_RL_NOT_MAPPED)
            continue;

        /* FIXME: ntfs_mapping_pairs_decompress should return error */
        if (lcn < 0 || lcn_length <= 0)
            FREE_FAULT(rl)

        if (relocate_run(resize, &rl, i))
            FREE_FAULT(rl)
    }

    if (resize->dirty_inode == DIRTY_ATTRIB) {
        if (replace_attribute_runlist(resize->vol, resize->ctx, rl))
            FREE_FAULT(rl)
        resize->dirty_inode = DIRTY_INODE;
    }

    free(rl);
    return 0;
}

static int is_mftdata(ntfs_resize_t *resize) {
    if (resize->ctx->attr->type != AT_DATA)
        return 0;

    if (resize->mref == 0)
        return 1;

    if (MREF_LE(resize->mrec->base_mft_record) == 0 &&
            MSEQNO_LE(resize->mrec->base_mft_record) != 0)
        return 1;

    return 0;
}

static int handle_mftdata(ntfs_resize_t *resize, int do_mftdata) {
    ATTR_RECORD *attr = resize->ctx->attr;
    VCN highest_vcn, lowest_vcn;

    if (do_mftdata) {
        if (!is_mftdata(resize)) return 0;

        highest_vcn = sle64_to_cpu(attr->highest_vcn);
        lowest_vcn = sle64_to_cpu(attr->lowest_vcn);
        if (resize->mft_highest_vcn != highest_vcn)
            return 0;

        if (lowest_vcn == 0)
            resize->mft_highest_vcn = lowest_vcn;
        else resize->mft_highest_vcn = lowest_vcn - 1;

    } else if (is_mftdata(resize)) {
        highest_vcn = sle64_to_cpu(attr->highest_vcn);
        if (resize->mft_highest_vcn < highest_vcn)
            resize->mft_highest_vcn = highest_vcn;
        return 0;
    }

    return 1;
}

static int relocate_attributes(ntfs_resize_t *resize, int do_mftdata) {
    int ret;

    if (!(resize->ctx = attr_get_search_ctx(NULL, resize->mrec)))
        return -1;

    while (!ntfs_attrs_walk(resize->ctx)) {
        if (resize->ctx->attr->type == AT_END)
            break;

        if (handle_mftdata(resize, do_mftdata) == 0)
            continue;

        ret = ntfs_inode_badclus_bad(resize->mref, resize->ctx->attr);
        if (ret == -1) return -1;
        else if (ret == 1) continue;

        if (resize->mref == FILE_Bitmap &&
                resize->ctx->attr->type == AT_DATA)
            continue;

        if (relocate_attribute(resize))
            return -1;
    }

    ntfs_attr_put_search_ctx(resize->ctx);
    return 0;
}

static int relocate_inode(ntfs_resize_t *resize, MFT_REF mref, int do_mftdata) {
    if (ntfs_file_record_read(resize->vol, mref, &resize->mrec, NULL)) {
        /* FIXME: continue only if it make sense, e.g.
           MFT record not in use based on $MFT bitmap */
        if (errno == EIO || errno == ENOENT)
            return 0;
        return -1;
    }

    if (!(resize->mrec->flags & MFT_RECORD_IN_USE))
        return 0;

    resize->mref = mref;
    resize->dirty_inode = DIRTY_NONE;

    if (relocate_attributes(resize, do_mftdata))
        return -1;

    if (resize->dirty_inode == DIRTY_INODE) {
        //		if (vol->dev->d_ops->sync(vol->dev) == -1)
        //			perr_exit("Failed to sync device");
        if (write_mft_record(resize->vol, mref, resize->mrec))
            return -1;
    }

    return 0;
}

static int relocate_inodes(ntfs_resize_t *resize) {
    s64 nr_mft_records;
    MFT_REF mref;
    VCN highest_vcn;

    resize->relocations = 0;
    resize->mrec = ntfs_malloc(resize->vol->mft_record_size);
    if (!resize->mrec) return -1;

    nr_mft_records = resize->vol->mft_na->initialized_size >>
            resize->vol->mft_record_size_bits;

    for (mref = 0; mref < (MFT_REF) nr_mft_records; mref++) {
        if (relocate_inode(resize, mref, 0))
            FREE_FAULT(resize->mrec)
    }

    while (1) {
        highest_vcn = resize->mft_highest_vcn;
        mref = nr_mft_records;
        do {
            relocate_inode(resize, --mref, 1);
            if (resize->mft_highest_vcn == 0)
                goto done;
        } while (mref);

        if (highest_vcn == resize->mft_highest_vcn)
            FREE_FAULT(resize->mrec)
    }
done:
    free(resize->mrec);

    return 0;
}

/**
 * truncate_badclust_bad_attr
 *
 * The metadata file $BadClus needs to be shrunk.
 *
 * FIXME: this function should go away and instead using a generalized
 * "truncate_bitmap_data_attr()"
 */
static int truncate_badclust_bad_attr(ntfs_resize_t *resize) {
    ATTR_RECORD *a;
    runlist *rl_bad;
    s64 nr_clusters = resize->new_volume_size;
    ntfs_volume *vol = resize->vol;

    a = resize->ctx->attr;
    if (!a->non_resident)
        /* FIXME: handle resident attribute value */
        return -1;

    if (!(rl_bad = ntfs_mapping_pairs_decompress(vol, a, NULL)))
        return -1;

    if (rl_truncate(&rl_bad, nr_clusters))
        return -1;

    a->highest_vcn = cpu_to_sle64(nr_clusters - 1LL);
    a->allocated_size = cpu_to_sle64(nr_clusters * vol->cluster_size);
    a->data_size = cpu_to_sle64(nr_clusters * vol->cluster_size);

    if (replace_attribute_runlist(vol, resize->ctx, rl_bad))
        return -1;
    free(rl_bad);

    return 0;
}

/**
 * truncate_badclust_file
 *
 * Shrink the $BadClus file to match the new volume size.
 */
static int truncate_badclust_file(ntfs_resize_t *resize) {
    lookup_data_attr(resize->vol, FILE_BadClus, "$Bad", &resize->ctx);
    /* FIXME: sanity_check_attr(ctx->attr); */
    if (truncate_badclust_bad_attr(resize))
        return -1;

    if (write_mft_record(resize->vol, resize->ctx->ntfs_ino->mft_no,
            resize->ctx->mrec))
        return -1;
    ntfs_attr_put_search_ctx(resize->ctx);

    return 0;
}

/**
 * nr_clusters_to_bitmap_byte_size
 *
 * Take the number of clusters in the volume and calculate the size of $Bitmap.
 * The size must be always a multiple of 8 bytes.
 */
static s64 nr_clusters_to_bitmap_byte_size(s64 nr_clusters) {
    s64 bm_bsize;

    bm_bsize = rounded_up_division(nr_clusters, 8);
    bm_bsize = (bm_bsize + 7) & ~7;

    return bm_bsize;
}

/**
 * realloc_bitmap_data_attr
 *
 * Reallocate the metadata file $Bitmap.  It must be large enough for one bit
 * per cluster of the shrunken volume.  Also it must be a of 8 bytes in size.
 */
static int realloc_bitmap_data_attr(ntfs_resize_t *resize,
        runlist **rl,
        s64 nr_bm_clusters) {
    s64 i;
    ntfs_volume *vol = resize->vol;
    ATTR_RECORD *a = resize->ctx->attr;
    s64 new_size = resize->new_volume_size;
    struct bitmap *bm = &resize->lcn_bitmap;

    if (!(*rl = ntfs_mapping_pairs_decompress(vol, a, NULL)))
        return -1;

    release_bitmap_clusters(bm, *rl);
    free(*rl);

    for (i = vol->nr_clusters; i < new_size; i++)
        ntfs_bit_set(bm->bm, i, 0);

    if (!(*rl = alloc_cluster(bm, nr_bm_clusters, new_size, 0)))
        return -1;

    return 0;
}

static int realloc_lcn_bitmap(ntfs_resize_t *resize, s64 bm_bsize) {
    u8 *tmp;

    if (!(tmp = realloc(resize->lcn_bitmap.bm, bm_bsize)))
        return -1;

    resize->lcn_bitmap.bm = tmp;
    resize->lcn_bitmap.size = bm_bsize;
    bitmap_file_data_fixup(resize->new_volume_size, &resize->lcn_bitmap);

    return 0;
}

/**
 * truncate_bitmap_data_attr
 */
static int truncate_bitmap_data_attr(ntfs_resize_t *resize) {
    ATTR_RECORD *a;
    runlist *rl;
    s64 bm_bsize, size;
    s64 nr_bm_clusters;
    ntfs_volume *vol = resize->vol;

    a = resize->ctx->attr;
    if (!a->non_resident)
        /* FIXME: handle resident attribute value */
        return -1;

    bm_bsize = nr_clusters_to_bitmap_byte_size(resize->new_volume_size);
    nr_bm_clusters = rounded_up_division(bm_bsize, vol->cluster_size);

    if (resize->shrink) {
        if (realloc_bitmap_data_attr(resize, &rl, nr_bm_clusters))
            return -1;
        if (realloc_lcn_bitmap(resize, bm_bsize))
            return -1;
    } else {
        if (realloc_lcn_bitmap(resize, bm_bsize))
            return -1;
        if (realloc_bitmap_data_attr(resize, &rl, nr_bm_clusters))
            return -1;
    }

    a->highest_vcn = cpu_to_sle64(nr_bm_clusters - 1LL);
    a->allocated_size = cpu_to_sle64(nr_bm_clusters * vol->cluster_size);
    a->data_size = cpu_to_sle64(bm_bsize);
    a->initialized_size = cpu_to_sle64(bm_bsize);

    replace_attribute_runlist(vol, resize->ctx, rl);

    /*
     * FIXME: update allocated/data sizes and timestamps in $FILE_NAME
     * attribute too, for now chkdsk will do this for us.
     */

    size = ntfs_rl_pwrite(vol, rl, 0, bm_bsize, resize->lcn_bitmap.bm);
    if (bm_bsize != size) return -1;

    free(rl);
    return 0;
}

/**
 * truncate_bitmap_file
 *
 * Shrink the $Bitmap file to match the new volume size.
 */
static int truncate_bitmap_file(ntfs_resize_t *resize) {
    lookup_data_attr(resize->vol, FILE_Bitmap, NULL, &resize->ctx);
    if (truncate_bitmap_data_attr(resize))
        return -1;

    if (write_mft_record(resize->vol, resize->ctx->ntfs_ino->mft_no,
            resize->ctx->mrec))
        return -1;
    ntfs_attr_put_search_ctx(resize->ctx);

    return 0;
}

/**
 * update_bootsector
 *
 * FIXME: should be done using ntfs_* functions
 */
static int update_bootsector(ntfs_resize_t *r) {
    NTFS_BOOT_SECTOR bs;
    s64 bs_size = sizeof (NTFS_BOOT_SECTOR);
    ntfs_volume *vol = r->vol;

    if (vol->dev->d_ops->seek(vol->dev, 0, SEEK_SET) == (off_t) - 1)
        return -1;
    if (vol->dev->d_ops->read(vol->dev, &bs, bs_size) == -1)
        return -1;

    bs.number_of_sectors = cpu_to_sle64(r->new_volume_size *
            bs.bpb.sectors_per_cluster);

    if (r->mftmir_old) {
        if (copy_clusters(r, r->mftmir_rl.lcn, r->mftmir_old,
                r->mftmir_rl.length))
            return -1;
        bs.mftmirr_lcn = cpu_to_sle64(r->mftmir_rl.lcn);
    }
    if (vol->dev->d_ops->seek(vol->dev, 0, SEEK_SET) == (off_t) - 1)
        return -1;
    if (vol->dev->d_ops->write(vol->dev, &bs, bs_size) == -1)
        return -1;

    return 0;
}

int ntfsresiz(char *volume, int nsize) {
    ntfsck_t fsck;
    ntfs_resize_t resize;
    s64 new_size = 0;
    s64 device_size; /* in bytes */
    ntfs_volume *vol = NULL;
    int result = 0;

    if (!volume) return EPARAM;
    if ((result = mount_volume(&vol, volume)))
        return result;

    device_size = ntfs_device_size_get(vol->dev, vol->sector_size);
    device_size *= vol->sector_size;
    if (device_size <= 0) return EDEVICE;
    if (device_size < vol->nr_clusters * vol->cluster_size)
        return EDEVICE;

    /* Take the integer part: don't make the volume bigger than requested */
    s64 new_bytes = nsize;
    new_bytes *= NTFS_MBYTE;
    new_size = new_bytes / vol->cluster_size;

    /* Backup boot sector at the end of device isn't counted in NTFS
       volume size thus we have to reserve space for it. */
    if (new_size) --new_size;

    if (device_size < new_bytes)
        return ENEWSIZ;
    if (new_size == vol->nr_clusters || (new_bytes == device_size && new_size == vol->nr_clusters - 1))
        return ENOTHING;

    memset(&resize, 0, sizeof (resize));
    resize.vol = vol;
    resize.new_volume_size = new_size;
    /* This is also true if --info was used w/o --size (new_size = 0) */
    if (new_size < vol->nr_clusters)
        resize.shrink = 1;
    //printf("new_size:%ld\n", new_size);
    //printf("resize.shrink:%ld\n", resize.shrink);
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

        /* FIXME: performance - relocate logfile here if it's needed */
        if (prepare_volume_fixup(vol))
            FREE_RESULT(resize.lcn_bitmap.bm, EPREPARE)
            if (resize.relocations) {
                if (relocate_inodes(&resize))
                    FREE_RESULT(resize.lcn_bitmap.bm, ERELOCATE)
                }

    if (truncate_badclust_file(&resize))
        FREE_RESULT(resize.lcn_bitmap.bm, ETCLUSE)
        if (truncate_bitmap_file(&resize))
            FREE_RESULT(resize.lcn_bitmap.bm, ETBITMAP)
            if (update_bootsector(&resize))
                FREE_RESULT(resize.lcn_bitmap.bm, EBOOT)

                /* WARNING: don't modify the texts, external tools grep for them */
                //printf("Syncing device ...\n");
                if (vol->dev->d_ops->sync(vol->dev) == -1)
                    FREE_RESULT(resize.lcn_bitmap.bm, EFSYNC)

                    //printf("Successfully resized NTFS on device '%s'.\n", vol->dev->d_name);
                    //if (resize.shrink) return ERR_SHRINK;
#if CLEAN_EXIT
                    if (resize.lcn_bitmap.bm) free(resize.lcn_bitmap.bm);
#endif
    if (vol) umount_volume(vol);

    return 0;
}
