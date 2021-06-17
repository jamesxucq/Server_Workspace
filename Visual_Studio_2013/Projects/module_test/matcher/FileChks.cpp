#include "stdafx.h"

// #include "third_party.h"
// #include "FileUtility.h"
#include "tiny_map64.h"
#include "FileChks.h"

//
unsigned char *FileChks::BuildFileChks(TCHAR *szFileName, unsigned __int64 offset) {
    if (!szFileName) return NULL;
    //
    unsigned char *md5_chks = (unsigned char *) malloc(MD5_DIGEST_LEN);
    if (!md5_chks) return NULL;
    chksm::file_chks(md5_chks, szFileName, offset);
    //
    return md5_chks;
}

struct complex_head *FileChks::BuildFileComplexChks(DWORD *bchksValue, TCHAR *szFileName, unsigned __int64 offset) {
    unsigned int complex_tatol;
    struct complex_head *phead_complex;
    //
    HANDLE hFileComplex = CreateFile( szFileName,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
    if( INVALID_HANDLE_VALUE == hFileComplex ) {
// _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			*bchksValue = BCHKS_NOT_FOUND;
		} else *bchksValue = BCHKS_VIOLATION;
        return NULL;
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileComplex, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    if(qwFileSize < offset) return NULL;
    struct tiny_map64 *buffer = (struct tiny_map64 *) map_file64(hFileComplex, qwFileSize, MAX_MAP_SIZE, BLOCK_SIZE);
    //
    unsigned __int64 vleng = qwFileSize - offset;
	unsigned int whole_block = (unsigned int)(vleng / BLOCK_SIZE);
    complex_tatol = whole_block;
	if(POW2N_MOD(vleng, BLOCK_SIZE)) ++complex_tatol;
    //
    phead_complex = (struct complex_head*) malloc(sizeof (struct complex_head));
	if (!phead_complex) return NULL;
	phead_complex->chks = (struct complex_chks*) malloc(sizeof (struct complex_chks) * complex_tatol);
	if (!phead_complex->chks) {
		free(phead_complex);
		return NULL;
    }
    phead_complex->tatol = complex_tatol;
	unsigned int inde;
    unsigned __int64 posit;
    for (inde = 0; inde < whole_block; inde++) {
        phead_complex->chks[inde].offset = inde*BLOCK_SIZE;
        posit = offset + inde*BLOCK_SIZE;
        complex_chks(&(phead_complex->chks[inde]), buffer, posit);
    }
	if (POW2N_MOD(vleng, BLOCK_SIZE)) {
        phead_complex->chks[inde].offset = inde*BLOCK_SIZE;
        posit = offset + inde*BLOCK_SIZE;
        complex_leng_chks(&(phead_complex->chks[inde]), buffer, posit, POW2N_MOD(vleng, BLOCK_SIZE));
	}
    //
    unmap_file64(buffer);
    if(INVALID_HANDLE_VALUE != hFileComplex) {
        CloseHandle( hFileComplex );
        hFileComplex = INVALID_HANDLE_VALUE;
    }
    //
    return phead_complex;
}

struct complex_head *FileChks::BuildChunkComplexChks(DWORD *bchksValue, TCHAR *szFileName, unsigned __int64 offset) {
    unsigned int complex_tatol;
    struct complex_head *phead_complex;
    //
    HANDLE hFileComplex = CreateFile( szFileName,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
    if( INVALID_HANDLE_VALUE == hFileComplex ) {
// _LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			*bchksValue = BCHKS_NOT_FOUND;
		} else *bchksValue = BCHKS_VIOLATION;
        return NULL;
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileComplex, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    if(qwFileSize < (offset + CHUNK_SIZE)) return NULL;
    struct tiny_map64 *buffer = (struct tiny_map64 *) map_file64(hFileComplex, qwFileSize, MAX_MAP_SIZE, BLOCK_SIZE);
    //
    complex_tatol = CHUNK_SIZE / BLOCK_SIZE;
    phead_complex = (struct complex_head*) malloc(sizeof (struct complex_head));
    if (!phead_complex) return NULL;
    phead_complex->chks = (struct complex_chks*) malloc(sizeof (struct complex_chks) * complex_tatol);
    if (!phead_complex->chks) {
        free(phead_complex);
        return NULL;
    }
    phead_complex->tatol = complex_tatol;
	unsigned int inde;
    unsigned __int64 posit;
    for (inde = 0; inde < complex_tatol; inde++) {
        phead_complex->chks[inde].offset = inde*BLOCK_SIZE;
        posit = offset + inde*BLOCK_SIZE;
        complex_chks(&(phead_complex->chks[inde]), buffer, posit);
    }
    //
    unmap_file64(buffer);
    if(INVALID_HANDLE_VALUE != hFileComplex) {
        CloseHandle( hFileComplex );
        hFileComplex = INVALID_HANDLE_VALUE;
    }
    //
    return phead_complex;
}

unsigned char *FileChks::FileChksMD5(TCHAR *szFileName, unsigned char *md5_chks) {  // return 1 exception; 0 succ ; -1 error
    HANDLE hFile;
    struct tiny_map64 *buffer;
    unsigned __int64 inde, qwFileSize;
    DWORD dwFileSizeHigh;
    MD5_CTX md;
    int remainder;
    //
    memset(md5_chks, '\0', MD5_DIGEST_LEN);
    hFile = CreateFile( szFileName,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);
    if( INVALID_HANDLE_VALUE == hFile ) {
// _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return NULL;
    }
    //
    qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    buffer = (struct tiny_map64 *) map_file64(hFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = 0; inde + CHKS_CHUNK <= qwFileSize; inde += CHKS_CHUNK) {
        MD5Update(&md, map_ptr64(buffer, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int) (qwFileSize - inde);
    if (remainder > 0)
        MD5Update(&md, map_ptr64(buffer, inde, remainder), remainder);
    MD5Final(&md);
    memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
    //
    unmap_file64(buffer);
    if(INVALID_HANDLE_VALUE != hFile) {
        CloseHandle( hFile );
        hFile = INVALID_HANDLE_VALUE;
    }
    //
    return md5_chks;
}

unsigned char *FileChks::HandleChksMD5(HANDLE hChksFile, unsigned char *md5_chks) {
    struct tiny_map64 *buffer;
    unsigned __int64 inde, qwFileSize;
    DWORD dwFileSizeHigh;
    MD5_CTX md;
    int remainder;
    //
    memset(md5_chks, '\0', MD5_DIGEST_LEN);
    if( INVALID_HANDLE_VALUE == hChksFile ) return NULL;
    //
    qwFileSize = GetFileSize(hChksFile, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    buffer = (struct tiny_map64 *) map_file64(hChksFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
    MD5Init(&md);
    for (inde = 0; inde + CHKS_CHUNK <= qwFileSize; inde += CHKS_CHUNK) {
        MD5Update(&md, map_ptr64(buffer, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int) (qwFileSize - inde);
    if (remainder > 0)
        MD5Update(&md, map_ptr64(buffer, inde, remainder), remainder);
    MD5Final(&md);
    memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
    //
    unmap_file64(buffer);
    return md5_chks;
}

void complex_chks(struct complex_chks *pchks, struct tiny_map64 *buffer, unsigned __int64 offset) {
	unsigned int adler32_chk = get_adler32_chksum(0, NULL, 0);
    pchks->adler32_chk = get_adler32_chksum(adler32_chk, map_ptr64(buffer, offset, BLOCK_SIZE), BLOCK_SIZE);
    chksm::get_chksum_md5(map_ptr64(buffer, offset, BLOCK_SIZE), BLOCK_SIZE, pchks->md5_chks);
    // pchks->offset = offset;
    //
}

void complex_leng_chks(struct complex_chks *pchks, struct tiny_map64 *buffer, unsigned __int64 offset, unsigned colen) {
	unsigned int adler32_chk = get_adler32_chksum(0, NULL, 0);
    pchks->adler32_chk = get_adler32_chksum(adler32_chk, map_ptr64(buffer, offset, colen), colen);
    chksm::get_chksum_md5(map_ptr64(buffer, offset, colen), colen, pchks->md5_chks);
    // pchks->offset = offset;
}

int FileChks::ChunkChksExt(unsigned char *md5_chks, HANDLE hChunkFile, unsigned __int64 qwFileSize, unsigned __int64 offset) {
    struct tiny_map64 *buffer;
    unsigned __int64 inde, offset_end;
    MD5_CTX md;
    int remainder;
    //
    if(offset > qwFileSize) return -1;
// _LOG_DEBUG(_T("chunk check sum file size:%d offset:%d"), qwFileSize, offset); // disable by james 20140410
    memset(md5_chks, '\0', MD5_DIGEST_LEN);
    buffer = (struct tiny_map64 *) map_file64(hChunkFile, qwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
    offset_end = offset + CHUNK_SIZE;
    if (offset_end > qwFileSize) offset_end = qwFileSize;
    //
    MD5Init(&md);
    for (inde = offset; inde + CHKS_CHUNK <= offset_end; inde += CHKS_CHUNK) {
        MD5Update(&md, map_ptr64(buffer, inde, CHKS_CHUNK), CHKS_CHUNK);
    }
    remainder = (int) (offset_end - inde);
    if (remainder > 0)
        MD5Update(&md, map_ptr64(buffer, inde, remainder), remainder);
    MD5Final(&md);
    memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
    //
    unmap_file64(buffer);
    return 0;
}
