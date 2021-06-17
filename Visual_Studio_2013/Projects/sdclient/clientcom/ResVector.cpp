#include "StdAfx.h"
#include "ResVector.h"

CResVector::CResVector(void) {
	InitializeCriticalSection(&m_csResVector);
}

CResVector::~CResVector(void) {
	DeleteCriticalSection(&m_csResVector);
}

//
class CResVector objVerifyVector;
class CResVector objBuilderVector;

//
DWORD CResVector::AppendResource(VOID *handle) {
	vector<VOID *>::iterator iter;
	BOOL bFoundOld;

	if(!handle) return ((DWORD)-1);
	EnterCriticalSection(&m_csResVector);

	bFoundOld = FALSE;
	for(iter=m_vResVector.begin(); m_vResVector.end()!=iter; ++iter) {
		if(handle == *iter) {
			bFoundOld = TRUE;
			break;
		}
	}
	if(!bFoundOld) m_vResVector.push_back(handle);

	LeaveCriticalSection(&m_csResVector);
	return 0x00;
}

DWORD CResVector::EraseResource(VOID *handle) {
	vector<VOID *>::iterator iter;
	BOOL bFoundOld;

	if(!handle) return ((DWORD)-1);
	EnterCriticalSection(&m_csResVector);

	bFoundOld = TRUE;
	for(iter=m_vResVector.begin(); m_vResVector.end()!=iter; ) {
		if(*iter == handle) {
			bFoundOld = FALSE;
			iter = m_vResVector.erase(iter);
		} else ++iter;
	}

	LeaveCriticalSection(&m_csResVector);
	return bFoundOld;
}

DWORD CResVector::CloseResource() {
	vector<VOID *>::iterator iter;

	EnterCriticalSection(&m_csResVector);
	for(iter=m_vResVector.begin(); m_vResVector.end()!=iter; ) {
		if(INVALID_HANDLE_VALUE != *iter) CloseHandle(*iter);
		iter = m_vResVector.erase(iter);
	}
	LeaveCriticalSection(&m_csResVector);

	return 0x00;
}
