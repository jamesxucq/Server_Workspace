#include "stdafx.h"

#include "third_party.h"
#include "FileUtility.h"
#include "FileChks.h"

//
void complex_leng_chks(struct complex_chks *pchks, struct tiny_map64 *buffer, unsigned __int64 offset, unsigned colen);
void complex_chks(struct complex_chks *pcchks, struct tiny_map64 *buffer, unsigned __int64 offset);

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

DWORD FileChks::BuildFileComplexChks(struct complex_head **phead_compl, TCHAR *szFileName, unsigned __int64 offset) {
    unsigned int compl_tatol;
    struct complex_head *phcom;
    //
    HANDLE hFileCompl = CreateFile( szFileName,
                                      GENERIC_READ,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
    if( INVALID_HANDLE_VALUE == hFileCompl ) {
_LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError())
			return BCHKS_NOT_FOUND;
		return BCHKS_VIOLATION;
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileCompl, &dwFileSizeHigh);
	if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError()) {
_LOG_WARN( _T("get file size failed"));
		CloseHandle( hFileCompl );    
		return BCHKS_FAULT;
	}
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
	if(qwFileSize <= offset) {
_LOG_WARN( _T("file offset error.qwFileSize:%I64u offset:%I64u"), qwFileSize, offset);
		CloseHandle( hFileCompl ); 
		*phead_compl = (struct complex_head*) malloc(sizeof (struct complex_head));
		memset(*phead_compl, '\0', sizeof(struct complex_head));
		return BCHKS_SUCCESS;
	}
    struct tiny_map64 *buffer = (struct tiny_map64 *) map_file64(hFileCompl, qwFileSize, MAX_MAP_SIZE, BLOCK_SIZE);
    //
    unsigned __int64 vleng = qwFileSize - offset;
	unsigned int whole_block = (unsigned int)(vleng / BLOCK_SIZE);
    compl_tatol = whole_block;
	if(POW2N_MOD(vleng, BLOCK_SIZE)) ++compl_tatol;
    //
    *phead_compl = phcom = (struct complex_head*) malloc(sizeof (struct complex_head));
	if (!phcom) {
_LOG_WARN( _T("malloc complex failed."));
		CloseHandle( hFileCompl );
		return BCHKS_FAULT;
	}
	phcom->chks = (struct complex_chks*) malloc(sizeof (struct complex_chks) * compl_tatol);
	if (!phcom->chks) {
_LOG_WARN( _T("malloc chks failed."));
		free(phcom);
		CloseHandle( hFileCompl );
		return BCHKS_FAULT;
    }
    phcom->tatol = compl_tatol;
	unsigned int inde;
    unsigned __int64 posit;
    for (inde = 0; inde < whole_block; inde++) {
        phcom->chks[inde].offset = inde*BLOCK_SIZE;
        posit = offset + inde*BLOCK_SIZE;
        complex_chks(&(phcom->chks[inde]), buffer, posit);
    }
	if (POW2N_MOD(vleng, BLOCK_SIZE)) {
        phcom->chks[inde].offset = inde*BLOCK_SIZE;
        posit = offset + inde*BLOCK_SIZE;
        complex_leng_chks(&(phcom->chks[inde]), buffer, posit, POW2N_MOD(vleng, BLOCK_SIZE));
	}
    //
    unmap_file64(buffer);
    if(INVALID_HANDLE_VALUE != hFileCompl) {
        CloseHandle( hFileCompl );
        hFileCompl = INVALID_HANDLE_VALUE;
    }
    //
    return BCHKS_SUCCESS;
}

