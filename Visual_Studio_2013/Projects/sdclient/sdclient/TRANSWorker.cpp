#include "StdAfx.h"

#include "AnchorBzl.h" // modify bu james 20110211
#include "ExecuteTransmitBzl.h" // modify by james 20110302
#include "TRANSObject.h"

#include "TRANSWorker.h"

CTRANSWorker::CTRANSWorker(void) {
}

CTRANSWorker::~CTRANSWorker(void) {
}

//
DWORD CTRANSWorker::GetNewAnchor() {
    if(!m_pTRANSSeionBzl) return NULL;
    return m_pTRANSSeionBzl->GetNewAncher();
}

CTRANSWorker *NTRANSWorker::Factory(struct SessionArgu *pSeionArgu, struct SocketsArgu *pSockeArgu) {
    CTRANSWorker *pTransWorker = new CTRANSWorker();
	if(pTransWorker->Create(pSeionArgu, pSockeArgu)) {
_LOG_WARN(_T("worker factory error!"));
		pTransWorker->Destroy();
		delete pTransWorker;
		pTransWorker = NULL;
	} else {
_LOG_DEBUG(_T("worker factory ok!"));
		objWorkerVector.AppendTransWorker(pTransWorker);
	}
    return pTransWorker;
}

VOID NTRANSWorker::DestroyObject(CTRANSWorker *pTransWorker) {
	if(pTransWorker) {
		objWorkerVector.EraseTransWorker(pTransWorker);
		pTransWorker->Destroy();
		delete pTransWorker;	
	}
}

VOID NTRANSWorker::CloseWorkerResource() {
    objWorkerVector.CloseTransWorker();
}

//
CWorkerVector::CWorkerVector(void) {
    InitializeCriticalSection(&m_csWorkerVector);
}

CWorkerVector::~CWorkerVector(void) {
    DeleteCriticalSection(&m_csWorkerVector);
}

DWORD CWorkerVector::AppendTransWorker(CTRANSWorker *pTransWorker) {
    vector<CTRANSWorker *>::iterator iter;
    BOOL bFoundOld;
    //
    if(!pTransWorker) return ((DWORD)-1);
    EnterCriticalSection(&m_csWorkerVector);
    //
    bFoundOld = FALSE;
    for(iter=vWorkerVector.begin(); vWorkerVector.end()!=iter; ++iter) {
        if(pTransWorker == *iter) {
            bFoundOld = TRUE;
            break;
        }
    }
    if(!bFoundOld) vWorkerVector.push_back(pTransWorker);
    //
    LeaveCriticalSection(&m_csWorkerVector);
    //
    return 0x00;
}

DWORD CWorkerVector::EraseTransWorker(CTRANSWorker *pTransWorker) {
    vector<CTRANSWorker *>::iterator iter;
    BOOL bFoundOld;
    //
    if(!pTransWorker) return ((DWORD)-1);
    EnterCriticalSection(&m_csWorkerVector);
    //
    bFoundOld = TRUE;
    for(iter=vWorkerVector.begin(); vWorkerVector.end()!=iter; ) {
        if(*iter == pTransWorker) {
            bFoundOld = FALSE;
            iter = vWorkerVector.erase(iter);
        } else ++iter;
    }
    //
    LeaveCriticalSection(&m_csWorkerVector);
    //
    return bFoundOld;
}

DWORD CWorkerVector::CloseTransWorker() {
    vector<CTRANSWorker *>::iterator iter;
    //
    EnterCriticalSection(&m_csWorkerVector);
    for(iter=vWorkerVector.begin(); vWorkerVector.end()!=iter; ) {
        (*iter)->CloseResource();
        iter = vWorkerVector.erase(iter);
    }
    LeaveCriticalSection(&m_csWorkerVector);
    //
    return 0x00;
}

//
class CWorkerVector objWorkerVector;