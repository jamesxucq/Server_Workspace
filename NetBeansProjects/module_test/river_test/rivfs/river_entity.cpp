#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>

#include "riv_common.h"
#include "riv_sha1.h"
#include "river_utility.h"
#include "river_entity.h"

int entity_initial(struct riv_live *plive, char *location) {
    char file_name[MAX_PATH * 3];
    //
    RIVER_PATH(file_name, location, RIV_CHKS_DEFAULT);
    plive->fpchks = fopen64(file_name, "rb+");
    if (!plive->fpchks) return 0x01;
    //
    RIVER_PATH(file_name, location, RIV_ATTB_DEFAULT);
    plive->fpattb = fopen64(file_name, "rb+");
    if (!plive->fpattb) return 0x01;
    //
    return 0x00;
}

int entity_final(struct riv_live *plive) {
    if (plive->fpattb) fclose(plive->fpattb);
    if (plive->fpchks) fclose(plive->fpchks);
    return 0x00;
}

uint64 init_attrib(FILE *fpattb, char *file_name) {
    struct fattb attb;
    memset(&attb, 0, sizeof (struct fattb));
    strcpy(attb.file_name, file_name);
    uint64 ainde = find_idle_attb(fpattb);
    // printf("idle ainde:%llu\n", ainde);
    attb_write(fpattb, &attb, ainde);
    return ainde;
}

//

uint64 init_cchks(FILE *fpchks) {
    struct ckchks cchks;
    memset(&cchks, 0, sizeof (struct ckchks));
    uint64 cinde = find_idle_chks(fpchks);
    // printf("idle cinde:%llu\n", cinde);
    chks_write(fpchks, &cchks, cinde);
    return cinde;
}

// for test

uint32 cchks_inde_numbe(FILE *fpchks, uint64 cuks_hind) {
    struct ckchks cchks;
    uint32 ind;
    cchks.sibling = cuks_hind;
    for (ind = 0; INVALID_INDEX_VALUE != cchks.sibling; ind++) {
        printf("ind:%d cchks.sibling:%llu \n", ind, cchks.sibling);
        chks_read(&cchks, fpchks, cchks.sibling);
    }
    return ind;
}

void attrib_idle_link(FILE *fpattb) {
    struct fattb attb;
    if (fseeko64(fpattb, -(__off64_t)sizeof (struct fattb), SEEK_END))
        return;
    __off64_t eose = ftello64(fpattb);
    uint64 eind = eose / sizeof (struct fattb);
    if (0 >= fread(&attb, sizeof (struct fattb), 1, fpattb))
        return;
    printf("file eind:%lld sibling:%llu\n", eind, attb.sibling);
    for (; INVALID_INDEX_VALUE != attb.sibling;) {
        printf("attb.sibling:%llu\n", attb.sibling);
        attb_read(&attb, fpattb, attb.sibling);
    }
}

void cchks_idle_link(FILE *fpchks) {
    struct ckchks cchks;
    if (fseeko64(fpchks, -(__off64_t)sizeof (struct ckchks), SEEK_END))
        return;
    __off64_t eose = ftello64(fpchks);
    uint64 eind = eose / sizeof (struct ckchks);
    if (0 >= fread(&cchks, sizeof (struct ckchks), 1, fpchks))
        return;
    printf("chunk eind:%lld sibling:%llu\n", eind, cchks.sibling);
    for (; INVALID_INDEX_VALUE != cchks.sibling;) {
        printf("cchks.sibling:%llu\n", cchks.sibling);
        chks_read(&cchks, fpchks, cchks.sibling);
    }
}

//

uint64 cchks_inde_read(struct ckchks *pchks, FILE *fpchks, uint64 cuks_hind, uint32 cinde) {
    uint64 ckind;
    uint32 ind;
    if (INVALID_INDEX_VALUE == cuks_hind) return INVALID_INDEX_VALUE;
    pchks->sibling = cuks_hind;
    for (ind = 0; cinde >= ind; ind++) {
        ckind = chks_read(pchks, fpchks, pchks->sibling);
        if (INVALID_INDEX_VALUE == ckind) break;
    }
    return ckind;
}

