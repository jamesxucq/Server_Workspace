#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "third_party.h"
#include "utility/md5.h"
#include "checksum.h"

void get_md5(unsigned char *buf, int32 len, unsigned char *chks) {
    MD5_CTX md;
    MD5Init(&md);
    MD5Update(&md, buf, len);
    MD5Final(&md);
    memcpy(chks, md.digest, MD5_DIGEST_LEN);
}

//static int32 sumresidue;
static MD5_CTX md;

inline void sum_init(int seed) {
    MD5Init(&md);
}

/**
 * Feed data into an MD4 accumulator, md.  The results may be
 * retrieved using sum_end().  md is used for different purposes at
 * different points during execution.
 *
 * @todo Perhaps get rid of md and just pass in the address each time.
 * Very slightly clearer and slower.
 **/
inline void sum_update(const char *p, int32 len) {
    MD5Update(&md, (uchar *) p, len);
}

inline int sum_end(unsigned char *chks) {
    MD5Final(&md);
    memcpy(chks, md.digest, MD5_DIGEST_LEN);
    return MD5_DIGEST_LEN;
}

void file_chks(unsigned char *sum, char *file_name, uint64 offset) {
    struct dmap64 *buf;
    uint64 inde, file_size;
    MD5_CTX md;
    int32 remainder;

    memset(sum, 0, MD5_DIGEST_LEN);
    int fildes = open64(file_name, O_RDONLY, 0);
    if (fildes == -1) {
        _LOG_ERROR("not open file:%s", file_name);
        return;
    }

    struct stat64 statb;
    if (fstat64(fildes, &statb) < 0) return;
    file_size = statb.st_size;

    buf = (struct dmap64 *) dmap_file64(fildes, file_size, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = offset; inde + CHKS_CHUNK <= file_size; inde += CHKS_CHUNK) {
        MD5Update(&md, dmap_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int32) (file_size - inde);
    if (remainder > 0)
        MD5Update(&md, dmap_ptr64(buf, inde, remainder), remainder);
    MD5Final(&md);
    memcpy(sum, md.digest, MD5_DIGEST_LEN);

    undmap_file64(buf);
    close(fildes);
}

void file_chks_text(char *chks_str, char *file_name, uint64 offset) {
    struct dmap64 *buf;
    uint64 inde, file_size;
    MD5_CTX md;
    int32 remainder;

    int fildes = open64(file_name, O_RDONLY, 0);
    if (fildes == -1) {
        _LOG_ERROR("not open file:%s", file_name);
        return;
    }

    struct stat64 statb;
    if (fstat64(fildes, &statb) < 0) return;
    file_size = statb.st_size;

    buf = (struct dmap64 *) dmap_file64(fildes, file_size, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = offset; inde + CHKS_CHUNK <= file_size; inde += CHKS_CHUNK) {
        MD5Update(&md, dmap_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int32) (file_size - inde);
    if (remainder > 0)
        MD5Update(&md, dmap_ptr64(buf, inde, remainder), remainder);
    MD5Final(&md);

    undmap_file64(buf);
    close(fildes);
    //
    uint32 ind;
    for (ind = 0; ind < MD5_DIGEST_LEN; ind++)
        sprintf(chks_str + ind * 2, "%02x", md.digest[ind]);
    chks_str[32] = '\0';
}

void file_chks64(unsigned char *chks, char *file_name, uint64 offset) {
    struct dmap64 *buf;
    uint64 inde, file_size;
    MD5_CTX md;
    int32 remainder;

    memset(chks, 0, MD5_DIGEST_LEN);
    int fildes = open64(file_name, O_RDONLY, 0);
    if (fildes == -1) {
        _LOG_ERROR("not open file:%s", file_name);
        return;
    }

    struct stat64 statb;
    if (fstat64(fildes, &statb) < 0) return;
    file_size = statb.st_size;

    buf = (struct dmap64 *) dmap_file64(fildes, file_size, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = offset; inde + CHKS_CHUNK <= file_size; inde += CHKS_CHUNK) {
        MD5Update(&md, dmap_ptr64(buf, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int32) (file_size - inde);
    if (remainder > 0)
        MD5Update(&md, dmap_ptr64(buf, inde, remainder), remainder);
    MD5Final(&md);
    memcpy(chks, md.digest, MD5_DIGEST_LEN);

    undmap_file64(buf);
    close(fildes);
}

void get_md5sum_text(char *buf, int len, char *chks_str) {
    MD5_CTX md;

    MD5Init(&md);
    MD5Update(&md, (unsigned char *) buf, len);
    MD5Final(&md);

    uint32 inde;
    for (inde = 0; inde < MD5_DIGEST_LEN; inde++)
        sprintf(chks_str + inde * 2, "%02x", md.digest[inde]);
    chks_str[32] = '\0';
}

