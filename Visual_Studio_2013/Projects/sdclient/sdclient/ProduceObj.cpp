#include "StdAfx.h"

#include "ProduceObj.h"

//
CVerifyObj::CVerifyObj(void)
:m_dwVerifyThreadsCount(0)
,m_hVerifyThreads(NULL) 
{
	InitializeCriticalSection(&m_csVerifyThreads);
}

CVerifyObj::~CVerifyObj(void)
{
	DeleteCriticalSection(&m_csVerifyThreads);
}

CVerifyObj objVerify;

DWORD CVerifyObj::SetVerifyThreads(HANDLE *hVerifyThreads, DWORD dwThreadsCount) {
	EnterCriticalSection(&m_csVerifyThreads);
	m_dwVerifyThreadsCount = dwThreadsCount;
	m_hVerifyThreads = hVerifyThreads;
	LeaveCriticalSection(&m_csVerifyThreads);

	return 0x00;
}

HANDLE *CVerifyObj::GetVerifyThreads(DWORD *pVerifyThreadsCount) {
	if(!pVerifyThreadsCount) return NULL;

	EnterCriticalSection(&m_csVerifyThreads);
	*pVerifyThreadsCount = m_dwVerifyThreadsCount;
	HANDLE *hVerifyThreads = m_hVerifyThreads;
	LeaveCriticalSection(&m_csVerifyThreads);

	return hVerifyThreads;
}

//
CProduceObj::CProduceObj(void)
:m_dwListThreadsCount(0)
,m_hListThreads(NULL) 
{
	InitializeCriticalSection(&m_csListThreads);
}

CProduceObj::~CProduceObj(void)
{
	DeleteCriticalSection(&m_csListThreads);
}

CProduceObj objProduce;

DWORD CProduceObj::SetListThreads(HANDLE *hListThreads, DWORD dwThreadsCount) {
	EnterCriticalSection(&m_csListThreads);
	m_dwListThreadsCount = dwThreadsCount;
	m_hListThreads = hListThreads;
	LeaveCriticalSection(&m_csListThreads);

	return 0x00;
}

HANDLE *CProduceObj::GetListThreads(DWORD *pListThreadsCount) {
	if(!pListThreadsCount) return NULL;

	EnterCriticalSection(&m_csListThreads);
	*pListThreadsCount = m_dwListThreadsCount;
	HANDLE *hListThreads = m_hListThreads;
	LeaveCriticalSection(&m_csListThreads);

	return hListThreads;
}

//
