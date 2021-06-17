#include "StdAfx.h"

#include "../third party/RiverFS/RiverFS.h"
#include "WatcherBzl.h"

//
#define ACTION_CACHE_DEFAULT				_T("~\\action_cache.acd")

//
CWatcherObj::CWatcherObj(void):
m_hActioCache(INVALID_HANDLE_VALUE)
{
	MKZEO(m_szWatchDire);
	MKZEO(m_szActioCache);
}

CWatcherObj::~CWatcherObj(void) {
}
CWatcherObj objWatcher;

DWORD CWatcherObj::Initialize(TCHAR *szLocation, TCHAR *szWatchDirectory) {
	struti::get_name(m_szActioCache, szLocation, ACTION_CACHE_DEFAULT);
	m_hActioCache = CreateFile( m_szActioCache, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /* FILE_SHARE_READ */ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_WRITE_THROUGH,
		NULL);
	if( INVALID_HANDLE_VALUE == m_hActioCache ) {
		_LOG_WARN( _T("create file failed: %d"), GetLastError() );
		return ((DWORD)-1);
	} 
	DWORD dwResult = SetFilePointer(m_hActioCache, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) 
		return ((DWORD)-1);
	m_obActionHandler.Initialize(m_hActioCache);
	_tcscpy_s(m_szWatchDire, szWatchDirectory);
	//
	DWORD dwChangeFilter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE;
	DWORD dwWatchValue = m_obFileWatcher.WatchDirectory(szWatchDirectory, dwChangeFilter, &m_obActionHandler);
	if(ERROR_SUCCESS != dwWatchValue) return ((DWORD)-1);
	// MessageBox(_T("Failed to start watch:\n") + GetLastErrorMessageString( dwWatch ) );
// _LOG_DEBUG(_T("dire watching start ok")); // disable by james 20140410
	return 0x00;
}

DWORD CWatcherObj::Finalize() {
	if(!m_obFileWatcher.UnwatchAllDirectories()) return ((DWORD)-1);
// _LOG_DEBUG(_T("dire watching stop ok")); // disable by james 20140410
	unsigned __int64 qwFileSize = 0x00;
	if(INVALID_HANDLE_VALUE != m_hActioCache) {
		qwFileSize = NFileUtility::FileSizeHandle(m_hActioCache);
		CloseHandle( m_hActioCache );
		m_hActioCache = INVALID_HANDLE_VALUE;
	}
	if(!qwFileSize) DeleteFile(m_szActioCache);
//
	return 0x00;
}

BOOL CWatcherObj::IsCacheExist() { // 文件存在,且有数据
	unsigned __int64 qwFileSize;
	qwFileSize = NFileUtility::FileSizeHandle(m_hActioCache);
	return qwFileSize? TRUE : FALSE;
}

//
// static int watching_reference = 0; // just for test 20120307 by james
DWORD NWatcherBzl::Initialize(TCHAR *szLocation, const TCHAR *szDriveLetter) {
	TCHAR szWatchDirectory[MAX_PATH];
	if (!szDriveLetter) return ((DWORD)-1);
	//
	// _LOG_DEBUG(_T("create watching_reference:%d directory:%s"), ++watching_reference, szWatchDirectory);
	// Set up change notify register
	_tcscpy_s(szWatchDirectory, szDriveLetter); _tcscat(szWatchDirectory, _T("\\"));
	if(DWATCH_OBJECT(Initialize(szLocation, szWatchDirectory))) return ((DWORD)-1);
	//
	return 0x00;
}

DWORD NWatcherBzl::Finalize() {
	// _LOG_DEBUG(_T("destroy watching_reference:%d"), --watching_reference);
	//
	DWATCH_OBJECT(Finalize());
	//
	_LOG_INFO(_T("watcher bzl finalize!"));
	return 0x00;
}

// PFILE_NOTIFY_INFORMATION
