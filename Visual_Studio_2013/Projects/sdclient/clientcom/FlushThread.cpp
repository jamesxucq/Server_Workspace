#include "StdAfx.h"
#include "third_party.h"
#include "FlushThread.h"
/*
CFlushThread::CFlushThread(void):
m_pFlushThread(NULL),
m_hFlushEvent(INVALID_HANDLE_VALUE)
{
}

CFlushThread::~CFlushThread(void) {
}

class CFlushThread objFlushThread;

DWORD CFlushThread::Initialize(CAnchor *pAnchor, CAnchorCache *pAnchorCache) {
	if(!pAnchor || !pAnchorCache) return -1;

	m_tFlushArgu.pAnchor = pAnchor;
	m_tFlushArgu.pAnchorCache = pAnchorCache;

	m_hFlushEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hFlushEvent == NULL || m_hFlushEvent == INVALID_HANDLE_VALUE) {
		LOG_DEBUG(_T("flush cache event create failed")); 
	}
	m_tFlushArgu.hFlushEvent = m_hFlushEvent;

	return 0;
}

DWORD CFlushThread::Finalize() {
	InterlockedExchange((LONG*)&m_tFlushArgu.bProcessStarted, FALSE);
	SetEvent(m_hFlushEvent);
	// while(WAIT_OBJECT_0 != WaitForSingleObject(m_pFlushThread->m_hThread, 3000)) //wait five seconds
	LONG lProcessExit;
	InterlockedExchange(&lProcessExit, m_tFlushArgu.bProcessStarted);
	while(!lProcessExit && (WAIT_OBJECT_0 != WaitForSingleObject(m_pFlushThread->m_hThread, 3000))) {
		LOG_DEBUG(_T("wait flush thread exit!"));
		Sleep(2048);
		InterlockedExchange(&lProcessExit, m_tFlushArgu.bProcessStarted);
	}

	if(m_hFlushEvent != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hFlushEvent );
		m_hFlushEvent = INVALID_HANDLE_VALUE;
	}

	return 0;
}

DWORD CFlushThread::BeginFlushThread() {
	InterlockedExchange((LONG*)&m_tFlushArgu.bProcessStarted, TRUE);
	m_pFlushThread = AfxBeginThread(FlushCacheProcess, (LPVOID)&m_tFlushArgu);
	return m_pFlushThread? 0: -1;
}

UINT FlushCacheProcess(LPVOID lpParam) {

	struct FLUSH_ARGUMENT *lpObject = (struct FLUSH_ARGUMENT *)lpParam;
	if (lpObject == NULL) return - 1; //输入参数非法

	CAnchor *pAnchor = lpObject->pAnchor;
	CAnchorCache *pAnchorCache = lpObject->pAnchorCache;
	HANDLE hFlushEvent = lpObject->hFlushEvent;

	FilesVec vFilesVec;
	// Enter a loop reading data
	LONG lProcessContinue;
	InterlockedExchange(&lProcessContinue, lpObject->bProcessStarted);
	while(lProcessContinue){
		/////////////////////////////////////////////////////////////////////////////////
		// 循环等待事件发生或超时.
		//WaitForSingleObject(hEvent, INFINITE);
		DWORD wait_object_status = WaitForSingleObject(hFlushEvent, INFINITE);
		// LOG_DEBUG(_T("--------------------------- wait_object_status"));
		switch(wait_object_status) {
		case WAIT_TIMEOUT:
			// The process terminated.
			break;
		case WAIT_OBJECT_0:
			// the process did not terminate within 5000 milliseconds.
			for(;;) {
				Sleep(2048);
				if(GetTickCount()-(pAnchorCache->GetLastTickCount()) > 0x2000) { // 8s
					if(NFilesVec::DeepCopyVec(&vFilesVec, pAnchorCache->GetLastFilesPoint()))
						break;
					pAnchor->FlushFilesVecFile(&vFilesVec);
					NFilesVec::DeleteFilesVec(&vFilesVec);
					break;
				}
			}
			break;
		case WAIT_FAILED:
			// Bad call to function (invalid handle?)
			return -1;
			break;		
		}
		/////////////////////////////////////////////////////////////////////////////////
		InterlockedExchange(&lProcessContinue, lpObject->bProcessStarted);
	}
	/////////////////////////////////////////////////////////////////////////////////
	InterlockedExchange(&lpObject->bProcessStarted, TRUE);
	return 0;
}
*/