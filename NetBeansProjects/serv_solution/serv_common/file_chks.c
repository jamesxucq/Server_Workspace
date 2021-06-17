#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "third_party.h"
#include "file_utility.h"
#include "file_chks.h"
#include "checksum.h"

void complex_chks(struct complex_chks *pcchks, struct dmap64 *buffer, uint64 offset) {
    unsigned int adler32_chk = get_adler32_chks(0, NULL, 0);
    pcchks->adler32_chk = get_adler32_chks(adler32_chk, dmap_ptr64(buffer, offset, BLOCK_SIZE), BLOCK_SIZE);
    get_chks_md5(dmap_ptr64(buffer, offset, BLOCK_SIZE), BLOCK_SIZE, pcchks->md5_chks);
    // pcchks->offset = offset;
}

void complex_leng_chks(struct complex_chks *pcchks, struct dmap64 *buffer, uint64 offset, uint32 colen) {
    unsigned int adler32_chk = get_adler32_chks(0, NULL, 0);
    pcchks->adler32_chk = get_adler32_chks(adler32_chk, dmap_ptr64(buffer, offset, colen), colen);
    get_chks_md5(dmap_ptr64(buffer, offset, colen), colen, pcchks->md5_chks);
    // pcchks->offset = offset;
}

uint32 creat_file_complex_chks(FILE **fpchks, char *file_name, uint64 offset) {
_LOG_INFO("build file complex chks");
    FILE *fpcrea;
    if (!(fpcrea = tmpfile64())) return 0x03;
    //
    int fildes = open64(file_name, O_RDONLY, 0);
    if (fildes == -1) {
        _LOG_ERROR("not open file:%s", file_name);
        return 0x02;
    }
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, fildes)
    if (file_size <= offset) return 0x01;
    struct dmap64 *buffer = (struct dmap64 *) dmap_file64(fildes, file_size, MAX_MAP_SIZE, BLOCK_SIZE);
    //
    uint64 vleng = file_size - offset;
    int complex_tatol = vleng / BLOCK_SIZE;
    struct complex_chks cchks;
    int inde;
    uint64 posit;
    for (cchks.offset = 0, posit = offset, inde = 0; inde < complex_tatol; cchks.offset += BLOCK_SIZE, posit += BLOCK_SIZE, inde++) {
        // cchks.offset = inde*BLOCK_SIZE; posit = offset + inde*BLOCK_SIZE;
        complex_chks(&cchks, buffer, posit);
        fwrite(&cchks, sizeof (struct complex_chks), 1, fpcrea);
    }
    if (POW2N_MOD(vleng, BLOCK_SIZE)) {
        complex_leng_chks(&cchks, buffer, posit, POW2N_MOD(vleng, BLOCK_SIZE));
        fwrite(&cchks, sizeof (struct complex_chks), 1, fpcrea);
    }
    //
    undmap_file64(buffer);
    close(fildes);
    *fpchks = fpcrea;
    return 0x00;
}

char *get_md5sum_textT(unsigned char *md5sum) {
    static char chks_str[33];
    uint32 ind;
    for (ind = 0; ind < MD5_DIGEST_LEN; ind++)
        sprintf(chks_str + ind * 2, "%02x", md5sum[ind]);
    chks_str[32] = '\0';
    return chks_str;
}

uint32 creat_chunk_complex_chks(FILE **fpchks, char *file_name, uint64 offset) {
_LOG_INFO("build chunk complex chks");
    FILE *fpcrea;
    if (!(fpcrea = tmpfile64())) return 0x03;
    //
    int fildes = open64(file_name, O_RDONLY, 0);
    if (fildes == -1) {
        _LOG_ERROR("not open file:%s", file_name);
        return 0x02;
    }
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, fildes)
    if (file_size < (offset + CHUNK_SIZE)) return 0x01;
    struct dmap64 *buffer = (struct dmap64 *) dmap_file64(fildes, file_size, MAX_MAP_SIZE, BLOCK_SIZE);
    //
    int complex_tatol = CHUNK_SIZE / BLOCK_SIZE;
    struct complex_chks cchks;
    int inde;
    uint64 posit;
    for (cchks.offset = 0, posit = offset, inde = 0; inde < complex_tatol; cchks.offset += BLOCK_SIZE, posit += BLOCK_SIZE, inde++) {
        // cchks.offset = inde*BLOCK_SIZE; posit = offset + inde*BLOCK_SIZE;
        complex_chks(&cchks, buffer, posit);
        // _LOG_DEBUG("offset:%u adler32_chk:%u md5_chks:%s", cchks.offset, cchks.adler32_chk, get_md5sum_textT(cchks.md5_chks));
        fwrite(&cchks, sizeof (struct complex_chks), 1, fpcrea);
    }
    //
    undmap_file64(buffer);
    close(fildes);
    *fpchks = fpcrea;
    return 0x00;
}
//

int chks_text_stre(char *chks_str, FILE *file_stre, uint64 file_size) {
    struct pmap64 *buf;
    uint64 inde;
    MD5_CTX md;
    int32 remainder;
    //
    buf = (struct pmap64 *) pmap_file64(file_stre, file_size, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = 0; inde + CHKS_CHUNK <= file_size; inde += CHKS_CHUNK) {
        MD5Update(&md, pmap_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int32) (file_size - inde);
    if (remainder > 0)
        MD5Update(&md, pmap_ptr64(buf, inde, remainder), remainder);
// fprintf(stderr, "map->status:%d\n", buf->status);
    MD5Final(&md);
    //
    unpmap_file64(buf);
    //
    uint32 ind;
    for (ind = 0; ind < MD5_DIGEST_LEN; ind++)
        sprintf(chks_str + ind * 2, "%02x", md.digest[ind]);
    chks_str[32] = '\0';
    //
    return 0;
}

int file_chks_name(unsigned char *chks, char *file_name, uint64 file_size) {
    struct dmap64 *buf;
    uint64 inde;
    MD5_CTX md;
    int32 remainder;

    memset(chks, 0, MD5_DIGEST_LEN);
    int fildes = open64(file_name, O_RDONLY, 0);
    if (fildes == -1) {
        _LOG_ERROR("not open file:%s", file_name);
        return -1;
    }

    buf = (struct dmap64 *) dmap_file64(fildes, file_size, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = 0; inde + CHKS_CHUNK <= file_size; inde += CHKS_CHUNK) {
        MD5Update(&md, dmap_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int32) (file_size - inde);
    if (remainder > 0)
        MD5Update(&md, dmap_ptr64(buf, inde, remainder), remainder);
    MD5Final(&md);
    memcpy(chks, md.digest, MD5_DIGEST_LEN);
    //
    undmap_file64(buf);
    close(fildes);
    return 0;
}



