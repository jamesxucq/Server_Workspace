#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>

#include "riv_md5.h"
#include "riv_common.h"
#include "../pmap64.h"
#include "river_utility.h"
#include "river_entity.h"

#define INIT_ATTRIB_ENTRY(ENTRY) \
	memset(&ENTRY, '\0', sizeof(struct fattb)); \
	ENTRY.sibling = INVALID_INDEX_VALUE;

#define INIT_CUKCHK_ENTRY(ENTRY) \
	memset(&ENTRY, '\0', sizeof(struct ckchks)); \
	ENTRY.sibling = INVALID_INDEX_VALUE;

int fill_idle_entity(FILE *fpchks, FILE *fpattb) {
    struct fattb attb;
    INIT_ATTRIB_ENTRY(attb)
    if (fseeko64(fpattb, 0, SEEK_SET)) return 0x01;
    if (0 >= fwrite(&attb, sizeof (struct fattb), 1, fpattb)) return 0x01;
    // printf("attb.file_name:%s attb.sibling:%llu attb.time_stamp:%d\n", attb.file_name, attb.sibling, attb.time_stamp);
    //
    struct ckchks cchks;
    INIT_CUKCHK_ENTRY(cchks)
    if (fseeko64(fpchks, 0, SEEK_SET)) return 0x01;
    if (0 >= fwrite(&cchks, sizeof (struct ckchks), 1, fpchks)) return 0x01;
    // printf("cchks.sibling:%llu cchks.time_stamp:%d\n", cchks.sibling, cchks.time_stamp);
    //
    return 0x00;
}

static int fill_idle_attb(FILE *fpattb) {
    struct fattb attb;
    INIT_ATTRIB_ENTRY(attb)
    if (fseeko64(fpattb, 0, SEEK_END)) return 0x01;
    if (0 >= fwrite(&attb, sizeof (struct fattb), 1, fpattb)) return 0x01;
    //
    return 0x00;
}

static int fill_idle_chks(FILE *fpchks) {
    struct ckchks cchks;
    INIT_CUKCHK_ENTRY(cchks)
    if (fseeko64(fpchks, 0, SEEK_END)) return 0x01;
    if (0 >= fwrite(&cchks, sizeof (struct ckchks), 1, fpchks)) return 0x01;
    //
    return 0x00;
}
//

