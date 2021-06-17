#include "StdAfx.h"

#include <afxmt.h>
#include "StringUtility.h"
#include "TasksListObject.h"

#include "third_party.h"

CTasksListObject::CTasksListObject(void):
m_hTasksListFile(INVALID_HANDLE_VALUE),
m_dwHandleNode(0)
{
	memset(m_szTasksListName, '\0', MAX_PATH);
}

CTasksListObject::~CTasksListObject(void) {
}

CTasksListObject objTasksList;

DWORD CTasksListObject::Initialize(TCHAR *szTasksListFile, TCHAR *szLocation) {
	nstriutility::get_name(m_szTasksListName, szLocation, szTasksListFile);
	m_hTasksListFile = INVALID_HANDLE_VALUE;
	return 0;
}

DWORD CTasksListObject::Finalize() {
	if(m_hTasksListFile != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hTasksListFile );
		m_hTasksListFile = INVALID_HANDLE_VALUE;
	}
	DeleteFile(m_szTasksListName); // delete by james 20110414
	return 0;
}

DWORD CTasksListObject::OpenTasksFile() {
	if(m_hTasksListFile != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hTasksListFile );
		m_hTasksListFile = INVALID_HANDLE_VALUE;
	} else {
		m_hTasksListFile = CreateFile( m_szTasksListName, 
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if( m_hTasksListFile == INVALID_HANDLE_VALUE ) {
			TRACE( _T("create file failed.\n") );
			TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
			return -1;
		}
	}
	return 0;
}

DWORD CTasksListObject::CloseTasksFile() {
	if(m_hTasksListFile != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hTasksListFile );
		m_hTasksListFile = INVALID_HANDLE_VALUE;
	}
	DeleteFile(m_szTasksListName);
	return 0;
}

CCriticalSection gcs_ListHandle;
DWORD CTasksListObject::SetNodePoint(int iNodeCount) {
	DWORD dwSetPointValue = -1;

	gcs_ListHandle.Lock();
	DWORD result = SetFilePointer(m_hTasksListFile, iNodeCount*sizeof(struct TaskNode), NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		dwSetPointValue = -1;
	else dwSetPointValue = 0;
	// FlushFileBuffers(m_hTasksListFile);
	gcs_ListHandle.Unlock();

	return dwSetPointValue;
}

DWORD CTasksListObject::WriteNode(struct TaskNode *task_node) {
	DWORD dwWritenSize = 0;
	if(!task_node) return -1;

	// LOG_DEBUG(_T("----------------------------------"));
	// LOG_DEBUG(_T("node_index:%d"), task_node->node_index);
	// LOG_DEBUG(_T("task_node.success_flags:%x"), task_node->success_flags);
	// LOG_DEBUG(_T("task_node.control_code:%x"), task_node->control_code);
	// LOG_DEBUG(_T("task_node.szFileName:%s"), task_node->szFileName);
	// LOG_DEBUG(_T("task_node.dwFileSize:%d"), task_node->dwFileSize);
	gcs_ListHandle.Lock();
	if(!WriteFile(m_hTasksListFile, task_node, sizeof(struct TaskNode), &dwWritenSize, NULL))
		dwWritenSize = -1;
	gcs_ListHandle.Unlock();

	return dwWritenSize;
}

DWORD CTasksListObject::WriteNodeExt(struct TaskNode *task_node, int iNodeCount) {
	DWORD dwWritenSize = 0;
	if(!task_node) return -1;

	gcs_ListHandle.Lock();
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = iNodeCount*sizeof(struct TaskNode);
	if(!WriteFile(m_hTasksListFile, task_node, sizeof(struct TaskNode), &dwWritenSize, &OverLapped))
		dwWritenSize = -1;
	gcs_ListHandle.Unlock();

	return dwWritenSize;
}

DWORD CTasksListObject::ReadNode(TaskNode *task_node) {
	DWORD dwReadSize = 0;
	if(!task_node) return -1;

	gcs_ListHandle.Lock();
	if(!ReadFile(m_hTasksListFile, task_node, sizeof(struct TaskNode), &dwReadSize, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) dwReadSize = -1;
	}
	gcs_ListHandle.Unlock();

	return dwReadSize;
}

DWORD CTasksListObject::ReadNodeExt(TaskNode *task_node, int iNodeCount) {
	DWORD dwReadSize = 0;
	if(!task_node) return -1;

	gcs_ListHandle.Lock();
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = iNodeCount*sizeof(struct TaskNode);
	if(!ReadFile(m_hTasksListFile, task_node, sizeof(struct TaskNode), &dwReadSize, &OverLapped)) {
		if(ERROR_HANDLE_EOF != GetLastError()) dwReadSize = -1;
	}
	gcs_ListHandle.Unlock();

	return dwReadSize;
}


CCriticalSection gcs_HandleNodeNum;
DWORD CTasksListObject::GetHandleNumber() {
	gcs_HandleNodeNum.Lock();
	DWORD dwHandleNumber = m_dwHandleNode; 
	gcs_HandleNodeNum.Unlock();

	return dwHandleNumber;
}

DWORD CTasksListObject::GetHandleNodeAddition() {
	gcs_HandleNodeNum.Lock();
	DWORD dwHandleNumber = m_dwHandleNode; 
	m_dwHandleNode ++;
	gcs_HandleNodeNum.Unlock();

	return dwHandleNumber;
}

void CTasksListObject::SetHandleNumber(int dwHandleNumber) { 
	gcs_HandleNodeNum.Lock();
	m_dwHandleNode = dwHandleNumber; 
	gcs_HandleNodeNum.Unlock();
}
