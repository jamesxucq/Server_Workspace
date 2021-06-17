#pragma once

#include "clientcom/lwizardcom.h"

#include "LoginWizdObj.h"

namespace LoginWizdBzl {
	DWORD Initialize();
	DWORD Finalize();
	//
	inline DWORD GetCVolStatus()
	{ DWORD dwCVolStatus; InterlockedExchange((LONG*)&dwCVolStatus, LWIZARD_OBJECT(m_dwCVolStatus)); return dwCVolStatus; }
	inline void SetCVolStatus(DWORD dwCVolStatus) 
	{ InterlockedExchange((LONG*)&LWIZARD_OBJECT(m_dwCVolStatus), dwCVolStatus); }
	//
	inline DWORD GetFormatStatus()
	{ DWORD dwFormatStatus; InterlockedExchange((LONG*)&dwFormatStatus, LWIZARD_OBJECT(m_dwFormatStatus)); return dwFormatStatus; }
	inline void SetFormatStatus(DWORD dwFormatStatus) 
	{ InterlockedExchange((LONG*)&LWIZARD_OBJECT(m_dwFormatStatus), dwFormatStatus); }
	//
	static DWORD SingleInstance(LPCTSTR szAppName);
	static DWORD MakeApplicaData(TCHAR *szCacheName, TCHAR *szUpdateName, TCHAR *szConfigName);
// system
#define EXECUTE_FAULT						-1
#define EXECUTE_SUCCESS						0x00000000
// exception
#define EXECUTE_ONE_INSTANCE				0x00000001
	DWORD ApplicaExecuteType();
//
#define PROGRAM_FIRSTRUN	0x00000001
#define PROGRAM_NORMALRUN	0x00000003
	DWORD LoginExecuteType();
	//
	static DWORD CheckOpdInput();
	static DWORD SaveWizardConfiguration();
	//
	static DWORD CopyFileForbid(TCHAR *szDirePath, TCHAR *szFileForbid);
	static DWORD ValidateUserspaceExist();
	//
	DWORD ValidateUserspaceExistEx(TCHAR *szNewLocation);
	//
#define ERROR_ERR			-1
#define ERROR_OKAY			0x00000000
#define ERROR_LOGIN			0x00001000
#define ERROR_FORMAT		0x00003000
	static DWORD Begain();
	static DWORD Login();
	static DWORD DiskSettings();
	static DWORD FormatBegin(HWND hFormatDlgWnd);
	static DWORD FormatFinish(HWND hFormatDlgWnd);
	static DWORD Finish();
	//
#define SPACE_NOT_EXIST		0x00000001
#define DRIVE_NOT_EXIST		0x00000002
#define ERROR_FREE_SPACE	0x00000004
#define ERROR_NOT_NTFS		0x00000008
	//#define 	0x0008
	static DWORD ValidateDiskSettings();
	DWORD FormatFinishAction();
	//
	void RefreshWindowNormal(DWORD dwCurrentNumbe);
	void PerformPrevRefresh();
	void PerformNextRefresh();
	//
	inline void SetApplicaMainDlg(CLoginWizdDlg *pCVolWizardDlg)
	{ LWIZARD_OBJECT(m_pCVolWizardDlg) = pCVolWizardDlg; }
};