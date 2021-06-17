#pragma once

class CLocalObj {
public:
	CLocalObj(void);
	~CLocalObj(void);
public:
	inline DWORD Create(HWND hNotifyWnd) { m_hNotifyWnd = hNotifyWnd; return 0x00; }
	inline VOID Destroy() { };
public:
	HWND		m_hNotifyWnd;
	BOOL		m_bShutdown;	// OK to terminate TRAYTEST
};

extern class CLocalObj	objLocalObj;
#define LOCAL_OBJECT(member) objLocalObj.member