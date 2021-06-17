#include "StdAfx.h"
#include "Resource.h"

#include <afxpriv.h>		// for AfxLoadString
#include "sdTrayIcon.h"

CsdTrayIcon::CsdTrayIcon(void):
m_hiconNormal(NULL),
m_hiconPaused(NULL),
m_hiconSyncok(NULL),
m_hiconSyncingFirst(NULL),
m_hiconSyncingSecond(NULL),
m_pTrayIcon(NULL),
m_bShowBalloonTip(FALSE),
m_hNotifyWnd(NULL),
m_bHaveAnimateTimer(FALSE)
{
	INIT_TRAY_FLAGS(m_dwTrayInitFlags)
		InitializeCriticalSection(&m_csTrayNotify);
}

CsdTrayIcon::~CsdTrayIcon(void) {
	DeleteCriticalSection(&m_csTrayNotify);
}

CsdTrayIcon objTrayIcon;

// MFC临界区类对象
DWORD CsdTrayIcon::Create(CWnd* pNotifyWnd) {
	if(!SETTRAY_CHECK_INIT(m_dwTrayInitFlags)) {
		m_hiconNormal = AfxGetApp()->LoadIcon(IDI_NORMAL);
		m_hiconSyncingFirst = AfxGetApp()->LoadIcon(IDI_FIRST_SYNCING);
		m_hiconSyncingSecond = AfxGetApp()->LoadIcon(IDI_SECOND_SYNCING);
		m_hiconPaused = AfxGetApp()->LoadIcon(IDI_PAUSED);
		m_hiconSyncok = AfxGetApp()->LoadIcon(IDI_SYNCOK);
//
		m_bitmapPaused.LoadBitmap(IDB_PAUSED);
		m_bitmapRuning.LoadBitmap(IDB_RUNING);
		m_bitmapConnect.LoadBitmap(IDB_CONNECT);
//
		SETTRAY_INIT_OKAY(m_dwTrayInitFlags);
	}
	// Set up tray icon
	DWORD dwTrayValue = 0;
	if(!NEWTRAY_CHECK_INIT(m_dwTrayInitFlags)) {
		m_hNotifyWnd = pNotifyWnd->m_hWnd;
//
		if(m_pTrayIcon) delete m_pTrayIcon;
		m_pTrayIcon = new CTrayIcon(IDR_TRAYICON_MENU);
		if(m_pTrayIcon) {
			m_pTrayIcon->SetNotificationWnd(pNotifyWnd, WM_TRAY_NOTIFICATION);
			EnterCriticalSection(&m_csTrayNotify);
			m_pTrayIcon->SetIcon(m_hiconNormal, NULL);
			LeaveCriticalSection(&m_csTrayNotify);
		} else dwTrayValue = ((DWORD)-1);
//
		NEWTRAY_INIT_OKAY(m_dwTrayInitFlags);
	}
	return dwTrayValue;
}

DWORD CsdTrayIcon::Destroy() {
	if(SETTRAY_CHECK_INIT(m_dwTrayInitFlags)) {
		// SETTRAY_FINAL_OKAY(m_dwTrayInitFlags);
	}
	if(NEWTRAY_CHECK_INIT(m_dwTrayInitFlags)) {
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->SetIcon(NULL, NULL);
		LeaveCriticalSection(&m_csTrayNotify);
		if(m_pTrayIcon) delete m_pTrayIcon;
		m_pTrayIcon = NULL;
//
		NEWTRAY_FINAL_OKAY(m_dwTrayInitFlags);
	}
	return 0x00;
}

//
BOOL CsdTrayIcon::BalloonTipEnable(LPCTSTR szMessage, LPCTSTR szTitle, DWORD dwInfoFlags) {
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetTip(szMessage);
	BOOL bDisplayValue = m_pTrayIcon->ShowBalloonTip(szMessage, szTitle, BALLOONTIP_TIMEOUT, dwInfoFlags);
	LeaveCriticalSection(&m_csTrayNotify);
	//
	return bDisplayValue;
}

BOOL CsdTrayIcon::BalloonTip(LPCTSTR szMessage, LPCTSTR szTitle) {
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetTip(szMessage);
	BOOL bDisplayValue = m_bShowBalloonTip? m_pTrayIcon->ShowBalloonTip(szMessage, szTitle, BALLOONTIP_TIMEOUT): TRUE;
	LeaveCriticalSection(&m_csTrayNotify);
	//
	return bDisplayValue;
}

const UINT nTrayiconTimerID = 0x0003;

VOID CALLBACK AnimateTimerFunction(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime);

#define	KILL_ANIMATE_TIMER \
	if(m_bHaveAnimateTimer) { \
	KillTimer(m_hNotifyWnd, nTrayiconTimerID); \
	m_bHaveAnimateTimer = FALSE; \
}

