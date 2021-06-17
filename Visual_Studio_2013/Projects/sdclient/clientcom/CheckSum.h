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

#define MD5_DIGEST_LEN 16

#define MAX_MAP_SIZE 0x80000 // 512K
#define CHKS_CHUNK	4096

namespace chksm {
#define get_adler32_chksum(adler, buf, len) adler32_chksum(adler, buf, len)
#define get_adler32_rolling(chksum, len, c1, c2) adler32_rolling(chksum, len, c1, c2);
	//
#define get_chksum_md5(buf, len, sum) get_md5(buf, len, sum)
	void get_md5(unsigned char *buf, int len, unsigned char *sum);
	//
	char *get_md5sum_text(char *chksum_str, char *buffer, unsigned int length);
	unsigned char *file_chks(unsigned char *md5_chks, const TCHAR *file_name, unsigned __int64 offset);
	unsigned char *file_chks(unsigned char *md5_chks, const TCHAR *file_name);
};

#endif /* CHECKSUM_H_ */
