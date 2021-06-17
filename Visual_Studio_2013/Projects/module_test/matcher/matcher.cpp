// matcher.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

#include "tiny_map64.h"
#include "Checksum.h"
#include "FileChks.h"
#include "MatchUtility.h"

#include "BuildUtility.h"

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
wprintf( _T("create file failed: %d\n"), GetLastError() );
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
struct local_match *AppendLocalMatch(vector<struct local_match *> &vlocal_match, struct file_matcher *matcher) {
    struct file_matcher *new_mcher = (struct file_matcher *)malloc(sizeof(struct file_matcher));
    if(!new_mcher) return NULL;
    memcpy(new_mcher, matcher, sizeof(struct file_matcher));
    //
    struct local_match *alloc_match = (struct local_match *)malloc(sizeof(struct local_match));
    if(!alloc_match) {
        free(new_mcher);
        return NULL;
    }
    vlocal_match.push_back(alloc_match);
    //
    alloc_match->matcher = new_mcher;
    alloc_match->buffer = NULL;
    //
    return alloc_match;
}

//
VOID ClosePutChksRecvSe(vector<struct local_match *> &vlocal_match, HANDLE hFileFlush, DWORD content_length) {
    // struct InteValue *ival = &(m_pTRANSSeion->ivalue);
    // struct SeionCtrl *sctrl = &(m_pTRANSSeion->control);
    struct file_matcher matcher;
    DWORD dwReadSize = 0x00;
    size_t inde, match_tatol;
    //
// _LOG_DEBUG(_T("FileSizePoint(sctrl->file_handle):%d"), NFileUtility::FileSizeHandle(sctrl->file_handle)); // disable by james 20130410
    DWORD dwResult = SetFilePointer(hFileFlush, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return;
    //
    for(inde = 0; inde < content_length; inde += match_tatol) {
        match_tatol = sizeof(struct file_matcher);
        if(!ReadFile(hFileFlush, &matcher, sizeof(struct file_matcher), &dwReadSize, NULL) || !dwReadSize) {
            // if(ERROR_HANDLE_EOF != GetLastError()) return ((DWORD)-1);
            break;
        }
// _LOG_DEBUG(_T("match_type:%d offset:%u inde_len:%u"), matcher.match_type, matcher.offset, matcher.inde_len); // disable by james 20130410
        struct local_match *alloc_local = AppendLocalMatch(vlocal_match, &matcher);
        if (CONEN_MODI & matcher.match_type) {
            alloc_local->buffer = (unsigned char *) malloc(matcher.inde_len);
			if(!alloc_local->buffer) {
wprintf(_T("alloc_local->buffer malloc error!\n"));
				break;
			}
            if(!ReadFile(hFileFlush, alloc_local->buffer, matcher.inde_len, &dwReadSize, NULL) || !dwReadSize) {
                // if(ERROR_HANDLE_EOF != GetLastError()) return ((DWORD)-1);
                break;
            }
            match_tatol += matcher.inde_len;
        }
    }
    //
    CloseHandle(hFileFlush);
    hFileFlush = INVALID_HANDLE_VALUE;
    //
    // memset(sctrl, '\0', sizeof(struct SeionCtrl));
}

//
HANDLE BuildCacheFile(const TCHAR *szFileName) {
    static TCHAR szCacheDirectory[MAX_PATH];
    TCHAR szBuildFile[MAX_PATH];
    //
    if(_T(':') != szCacheDirectory[1]) {
        if(!GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH))
            return INVALID_HANDLE_VALUE;
        _tcscat_s(szCacheDirectory, _T("\\sdclient"));
        CreateDirectory(szCacheDirectory, NULL);
    }
    _stprintf_s(szBuildFile, MAX_PATH, _T("%s\\%s"), szCacheDirectory, szFileName);
    //
    HANDLE hFileCache = CreateFile( szBuildFile,
                                    GENERIC_WRITE | GENERIC_READ,
                                    NULL, /* FILE_SHARE_READ */
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileCache ) {
        wprintf( _T("create file failed: %d\n"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    //
    return hFileCache;
}

unsigned __int64 FileSizeHandle(HANDLE hFileHand) {
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileHand, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    return qwFileSize;
}

TCHAR *GetFileFolderPath(TCHAR *szDirePath, const TCHAR *szFileName) {
    TCHAR *toke = NULL;
    //
    if(!szFileName || !szDirePath) return NULL;
    MKZEO(szDirePath);
    _tcscpy_s(szDirePath, MAX_PATH, szFileName);
    toke = _tcsrchr(szDirePath, _T('\\'));
    if(toke) *toke = _T('\0');
    //
    return szDirePath;
}

BOOL FolderExist(const TCHAR *szFolderPath) {
    WIN32_FIND_DATA wfd;
    BOOL bFound = FALSE;
    //
    HANDLE hFind = FindFirstFile(szFolderPath, &wfd);
    if ((INVALID_HANDLE_VALUE!=hFind) && (wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) bFound = TRUE;
    //
    FindClose(hFind);
    return bFound;
}

BOOL RecursMakeFolder(LPCTSTR szDirePath) {
    TCHAR szParent[MAX_PATH];
    //
    if (FolderExist(szDirePath) || CreateDirectory(szDirePath, NULL)) 
		return TRUE;
    GetFileFolderPath(szParent, szDirePath);
    if (!RecursMakeFolder(szParent) || !CreateDirectory(szDirePath, NULL))
        return FALSE;
    //
    return TRUE;
}

//  Returns: bool
//		true if csPath is a path to a directory
//		false otherwise.
bool IsDirectory(const TCHAR *szFolderPath) {
    DWORD dwAttrib = GetFileAttributes( szFolderPath );
    return static_cast<bool>((INVALID_FILE_ATTRIBUTES!=dwAttrib)
                             && (FILE_ATTRIBUTE_DIRECTORY&dwAttrib));
}

BOOL MakeFileFolder(const TCHAR *szFileName) {
    TCHAR szDirePath[MAX_PATH];
    //
    if(!szFileName) return FALSE;
    if(!GetFileFolderPath(szDirePath, szFileName)) return FALSE;
    //
    // Fix windows API FolderExist bug 20130226
    if(0x02 == _tcslen(szDirePath)) {
        if(IsDirectory(szDirePath)) return TRUE;
		else {
			return FALSE;
		}
    }
    //
    return RecursMakeFolder(szDirePath);
}

//
#define COPY_BUFFER	0x2000  // 8K
DWORD HandleOwriteModified(TCHAR *builder_cache, const TCHAR *szFileName, unsigned __int64 offset) {
    HANDLE	hBuilderCache = INVALID_HANDLE_VALUE;
    HANDLE	hTo = INVALID_HANDLE_VALUE;
    DWORD dwWriteValue = 0x00;

//    if(ONLY_TRANSMIT_VERIFY_OKAY(task_node->success_flags) || !ONLY_OWRTMRVE_OKAY(task_node->success_flags))
//        return OWRITE_STATUS_FAULT;
    //
    // open source and desti file
    hBuilderCache = NTRANSCache::OpenBuilder(builder_cache);
    if( INVALID_HANDLE_VALUE == hBuilderCache ) {
wprintf( _T("overwrite open file failed. file name is %s."), builder_cache);
        return -1;
    }
//  struti::full_path(szFileName, szDriveLetter, task_node->szFileName1);
// _LOG_DEBUG(_T("szFileName:%s szDriveLetter:%s task_node->szFileName1:%s"), szFileName, szDriveLetter, task_node->szFileName1);
    MakeFileFolder(szFileName);
    hTo = CreateFile( szFileName,
                      GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE, // | FILE_SHARE_DELETE,
                      NULL,
                      OPEN_ALWAYS,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL);
    if( INVALID_HANDLE_VALUE == hTo ) {
wprintf( _T("overwrite create file failed. file name is %s."), szFileName);
        return -1;
    }
    // lock on the opened file
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
    OverLapped.Offset = offset & 0xffffffff;
    OverLapped.OffsetHigh = offset >> 32;
    if(!LockFileEx(hTo,
                   LOCKFILE_EXCLUSIVE_LOCK,
                   0,
                   CHUNK_SIZE,
                   0x00000000,
                   &OverLapped)) {
wprintf(_T("overwrite lock file %s fail!"), szFileName);
        return -1;
    }
    long lFileSizeHigh = offset >> 32;
    DWORD dwResult = SetFilePointer(hTo, offset&0xffffffff, &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
wprintf(_T("overwrite set file pointer fail!"));
        dwWriteValue = -1;
    }
    // overwrite desti file
    DWORD dwWritenSize = 0x00;
    DWORD dwReadSize = 0x00;
    CHAR szBuffer[COPY_BUFFER];

    while(ReadFile(hBuilderCache, szBuffer, COPY_BUFFER, &dwReadSize, NULL) && 0<dwReadSize) {
        WriteFile(hTo, szBuffer, dwReadSize, &dwWritenSize, NULL);
        if(dwReadSize != dwWritenSize) {
wprintf(_T("fatal write error: %d"), GetLastError());
            dwWriteValue = -1;
        }
    }
    // close source and desti file
    // unlock on the opened file
    UnlockFileEx(hTo, 0, CHUNK_SIZE, 0x00000000, &OverLapped);
    CloseHandle(hTo);
    NTRANSCache::DestroyBuilder(hBuilderCache, builder_cache);
//
    return dwWriteValue;
}

//
#define BLOCK_INDEX	1
#define LOCAL_FILE _T("E:\\9 Aretha Franklin.dat")
#define SERVER_FILE _T("F:\\9 Aretha Franklin.dat")

#define FOLDER_RECURS_DEFAULT   _T("folder_recurs.tmp")

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hChksFile;
	hChksFile = CreateFile( LOCAL_FILE, 
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
	HANDLE hFileFlush = BuildCacheFile(FOLDER_RECURS_DEFAULT); 
	//
	unsigned __int64 offset = ((unsigned __int64)BLOCK_INDEX<<22);
	if(qwFileSize >= (offset+CHUNK_SIZE)) {
		struct complex_head *phead_complex = BuildChunkComplexChks(&bchksValue, LOCAL_FILE, offset);	
		if(phead_complex) MUtil::BlockMatchChunk(hFileFlush, phead_complex, offset, SERVER_FILE);
		else wprintf( _T("chks error!\n") );
	} else {
		struct complex_head *phead_complex = BuildFileComplexChks(&bchksValue, LOCAL_FILE, offset);
		if(phead_complex) MUtil::BlockMatchFile(hFileFlush, phead_complex, offset, SERVER_FILE);
		else wprintf( _T("chks error!\n") );
	}
//
	DWORD content_length = (size_t)FileSizeHandle(hFileFlush);
	vector<struct local_match *> vlocal_match;
	ClosePutChksRecvSe(vlocal_match, hFileFlush, content_length);
//
wprintf(_T("put sync build local data section.\n")); // disable by james 20140410
	struct FileBuilder builder;
	_tcscpy_s(builder.file_name, LOCAL_FILE);
	builder.build_type = RANGE_TYPE_CHUNK;
	builder.offset = offset;
	builder.length = CHUNK_SIZE; // ival->content_length;
	_tcscpy_s(builder.builder_cache, _T("build_cache.xxx"));
wprintf(_T("builder :%08X :%s :%llu :%u :%s\n"), builder.build_type, builder.file_name, builder.offset, builder.length, builder.builder_cache);
	DWORD buildValue = NBuildUtility::ContentBuilder(&builder, &vlocal_match);
wprintf(_T("put sync build local data OK. buildValue:%08X\n"), buildValue); // disable by james 20140410
	// MUtil::DeleMatchVec(&vlocal_match);
	// set last_write at overwrite and finish trans
wprintf(_T("put sync ret okay, buildValue:%08X\n"), buildValue);
//
	HandleOwriteModified(_T("build_cache.xxx"), LOCAL_FILE, (unsigned __int64)BLOCK_INDEX<<22);
	//
	return 0;
}

// 4186114
// 4194304
// 8190