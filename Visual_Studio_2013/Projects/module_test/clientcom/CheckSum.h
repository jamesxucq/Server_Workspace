/*
* Checksum.h
*
*  Created on: 2010-2-6
*      Author: Administrator
*/

#ifndef CHECKSUM_H_
#define CHECKSUM_H_

#include "common_macro.h"

#include "utility\md5.h"
#include "utility\adler32.h"

#define DIGEST_LENGTH 16
// #define MD5_TEXT_LENGTH	(DIGEST_LENGTH * 2 + 1)
#define MD5_TEXT_LENGTH	33

#define MAX_MAP_SIZE (256*1024)
#define CSUM_CHUNK 64

#define get_adler32_csum(buf, len) adler32_csum(buf, len)
#define get_adler32_rolling(csum, len, c1, c2) adler32_rolling_csum(csum, len, c1, c2);

#define get_csum_md5(buf, len, sum) get_md5(buf, len ,sum)
void get_md5(char *buf, int len, unsigned char *sum);

inline void sum_init(int seed);
inline void sum_update(const char *p, int len);
inline int sum_end(unsigned char *sum);


////////////////////////////////////////////////////////////////////////////////
char *get_md5sum_text(char *buffer, int length, char *sum_text);
void file_csum(unsigned char *sum, const TCHAR *file_name, off_t offset);
void file_csum(unsigned char *sum, const TCHAR *file_name);


#endif /* CHECKSUM_H_ */
