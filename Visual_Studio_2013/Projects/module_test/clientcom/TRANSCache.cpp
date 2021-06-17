#include "StdAfx.h"
#include <afxmt.h>

#include "third_party.h"
#include "FileUtility.h"

#include "TRANSCache.h"

TCHAR *GetFileBaseName(TCHAR *szBaseName, const TCHAR *szFileName, DWORD dwMaxCount) {
	TCHAR szDupName[MAX_PATH];
	TCHAR *position = NULL;

	if(!szFileName || !szBaseName) return NULL;

	_tcscpy_s(szDupName, szFileName);
	position = _tcsrchr(szDupName, _T('.'));
	if(position) position[0] = _T('\0');

	position = _tcsrchr(szDupName, _T('\\'));
	if(position) _tcsncpy_s(szBaseName, MAX_PATH, ++position, dwMaxCount);
	else _tcsncpy_s(szBaseName, MAX_PATH, szDupName, dwMaxCount);

	return szBaseName;
}

TCHAR *NTRANSCache::CreatBuildName(TCHAR *szBuildName, const TCHAR *szFileName, off_t offset) {
	static DWORD dwBlockCount;
	TCHAR szFileBase[MAX_PATH];

	if(!szBuildName || !szFileName) return NULL;

	if(!GetFileBaseName(szFileBase, szFileName, 12)) return NULL;
	_stprintf_s(szBuildName, MAX_PATH, _T("%s-%x-%x"), szFileBase, dwBlockCount++, offset);
LOG_DEBUG( _T("create cache name is: %s"), szBuildName);

	return szBuildName;
}

BOOL CacheDirectoryEmpty() {
	TCHAR szEmptyDirectory[MAX_PATH];

	if(!GetEnvironmentVariable(_T("APPDATA"), szEmptyDirectory, MAX_PATH))
		return FALSE;
	_tcscat_s(szEmptyDirectory, _T("\\sdclient"));
	
	return NFileUtility::EmptyDirectory(szEmptyDirectory);
}

// MFC临界区类对象
CCriticalSection gcs_BackupFile;
TCHAR *GetBuildFile(TCHAR *szBuildFile, const TCHAR *szBuildName) {
	static TCHAR szCacheDirectory[MAX_PATH];

	if(!szBuildFile || !szBuildName) return NULL;
	// 进入临界区
	gcs_BackupFile.Lock();
	if(_T(':') != szCacheDirectory[1]) {
		if(!GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH))
			return NULL;
		_tcscat_s(szCacheDirectory, _T("\\sdclient\\"));
		CreateDirectory(szCacheDirectory, NULL);
		// empty cache directory;
		CacheDirectoryEmpty();
	}
	_tcscpy_s(szBuildFile, MAX_PATH, szCacheDirectory);
	//离开临界区
	gcs_BackupFile.Unlock();
	_tcscat_s(szBuildFile, MAX_PATH, szBuildName);

	return szBuildFile;
}

HANDLE NTRANSCache::OpenBuild(const TCHAR *szBuildName) {
	TCHAR szBuildFile[MAX_PATH];
	HANDLE	hBuildCache = INVALID_HANDLE_VALUE;

	if(!szBuildName) return INVALID_HANDLE_VALUE;
	hBuildCache = CreateFile(GetBuildFile(szBuildFile, szBuildName), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hBuildCache == INVALID_HANDLE_VALUE ) {
		LOG_WARN( _T("create cache file failed, file name is: %s"), szBuildFile);
		LOG_WARN( _T("GetLastError() is: %d"), GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	return hBuildCache;
}

void NTRANSCache::CloseBuild(HANDLE hBuildCache)
{ if(INVALID_HANDLE_VALUE != hBuildCache) CloseHandle(hBuildCache); }

void NTRANSCache::DestroyBuild(HANDLE hBuildCache, const TCHAR *szBuildName) {
	TCHAR szBuildFile[MAX_PATH];

	if(INVALID_HANDLE_VALUE != hBuildCache) CloseHandle(hBuildCache);
	DeleteFile(GetBuildFile(szBuildFile, szBuildName));
}