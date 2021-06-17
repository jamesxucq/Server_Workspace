/* 
 * File:   main.c
 * Author: Administrator
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "dmap64.h"
#include "pmap64.h"
#include "rivfs/riv_md5.h"
#include "rivfs/riv_sha1.h"
#include "rivfs/rivfs.h"
#include "rivfs/river_entity.h"
//
#define FILE_SIZE_DESCRIP(SIZE, FILDES) { \
    struct stat64 statbuf; \
    if (fstat64(FILDES, &statbuf) < 0) printf("fstat errno:%d", errno); \
    SIZE = statbuf.st_size; \
}

#pragma	pack(1)

struct simple_chks {
    uint64 offset;
    unsigned char md5_chks[MD5_DIGEST_LEN];
};

struct complex_chks {
    uint64 offset;
    uint32 adler32_cks;
    unsigned char md5_chks[MD5_DIGEST_LEN];
};
#pragma	pack()

void simple_chks(unsigned char *pchks_simple, int simple_fildes, uint64 file_size, uint64 offset) {
    struct dmap64 *buffer;
    uint64 index, offset_end;
    RIV_MD5_CTX md;
    int32 remainder;

    buffer = (struct dmap64 *) dmap_file64(simple_fildes, file_size, MAX_MAP_SIZE, CHKS_CHUNK);
    offset_end = offset + CHUNK_SIZE;
    if (offset_end > file_size) offset_end = file_size;

    RIV_MD5Init(&md);
    for (index = offset; index + CHKS_CHUNK <= offset_end; index += CHKS_CHUNK) {
        RIV_MD5Update(&md, (uchar *) dmap_ptr64(buffer, index, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int32) (offset_end - index);
    if (remainder > 0)
        RIV_MD5Update(&md, (uchar *) dmap_ptr64(buffer, index, remainder), remainder);
    RIV_MD5Final(&md);
    memcpy(pchks_simple, md.digest, MD5_DIGEST_LEN);

    undmap_file64(buffer);
}

struct simple_chks *creat_file_simple_chks(int *chks_tally, char *file_name, uint64 offset) {
    int simple_tally;
    uint64 veri_length;
    uint64 file_size;

    int simple_fildes = open64(file_name, O_RDONLY, 0);
    if (simple_fildes == -1) return NULL;
    FILE_SIZE_DESCRIP(file_size, simple_fildes)
    if (offset > file_size) return NULL;
    veri_length = file_size - offset;

    simple_tally = veri_length / CHUNK_SIZE;
    if (POW2N_MOD(veri_length, CHUNK_SIZE)) simple_tally++;

    struct simple_chks *pchks_simple = (struct simple_chks*) malloc(sizeof (struct simple_chks) * simple_tally);
    if (!pchks_simple) return NULL;
    *chks_tally = simple_tally;

    int index;
    uint64 token;
    for (index = 0; index < simple_tally; index++) {
        pchks_simple[index].offset = index*CHUNK_SIZE;
        token = offset + index*CHUNK_SIZE;
        simple_chks(pchks_simple[index].md5_chks, simple_fildes, file_size, token);
    }

    close(simple_fildes);
    return pchks_simple;
}

struct simple_chks *creat_chunk_simple_chks(char *file_name, uint64 offset) {
    uint64 file_size;

    int simple_fildes = open64(file_name, O_RDONLY, 0);
    if (simple_fildes == -1) return NULL;
    FILE_SIZE_DESCRIP(file_size, simple_fildes)
    if (offset > file_size) return NULL;

    struct simple_chks *pchks_simple = (struct simple_chks*) malloc(sizeof (struct simple_chks));
    if (!pchks_simple) return NULL;
    pchks_simple[0].offset = offset;
    simple_chks(pchks_simple[0].md5_chks, simple_fildes, file_size, offset);

    close(simple_fildes);
    return pchks_simple;
}

struct simple_chks *build_file_simple_bzl(int *chks_tally, struct riv_live *plive, char *file_name, uint64 offset) {
    char path[MAX_PATH * 3];
    uint64 veri_length;
    //
    POOL_PATH(path, plive->location, file_name);
    struct stat64 statbuf;
    if (stat64(path, &statbuf) < 0) return NULL;
    if (offset > statbuf.st_size) return NULL;
    uint32 cinde = offset >> 22;
    veri_length = statbuf.st_size - ((uint64) cinde << 22);

    int simple_tally = veri_length / CHUNK_SIZE;
    if (POW2N_MOD(veri_length, CHUNK_SIZE)) simple_tally++;

    struct simple_chks *pchks_simple = (struct simple_chks*) malloc(sizeof (struct simple_chks) * simple_tally);
    if (!pchks_simple) return NULL;
    //
    riv_file_chks((struct riv_chks *) pchks_simple, file_name, cinde, plive);
    *chks_tally = simple_tally;
    //
    return pchks_simple;
}

/*
 */
