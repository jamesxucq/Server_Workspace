/* 
 * File:   riv_common.h
 * Author: Administrator
 */

#ifndef RIV_COMMON_H
#define	RIV_COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef int32
#define int32 int
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef uint64
#define uint64 unsigned long long
#endif

#ifndef uchar
#define uchar unsigned char
#endif

#define MD5_DIGEST_LEN 16
#define SHA1_DIGEST_LEN 20

#define CHUNK_SIZE                      ((int32)1 << 22)  // 4M
#define CHKS_CHUNK 4096
#define INVALID_INDEX_VALUE ((unsigned long long)-1)

#define RIV_CHKS_DEFAULT				"~/riv_chks.sdo"
#define RIV_ATTB_DEFAULT				"~/riv_attb.sdo"
#define RIV_INDE_DEFAULT				"~/riv_inde.db"

#define RIVER_PATH(RPATH, PATH, SUBDIR) \
        sprintf(RPATH, "%s%s", PATH, SUBDIR + 1)
#define POOL_PATH(PPATH, PATH, SUBDIR) \
        sprintf(PPATH, "%s/user_pool%s", PATH, SUBDIR)

#define FILE_EXISTS(PATH_NAME) access(PATH_NAME, F_OK)

#define POW2N_MOD(NUA, NUB)  (NUA & (NUB -1)) // x=2^n a=b%x-->a=b&(x-1)

#pragma	pack(1)

    /* struct simple_chks {
        uint64 offset;
        unsigned char md5_chks[MD5_DIGEST_LEN];
    }; */
    struct riv_chks {
        uint64 offset;
        unsigned char md5_chks[MD5_DIGEST_LEN];
    };
#pragma	pack()
    
#ifdef	__cplusplus
}
#endif

#endif	/* RIV_COMMON_H */

