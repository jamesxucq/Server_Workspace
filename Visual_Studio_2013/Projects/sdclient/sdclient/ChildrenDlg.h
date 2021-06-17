#pragma once

#include "Resource.h"
#include "GeneralDlg.h"
#include "AccountDlg.h"
#include "BandwidthDlg.h"
#include "ProxiesDlg.h"
#include "SecurityDlg.h"
#include "AdvancedDlg.h"
#include "AboutDlg.h"

class CChildrenDlg
{
public:
    CChildrenDlg(void);
    ~CChildrenDlg(void);
public:
    CGeneralDlg		*m_pGeneralDlg;
    CAccountDlg		*m_pAccountDlg;
    CBandwidthDlg	*m_pBandwidthDlg;
    CProxiesDlg		*m_pProxiesDlg;
    CSecurityDlg	*m_pSecurityDlg;
    CAdvancedDlg	*m_pAdvancedDlg;
    CAboutDlg		*m_pAboutDlg;
public:
    BOOL Create(LPRECT lpRect, CWnd* pParent = NULL);
    BOOL Destroy();
private:
    DWORD m_dwDisplayWindowsFlags;
public:
    DWORD SetChildrenValue();
    DWORD GetChildrenValue();
public:
    BOOL DisplayWindow(DWORD dwOptionPage);
};

#define DISPLAY_FLAGS_HIDE(FLAGS)				FLAGS = 0;

#define GENERAL_PAGE_SHOW(FLAGS)				FLAGS |= 0x00000001
#define ACCOUNT_PAGE_SHOW(FLAGS)				FLAGS |= 0x00000002
#define BANDWIDTH_PAGE_SHOW(FLAGS)				FLAGS |= 0x00000004
#define PROXIES_PAGE_SHOW(FLAGS)				FLAGS |= 0x00000008
#define SECURITY_PAGE_SHOW(FLAGS)				FLAGS |= 0x00000010
#define ADVANCED_PAGE_SHOW(FLAGS)				FLAGS |= 0x00000020
#define ABOUT_PAGE_SHOW(FLAGS)					FLAGS |= 0x00000040

#define GENERAL_PAGE_HIDE(FLAGS)				FLAGS &= 0xFFFFFFFE
#define ACCOUNT_PAGE_HIDE(FLAGS)				FLAGS &= 0xFFFFFFFD
#define BANDWIDTH_PAGE_HIDE(FLAGS)				FLAGS &= 0xFFFFFFFB
#define PROXIES_PAGE_HIDE(FLAGS)				FLAGS &= 0xFFFFFFF7
#define SECURITY_PAGE_HIDE(FLAGS)				FLAGS &= 0xFFFFFFEF
#define ADVANCED_PAGE_HIDE(FLAGS)				FLAGS &= 0xFFFFFFDF
#define ABOUT_PAGE_HIDE(FLAGS)					FLAGS &= 0xFFFFFFBF

#define GENERAL_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000001)
#define ACCOUNT_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000002)
#define BANDWIDTH_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000004)
#define PROXIES_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000008)
#define SECURITY_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000010)
#define ADVANCED_CHECK_SHOW(FLAGS)				(FLAGS & 0x00000020)
#define ABOUT_CHECK_SHOW(FLAGS)					(FLAGS & 0x00000040)
