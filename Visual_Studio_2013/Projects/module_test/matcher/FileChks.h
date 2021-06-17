/*
* FileChks.h
*
*  Created on: 2010-3-9
*      Author: Administrator
*/
#ifndef FILECHKS_H_
#define FILECHKS_H_

#include "Checksum.h"
#include "windows.h"

//
#define BLOCK_SIZE 0x2000  // 8K
#define CHUNK_SIZE ((int)1 << 22) // 4M

struct simple_head {
	unsigned int tatol;
	struct simple_chks *chks;
};

struct complex_head {
	unsigned int tatol;
	struct complex_chks *chks;
};

#pragma	pack(1)

struct whole_chks {
	unsigned char sha1_chks[SHA1_DIGEST_LEN];
};

struct simple_chks {
	unsigned __int64 offset; // posit in file
	unsigned char md5_chks[MD5_DIGEST_LEN];
};

struct complex_chks {
	unsigned int offset; // posit in chunk
	DWORD adler32_chk;
	unsigned char md5_chks[MD5_DIGEST_LEN];
};

#pragma	pack()

//
namespace FileChks {
	int ChunkChksExt(unsigned char *md5_chks, HANDLE hChunkFile, unsigned __int64 qwFileSize, unsigned __int64 offset);
	unsigned char *FileChksMD5(TCHAR *szFileName, unsigned char *md5_chks);
	unsigned char *HandleChksMD5(HANDLE hChksFile, unsigned char *md5_chks);
	//
	inline VOID DeleFileChks(unsigned char *md5_chks)
	{ if (md5_chks) free(md5_chks); }
	inline VOID DeleSimpleChks(struct simple_head *phead_simple) {
		if (!phead_simple) return;
		if (phead_simple->chks) free(phead_simple->chks);
		free(phead_simple);
	}
	inline VOID DeleComplexChks(struct complex_head *phead_complex) {
		if (!phead_complex) return;
		if (phead_complex->chks) free(phead_complex->chks);
		free(phead_complex);
	}
	//
	unsigned char *BuildFileChks(TCHAR *szFileName, unsigned __int64 offset);
//
#define BCHKS_FAULT			((DWORD)-1)
#define BCHKS_SUCCESS		0x00
#define BCHKS_NOT_FOUND		0x01
#define BCHKS_VIOLATION		0x02
	struct complex_head *BuildFileComplexChks(DWORD *bchksValue, TCHAR *szFileName, unsigned __int64 offset);
	struct complex_head *BuildChunkComplexChks(DWORD *bchksValue, TCHAR *szFileName, unsigned __int64 offset);
};

//
void complex_leng_chks(struct complex_chks *pchks, struct tiny_map64 *buffer, unsigned __int64 offset, unsigned colen);
void complex_chks(struct complex_chks *pcchks, struct tiny_map64 *buffer, unsigned __int64 offset);

#endif /* FILECHKS_H_ */