uint64 reset_ckchks_length(FILE *fpchks, uint64 cuks_hind, uint32 old_clen, uint32 new_clen) {
    int32 diff_clen = new_clen - old_clen;
    int32 ind;
    uint64 eind, nind, sibli;
    //
    // printf("cuks_hind:%llu old_clen:%d new_clen:%d diff_clen:%d\n", cuks_hind, old_clen, new_clen, diff_clen);
    if (0 < diff_clen) { // make long then old
        // printf("0 < diff_clen\n");
        for (nind = cuks_hind; INVALID_INDEX_VALUE != (sibli = find_sibli_chks(fpchks, nind)); nind = sibli);
        for (ind = 0; diff_clen > ind; ind++) {
            eind = find_idle_chks(fpchks);
            // printf("nind:%llu eind:%llu\n", nind, eind);
            if (!ind && (INVALID_INDEX_VALUE == nind)) cuks_hind = eind;
            chks_sibli_modify(fpchks, nind, eind);
            nind = eind;
        }
    } else if (0 > diff_clen) { // make short
        // printf("0 > diff_clen\n");
        for (nind = cuks_hind, ind = 0x01; new_clen > ind; ind++) nind = find_sibli_chks(fpchks, nind);
        add_idle_chks(fpchks, find_sibli_chks(fpchks, nind));
        chks_sibli_modify(fpchks, nind, INVALID_INDEX_VALUE);
    }
    //
    return cuks_hind;
}
//

// for test

void p_chks(unsigned char *md5_chks, int index) {
    char chks_str[64];
    int ind;
    for (ind = 0; ind < MD5_DIGEST_LEN; ind++)
        sprintf(chks_str + ind * 2, "%02x", md5_chks[ind]);
    chks_str[32] = '\0';
    printf("index:%d chks:%s\n", index, chks_str);
}

uint64 file_chks_update(FILE *fpchks, char *file_name, uint64 cuks_hind, struct stat64 *pstat) {
    struct ckchks cchks;
    uint64 ckind;
    uint32 ind;
    //
    if (INVALID_INDEX_VALUE == cuks_hind) return INVALID_INDEX_VALUE;
    FILE *fpchuk;
    fpchuk = fopen64(file_name, "rb");
    if (!fpchuk) return INVALID_INDEX_VALUE;
    //
    cchks.sibling = cuks_hind;
    for (ind = 0; INVALID_INDEX_VALUE != cchks.sibling; ind++) {
        ckind = chks_read(&cchks, fpchks, cchks.sibling);
        if (pstat->st_mtime != cchks.time_stamp) {
            // printf("pstat->st_mtime:%ld cchks.time_stamp:%ld, ckind:%llu\n", pstat->st_mtime, cchks.time_stamp, ckind);
            chunk_chks(cchks.md5_chks, fpchuk, pstat->st_size, ind);
            // p_chks(cchks.md5_chks, ind);
            cchks.time_stamp = pstat->st_mtime;
            chks_write(fpchks, &cchks, ckind);
        }
    }
    //
    if (fpchuk) fclose(fpchuk);
    return cuks_hind;

}

uint64 file_chunk_update(FILE *fpchks, char *file_name, uint64 cuks_hind, uint32 cinde, struct stat64 *pstat) {
    struct ckchks cchks;
    uint64 ckind;
    uint32 ind;
    //
    if (INVALID_INDEX_VALUE == cuks_hind) return INVALID_INDEX_VALUE;
    cchks.sibling = cuks_hind;
    for (ind = 0; cinde > ind; ind++) {
        ckind = chks_read(&cchks, fpchks, cchks.sibling);
        if (INVALID_INDEX_VALUE == ckind) return INVALID_INDEX_VALUE;
    }
    //
    FILE *fpchuk;
    fpchuk = fopen64(file_name, "rb");
    if (!fpchuk) return INVALID_INDEX_VALUE;
    //
    uint64 hinde = cchks.sibling;
    for (; INVALID_INDEX_VALUE != cchks.sibling; ind++) {
        ckind = chks_read(&cchks, fpchks, cchks.sibling);
        if (pstat->st_mtime != cchks.time_stamp) {
            // printf("pstat->st_mtime:%ld cchks.time_stamp:%ld, ckind:%llu\n", pstat->st_mtime, cchks.time_stamp, ckind);
            chunk_chks(cchks.md5_chks, fpchuk, pstat->st_size, ind);
            // p_chks(cchks.md5_chks, ind);
            cchks.time_stamp = pstat->st_mtime;
            chks_write(fpchks, &cchks, ckind);
            // chks_read(&cchks, fpchks, ckind);
            // printf("cchks.time_stamp:%d\n", cchks.time_stamp);
        }
    }
    //
    if (fpchuk) fclose(fpchuk);
    return hinde;
}

