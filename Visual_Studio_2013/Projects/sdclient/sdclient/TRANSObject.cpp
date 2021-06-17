#include "StdAfx.h"

// #include "client_macro.h"
#include "TRANSObject.h"

CTRANSObject::CTRANSObject(void)
:m_pTRANSThread(NULL)
,m_dwLastServAnchor((DWORD)-1)
,m_dwCurrentStatus(PROCESS_STATUS_INITIAL)
,m_dwNextStatus(PROCESS_STATUS_INITIAL)
,m_dwConnected(FALSE)
,m_hWaitingEvent(INVALID_HANDLE_VALUE)
,m_dwWorkerThreadsCount(0x00)
,m_hWorkerThreads(NULL) 

{
	InitializeCriticalSection(&m_csWorkerThreads);
}

CTRANSObject::~CTRANSObject(void) {
	DeleteCriticalSection(&m_csWorkerThreads);
}

CTRANSObject objTRANSObject;

DWORD CTRANSObject::Initialize() { 
	// �����¼�
	if(INVALID_HANDLE_VALUE == m_hWaitingEvent)
		m_hWaitingEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // �Զ����� �������ź�
	return 0x00;
}

DWORD CTRANSObject::Finalize() {
	if(INVALID_HANDLE_VALUE != m_hWaitingEvent) {
		CloseHandle( m_hWaitingEvent );
		m_hWaitingEvent = INVALID_HANDLE_VALUE;
	}
	return 0x00;
}

DWORD CTRANSObject::SetWorkerThreads(HANDLE *hWorkerThreads, DWORD dwThreadsCount) {
	EnterCriticalSection(&m_csWorkerThreads);
	m_dwWorkerThreadsCount = dwThreadsCount;
	m_hWorkerThreads = hWorkerThreads;
	LeaveCriticalSection(&m_csWorkerThreads);
//
	return 0x00;
}

HANDLE *CTRANSObject::GetWorkerThreads(DWORD *pWorkerThreadsCount) {
	if(!pWorkerThreadsCount) return NULL;
//
	EnterCriticalSection(&m_csWorkerThreads);
	*pWorkerThreadsCount = m_dwWorkerThreadsCount;
	HANDLE *hWorkerThreads = m_hWorkerThreads;
	LeaveCriticalSection(&m_csWorkerThreads);
//
	return hWorkerThreads;
}
