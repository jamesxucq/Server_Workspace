#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility/md5.h"
#include "utility/adler32.h"
#include "common_macro.h"

#define MD5_DIGEST_LEN 16
// #define MD5_TEXT_LENGTH	(MD5_DIGEST_LEN * 2 + 1)
#define MD5_TEXT_LENGTH	33

#define CHKS_CHUNK 4096

//
#define get_adler32_chks(adler, buf, len)               adler32_chksum(adler, buf, len)
#define get_adler32_rolling(chks, len, c1, c2)          adler32_rolling(chks, len, c1, c2);

#define get_chks_md5(buf, len, chks) get_md5(buf, len, chks)
void get_md5(unsigned char *buf, int32 len, unsigned char *chks);

//
inline void sum_init(int seed);
inline void sum_update(const char *p, int32 len);
inline int sum_end(unsigned char *chks);

//
void get_md5sum_text(char *buf, int len, char *chks_str);
void file_chks(unsigned char *chks, char *file_name, uint64 offset);
void file_chks_text(char *chks_str, char *file_name, uint64 offset);
void file_chks64(unsigned char *chks, char *file_name, uint64 offset);

#ifdef	__cplusplus
}
#endif

#endif /* CHECKSUM_H_ */