void print_rivchks(struct riv_chks *chks) {
    char chks_str[64];
    int ind;
    for (ind = 0; ind < MD5_DIGEST_LEN; ind++)
        sprintf(chks_str + ind * 2, "%02x", chks->md5_chks[ind]);
    chks_str[32] = '\0';
    printf("offset:%llu chks:%s\n", chks->offset, chks_str);
}

void print_river_checks(struct riv_chks *chks, int chks_tally) {
    int i;
    printf("chks_tally:%d\n", chks_tally);
    for (i = 0; i < chks_tally; i++)
        print_rivchks(&chks[i]);
}

void print_sha1(unsigned char *pchks) {
    char chks_str[64];
    int ind;
    for (ind = 0; ind < SHA1_DIGEST_LEN; ind++)
        sprintf(chks_str + ind * 2, "%02x", pchks[ind]);
    chks_str[40] = '\0';
    printf("sha1 chks:%s\n", chks_str);
}

void print_sha1_checks(struct riv_chks *chks, int chks_tally) {
    unsigned char sha1_chks[SHA1_DIGEST_LEN];
    int i;
    riv_sha1_ctx cx[1];
    //
    printf("chks_tally:%d\n", chks_tally);
    riv_sha1_begin(cx);
    for (i = 0; i < chks_tally; i++) {
        riv_sha1_hash(chks[i].md5_chks, MD5_DIGEST_LEN, cx);
    }
    riv_sha1_end(sha1_chks, cx);
    print_sha1(sha1_chks);
}

int main(int argc, char** argv) {
    struct riv_live live;
    //
    printf("------------------------ riv_initial \n");
    riv_initial(&live, "/home/james/test/live");
    //
    printf("------------------------ river_insert \n");
    river_insert("/UNIXfavicon.pdf", &live);
    printf("------------------------ riv_cuk_chks \n");
    /*
        struct riv_chks cchks;
        riv_cuk_chks(&cchks, "/UNIXfavicon.pdf", 0, &live);
        print_rivchks(&cchks);
        printf("------------------------ creat_chunk_simple_chks \n");
        char file_name[MAX_PATH * 3];
        POOL_PATH(file_name, live.location, "/UNIXfavicon.pdf");
        struct simple_chks *pchks = creat_chunk_simple_chks(file_name, 0 << 22);
        print_rivchks((struct riv_chks *) pchks);
     */
    //
    //    printf("------------------------ riv_file_chks \n");
    //    int chks_tally;
    //    struct simple_chks *pchks;
    //    pchks = build_file_simple_bzl(&chks_tally, &live, "/UNIXfavicon.pdf", 0 << 22);
    //    print_river_checks((struct riv_chks *)pchks, chks_tally);
    //    char file_name[MAX_PATH * 3];
    //    POOL_PATH(file_name, live.location, "/UNIXfavicon.pdf");
    //    // printf("file_name:%s\n", file_name);
    //    pchks = creat_file_simple_chks(&chks_tally, file_name, 0 << 22);
    //    print_river_checks((struct riv_chks *)pchks, chks_tally);
    //
    unsigned char chks[SHA1_DIGEST_LEN];
    riv_file_sha1(chks, "/UNIXfavicon.pdf", &live);
    print_sha1(chks);
    int chks_tally;
    char file_name[MAX_PATH * 3];
    POOL_PATH(file_name, live.location, "/UNIXfavicon.pdf");
    struct simple_chks *pchks = creat_file_simple_chks(&chks_tally, file_name, 0);
    print_sha1_checks((struct riv_chks *) pchks, chks_tally);
    //
    printf("------------------------ river_copy \n");
    // river_copy("/favicon.pdf", "/UNIXfavicon.pdf", &live);
    //
    printf("------------------------ river_move \n");
    // river_move("/favicon.pdf", "/UNIXfavicon.pdf", &live);
    //
    printf("------------------------ river_remove \n");
    // cchks_idle_link(live.fpchks);
    // river_remove("/favicon.pdf", &live);
    river_remove("/UNIXfavicon.pdf", &live);
    //
    printf("------------------------ riv_final \n");
    riv_final(&live);
    //
    printf("------------------------ EXIT_SUCCESS \n");
    //
    return (EXIT_SUCCESS);
}

