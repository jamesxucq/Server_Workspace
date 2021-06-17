#include "stdafx.h"

#include "third_party.h"
#include "FileUtility.h"
#include "FileCsums.h"

////////////////////////////////////////////////////////////////
void simple_csum(unsigned char *psum, HANDLE hFile, off_t file_size, off_t offset);
void complex_csum(struct complex_sum*, HANDLE hFile, off_t file_size, off_t offset);

////////////////////////////////////////////////////////////////
unsigned char *BuildFileCsum(TCHAR *file_name, size_t offset) {
	if (!file_name) return NULL;

	unsigned char *psum = (unsigned char *) malloc(DIGEST_LENGTH);
	if (!psum) return NULL;
	file_csum(psum, file_name, offset);

	return psum;
}

struct simple_sums *BuildFileSimpleCsum(TCHAR *file_name, size_t offset) {
	int simple_count;
	struct simple_sums *psums_simple;
	size_t verify_length;

	HANDLE hFileSimple = CreateFile( file_name, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileSimple == INVALID_HANDLE_VALUE ) {
		LOG_DEBUG( _T("create file failed.\n") );
		return NULL;
	}
	size_t file_size = GetFileSize(hFileSimple, NULL);
	if(offset > file_size) return NULL;
	verify_length = file_size - offset;

	if (POW2N_MOD(verify_length, CHUNK_SIZE)) simple_count = (verify_length / CHUNK_SIZE) + 1;
	else simple_count = verify_length / CHUNK_SIZE;

	psums_simple = (struct simple_sums*) malloc(sizeof (struct simple_sums));
	if (!psums_simple) return NULL;
	psums_simple->sums = (struct simple_sum*) malloc(sizeof (struct simple_sum) * simple_count);
	if (!psums_simple->sums) return NULL;
	psums_simple->count = simple_count;

	off_t position;
	for (int index = 0; index < simple_count; index++) {
		position = offset + index * CHUNK_SIZE;
		psums_simple->sums[index].offset = position;
		simple_csum(psums_simple->sums[index].md5_sum, hFileSimple, file_size, position);
	}

	if(hFileSimple != INVALID_HANDLE_VALUE) {
		CloseHandle( hFileSimple );
		hFileSimple = INVALID_HANDLE_VALUE;
	}

	return psums_simple;
}

struct complex_sums *BuildFileComplexCsum(TCHAR *file_name, size_t offset) {
	int complex_count;
	struct complex_sums *psums_complex;
	size_t verify_length;

	HANDLE hFileComplex = CreateFile( file_name, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileComplex == INVALID_HANDLE_VALUE ) {
		LOG_DEBUG( _T("create file failed.\n") );
		return NULL;
	}
	size_t file_size = GetFileSize(hFileComplex, NULL);
	if(offset > file_size) return NULL;
	verify_length = file_size - offset;

	if (POW2N_MOD(verify_length, BLOCK_SIZE)) complex_count = (verify_length / BLOCK_SIZE) + 1;
	else complex_count = verify_length / BLOCK_SIZE;

	psums_complex = (struct complex_sums*) malloc(sizeof (struct complex_sums));
	if (!psums_complex) return NULL;
	psums_complex->sums = (struct complex_sum*) malloc(sizeof (struct complex_sum) * complex_count);
	if (!psums_complex->sums) return NULL;
	psums_complex->count = complex_count;
	off_t position;
	for (int index = 0; index < complex_count; index++) {
		psums_complex->sums[index].offset = index*BLOCK_SIZE;
		position = offset + index*BLOCK_SIZE;
		complex_csum(&(psums_complex->sums[index]), hFileComplex, file_size, position);
	}

	if(hFileComplex != INVALID_HANDLE_VALUE) {
		CloseHandle( hFileComplex );
		hFileComplex = INVALID_HANDLE_VALUE;
	}

	return psums_complex;
}

struct complex_sums *BuildChunkComplexCsum(TCHAR *file_name, size_t offset) {
	int complex_count;
	struct complex_sums *psums_complex;
	size_t verify_length;

	HANDLE hFileComplex = CreateFile( file_name, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFileComplex == INVALID_HANDLE_VALUE ) {
		LOG_DEBUG( _T("create file failed.\n") );
		return NULL;
	} 
	size_t file_size = GetFileSize(hFileComplex, NULL);

	if(offset > file_size) return NULL;
	if ((offset + CHUNK_SIZE) > file_size) verify_length = file_size - offset;   
	else verify_length = CHUNK_SIZE;

	if (POW2N_MOD(verify_length, BLOCK_SIZE)) complex_count = (verify_length / BLOCK_SIZE) + 1;
	else complex_count = verify_length / BLOCK_SIZE;

	psums_complex = (struct complex_sums*) malloc(sizeof (struct complex_sums));
	if (!psums_complex) return NULL;
	psums_complex->sums = (struct complex_sum*) malloc(sizeof (struct complex_sum) * complex_count);
	if (!psums_complex->sums) return NULL;
	psums_complex->count = complex_count;

