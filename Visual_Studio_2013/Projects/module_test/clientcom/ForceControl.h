#pragma once

#define FORCE_CONTROL_DEFAULT				_T("~\\ForceControl.flag")

class CForceControl {
public:
	CForceControl(void);
	~CForceControl(void);
public:
	DWORD Initialize(TCHAR *szLocation);
	DWORD Finalize();
private:
	TCHAR m_szForceControl[MAX_PATH];
	HANDLE m_hForceControl;
public:
#define STYPE_FORCE_FAULT			-1
#define STYPE_FORCE_NORMAL			0x0000
#define STYPE_FORCE_SLOWLY			0x0001
	DWORD LoadForceControl(); //0 is slow; 1 is fast; -1 error
	DWORD SaveForceControl(DWORD dwForceType); //0 is slow; 1 is fast; -1 error
};

extern CForceControl objForceControl;