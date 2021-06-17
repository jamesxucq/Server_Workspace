#pragma once

#include "TRANSObject.h"

//
#define RETRY_PRODUCER_TIMES	0x0002 // modify by james 20140225 for test
#define RETRY_EXCEPTION_TIMES	0x0002
//#define RETRY_EXCEPTION_TIMES	0x0001 // modify by james 20140225 for test
#define RETRY_EXCEPTION_FIRST	0x0001
#define RETRY_EXCEPTION_SECOND	0x0002
//
namespace NExecuteUtility {
	inline DWORD GetServAnchor()
	{ DWORD dwLastServAnchor; InterlockedExchange((LONG*)&dwLastServAnchor, TRANS_OBJECT(m_dwLastServAnchor)); return dwLastServAnchor; }
	inline VOID SetServAnchor(DWORD dwLastServAnchor)
	{ InterlockedExchange((LONG*)&TRANS_OBJECT(m_dwLastServAnchor), dwLastServAnchor); }
	//
	DWORD TasksListProducerNOR(struct LocalConfig *pLocalConfig);
	DWORD TasksListProducerEXP();
	DWORD TasksListVerifier(struct LocalConfig *pLocalConfig);
	DWORD TasksListHandler(struct LocalConfig *pLocalConfig);
	//
	DWORD FinishTasksList(DWORD *dwLockAnchor, struct LocalConfig *pLocalConfig, DWORD handle_value, DWORD dwChanValue);
	DWORD GetFilesByDirectory(FilesVec *pFilesVec, const TCHAR *szPath);
	//
	VOID BeginRefreshIcon(TCHAR *szDriveLetter);
	VOID FailedRefreshIcon(TCHAR *szDriveLetter);
	static VOID BeginRefreshExcept();
	static VOID FailedRefreshExcept();
}

