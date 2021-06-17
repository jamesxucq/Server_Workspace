#pragma once

#include "clientcom/clientcom.h"

#define WM_TRAY_NOTIFICATION  WM_USER + 0x0100
#define BALLOONTIP_TIMEOUT  3000

class CsdTrayIcon {
public:
    CsdTrayIcon(void);
    ~CsdTrayIcon(void);
private:
    //
    CTrayIcon	*m_pTrayIcon;		// my tray icon
    //
    HICON		m_hiconNormal;
    HICON		m_hiconPaused;
    HICON		m_hiconSyncok;
    HICON		m_hiconSyncingFirst;
    HICON		m_hiconSyncingSecond;
    //
    CBitmap		m_bitmapPaused;
    CBitmap		m_bitmapRuning;
    CBitmap		m_bitmapConnect;
private:
    DWORD m_dwTrayInitFlags;
    //
    CRITICAL_SECTION m_csTrayNotify;
public:
    DWORD Create(CWnd* pNotifyWnd);
    DWORD Destroy();
private:
    volatile BOOL m_bShowBalloonTip;	// display info in main window
public:
    inline VOID ShowBalloonTip(BOOL bShowTip) {
        m_bShowBalloonTip = bShowTip;
    }
    // BOOL TarySetTip(LPCTSTR lpTip);
    BOOL BalloonTipEnable(LPCTSTR szMessage, LPCTSTR szTitle, DWORD dwInfoFlags);
    BOOL BalloonTip(LPCTSTR szMessage, LPCTSTR szTitle);
    //
public:
    VOID TrayiconConnecting(LPCTSTR szMessage, DWORD dwEnableBalloonTip);
    VOID TrayiconConnected();
public:
    VOID TrayiconNoraml();
    VOID TrayiconPaused();
    VOID TrayiconSuccess(DWORD dwEnableBalloonTip);
    VOID TrayiconWaiting(LPCTSTR szMessage);
private:
    HWND m_hNotifyWnd;
    BOOL m_bHaveAnimateTimer;
public:
    VOID AnimateSyncingTrayicon();
    VOID TrayiconSyncing(LPCTSTR szMessage);
	VOID TrayiconPerformance(LPCTSTR szTip);
public:
	VOID MenuOptionsEnable(BOOL bEnable);
};

extern CsdTrayIcon objTrayIcon;


#define INIT_TRAY_FLAGS(FLAGS)					FLAGS = 0;

#define NEWTRAY_INIT_OKAY(FLAGS)				FLAGS |= 0x00000001
#define SETTRAY_INIT_OKAY(FLAGS)				FLAGS |= 0x00000002

#define NEWTRAY_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFFE
#define SETTRAY_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFFD

#define NEWTRAY_CHECK_INIT(FLAGS)				(FLAGS & 0x00000001)
#define SETTRAY_CHECK_INIT(FLAGS)				(FLAGS & 0x00000002)