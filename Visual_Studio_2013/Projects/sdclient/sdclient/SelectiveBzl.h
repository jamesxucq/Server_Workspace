#pragma once
#include "clientcom/clientcom.h"

class CSelectiveObject {
public:
	CSelectiveObject(void);
	~CSelectiveObject(void);
public:
#define DOWND_THREAD_FAULT			-1
#define DOWND_THREAD_SUCCESS			0x0000
#define DOWND_THREAD_WAITING			0x0001
#define DOWND_THREAD_ACTIVE			0x0003
#define DOWND_THREAD_DISCONNECT		0x0005
#define DOWND_THREAD_LOCALBUSY		0x0007
#define DOWND_THREAD_SERVBUSY		0x0009
	volatile DWORD m_dwDowndThreadStatus;
public:
	vector <TCHAR *> m_vDiresVec;
	DiresHmap m_mTViewHmap;
};
extern class CSelectiveObject objSelectObject;


namespace NSelectiveBzl {
	inline DWORD GetDowndThreadStatus() {
		DWORD dwThreadStatus;
		InterlockedExchange((LONG*)&dwThreadStatus, objSelectObject.m_dwDowndThreadStatus);
		return dwThreadStatus;
	}
	inline VOID SetDowndThreadStatus(DWORD dwThreadStatus)
	{ InterlockedExchange((LONG*)&objSelectObject.m_dwDowndThreadStatus, dwThreadStatus); }
	//
	VOID ClearDiresAttrib(const TCHAR *szDriveLetter, FileFobdVec *pFobdVec);
	DWORD MakeLocalChanged(FileFobdVec *pFobdVec, DWORD dwAdded, const TCHAR *szDriveLetter);
	DWORD FinishDirectoriesTView(CAdvaTView *tvDiresTView);
	//
#define DOWND_VALUE_FAILED			((DWORD)-1)
#define DOWND_VALUE_SUCCESS			0x0000
#define DOWND_PROECSS_BUSY			0x0001
#define DOWND_VALUE_DISCONN			0x0003
#define DOWND_VALUE_QUERYERR		0x0005
#define DOWND_VALUE_SERVBUSY		0x0007
	DWORD PerfromDirectoriesDownd();
	DWORD PrepaDisplayTView();
	VOID FinalizeEnviro(CAdvaTView *tvDiresTView);
	//
#define NORMAL_SELECTIVE			0x0001
#define ADVANCED_SELECTIVE			0x0003
	int GetSelectiveDlgType();
	//
	inline DiresVec *GetDiresVec()
	{ return &objSelectObject.m_vDiresVec; }
	inline DiresHmap *GetDiresTViewHmap() 
	{ return &objSelectObject.m_mTViewHmap; }
	//
// void InitDiresTreeTest();
};

