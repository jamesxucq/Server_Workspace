#include "StdAfx.h"

#include <afxmt.h>
#include "StringUtility.h"
#include "TaskListObj.h"

#include "third_party.h"

//
CTaskListObj::CTaskListObj(void):
    m_hTListFile(INVALID_HANDLE_VALUE),
    m_dwHandNode(0),
	m_dwIndeTatol(0)
{
    MKZEO(m_szTListName);
    InitializeCriticalSection(&m_csListHand);
    InitializeCriticalSection(&m_csHandNumbe);
    InitializeCriticalSection(&m_csArrayID);
}

CTaskListObj::~CTaskListObj(void) {
    DeleteCriticalSection(&m_csArrayID);
    DeleteCriticalSection(&m_csHandNumbe);
    DeleteCriticalSection(&m_csListHand);
}

CTaskListObj objTaskList;

DWORD CTaskListObj::Initialize(TCHAR *szTListFile, TCHAR *szLocation) {
    struti::get_name(m_szTListName, szLocation, szTListFile);
    m_hTListFile = INVALID_HANDLE_VALUE;
    return 0x00;
}

DWORD CTaskListObj::Finalize() {
    if(INVALID_HANDLE_VALUE != m_hTListFile) {
        CloseHandle( m_hTListFile );
        m_hTListFile = INVALID_HANDLE_VALUE;
    }
    DeleteFile(m_szTListName); // delete by james 20110414
    return 0x00;
}

DWORD CTaskListObj::OpenTasksFile() {
    if(INVALID_HANDLE_VALUE != m_hTListFile) {
		DWORD dwResult = SetFilePointer(m_hTListFile, 0, NULL, FILE_BEGIN);
		if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
			CloseHandle( m_hTListFile );
			m_hTListFile = INVALID_HANDLE_VALUE;
			return ((DWORD)-1);
		}
		dwResult = SetEndOfFile(m_hTListFile);
		if(FALSE==dwResult && NO_ERROR!=GetLastError()) {
			CloseHandle( m_hTListFile );
			m_hTListFile = INVALID_HANDLE_VALUE;
			return ((DWORD)-1);
		}
// _LOG_DEBUG(_T("open exist tasks file."));
    } else {
        m_hTListFile = CreateFile( m_szTListName,
                                       GENERIC_WRITE | GENERIC_READ,
                                       FILE_SHARE_READ,
                                       NULL,
                                       CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_NORMAL,
                                       NULL);
        if( INVALID_HANDLE_VALUE == m_hTListFile ) {
_LOG_WARN( _T("create file failed: %d"), GetLastError() );
            return ((DWORD)-1);
        }
// _LOG_DEBUG(_T("create exist tasks file."));
    }
	//
// _LOG_DEBUG(_T("open tasks file."));
    return 0x00;
}

DWORD CTaskListObj::CloseTasksFile() {
    if(INVALID_HANDLE_VALUE != m_hTListFile) {
        CloseHandle( m_hTListFile );
        m_hTListFile = INVALID_HANDLE_VALUE;
    }
    DeleteFile(m_szTListName);
// _LOG_DEBUG(_T("close tasks file."));
    return 0x00;
}

int CTaskListObj::SetNodePoint(DWORD dwNodeInde) {
    DWORD dwSetPointValue = ((DWORD)-1);
    //
    EnterCriticalSection(&m_csListHand);
    DWORD dwResult = SetFilePointer(m_hTListFile, dwNodeInde*sizeof(struct TaskNode), NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
_LOG_DEBUG(_T("GetLastError():%d"), GetLastError());
        dwSetPointValue = ((DWORD)-1);
	} else dwSetPointValue = 0x00;
    // FlushFileBuffers(m_hTListFile);
    LeaveCriticalSection(&m_csListHand);
    //
// _LOG_DEBUG(_T("set node point, dwSetPointValue:%d"), dwSetPointValue);
    return dwSetPointValue;
}

int CTaskListObj::WriteNode(struct TaskNode *task_node) {
    DWORD dwWritenSize = 0x00;
    //
// _LOG_DEBUG(_T("----------------------------------"));
// _LOG_DEBUG(_T("node_inde:%d"), task_node->node_inde);
// _LOG_DEBUG(_T("task_node.success_flags:%08x"), task_node->success_flags);
// _LOG_DEBUG(_T("task_node.control_code:%08x"), task_node->control_code);
// _LOG_DEBUG(_T("task_node.szFileName:%s"), task_node->szFileName1);
// _LOG_DEBUG(_T("task_node.dwFileSize:%d"), task_node->dwFileSize);
    EnterCriticalSection(&m_csListHand);
    if(!WriteFile(m_hTListFile, task_node, sizeof(struct TaskNode), &dwWritenSize, NULL))
        dwWritenSize = ((DWORD)-1);
    LeaveCriticalSection(&m_csListHand);
    //
    return dwWritenSize;
}

int CTaskListObj::WriteNodeEx(struct TaskNode *task_node, DWORD dwNodeInde) {
    DWORD dwWritenSize = 0x00;
    //
    EnterCriticalSection(&m_csListHand);
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = dwNodeInde*sizeof(struct TaskNode);
    if(!WriteFile(m_hTListFile, task_node, sizeof(struct TaskNode), &dwWritenSize, &OverLapped))
        dwWritenSize = ((DWORD)-1);
    LeaveCriticalSection(&m_csListHand);
    //
    return dwWritenSize;
}