DWORD FileChks::BuildChunkComplexChks(struct complex_head **phead_compl, TCHAR *szFileName, unsigned __int64 offset) {
    unsigned int compl_tatol;
    struct complex_head *phcom;
    //
    HANDLE hFileCompl = CreateFile( szFileName,
                                      GENERIC_READ,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
    if( INVALID_HANDLE_VALUE == hFileCompl ) {
_LOG_WARN( _T("create file failed: %d"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError())
			return BCHKS_NOT_FOUND;
		return BCHKS_VIOLATION;
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileCompl, &dwFileSizeHigh);
	if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError()) {
_LOG_WARN( _T("get file size failed"));
		CloseHandle( hFileCompl );
		return BCHKS_FAULT;
	}
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
	if(qwFileSize < (offset + CHUNK_SIZE)) {
_LOG_WARN( _T("file offset error.qwFileSize:%I64u offset:%I64u"), qwFileSize, offset);
		CloseHandle( hFileCompl );
		*phead_compl = (struct complex_head*) malloc(sizeof (struct complex_head));
		memset(*phead_compl, '\0', sizeof(struct complex_head));
		return BCHKS_SUCCESS;
	}
    struct tiny_map64 *buffer = (struct tiny_map64 *) map_file64(hFileCompl, qwFileSize, MAX_MAP_SIZE, BLOCK_SIZE);
    //
    compl_tatol = CHUNK_SIZE / BLOCK_SIZE;
    *phead_compl = phcom = (struct complex_head*) malloc(sizeof (struct complex_head));
	if (!phcom) {
_LOG_WARN( _T("malloc complex failed."));
		CloseHandle( hFileCompl );
		return BCHKS_FAULT;
	}
    phcom->chks = (struct complex_chks*) malloc(sizeof (struct complex_chks) * compl_tatol);
    if (!phcom->chks) {
_LOG_WARN( _T("malloc chks failed."));
        free(phcom);
		CloseHandle( hFileCompl );
        return BCHKS_FAULT;
    }
    phcom->tatol = compl_tatol;
	unsigned int inde;
    unsigned __int64 posit;
    for (inde = 0; inde < compl_tatol; inde++) {
        phcom->chks[inde].offset = inde*BLOCK_SIZE;
        posit = offset + inde*BLOCK_SIZE;
        complex_chks(&(phcom->chks[inde]), buffer, posit);
    }
    //
    unmap_file64(buffer);
    if(INVALID_HANDLE_VALUE != hFileCompl) {
        CloseHandle( hFileCompl );
        hFileCompl = INVALID_HANDLE_VALUE;
    }
    //
    return BCHKS_SUCCESS;
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
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return NULL;
    }
    //
    qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError()) {
		CloseHandle( hFile );
		return NULL;
	}
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
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError())
        return NULL;
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

int FileChks::ChunkChks(unsigned char *md5_chks, HANDLE hChunkFile, unsigned int dwFileSize) {
    struct tiny_map *buffer;
    size_t inde, offset_end;
    MD5_CTX md;
    int remainder;
// _LOG_DEBUG(_T("chunk check sum file size:%d"), dwFileSize); // disable by james 20140410
	if(dwFileSize) {
		buffer = (struct tiny_map *) map_file(hChunkFile, dwFileSize, MAX_MAP_SIZE, CHKS_CHUNK);
		offset_end = dwFileSize;
		//
		MD5Init(&md);
		for (inde = 0; inde + CHKS_CHUNK <= offset_end; inde += CHKS_CHUNK) {
			MD5Update(&md, (unsigned char *) map_ptr(buffer, inde, CHKS_CHUNK), CHKS_CHUNK);
		}
		remainder = (int) (offset_end - inde);
		if (remainder > 0)
			MD5Update(&md, (unsigned char *) map_ptr(buffer, inde, remainder), remainder);
		MD5Final(&md);
		memcpy(md5_chks, md.digest, MD5_DIGEST_LEN);
		//
		unmap_file(buffer);	
	} else memset(md5_chks, '\0', MD5_DIGEST_LEN);
//
    return 0;
}

int FileChks::ChunkChksEx(unsigned char *md5_chks, HANDLE hChunkFile, unsigned __int64 qwFileSize, unsigned __int64 offset) {
    struct tiny_map64 *buffer;
    unsigned __int64 inde, offset_end;
    MD5_CTX md;
    int remainder;
    //
    if(qwFileSize < offset) return -1;
// _LOG_DEBUG(_T("chunk check sum file size:%d offset:%d"), qwFileSize, offset); // disable by james 20140410
	if(qwFileSize > offset) {
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
	} else memset(md5_chks, '\0', MD5_DIGEST_LEN);
//
    return 0;
}