VOID CsdTrayIcon::TrayiconConnecting(LPCTSTR szMessage, DWORD dwEnableBalloonTip) {
	if(!szMessage) return ;
	//
	KILL_ANIMATE_TIMER
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetIcon(m_hiconNormal, szMessage);
	LeaveCriticalSection(&m_csTrayNotify);
	//
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		pSubMenu->ModifyMenuW(ID_PROCESS_STATUS, MF_BYCOMMAND, ID_PROCESS_STATUS, _T("连接服务器..."));
		pSubMenu->SetMenuItemBitmaps(ID_PROCESS_STATUS, MF_BYCOMMAND, &m_bitmapConnect, &m_bitmapConnect);
		pSubMenu->EnableMenuItem(ID_PROCESS_STATUS, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);	
	}
	//
	if(m_bShowBalloonTip && dwEnableBalloonTip) {
		TCHAR szTitle[128];
		AfxLoadString(IDS_BALLOONTIP, szTitle, sizeof(szTitle));
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->ShowBalloonTip(szMessage, szTitle, BALLOONTIP_TIMEOUT);
		LeaveCriticalSection(&m_csTrayNotify);
	}
}

VOID CsdTrayIcon::TrayiconConnected() {
	KILL_ANIMATE_TIMER
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetIcon(m_hiconSyncok, _T("已连接上服务器."));
	LeaveCriticalSection(&m_csTrayNotify);
	//
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		pSubMenu->EnableMenuItem(ID_PROCESS_STATUS, MF_BYCOMMAND|MF_ENABLED);	
	}
}

VOID CsdTrayIcon::TrayiconNoraml() {
	KILL_ANIMATE_TIMER
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetIcon(m_hiconNormal, _T("已连接上服务器."));
	LeaveCriticalSection(&m_csTrayNotify);
	//
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		pSubMenu->ModifyMenuW(ID_PROCESS_STATUS, MF_BYCOMMAND, ID_PROCESS_STATUS, _T("已连接服务器"));
		pSubMenu->SetMenuItemBitmaps(ID_PROCESS_STATUS, MF_BYCOMMAND, &m_bitmapConnect, &m_bitmapConnect);	
	}
}

VOID CsdTrayIcon::TrayiconPaused() {
	KILL_ANIMATE_TIMER
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetIcon(m_hiconPaused, _T("暂停同步."));
	LeaveCriticalSection(&m_csTrayNotify);
	//
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		pSubMenu->ModifyMenuW(ID_PROCESS_STATUS, MF_BYCOMMAND, ID_PROCESS_STATUS, _T("开始同步"));
		pSubMenu->SetMenuItemBitmaps(ID_PROCESS_STATUS, MF_BYCOMMAND, &m_bitmapRuning, &m_bitmapRuning);	
	}
}

VOID CsdTrayIcon::TrayiconSuccess(DWORD dwEnableBalloonTip) {
	KILL_ANIMATE_TIMER
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetIcon(m_hiconSyncok, _T("所有文件都以更新."));
	LeaveCriticalSection(&m_csTrayNotify);
	//
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		pSubMenu->ModifyMenuW(ID_PROCESS_STATUS, MF_BYCOMMAND, ID_PROCESS_STATUS, _T("暂停同步"));
		pSubMenu->SetMenuItemBitmaps(ID_PROCESS_STATUS, MF_BYCOMMAND, &m_bitmapPaused, &m_bitmapPaused);	
	}
	//
	if(m_bShowBalloonTip && dwEnableBalloonTip) {
		TCHAR szTitle[128];
		AfxLoadString(IDS_BALLOONTIP, szTitle, sizeof(szTitle));
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->ShowBalloonTip(_T("所有文件都以更新."), szTitle, BALLOONTIP_TIMEOUT);
		LeaveCriticalSection(&m_csTrayNotify);
	}
}

VOID CsdTrayIcon::TrayiconWaiting(LPCTSTR szMessage) {
	KILL_ANIMATE_TIMER
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetIcon(m_hiconSyncok, _T("等待远端同步完成."));
	LeaveCriticalSection(&m_csTrayNotify);
	//
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		pSubMenu->ModifyMenuW(ID_PROCESS_STATUS, MF_BYCOMMAND, ID_PROCESS_STATUS, _T("暂停同步"));
		pSubMenu->SetMenuItemBitmaps(ID_PROCESS_STATUS, MF_BYCOMMAND, &m_bitmapPaused, &m_bitmapPaused);	
	}
	//
	if(m_bShowBalloonTip) {
		TCHAR szTitle[128];
		AfxLoadString(IDS_BALLOONTIP, szTitle, sizeof(szTitle));
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->ShowBalloonTip(szMessage, szTitle, BALLOONTIP_TIMEOUT);
		LeaveCriticalSection(&m_csTrayNotify);
	}
}