uint64 find_sibli_attb(FILE *fpattb, uint64 ainde) {
    struct fattb attb;
    //
    uint64 eose = sizeof (struct fattb)*ainde;
    if (fseeko64(fpattb, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fread(&attb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    uint64 sibli = attb.sibling;
    //
    memset(&attb, 0, sizeof (struct fattb));
    if (fseeko64(fpattb, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fwrite(&attb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    //
    return sibli;
}

uint64 find_idle_attb(FILE *fpattb) {
    uint64 eind;
    struct fattb attb;
    //
    if (fseeko64(fpattb, -(__off64_t)sizeof (struct fattb), SEEK_END))
        return INVALID_INDEX_VALUE;
    __off64_t eose = ftello64(fpattb);
    if (0 >= fread(&attb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    //
    if (INVALID_INDEX_VALUE != attb.sibling) {
        eind = attb.sibling;
        attb.sibling = find_sibli_attb(fpattb, attb.sibling);
        if (fseeko64(fpattb, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
        if (0 >= fwrite(&attb, sizeof (struct fattb), 1, fpattb))
            return INVALID_INDEX_VALUE;
    } else {
        if (fill_idle_attb(fpattb)) return INVALID_INDEX_VALUE;
        eind = eose / sizeof (struct fattb);
    }
    //
    return eind;
}

int add_idle_attb(FILE *fpattb, uint64 ainde) {
    struct fattb attb;
    //
    if (fseeko64(fpattb, -(__off64_t)sizeof (struct fattb), SEEK_END)) return 0x01;
    __off64_t eose = ftello64(fpattb);
    if (0 >= fread(&attb, sizeof (struct fattb), 1, fpattb)) return 0x01;
    //
    attb_sibli_modify(fpattb, ainde, attb.sibling);
    attb.sibling = ainde;
    if (fseeko64(fpattb, eose, SEEK_SET)) return 0x01;
    if (0 >= fwrite(&attb, sizeof (struct fattb), 1, fpattb)) return 0x01;
    //
    return 0x00;
}

uint64 attb_read(struct fattb *pattb, FILE *fpattb, uint64 ainde) {
    if (fseeko64(fpattb, sizeof (struct fattb)*ainde, SEEK_SET))
        return INVALID_INDEX_VALUE;
    if (0 >= fread(pattb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    return ainde;
}

uint64 attb_write(FILE *fpattb, struct fattb *pattb, uint64 ainde) {
    if (fseeko64(fpattb, sizeof (struct fattb)*ainde, SEEK_SET))
        return INVALID_INDEX_VALUE;
    if (0 >= fwrite(pattb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;

    return ainde;
}

uint64 attb_sibli_modify(FILE *fpattb, uint64 ainde, uint64 sibling) {
    struct fattb attb;
    //
    uint64 eose = sizeof (struct fattb) * ainde;
    if (fseeko64(fpattb, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fread(&attb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    uint64 osibl = attb.sibling;
    //
    attb.sibling = sibling;
    if (fseeko64(fpattb, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fwrite(&attb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    //
    return osibl;
}

uint64 attb_file_modify(FILE *fpattb, uint64 ainde, char *new_name) {
    struct fattb attb;
    //
    uint64 eose = sizeof (struct fattb) * ainde;
    if (fseeko64(fpattb, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fread(&attb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    //
    strcpy(attb.file_name, new_name);
    if (fseeko64(fpattb, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fwrite(&attb, sizeof (struct fattb), 1, fpattb))
        return INVALID_INDEX_VALUE;
    //
    return ainde;
}

//

uint64 find_sibli_chks(FILE *fpchks, uint64 cinde) {
    struct ckchks cchks;
    if (INVALID_INDEX_VALUE == cinde) return INVALID_INDEX_VALUE;
    if (fseeko64(fpchks, sizeof (struct ckchks)*cinde, SEEK_SET))
        return INVALID_INDEX_VALUE;
    if (0 >= fread(&cchks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    return cchks.sibling;
}

uint64 find_next_chks(FILE *fpchks, uint64 cinde) {
    struct ckchks cchks;
    //
    uint64 eose = sizeof (struct ckchks) * cinde;
    if (fseeko64(fpchks, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fread(&cchks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    uint64 sibli = cchks.sibling;
    //
    memset(&cchks, 0, sizeof (struct ckchks));
    cchks.sibling = INVALID_INDEX_VALUE;
    if (fseeko64(fpchks, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fwrite(&cchks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    //
    return sibli;
}

uint64 find_idle_chks(FILE *fpchks) {
    uint64 eind;
    struct ckchks cchks;
    //
    if (fseeko64(fpchks, -(__off64_t)sizeof (struct ckchks), SEEK_END))
        return INVALID_INDEX_VALUE;
    __off64_t eose = ftello64(fpchks);
    if (0 >= fread(&cchks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    //
    // printf("find idle ckchks:%llu\n", cchks.sibling);
    if (INVALID_INDEX_VALUE != cchks.sibling) {
        eind = cchks.sibling;
        cchks.sibling = find_next_chks(fpchks, cchks.sibling);
        //
        if (fseeko64(fpchks, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
        if (0 >= fwrite(&cchks, sizeof (struct ckchks), 1, fpchks))
            return INVALID_INDEX_VALUE;
    } else {
        if (fill_idle_chks(fpchks)) return INVALID_INDEX_VALUE;
        eind = eose / sizeof (struct ckchks);
        // printf("find idle eind:%llu\n", eind);
    }
    //
    return eind;
}

uint64 chks_sibli_modify(FILE *fpchks, uint64 cinde, uint64 sibling) {
    struct ckchks cchks;
    //
    if (INVALID_INDEX_VALUE == cinde) return INVALID_INDEX_VALUE;
    uint64 eose = sizeof (struct ckchks) * cinde;
    if (fseeko64(fpchks, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fread(&cchks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    uint64 osibl = cchks.sibling;
    //
    cchks.sibling = sibling;
    // printf("osibl:%llu cchks.sibling:%llu\n", osibl, cchks.sibling);
    if (fseeko64(fpchks, eose, SEEK_SET)) return INVALID_INDEX_VALUE;
    if (0 >= fwrite(&cchks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    //
    return osibl;
}

int add_idle_chks(FILE *fpchks, uint64 cuks_hind) {
    struct ckchks cchks;
    //
    if (fseeko64(fpchks, -(__off64_t)sizeof (struct ckchks), SEEK_END)) return 0x01;
    __off64_t eose = ftello64(fpchks);
    if (0 >= fread(&cchks, sizeof (struct ckchks), 1, fpchks)) return 0x01;
    //
    if (INVALID_INDEX_VALUE != cchks.sibling) {
        uint64 nind, sibli;
        for (nind = cuks_hind; INVALID_INDEX_VALUE != (sibli = find_sibli_chks(fpchks, nind)); nind = sibli);
        chks_sibli_modify(fpchks, nind, cchks.sibling);
    }
    //
    cchks.sibling = cuks_hind;
    if (fseeko64(fpchks, eose, SEEK_SET)) return 0x01;
    if (0 >= fwrite(&cchks, sizeof (struct ckchks), 1, fpchks)) return 0x01;
    //
    return 0x00;
}

uint64 chks_read(struct ckchks *pcks, FILE *fpchks, uint64 cinde) {
    if (INVALID_INDEX_VALUE == cinde) return INVALID_INDEX_VALUE;
    if (fseeko64(fpchks, sizeof (struct ckchks)*cinde, SEEK_SET))
        return INVALID_INDEX_VALUE;
    if (0 >= fread(pcks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    return cinde;
}

uint64 chks_write(FILE *fpchks, struct ckchks *pcks, uint64 cinde) {
    if (INVALID_INDEX_VALUE == cinde) return INVALID_INDEX_VALUE;
    if (fseeko64(fpchks, sizeof (struct ckchks)*cinde, SEEK_SET))
        return INVALID_INDEX_VALUE;
    if (0 >= fwrite(pcks, sizeof (struct ckchks), 1, fpchks))
        return INVALID_INDEX_VALUE;
    return cinde;
}

int chks_md5_modify(FILE *fpchks, unsigned char *md5_chks, uint64 cinde) {
    struct ckchks ccks;
    //
    if (INVALID_INDEX_VALUE == cinde) return 0x01;
    if (fseeko64(fpchks, sizeof (struct ckchks)*cinde, SEEK_SET)) return 0x01;
    if (0 >= fread(&ccks, sizeof (struct ckchks), 1, fpchks)) return 0x01;
    //
    memcpy(ccks.md5_chks, md5_chks, MD5_DIGEST_LEN);
    if (fseeko64(fpchks, sizeof (struct ckchks)*cinde, SEEK_SET)) return 0x01;
    if (0 >= fwrite(&ccks, sizeof (struct ckchks), 1, fpchks)) return 0x01;
    //
    return 0x00;
}

void chunk_chks(unsigned char *pchks, FILE *fpchks, uint64 file_size, uint64 cinde) {
    struct pmap64 *buffer;
    uint64 index, offset_end;
    RIV_MD5_CTX md;
    int32 remainder;

    uint64 offset = cinde << 22;
    buffer = (struct pmap64 *) pmap_file64(fpchks, file_size, MAX_MAP_SIZE, CHKS_CHUNK);
    offset_end = offset + CHUNK_SIZE;
    if (offset_end > file_size) offset_end = file_size;

    RIV_MD5Init(&md);
    for (index = offset; index + CHKS_CHUNK <= offset_end; index += CHKS_CHUNK) {
        RIV_MD5Update(&md, (uchar *) pmap_ptr64(buffer, index, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int32) (offset_end - index);
    if (remainder > 0)
        RIV_MD5Update(&md, (uchar *) pmap_ptr64(buffer, index, remainder), remainder);
    RIV_MD5Final(&md);
    memcpy(pchks, md.digest, MD5_DIGEST_LEN);

    unpmap_file64(buffer);
}
