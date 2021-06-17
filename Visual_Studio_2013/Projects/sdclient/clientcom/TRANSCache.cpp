#include "StdAfx.h"
#include <afxmt.h>

#include "third_party.h"
#include "FileUtility.h"

#include "TRANSCache.h"

TCHAR *GetFileBaseName(TCHAR *szBaseName, const TCHAR *szFileName, DWORD dwMaxCount) {
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

TCHAR *NTRANSCache::CreatBuilderName(TCHAR *szBuilderName, const TCHAR *szFileName, size_t offset) {
    static DWORD dwBlockCount;
    TCHAR szFileBase[MAX_PATH];
    //
    if(!GetFileBaseName(szFileBase, szFileName, 12)) return NULL;
    _stprintf_s(szBuilderName, MAX_PATH, _T("%s-%x-%x"), szFileBase, dwBlockCount++, offset);
// _LOG_DEBUG( _T("create cache name is: %s"), szBuilderName); // disable by james 20130410
    return szBuilderName;
}

// MFC临界区类对象
CCriticalSection gcsBackupFile;
TCHAR *GetBuildFile(TCHAR *szBuildFile, const TCHAR *szBuilderName) {
    static TCHAR szCacheDirectory[MAX_PATH];
    //
    gcsBackupFile.Lock();
    if(_T(':') != szCacheDirectory[1]) {
        GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH);
        _tcscat_s(szCacheDirectory, _T("\\sdclient\\cache"));
		NDireUtility::RecursMakeFolderW(szCacheDirectory);
    }
    _stprintf_s(szBuildFile, MAX_PATH, _T("%s\\"), szCacheDirectory);
    //
    gcsBackupFile.Unlock();
    _tcscat_s(szBuildFile, MAX_PATH, szBuilderName);
    //
    return szBuildFile;
}

HANDLE NTRANSCache::OpenBuilder(const TCHAR *szBuilderName) {
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
        _LOG_WARN( _T("create cache file failed, file name is: %s"), szBuildFile);
        _LOG_WARN( _T("GetLastError() is: %d"), GetLastError());
        return INVALID_HANDLE_VALUE;
    }
    //
    return hBuilderCache;
}

VOID NTRANSCache::CloseBuilder(HANDLE hBuilderCache) {
    if(INVALID_HANDLE_VALUE != hBuilderCache)
        CloseHandle(hBuilderCache);
}

VOID NTRANSCache::DestroyBuilder(HANDLE hBuilderCache, const TCHAR *szBuilderName) {
    TCHAR szBuildFile[MAX_PATH];
    if(INVALID_HANDLE_VALUE != hBuilderCache) CloseHandle(hBuilderCache);
    DeleteFile(GetBuildFile(szBuildFile, szBuilderName));
}