VOID CsdTrayIcon::TrayiconSyncing(LPCTSTR szMessage) {
	if(!szMessage) return ;
	//
	if(!m_bHaveAnimateTimer) {
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->SetIcon(m_hiconSyncingFirst, _T("正在进行同步."));
		LeaveCriticalSection(&m_csTrayNotify);
		SetTimer(m_hNotifyWnd, nTrayiconTimerID, 1024, AnimateTimerFunction);
		m_bHaveAnimateTimer = TRUE;
	}
	//
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		pSubMenu->ModifyMenuW(ID_PROCESS_STATUS, MF_BYCOMMAND, ID_PROCESS_STATUS, _T("暂停同步"));
		pSubMenu->SetMenuItemBitmaps(ID_PROCESS_STATUS, MF_BYCOMMAND, &m_bitmapPaused, &m_bitmapPaused);	
	}
	//
	if(m_bShowBalloonTip) {
		TCHAR szTitle[128];
		AfxLoadString(IDS_BALLOONTIP, szTitle, sizeof(szTitle));
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->ShowBalloonTip(szMessage, szTitle, BALLOONTIP_TIMEOUT);
		LeaveCriticalSection(&m_csTrayNotify);
	}
}

VOID CsdTrayIcon::TrayiconPerformance(LPCTSTR szTip) {
	EnterCriticalSection(&m_csTrayNotify);
	m_pTrayIcon->SetTip(szTip);
	LeaveCriticalSection(&m_csTrayNotify);
}

VOID CsdTrayIcon::AnimateSyncingTrayicon() {
	static DWORD dwFrameCounter;
	//
	if(POW2N_MOD(dwFrameCounter++, 2)) {
		// m_pTrayIcon->SetIcon(m_hiconSyncingFirst, _T("正在同步."));
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->SetIcon(m_hiconSyncingFirst, NULL);
		LeaveCriticalSection(&m_csTrayNotify);
		// _LOG_DEBUG(_T("----------- set icon first nCounter:%d-----------"), dwFrameCounter);
	} else {
		// m_pTrayIcon->SetIcon(m_hiconSyncingSecond, _T("正在同步."));
		EnterCriticalSection(&m_csTrayNotify);
		m_pTrayIcon->SetIcon(m_hiconSyncingSecond, NULL);
		LeaveCriticalSection(&m_csTrayNotify);
		// _LOG_DEBUG(_T("----------- set icon second nCounter:%d-----------"), dwFrameCounter);
	}
	// SendMessage(m_hNotifyWnd, WM_SETICON, NULL, (long) m_hiconSyncingFirst);
}

VOID CALLBACK AnimateTimerFunction(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime) {
	objTrayIcon.AnimateSyncingTrayicon();
}

/*	Function Name   : TimerAnimationIcon
Description		: Function which will keep track of the current displayed icons, based on that
this will select the next icon to be displayed by sending a modify message type
to the system tray icon.
Function Called	: AnimateIcon
Global Variable	: m_nCounter - Keep track of the icon which is currently dispalyed
nCounter   - Incremented one by one to select the next icon from the icon list.
*/
/*
VOID TimerAnimationIcon(HINSTANCE hInst, HWND hWnd) {
AnimateIcon(hInst, hWnd, NIM_MODIFY, nCounter);
m_nCounter = nCounter; // Store it here to keep track of the last displayed icon
nCounter++;
nCounter = nCounter%(NUM_ICON_FOR_ANIMATION);
}
*/
/*	Function Name   : AnimateIcon
Description		: Function which will act based on the message type that is received as parameter
like ADDI, MODIFY, DELETE icon in the system tray. Also send a message to display
the icon in title bar as well as in the task bar application.
Function Called	: Shell_NotifyIcon	-	API which will Add, Modify, Delete icon in tray.
SendMessage - Send a message to windows
Variable		: NOTIFYICONDATA - Structure which will have the details of the tray icons
*/
/*
VOID AnimateIcon(HINSTANCE hInstance, HWND hWnd, DWORD dwMsgType,UINT nIndexOfIcon) {
HICON hIconAtIndex = LoadIcon(hInstance, (LPCTSTR) MAKEINTRESOURCE(IconResourceArray[nIndexOfIcon]));

NOTIFYICONDATA IconData;

IconData.cbSize = sizeof(NOTIFYICONDATA);
IconData.hIcon  = hIconAtIndex;
IconData.hWnd   = hWnd;
lstrcpyn(IconData.szTip,"Animated System Tray Icons", (int) strlen("Animated System Tray Icons")+1);
IconData.uCallbackMessage = MYMSG_NOTIFYICON;
IconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

Shell_NotifyIcon(dwMsgType, &IconData);
SendMessage(hWnd, WM_SETICON, NULL, (long) hIconAtIndex);

if(hIconAtIndex)
DestroyIcon(hIconAtIndex);
}
*/

VOID CsdTrayIcon::MenuOptionsEnable(BOOL bEnable) {
	CMenu *pSubMenu = m_pTrayIcon->GetTrayMeun()->GetSubMenu(0);
	if(pSubMenu) {
		if(bEnable) pSubMenu->EnableMenuItem(ID_OPTIONS_OPEN, MF_BYCOMMAND|MF_ENABLED);
		else pSubMenu->EnableMenuItem(ID_OPTIONS_OPEN, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);	
	}
}