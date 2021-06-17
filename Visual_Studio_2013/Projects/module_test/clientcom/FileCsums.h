/*
* FileCsums.h
*
*  Created on: 2010-3-9
*      Author: Administrator
*/
#ifndef FILECSUMS_H_
#define FILECSUMS_H_

#include "Checksum.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define BLOCK_SIZE 0x2000  // 8K
#define CHUNK_SIZE ((int)1 << 22) // 4M

struct simple_sums {
    int count;
    struct simple_sum *sums;
};

struct complex_sums {
    int count;
    struct complex_sum *sums;
};

#pragma	pack(1)

struct simple_sum {
    off_t offset;
    unsigned char md5_sum[DIGEST_LENGTH];
};

struct complex_sum {
    off_t offset;
    DWORD adler32_sum;
    unsigned char md5_sum[DIGEST_LENGTH];
};
#pragma	pack()


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int ChunkCsum(unsigned char *psum, HANDLE hChunkFile, off_t file_size);
int ChunkCsumExt(unsigned char *psum, HANDLE hChunkFile, off_t file_size, off_t offset);
int FileCsumMD5(TCHAR *file_name, unsigned char *psum);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline void DeleteFileCsum(unsigned char *psum) 
{if (psum) free(psum);}

inline void DeleteSimpleCsum(struct simple_sums *psums_simple) {
	if (!psums_simple) return;
	if (psums_simple->sums) free(psums_simple->sums);
	free(psums_simple);
}

inline void DeleteComplexCsum(struct complex_sums *psums_complex) {
	if (!psums_complex) return;
	if (psums_complex->sums) free(psums_complex->sums);
	free(psums_complex);
}

//////////////////////////////////////////////////////////////////////////////
unsigned char *BuildFileCsum(TCHAR *file_name, size_t offset);
struct simple_sums *BuildFileSimpleCsum(TCHAR *file_name, size_t offset);
struct complex_sums *BuildFileComplexCsum(TCHAR *file_name, size_t offset);
struct complex_sums *BuildChunkComplexCsum(TCHAR *file_name, size_t offset);

#endif /* FILECSUMS_H_ */