int CTaskListObj::ReadNode(TaskNode *task_node) {
    DWORD dwReadSize = 0x00;
    //
    EnterCriticalSection(&m_csListHand);
    if(!ReadFile(m_hTListFile, task_node, sizeof(struct TaskNode), &dwReadSize, NULL)) {
_LOG_DEBUG(_T("dwReadSize:%d GetLastError():%d"), dwReadSize, GetLastError());
        if(ERROR_HANDLE_EOF != GetLastError()) 
			dwReadSize = ((DWORD)-1);
    }
    LeaveCriticalSection(&m_csListHand);
    //
// _LOG_DEBUG(_T("read node, dwReadSize:%d"), dwReadSize);
    return dwReadSize;
}

int CTaskListObj::ReadNodeEx(TaskNode *task_node, DWORD dwNodeInde) {
    DWORD dwReadSize = 0x00;
    //
    EnterCriticalSection(&m_csListHand);
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = dwNodeInde*sizeof(struct TaskNode);
    if(!ReadFile(m_hTListFile, task_node, sizeof(struct TaskNode), &dwReadSize, &OverLapped)) {
        if(ERROR_HANDLE_EOF != GetLastError()) 
			dwReadSize = ((DWORD)-1);
    }
    LeaveCriticalSection(&m_csListHand);
    //
    return dwReadSize;
}

DWORD CTaskListObj::GetHandNumbe() {
    EnterCriticalSection(&m_csHandNumbe);
    DWORD dwHandNumbe = m_dwHandNode;
    LeaveCriticalSection(&m_csHandNumbe);
    //
    return dwHandNumbe;
}

DWORD CTaskListObj::GetHandNodeAdd() {
    EnterCriticalSection(&m_csHandNumbe);
    DWORD dwHandNumbe = m_dwHandNode;
    m_dwHandNode ++;
    LeaveCriticalSection(&m_csHandNumbe);
    //
    return dwHandNumbe;
}

VOID CTaskListObj::SetHandNumbe(DWORD dwHandNumbe) {
    EnterCriticalSection(&m_csHandNumbe);
    m_dwHandNode = dwHandNumbe;
    LeaveCriticalSection(&m_csHandNumbe);
}

VOID CTaskListObj::InitArray(DWORD dwArrayID, DWORD dwNodeInde) {
    static DWORD dwProcessArray;
    struct TaskNode cent_node, task_node;
    //
    EnterCriticalSection(&m_csArrayID);
_LOG_DEBUG(_T("dwProcessArray:%u dwArrayID:%u"), dwProcessArray, dwArrayID);
    if(dwProcessArray != dwArrayID) {
        m_dwIndeTatol = 0x00;
        if(0 < ReadNodeEx(&cent_node, dwNodeInde)) {
            m_dwIndeTatol = 0x01; // self process
			//
            DWORD inde;
            if(!(FILE_TASK_BEGIN & cent_node.control_code)) {
                inde = dwNodeInde;
                do {
                    if(0 >= ReadNodeEx(&task_node, --inde)) break;
                    if(ONLY_TASK_UNDONE(task_node.success_flags)) m_dwIndeTatol++;
_LOG_DEBUG(_T("b inde:%u m_dwIndeTatol:%u ctrl:%X array_id:%d name:%s"), inde, m_dwIndeTatol, task_node.control_code, task_node.array_id, task_node.szFileName1);
                } while(!(FILE_TASK_BEGIN & task_node.control_code));
            }
            if(!(FILE_TASK_END & cent_node.control_code)) {
                inde = dwNodeInde;
                do {
                    if(0 >= ReadNodeEx(&task_node, ++inde)) break;
                    if(ONLY_TASK_UNDONE(task_node.success_flags)) m_dwIndeTatol++;
_LOG_DEBUG(_T("e inde:%u m_dwIndeTatol:%u ctrl:%X array_id:%d name:%s"), inde, m_dwIndeTatol, task_node.control_code, task_node.array_id, task_node.szFileName1);
                } while(!(FILE_TASK_END & task_node.control_code));
            }
        } else m_dwIndeTatol = -1;
        dwProcessArray = dwArrayID;
    }
    LeaveCriticalSection(&m_csArrayID);
    //
// disable by james 20140115
_LOG_DEBUG(_T("dwArrayID:%u dwNodeInde:%u m_dwIndeTatol:%d"), dwArrayID, dwNodeInde, m_dwIndeTatol);
}

DWORD CTaskListObj::ArrayCapacity() {
    DWORD dwNodeTatol;
    //
    EnterCriticalSection(&m_csArrayID);
    dwNodeTatol = m_dwIndeTatol;
    LeaveCriticalSection(&m_csArrayID);
    //
// disable by james 20140115
_LOG_DEBUG(_T("ArrayCapacity dwNodeTatol:%u"), dwNodeTatol);
    return dwNodeTatol;
}

//
CTasksCache objTasksCache;