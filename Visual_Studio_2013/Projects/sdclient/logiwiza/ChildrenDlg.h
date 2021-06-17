#pragma once

#include "CoverDlg.h"
#include "DiskSettingDlg.h"
#include "FormatDlg.h"
#include "LoginDlg.h"
#include "FinishDlg.h"

class CChildrenDlg {
public:
    CChildrenDlg(void);
    ~CChildrenDlg(void);
private:
    CWnd* m_pParentWnd;
public:
    CCoverDlg		*m_pCoverDlg;
    CLoginDlg		*m_pLoginDlg;
    CDiskSettingDlg *m_pDiskSettingDlg;
    CFormatDlg		*m_pFormatDlg;
    CFinishDlg		*m_pFinishDlg;
public:
    BOOL Create(LPRECT lpRect, CWnd* pParent = NULL);
    BOOL Destroy();
private:
    DWORD m_dwDisplayWindowsFlags;
public:
    //	bool SetChildrenValue();
    //	bool GetChildrenValue();
public:
    HWND DisplayNormalWindow(DWORD dwWizardNumbe);
    void DisplayActionWindow(DWORD dwWizardNumbe);
    void DisplayFinishWindow(DWORD dwWizardNumbe);
};

#define DISPLAY_FLAGS_HIDE(FLAGS)				FLAGS = 0;

#define COVER_PAGE_SHOW(FLAGS)					FLAGS |= 0x00000001
#define LOGIN_PAGE_SHOW(FLAGS)					FLAGS |= 0x00000002
#define DISKSETTING_PAGE_SHOW(FLAGS)			FLAGS |= 0x00000004
#define FORMAT_PAGE_SHOW(FLAGS)					FLAGS |= 0x00000008
#define FINISH_PAGE_SHOW(FLAGS)					FLAGS |= 0x00000010

#define COVER_PAGE_HIDE(FLAGS)					FLAGS &= 0xFFFFFFFE
#define LOGIN_PAGE_HIDE(FLAGS)					FLAGS &= 0xFFFFFFFD
#define DISKSETTING_PAGE_HIDE(FLAGS)			FLAGS &= 0xFFFFFFFB
#define FORMAT_PAGE_HIDE(FLAGS)					FLAGS &= 0xFFFFFFF7
#define FINISH_PAGE_HIDE(FLAGS)					FLAGS &= 0xFFFFFFEF

#define COVER_CHECK_SHOW(FLAGS)					(FLAGS & 0x00000001)
#define LOGIN_CHECK_SHOW(FLAGS)					(FLAGS & 0x00000002)
#define DISKSETTING_CHECK_SHOW(FLAGS)			(FLAGS & 0x00000004)
#define FORMAT_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000008)
#define FINISH_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000010)
