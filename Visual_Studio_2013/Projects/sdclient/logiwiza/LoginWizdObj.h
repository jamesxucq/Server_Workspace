#pragma once
#include "LoginWizdDlg.h"

//
// format status code
#define FORMAT_STATUS_FAULT		-1
#define FORMAT_STATUS_SUCCESS	0x0000
#define FORMAT_STATUS_CREATE	0x0001
#define FORMAT_STATUS_MOUNT		0x0002
#define FORMAT_STATUS_FORMAT	0x0004
#define FORMAT_STATUS_UNMOUNT	0x0008
#define FORMAT_STATUS_FAILED	0x0010
#define FORMAT_STATUS_WAITING	0x0011

class CLoginWizdObj {
public:
	CLoginWizdObj(void);
	~CLoginWizdObj(void);
public:
	CLoginWizdDlg *m_pCVolWizardDlg;
	DWORD	m_dwCurrentNumbe;		/* The current wizard page */
/*
login fail: 0x000000FF
new setting: 0x00000000
exists setting: 0x0000FF00
format fail: 0x00FF0000
unused: 0xFF000000
*/
	volatile DWORD m_dwCVolStatus; //0:succ 1: 2: 3:
	HWND m_hCurrentPageHandle;		/* Handle to current wizard page */
public:
	inline CLoginWizdDlg * GetCVolWizardDlgPoint() { return m_pCVolWizardDlg; }
	inline DWORD *GetCurrentNumbePoint() { return &m_dwCurrentNumbe; }
public:
	volatile DWORD m_dwFormatStatus;
};

extern CLoginWizdObj objLoginWizdObj;
#define LWIZARD_OBJECT(member)	objLoginWizdObj.member