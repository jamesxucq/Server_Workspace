#include "stdafx.h"

#include "common_macro.h"
// #include "client_macro.h"

// #include "FileUtility.h"
// #include "TRANSCache.h"

#include "BuildUtility.h"

#include <vector>
using std::vector;

//

TCHAR *GetFileBaseName(TCHAR *szBaseName, const TCHAR *szFileName, DWORD dwMaxCount)
{
    TCHAR szDupName[MAX_PATH];
    TCHAR *toke = NULL;
    //
    _tcscpy_s(szDupName, szFileName);
    toke = _tcsrchr(szDupName, _T('.'));
    if(toke) toke[0] = _T('\0');
    //
    toke = _tcsrchr(szDupName, _T('\\'));
    if(toke) _tcsncpy_s(szBaseName, MAX_PATH, ++toke, dwMaxCount);
    else _tcsncpy_s(szBaseName, MAX_PATH, szDupName, dwMaxCount);
    //
    return szBaseName;
}

TCHAR *NTRANSCache::CreatBuilderName(TCHAR *szBuilderName, const TCHAR *szFileName, size_t offset)
{
    static DWORD dwBlockCount;
    TCHAR szFileBase[MAX_PATH];
    //
    if(!GetFileBaseName(szFileBase, szFileName, 12)) return NULL;
    _stprintf_s(szBuilderName, MAX_PATH, _T("%s-%x-%x"), szFileBase, dwBlockCount++, offset);
// _LOG_DEBUG( _T("create cache name is: %s"), szBuilderName); // disable by james 20130410
    return szBuilderName;
}

// MFC临界区类对象
// CCriticalSection gcsBackupFile;
TCHAR *GetBuildFile(TCHAR *szBuildFile, const TCHAR *szBuilderName)
{
    static TCHAR szCacheDirectory[MAX_PATH];
    //
    // gcsBackupFile.Lock();
    if(_T(':') != szCacheDirectory[1]) {
        GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH);
        _tcscat_s(szCacheDirectory, _T("\\sdclient"));
        CreateDirectory(szCacheDirectory, NULL);
    }
    _stprintf_s(szBuildFile, MAX_PATH, _T("%s\\"), szCacheDirectory);
    //
    // gcsBackupFile.Unlock();
    _tcscat_s(szBuildFile, MAX_PATH, szBuilderName);
    //
    return szBuildFile;
}

HANDLE NTRANSCache::OpenBuilder(const TCHAR *szBuilderName)
{
    TCHAR szBuildFile[MAX_PATH];
    HANDLE	hBuilderCache = INVALID_HANDLE_VALUE;
    //
    hBuilderCache = CreateFile(GetBuildFile(szBuildFile, szBuilderName),
                               GENERIC_WRITE | GENERIC_READ,
                               NULL,
                               NULL,
                               OPEN_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if( INVALID_HANDLE_VALUE == hBuilderCache ) {
        wprintf( _T("create cache file failed, file name is: %s\n"), szBuildFile);
        wprintf( _T("GetLastError() is: %d\n"), GetLastError());
        return INVALID_HANDLE_VALUE;
    }
    //
    return hBuilderCache;
}

VOID NTRANSCache::CloseBuilder(HANDLE hBuilderCache)
{
    if(INVALID_HANDLE_VALUE != hBuilderCache)
        CloseHandle(hBuilderCache);
}

VOID NTRANSCache::DestroyBuilder(HANDLE hBuilderCache, const TCHAR *szBuilderName)
{
    TCHAR szBuildFile[MAX_PATH];
    //
    if(INVALID_HANDLE_VALUE != hBuilderCache) CloseHandle(hBuilderCache);
    DeleteFile(GetBuildFile(szBuildFile, szBuilderName));
}


//
//
int BuildFileContent(HANDLE hFileLocal, struct FileBuilder *builder, vector<struct local_match *> *local_vector) {
    unsigned char buffer[BLOCK_SIZE * 0x02];
    vector <struct local_match *>::iterator iter;
    struct file_matcher *matcher;
    unsigned __int64 qwFileOffset;
//
// _LOG_DEBUG(_T("build utility builder->builder_cache is:%s"), builder->builder_cache);
    HANDLE hFileBackup = NTRANSCache::OpenBuilder(builder->builder_cache);
    if(INVALID_HANDLE_VALUE == hFileBackup) return -1;
//
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD wlen, slen;
    for(iter = local_vector->begin(); local_vector->end() != iter; ++iter) {
        matcher = (*iter)->matcher;
        if(CHECK_SUMS & matcher->match_type) {
            qwFileOffset = matcher->inde_len*BLOCK_SIZE + builder->offset;
            OverLapped.Offset = qwFileOffset&0xffffffff;
            OverLapped.OffsetHigh = qwFileOffset >> 32;
            if(!ReadFile(hFileLocal, buffer, BLOCK_SIZE, &slen, &OverLapped) || !slen) {
                // if(ERROR_HANDLE_EOF != GetLastError())
                wprintf(_T("build read error!\n"));
                return -1;
            }
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hFileBackup, buffer, BLOCK_SIZE, &wlen, &OverLapped)) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
            if(wlen != BLOCK_SIZE) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
        } else if(CONEN_MODI & matcher->match_type) {
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hFileBackup, (*iter)->buffer, matcher->inde_len, &wlen, &OverLapped)) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
            if(wlen != matcher->inde_len) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
		} else if(CONEN_VARI & matcher->match_type) {
			unsigned int colen = matcher->inde_len&0x0000ffff;
            qwFileOffset = (matcher->inde_len >> 16)*BLOCK_SIZE + builder->offset;
            OverLapped.Offset = qwFileOffset&0xffffffff;
            OverLapped.OffsetHigh = qwFileOffset >> 32;
            if(!ReadFile(hFileLocal, buffer, colen, &slen, &OverLapped) || !slen) {
                // if(ERROR_HANDLE_EOF != GetLastError()) 
				wprintf(_T("build read error!\n"));
                return -1;
            }
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hFileBackup, buffer, colen, &wlen, &OverLapped)) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
            if(wlen != colen) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
		}
    }
    NTRANSCache::CloseBuilder(hFileBackup);
