// adlmd5.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

#include "tiny_map64.h"
#include "Checksum.h"

//
#define MD5_DIGEST_LEN 16

#define BCHKS_FAULT			((DWORD)-1)
#define BCHKS_SUCCESS		0x00
#define BCHKS_NOT_FOUND		0x01
#define BCHKS_VIOLATION		0x02

//
#define BLOCK_SIZE						0x2000  // 8K
#define CHUNK_SIZE						((int)1 << 22) // 4M

#define MAX_MAP_SIZE 0x80000 // 512K

//
struct complex_head {
	unsigned int tatol;
	struct complex_chks *chks;
};

#pragma	pack(1)

struct complex_chks {
	unsigned int offset; // posit in chunk
	DWORD adler32_chk;
	unsigned char md5_chks[MD5_DIGEST_LEN];
};

#pragma	pack()


//
void complex_leng_chks(struct complex_chks *pchks, struct tiny_map64 *buffer, unsigned __int64 offset, unsigned colen) {
	unsigned int adler32_chk = get_adler32_chksum(0, NULL, 0);
    pchks->adler32_chk = get_adler32_chksum(adler32_chk, map_ptr64(buffer, offset, colen), colen);
    chksm::get_chksum_md5(map_ptr64(buffer, offset, colen), colen, pchks->md5_chks);
    // pchks->offset = offset;
}

void complex_chks(struct complex_chks *pchks, struct tiny_map64 *buffer, unsigned __int64 offset) {
	unsigned int adler32_chk = get_adler32_chksum(0, NULL, 0);
    pchks->adler32_chk = get_adler32_chksum(adler32_chk, map_ptr64(buffer, offset, BLOCK_SIZE), BLOCK_SIZE);
    chksm::get_chksum_md5(map_ptr64(buffer, offset, BLOCK_SIZE), BLOCK_SIZE, pchks->md5_chks);
    // pchks->offset = offset;
    //
}

struct complex_head *BuildFileComplexChks(DWORD *bchksValue, TCHAR *szFileName, unsigned __int64 offset) {
    unsigned int complex_tatol;
    struct complex_head *phead_complex;
    //
    HANDLE hFileComplex = CreateFile( szFileName,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_ALWAYS,
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

struct complex_head *BuildChunkComplexChks(DWORD *bchksValue, TCHAR *szFileName, unsigned __int64 offset) {
    unsigned int complex_tatol;
    struct complex_head *phead_complex;
    //
    HANDLE hFileComplex = CreateFile( szFileName,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL);
    if( INVALID_HANDLE_VALUE == hFileComplex ) {
wprintf( _T("create file failed: %d"), GetLastError() );
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


//
#define BLOCK_INDEX	1

VOID ChksMD5(TCHAR *label, unsigned char *md5sum) {
    TCHAR chksum_str[64];
    for (int inde=0; inde<MD5_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, MD5_TEXT_LENGTH, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
	wprintf(_T("%s:%s\n"), label, chksum_str);
}

void printComplexChks(struct complex_head *phead_complex) {
	wprintf(_T("complex tatol:%d\n"), phead_complex->tatol);
wprintf(_T("------ GCHKS ------\n"));
	unsigned int inde;
	struct complex_chks *cchks = phead_complex->chks;
	for(inde=0; phead_complex->tatol > inde; inde++) {
wprintf(_T("inde:%d offset:%d adle32:%u "), inde, cchks->offset, cchks->adler32_chk);
		ChksMD5(_T("md5"), cchks->md5_chks);
		cchks++;
	}
}

//
#define SERVER_FILE _T("E:\\9 Aretha Franklin.dat")

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hChksFile;
	hChksFile = CreateFile( SERVER_FILE, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hChksFile == INVALID_HANDLE_VALUE ) {
wprintf( _T("create file failed.\n") );
wprintf(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 
//
	DWORD bchksValue;
	unsigned __int64 qwFileSize;
	DWORD dwFileSizeHigh;
    qwFileSize = GetFileSize(hChksFile, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
	CloseHandle( hChksFile );
	//
	if(qwFileSize >= ((unsigned __int64)BLOCK_INDEX<<22)+CHUNK_SIZE) {
		struct complex_head *phead_complex = BuildChunkComplexChks(&bchksValue, _T("F:\\7 American Authors.dat"), ((unsigned __int64)BLOCK_INDEX<<22));	
		if(phead_complex) printComplexChks(phead_complex);
		else wprintf( _T("chks error!\n") );
	} else {
		struct complex_head *phead_complex = BuildFileComplexChks(&bchksValue, _T("F:\\7 American Authors.dat"), ((unsigned __int64)BLOCK_INDEX<<22));
		if(phead_complex) printComplexChks(phead_complex);
		else wprintf( _T("chks error!\n") );
	}
//
	return 0;
}