uint64 chunk_chks_update(FILE *fpchks, char *file_name, uint64 cuks_hind, uint32 cinde, struct stat64 *pstat) {
    struct ckchks cchks;
    uint64 ckind;
    uint32 ind;
    //
    if (INVALID_INDEX_VALUE == cuks_hind) return INVALID_INDEX_VALUE;
    cchks.sibling = cuks_hind;
    for (ind = 0; cinde >= ind; ind++) {
        // printf("cinde:%u ind:%u cchks.sibling:%llu\n", cinde, ind, cchks.sibling);
        ckind = chks_read(&cchks, fpchks, cchks.sibling);
        // printf("ckind:%llu\n", ckind);
        if (INVALID_INDEX_VALUE == ckind) return INVALID_INDEX_VALUE;
    }
    //
    if (pstat->st_mtime != cchks.time_stamp) {
        // printf("pstat->st_mtime:%ld cchks.time_stamp:%ld\n", pstat->st_mtime, cchks.time_stamp);
        FILE *fpchuk = fopen64(file_name, "rb");
        if (!fpchuk) return INVALID_INDEX_VALUE;
        //
        chunk_chks(cchks.md5_chks, fpchuk, pstat->st_size, cinde);
        cchks.time_stamp = pstat->st_mtime;
        chks_write(fpchks, &cchks, ckind);
        //
        if (fpchuk) fclose(fpchuk);
    }
    //
    return ckind;
}

//

uint64 repod_attrib(FILE *fpattb, uint64 ainde, char *new_name) {
    struct fattb attb;
    //
    if (INVALID_INDEX_VALUE == attb_read(&attb, fpattb, ainde))
        return INVALID_INDEX_VALUE;
    strcpy(attb.file_name, new_name);
    uint64 hinde = find_idle_attb(fpattb);
    attb_write(fpattb, &attb, hinde);
    //
    return hinde;
}

uint64 repod_ckchks(FILE *fpchks, uint64 cuks_hind) {
    struct ckchks cchks;
    uint64 hinde = INVALID_INDEX_VALUE;
    uint64 sibli, ckind = INVALID_INDEX_VALUE;
    //
    if (INVALID_INDEX_VALUE == cuks_hind) return INVALID_INDEX_VALUE;
    for (cchks.sibling = cuks_hind; INVALID_INDEX_VALUE != cchks.sibling; ckind = sibli) {
        chks_read(&cchks, fpchks, cchks.sibling);
        sibli = find_idle_chks(fpchks);
        if (INVALID_INDEX_VALUE == hinde) hinde = sibli;
        chks_write(fpchks, &cchks, sibli);
        chks_sibli_modify(fpchks, ckind, sibli);
    }
    //
    return hinde;
}

//

int file_sha1_chks(unsigned char *file_chks, FILE *fpchks, uint64 cuks_hind) {
    struct ckchks cchks;
    riv_sha1_ctx cx[1];
    int chk_value = 0x00;
    int cinde;
    //
    riv_sha1_begin(cx);
    cchks.sibling = cuks_hind;
    for (cinde = 0; INVALID_INDEX_VALUE != cchks.sibling; cinde++) {
        if (INVALID_INDEX_VALUE == chks_read(&cchks, fpchks, cchks.sibling)) {
            chk_value = 0x01;
            break;
        }
        riv_sha1_hash(cchks.md5_chks, MD5_DIGEST_LEN, cx);
    }
    riv_sha1_end(file_chks, cx);
    //
    return chk_value;
}

int file_rive_chks(struct riv_chks *pchks, FILE *fpchks, uint64 ind_pos) {
    struct ckchks cchks;
    uint32 ind;
    cchks.sibling = ind_pos;
    for (ind = 0; INVALID_INDEX_VALUE != cchks.sibling; ind++) {
        chks_read(&cchks, fpchks, cchks.sibling);
        // printf("cchks.time_stamp:%d\n", cchks.time_stamp);
        // p_chks(cchks.md5_chks, ind);
        memcpy(pchks[ind].md5_chks, cchks.md5_chks, MD5_DIGEST_LEN);
        pchks[ind].offset = ind << 22;
    }
    return 0x00;
}

int chunk_rive_chks(struct riv_chks *pchks, FILE *fpchks, uint64 ind_pos, uint32 cinde) {
    struct ckchks cchks;
    if (INVALID_INDEX_VALUE == chks_read(&cchks, fpchks, ind_pos))
        return 0x01;
    memcpy(pchks->md5_chks, cchks.md5_chks, MD5_DIGEST_LEN);
    pchks->offset = cinde << 22;
    return 0x00;
}