	off_t position;
	for (int index = 0; index < complex_count; index++) {
		psums_complex->sums[index].offset = index*BLOCK_SIZE;
		position = offset + index*BLOCK_SIZE;
		complex_csum(&(psums_complex->sums[index]), hFileComplex, file_size, position);
	}

	if(hFileComplex != INVALID_HANDLE_VALUE) {
		CloseHandle( hFileComplex );
		hFileComplex = INVALID_HANDLE_VALUE;
	}

	return psums_complex;
}


int FileCsumMD5(TCHAR *file_name, unsigned char *psum) { // return 1 exception; 0 succ ; -1 error
	DWORD dwFileSize, dwFileHigh;
	BYTE *sFileContent;

	HANDLE hFile = CreateFile( file_name, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFile == INVALID_HANDLE_VALUE ) {
		LOG_DEBUG( _T("create file failed.\n") );
		return -1;
	} 
	dwFileSize = GetFileSize(hFile, &dwFileHigh);
	if(INVALID_FILE_SIZE == dwFileSize) {
		CloseHandle( hFile );
		return 1;
	}
	HANDLE hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwFileSize, NULL);
	if(hMapFile == NULL) {
		LOG_DEBUG( _T("create file mapping failed.\n") );
		CloseHandle( hFile );
		return -1;
	} 
	sFileContent = (BYTE *)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, dwFileSize);
	if(!sFileContent) return -1; 	
	get_md5((char *)sFileContent, dwFileSize, (unsigned char *)psum);

	UnmapViewOfFile(sFileContent);
	CloseHandle( hMapFile );
	CloseHandle( hFile );

	return 0;
}



void simple_csum(unsigned char *psum, HANDLE hFile, off_t file_size, off_t offset) {
	struct tiny_map *buf;
	off_t i, offset_end;
	MD5_CTX md;
	int remainder;

	memset(psum, '\0', DIGEST_LENGTH);
	buf = (struct tiny_map *) map_file(hFile, file_size, MAX_MAP_SIZE, CSUM_CHUNK);
	offset_end = offset + CHUNK_SIZE;
	if (offset_end > file_size) offset_end = file_size;

	MD5Init(&md);
	for (i = offset; i + CSUM_CHUNK <= offset_end; i += CSUM_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, CSUM_CHUNK), CSUM_CHUNK);
	}
	remainder = (int) (offset_end - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(psum, md.digest, DIGEST_LENGTH);

	unmap_file(buf);
}

void complex_csum(struct complex_sum *psum, HANDLE hFile, off_t file_size, off_t offset) {
	struct tiny_map *buf;
	int block_true_size;

	buf = (struct tiny_map *) map_file(hFile, file_size, MAX_MAP_SIZE, BLOCK_SIZE);
	block_true_size = BLOCK_SIZE;
	if (offset+BLOCK_SIZE > file_size) block_true_size = file_size - offset;

	psum->adler32_sum = get_adler32_csum((char *) map_ptr(buf, offset, block_true_size), block_true_size);
	get_csum_md5((char *) map_ptr(buf, offset, block_true_size), block_true_size, psum->md5_sum);
	psum->offset = offset;

	unmap_file(buf);
}

int ChunkCsum(unsigned char *psum, HANDLE hChunkFile, off_t file_size) {
	struct tiny_map *buf;
	off_t i, offset_end;
	MD5_CTX md;
	int remainder;

	LOG_DEBUG(_T("chunk check sum file size:%d"), file_size);
	memset(psum, '\0', DIGEST_LENGTH);
	buf = (struct tiny_map *) map_file(hChunkFile, file_size, MAX_MAP_SIZE, CSUM_CHUNK);
	offset_end = file_size;

	MD5Init(&md);
	for (i = 0; i + CSUM_CHUNK <= offset_end; i += CSUM_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, CSUM_CHUNK), CSUM_CHUNK);
	}
	remainder = (int) (offset_end - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(psum, md.digest, DIGEST_LENGTH);

	unmap_file(buf);
	return 0;
}

int ChunkCsumExt(unsigned char *psum, HANDLE hChunkFile, off_t file_size, off_t offset) {
	struct tiny_map *buf;
	off_t i, offset_end;
	MD5_CTX md;
	int remainder;

	if(offset > file_size) return -1;
	LOG_DEBUG(_T("chunk check sum file size:%d offset:%d"), file_size, offset);

	memset(psum, '\0', DIGEST_LENGTH);
	buf = (struct tiny_map *) map_file(hChunkFile, file_size, MAX_MAP_SIZE, CSUM_CHUNK);
	offset_end = offset + CHUNK_SIZE;
	if (offset_end > file_size) offset_end = file_size;

	MD5Init(&md);
	for (i = offset; i + CSUM_CHUNK <= offset_end; i += CSUM_CHUNK) {
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, CSUM_CHUNK), CSUM_CHUNK);
	}
	remainder = (int) (offset_end - i);
	if (remainder > 0)
		MD5Update(&md, (unsigned char *) map_ptr(buf, i, remainder), remainder);
	MD5Final(&md);
	memcpy(psum, md.digest, DIGEST_LENGTH);

	unmap_file(buf);
	return 0;
}
