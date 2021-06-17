#include "StdAfx.h"
#include "OptionsDlg.h"
#include "ChildrenDlg.h"

CChildrenDlg::CChildrenDlg(void):
    m_pGeneralDlg(NULL)
    ,m_pAccountDlg(NULL)
    ,m_pBandwidthDlg(NULL)
    ,m_pProxiesDlg(NULL)
    ,m_pSecurityDlg(NULL)
    ,m_pAdvancedDlg(NULL)
    ,m_pAboutDlg(NULL)
{
    DISPLAY_FLAGS_HIDE(m_dwDisplayWindowsFlags);
}

CChildrenDlg::~CChildrenDlg(void)
{
}


BOOL CChildrenDlg::Create(LPRECT lpRect, CWnd* pParent/* = NULL*/)
{
    if(!lpRect || !pParent) return FALSE;
//
    m_pGeneralDlg = new CGeneralDlg(pParent);
    m_pGeneralDlg->Create(IDD_GENERAL_PAGE_DLG, pParent);
    m_pGeneralDlg->MoveWindow(lpRect);
//
    m_pAccountDlg = new CAccountDlg(pParent);
    m_pAccountDlg->Create(IDD_ACCOUNT_PAGE_DLG, pParent);
    m_pAccountDlg->MoveWindow(lpRect);
//
    m_pBandwidthDlg = new CBandwidthDlg(pParent);
    m_pBandwidthDlg->Create(IDD_BANDWIDTH_PAGE_DLG, pParent);
    m_pBandwidthDlg->MoveWindow(lpRect);
//
    m_pProxiesDlg = new CProxiesDlg(pParent);
    m_pProxiesDlg->Create(IDD_PROXIES_PAGE_DLG, pParent);
    m_pProxiesDlg->MoveWindow(lpRect);
//
    m_pSecurityDlg = new CSecurityDlg(pParent);
    m_pSecurityDlg->Create(IDD_SECURITY_PAGE_DLG, pParent);
    m_pSecurityDlg->MoveWindow(lpRect);
//
    m_pAdvancedDlg = new CAdvancedDlg(pParent);
    m_pAdvancedDlg->Create(IDD_ADVANCED_PAGE_DLG, pParent);
    m_pAdvancedDlg->MoveWindow(lpRect);
//
    m_pAboutDlg = new CAboutDlg(pParent);
    m_pAboutDlg->Create(IDD_ABOUT_PAGE_DLG, pParent);
    m_pAboutDlg->MoveWindow(lpRect);
//
    return TRUE;
}


BOOL CChildrenDlg::Destroy()
{
    m_pGeneralDlg->DestroyWindow();
    delete  m_pGeneralDlg;
//
    m_pAccountDlg->DestroyWindow();
    delete	m_pAccountDlg;
//
    m_pBandwidthDlg->DestroyWindow();
    delete	m_pBandwidthDlg;
//
    m_pProxiesDlg->DestroyWindow();
    delete	m_pProxiesDlg;
//
    m_pSecurityDlg->DestroyWindow();
    delete	m_pSecurityDlg;
//
    m_pAdvancedDlg->DestroyWindow();
    delete	m_pAdvancedDlg;
//
    m_pAboutDlg->DestroyWindow();
    delete	m_pAboutDlg;
//
    return TRUE;
}

DWORD CChildrenDlg::SetChildrenValue()
{
    m_pGeneralDlg->SetGeneralValue();
    m_pAccountDlg->SetAccountValue();
    m_pBandwidthDlg->SetBandwidthValue();
    m_pProxiesDlg->SetProxieszValue();
    m_pSecurityDlg->SetSecurityValue();
    m_pAdvancedDlg->SetAdvancedValue();
    m_pAboutDlg->SetAboutValue();
//
    return 0x00;
}

DWORD CChildrenDlg::GetChildrenValue()
{
    m_pGeneralDlg->GetGeneralValue();
    // m_pAccountDlg->GetAccountValue(); // modify by james 20101205
    m_pBandwidthDlg->GetBandwidthValue();
    m_pProxiesDlg->GetProxieszValue();
    m_pSecurityDlg->GetSecurityValue();
    m_pAdvancedDlg->GetAdvancedValue();
//
    return 0x00;
}

BOOL CChildrenDlg::DisplayWindow(DWORD dwOptionPage)
{
    if(GENERAL_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pGeneralDlg->ShowWindow(SW_HIDE);
        GENERAL_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(ACCOUNT_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pAccountDlg->ShowWindow(SW_HIDE);
        ACCOUNT_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(BANDWIDTH_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pBandwidthDlg->ShowWindow(SW_HIDE);
        BANDWIDTH_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(PROXIES_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pProxiesDlg->ShowWindow(SW_HIDE);
        PROXIES_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(SECURITY_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pSecurityDlg->ShowWindow(SW_HIDE);
        SECURITY_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(ADVANCED_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pAdvancedDlg->ShowWindow(SW_HIDE);
        ADVANCED_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(ABOUT_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pAboutDlg->ShowWindow(SW_HIDE);
        ABOUT_PAGE_HIDE(m_dwDisplayWindowsFlags);
    }

    switch (dwOptionPage) {
    case ENUM_GENERAL:
        m_pGeneralDlg->ShowWindow(SW_SHOW);
        GENERAL_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case ENUM_ACCOUNT:
        m_pAccountDlg->ShowWindow(SW_SHOW);
        ACCOUNT_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case ENUM_BANDWIDTH:
        m_pBandwidthDlg->ShowWindow(SW_SHOW);
        BANDWIDTH_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case ENUM_PROXIES:
        m_pProxiesDlg->ShowWindow(SW_SHOW);
        PROXIES_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case ENUM_SECURITY:
        m_pSecurityDlg->ShowWindow(SW_SHOW);
        SECURITY_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case ENUM_ADVANCED:
        m_pAdvancedDlg->ShowWindow(SW_SHOW);
        ADVANCED_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case ENUM_ABOUT:
        m_pAboutDlg->ShowWindow(SW_SHOW);
        ABOUT_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    }

    return TRUE;
}