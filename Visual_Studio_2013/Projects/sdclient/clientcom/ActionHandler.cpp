#include "StdAfx.h"
#include "AnchorBzl.h"

#include "third_party.h"
#include "ShellLinkBzl.h"
#include "ExecuteTransmitBzl.h"

#include "ActionHandler.h"

// 文件发生改变,触发同步事件
#define SET_ACTION_EVENT { \
	if(!m_dwLastActio) NExecTransBzl::SetWaitingEvent(); \
	m_dwLastActio = GetTickCount(); \
}

//
CActionHandler::CActionHandler(void): CDirectoryHandler()
    ,m_dwLastActio(0)
    ,m_hActioCache(INVALID_HANDLE_VALUE)
{
    MKZEO(m_tActioAdd.szFileName);
    InitializeCriticalSection(&m_csValidLink);
}

CActionHandler::~CActionHandler(void) {
    DeleteCriticalSection(&m_csValidLink);
}

#define LOCK_OPERATION_DELAY		6000 // 6s
// todo: add file check locked
int CActionHandler::CacheActioCopy(HANDLE hLockActio) { // -1:exception 0:finish >0:work
    int iLockedValue = -1;
    EnterCriticalSection(&m_csValidLink);
// _LOG_DEBUG(_T("added file name:%s m_tActioAdd.szFileName:%s"), szFileName, m_tActioAdd.szFileName);
// _LOG_DEBUG(_T("LOCK_OPERATION_DELAY:%d GetTickCount:%d m_dwLastActio:%d diff:%d"), LOCK_OPERATION_DELAY, GetTickCount(), m_dwLastActio, GetTickCount()-m_dwLastActio); // disable by james 20140410
    if(LOCK_OPERATION_DELAY < (GetTickCount()-m_dwLastActio)) {
        if(iLockedValue = m_dwLastActio) {
            MKZEO(m_tActioAdd.szFileName);
            m_dwLastActio = 0x00;
			// 拷贝用户修改文件的数据
            ActionUtility::ActioCopy(hLockActio, m_hActioCache);
// _LOG_DEBUG(_T("xdwLastAction:%d iLockedValue:%d"), m_dwLastActio, iLockedValue); // disable by james 20140410
        }
    }
    //
    LeaveCriticalSection(&m_csValidLink);
    return iLockedValue;
}

void CActionHandler::LastActioCopy(TCHAR *szFileName) {
    EnterCriticalSection(&m_csValidLink);
	// 拷贝用户修改文件的数据
    _tcscpy_s(szFileName, MAX_PATH, m_tActioAdd.szFileName);
    //
    LeaveCriticalSection(&m_csValidLink);
}

void CActionHandler::ClearActioCache() {
    EnterCriticalSection(&m_csValidLink);
	// 拷贝用户修改文件的数据
    ActionUtility::EraseActio(m_hActioCache);
    //
    LeaveCriticalSection(&m_csValidLink);
}

VOID CActionHandler::Initialize(HANDLE hActioCache) {
    EnterCriticalSection(&m_csValidLink);
    m_hActioCache = hActioCache;
    MKZEO(m_tActioAdd.szFileName);
    //
    LeaveCriticalSection(&m_csValidLink);
}

//
void CActionHandler::On_FileAdded(const TCHAR *szFileName, DWORD ulTimestamp) {
// logger(_T("c:\\river.log"), _T("on added: %s\r\n"), szFileName);
    EnterCriticalSection(&m_csValidLink);
    ADDITION_ACTION_LOGGER(m_hActioCache, m_tActioAdd, szFileName, ulTimestamp)
_LOG_DEBUG(_T("added file name:%s"), szFileName);
    SET_ACTION_EVENT
    LeaveCriticalSection(&m_csValidLink);
}

void CActionHandler::On_FileRemoved(const TCHAR *szFileName, DWORD ulTimestamp) {
// logger(_T("c:\\river.log"), _T("on removed: %s\r\n"), szFileName);
    EnterCriticalSection(&m_csValidLink);
    DELETE_ACTION_LOGGER(m_hActioCache, m_tActioAdd, szFileName, ulTimestamp)
_LOG_DEBUG(_T("remove file name:%s"), szFileName);
    SET_ACTION_EVENT
    LeaveCriticalSection(&m_csValidLink);
}

void CActionHandler::On_FileModified(const TCHAR *szFileName, DWORD ulTimestamp) {
// logger(_T("c:\\river.log"), _T("on modified: %s\r\n"), szFileName);
    EnterCriticalSection(&m_csValidLink);
    MODIFY_ACTION_LOGGER(m_hActioCache, m_tActioAdd, szFileName, ulTimestamp)
_LOG_DEBUG(_T("modify file name:%s"), szFileName);
    SET_ACTION_EVENT
    LeaveCriticalSection(&m_csValidLink);
}

void CActionHandler::On_FileNameChanged(const TCHAR *szExistName, const TCHAR *szNewName, DWORD ulTimestamp) {
    EnterCriticalSection(&m_csValidLink);
    RENAME_ACTION_LOGGER(m_hActioCache, m_tActioAdd, szExistName, szNewName, ulTimestamp)
_LOG_DEBUG(_T("rename file name:%s"), szExistName);
    SET_ACTION_EVENT
    LeaveCriticalSection(&m_csValidLink);
}

//
void CActionHandler::On_WatchStarted(DWORD dwErrorCode, const TCHAR *szDirectory) {
    // if( 0 == dwError ) m_listBox.AddString( _T("Directory Watch Started: ") + szDirectory);
    // else.m_listBox.AddString( _T("Directory watch failed: ") + szDirectory);
	if( 0x00 == dwErrorCode ) {
		_LOG_INFO(_T("handler directory watch started: %s"), szDirectory);
	} else {
		_LOG_INFO(_T("handler directory watch failed: %s"), szDirectory);
	}
}

void CActionHandler::On_WatchStopped(const TCHAR *szDirectory) {
    _LOG_INFO(_T("handler directory watch stoped: %s"), szDirectory);
    //
}