//
    return 0;
}


int BuildChunkContent(HANDLE hFileLocal, struct FileBuilder *builder, vector<struct local_match *> *local_vector) {
    unsigned char buffer[BLOCK_SIZE * 0x02];
    vector <struct local_match *>::iterator iter;
    struct file_matcher *matcher;
    unsigned __int64 qwFileOffset;
//
    HANDLE hChunkBackup = NTRANSCache::OpenBuilder(builder->builder_cache);
    if(INVALID_HANDLE_VALUE == hChunkBackup) return -1;
//
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD wlen, slen;
    for(iter = local_vector->begin(); local_vector->end() != iter; ++iter) {
        matcher = (*iter)->matcher;
        if(CHECK_SUMS & matcher->match_type) {
            qwFileOffset = matcher->inde_len*BLOCK_SIZE + builder->offset;
            OverLapped.Offset = qwFileOffset&0xffffffff;
            OverLapped.OffsetHigh = qwFileOffset >> 32;
            if(!ReadFile(hFileLocal, buffer, BLOCK_SIZE, &slen, &OverLapped) || !slen) {
                // if(ERROR_HANDLE_EOF != GetLastError())
                wprintf(_T("build read error!\n"));
                return -1;
            }
			//
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hChunkBackup, buffer, BLOCK_SIZE, &wlen, &OverLapped)) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
            if(wlen != BLOCK_SIZE) return -1;
        } else if(CONEN_MODI & matcher->match_type) {
            OverLapped.Offset = matcher->offset;
            OverLapped.OffsetHigh = 0x0000;
            if(!WriteFile(hChunkBackup, (*iter)->buffer, matcher->inde_len, &wlen, &OverLapped)) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
            if(wlen != matcher->inde_len) {
                wprintf(_T("build write error!\n"));
                return -1;
            }
        }
    }
    NTRANSCache::CloseBuilder(hChunkBackup);
//
    return 0;
}

DWORD NBuildUtility::ContentBuilder(struct FileBuilder *builder, vector<struct local_match *> *local_vector) { 
    if (!builder) return BUILD_FAULT;
    if (local_vector->empty()) return BUILD_SUCCESS;
//
    HANDLE hFileLocal = CreateFile( builder->file_name,
                                    GENERIC_WRITE | GENERIC_READ,
                                    FILE_SHARE_READ, // | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileLocal ) {
        wprintf( _T("create file failed: %d\n"), GetLastError() );
		if(ERROR_FILE_NOT_FOUND==GetLastError() || ERROR_PATH_NOT_FOUND==GetLastError()) {
			return BUILD_NOT_FOUND;
		} else return BUILD_VIOLATION;
    }
    // objBuilderVector.AppendResource(hFileLocal);
// _LOG_DEBUG(_T("build utility file is:%s"), builder->file_name);
    //
    if (RANGE_TYPE_FILE == builder->build_type) {
        // lock on the opened file
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
        OverLapped.Offset = builder->offset&0xffffffff;
        OverLapped.OffsetHigh = builder->offset >> 32;
        if(!LockFileEx(hFileLocal,
                       LOCKFILE_EXCLUSIVE_LOCK,
                       0,
                       0xffffffff,
                       0x00000000,
                       &OverLapped)) {
            wprintf(_T("build file lock file fail!\n"));
            return BUILD_VIOLATION;
        }
        if(BuildFileContent(hFileLocal, builder, local_vector)) {
            wprintf(_T("build file content fail!\n"));
            return BUILD_FAULT;
        }
        // unlock on the opened file
        // OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        // OverLapped.Offset = task_node->offset;
        UnlockFileEx(hFileLocal, 0, 0xffffffff, 0x00000000, &OverLapped);
    } else if (RANGE_TYPE_CHUNK == builder->build_type) {
        // lock on the opened file
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
        OverLapped.Offset = builder->offset&0xffffffff;
        OverLapped.OffsetHigh = builder->offset >> 32;
        if(!LockFileEx(hFileLocal,
                       LOCKFILE_EXCLUSIVE_LOCK,
                       0,
                       CHUNK_SIZE,
                       0x00000000,
                       &OverLapped)) {
            wprintf(_T("build chunk lock file fail!\n"));
            return BUILD_VIOLATION;
        }
        if(BuildChunkContent(hFileLocal, builder, local_vector)) {
            wprintf(_T("build chunk content fail!\n"));
            return BUILD_FAULT;
        }
        // unlock on the opened file
        // OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        // OverLapped.Offset = task_node->offset;
        UnlockFileEx(hFileLocal, 0, CHUNK_SIZE, 0x00000000, &OverLapped);
    }
    //
    // objBuilderVector.EraseResource(hFileLocal);
    CloseHandle(hFileLocal);
//
    return BUILD_SUCCESS;
}

void NBuildUtility::CloseBuilderHandle() {
    // objBuilderVector.CloseResource();
}