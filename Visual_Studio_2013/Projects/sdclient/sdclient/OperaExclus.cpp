#include "StdAfx.h"

#include "clientcom/clientcom.h"
#include "OperaExclus.h"

COperaExclusi::COperaExclusi(void)
{
	InitializeCriticalSection(&m_gcsOperaExclus);
	m_dwOperaExclus = 0x00;
}

COperaExclusi::~COperaExclusi(void)
{
	DeleteCriticalSection(&m_gcsOperaExclus);
}

COperaExclusi objOperaExclus;

DWORD COperaExclusi::PassValid() {
DWORD dwPassValid = 0x00;
// _LOG_DEBUG(_T("pass m_dwOperaExclus:%08X"), m_dwOperaExclus); //
	EnterCriticalSection(&m_gcsOperaExclus);
	if(!m_dwOperaExclus) {
		m_dwOperaExclus++;
		dwPassValid = 0x01;
	}
	LeaveCriticalSection(&m_gcsOperaExclus);
// _LOG_DEBUG(_T("pass dwPassValid:%08X"), dwPassValid); //
	return dwPassValid;
}

VOID COperaExclusi::ReleaseExclusive(DWORD dwPassExclus) {
	if(dwPassExclus) {
		EnterCriticalSection(&m_gcsOperaExclus);
		m_dwOperaExclus = 0x00;
// _LOG_DEBUG(_T("release m_dwOperaExclus:%08X"), m_dwOperaExclus); //
		LeaveCriticalSection(&m_gcsOperaExclus);	
	}
}

DWORD COperaExclusi::GetExclus() {
	DWORD dwExclusValue;
//
	EnterCriticalSection(&m_gcsOperaExclus);
	dwExclusValue = m_dwOperaExclus;
	LeaveCriticalSection(&m_gcsOperaExclus);
//
	return dwExclusValue;
}