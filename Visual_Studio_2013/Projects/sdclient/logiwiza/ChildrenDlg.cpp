#include "StdAfx.h"

#include "LoginWizdBzl.h"
#include "LoginWizdDlg.h"
#include "ChildrenDlg.h"

CChildrenDlg::CChildrenDlg(void):
    m_pParentWnd(NULL)
    ,m_pCoverDlg(NULL)
    ,m_pLoginDlg(NULL)
    ,m_pDiskSettingDlg(NULL)
    ,m_pFormatDlg(NULL)
    ,m_pFinishDlg(NULL)
{
    DISPLAY_FLAGS_HIDE(m_dwDisplayWindowsFlags);
}

CChildrenDlg::~CChildrenDlg(void) {
}

BOOL CChildrenDlg::Create(LPRECT lpRect, CWnd* pParent/* = NULL*/) {
    if(!lpRect || !pParent) return FALSE;
    m_pParentWnd = pParent;

    m_pCoverDlg = new CCoverDlg(pParent);
    m_pCoverDlg->Create(IDD_COVER_DLG, pParent);
    m_pCoverDlg->MoveWindow(lpRect);

    m_pLoginDlg = new CLoginDlg(pParent);
    m_pLoginDlg->Create(IDD_LOGIN_PAGE_DLG, pParent);
    m_pLoginDlg->MoveWindow(lpRect);

    m_pDiskSettingDlg = new CDiskSettingDlg(pParent);
    m_pDiskSettingDlg->Create(IDD_DISKSETTING_PAGE_DLG, pParent);
    m_pDiskSettingDlg->MoveWindow(lpRect);

    m_pFormatDlg = new CFormatDlg(pParent);
    m_pFormatDlg->Create(IDD_FORMAT_PAGE_DLG, pParent);
    m_pFormatDlg->MoveWindow(lpRect);

    m_pFinishDlg = new CFinishDlg(pParent);
    m_pFinishDlg->Create(IDD_FINISH_DLG, pParent);
    m_pFinishDlg->MoveWindow(lpRect);

    return TRUE;
}


BOOL CChildrenDlg::Destroy() {
    m_pFinishDlg->DestroyWindow();
    delete	m_pFinishDlg;

    m_pFormatDlg->DestroyWindow();
    delete	m_pFormatDlg;

    m_pDiskSettingDlg->DestroyWindow();
    delete  m_pDiskSettingDlg;

    m_pLoginDlg->DestroyWindow();
    delete	m_pLoginDlg;

    m_pCoverDlg->DestroyWindow();
    delete  m_pCoverDlg;

    return TRUE;
}

/*
bool CChildrenDlg::SetChildrenValue() {
m_pCoverDlg->SetCoverAttrib();
m_pDiskSettingDlg->SetGeneralValue();
m_pFormatDlg->SetAccountValue();
m_pLoginDlg->SetBandwidthValue();
m_pFinishDlg->SetProxieszValue();

return true;
}
*/
/*
bool CChildrenDlg::GetChildrenValue() {
// m_pCoverDlg->GetCoverAttrib();
m_pDiskSettingDlg->GetDiskSettingValue();
m_pFormatDlg->GetFormatValue();
m_pLoginDlg->GetLoginValue();
// m_pFinishDlg->GetFinishAttrib();

return true;
}
*/

HWND CChildrenDlg::DisplayNormalWindow(DWORD dwWizardNumbe) {
    if(COVER_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pCoverDlg->ShowWindow(SW_HIDE);
        COVER_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(LOGIN_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pLoginDlg->ShowWindow(SW_HIDE);
        LOGIN_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(DISKSETTING_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pDiskSettingDlg->ShowWindow(SW_HIDE);
        DISKSETTING_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(FORMAT_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pFormatDlg->ShowWindow(SW_HIDE);
        FORMAT_PAGE_HIDE(m_dwDisplayWindowsFlags);
    } else if(FINISH_CHECK_SHOW(m_dwDisplayWindowsFlags)) {
        m_pFinishDlg->ShowWindow(SW_HIDE);
        FINISH_PAGE_HIDE(m_dwDisplayWindowsFlags);
    }

    HWND hWndReturn;
    switch (dwWizardNumbe) {
    case COVER_PAGE:
        // m_pCoverDlg->SetCoverAttrib();
        m_pCoverDlg->ShowWindow(SW_SHOW);
        hWndReturn = m_pCoverDlg->m_hWnd;
        COVER_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case LOGIN_PAGE:
        m_pLoginDlg->SetLoginValue();
        m_pLoginDlg->ShowWindow(SW_SHOW);
        hWndReturn = m_pLoginDlg->m_hWnd;
        LOGIN_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case DISKSETTING_PAGE:
        m_pDiskSettingDlg->SetDiskSettingValue();
        m_pDiskSettingDlg->ShowWindow(SW_SHOW);
        hWndReturn = m_pDiskSettingDlg->m_hWnd;
        DISKSETTING_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case START_FORMAT_PAGE:
    case FINISH_FORMAT_PAGE:
        m_pFormatDlg->SetFormatValue();
        m_pFormatDlg->ShowWindow(SW_SHOW);
        hWndReturn = m_pFormatDlg->m_hWnd;
        FORMAT_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    case FINISH_PAGE:
        m_pFinishDlg->SetFinishValue(LoginWizdBzl::GetCVolStatus());
        m_pFinishDlg->ShowWindow(SW_SHOW);
        hWndReturn = m_pFinishDlg->m_hWnd;
        FINISH_PAGE_SHOW(m_dwDisplayWindowsFlags);
        break;
    }
    return hWndReturn;
}

void CChildrenDlg::DisplayActionWindow(DWORD dwWizardNumbe) {
    switch (dwWizardNumbe) {
    case COVER_PAGE:
        break;
    case DISKSETTING_PAGE:
        break;
    case LOGIN_PAGE:
        m_pLoginDlg->GetLoginValue();
        m_pLoginDlg->m_stcLoginStatus.SetWindowText(_T("等待认证..."));
        m_pLoginDlg->UpdateData(FALSE);
        break;
    case START_FORMAT_PAGE:
        //分配 挂载 格式化磁盘空间
        m_pFormatDlg->m_proFormatStatus.SetPos(0);
        m_pFormatDlg->m_stcFormatStatus.SetWindowText(_T("等待创建空间..."));
        m_pFormatDlg->UpdateData(FALSE);
        break;
    case FINISH_FORMAT_PAGE:
        m_pFormatDlg->m_stcFormatStatus.SetWindowText(_T("创建空间完成..."));
        m_pFormatDlg->UpdateData(FALSE);
        break;
    case FINISH_PAGE:
        break;
    }
}

void CChildrenDlg::DisplayFinishWindow(DWORD dwWizardNumbe) {
    switch (dwWizardNumbe) {
    case COVER_PAGE:
        break;
    case LOGIN_PAGE:
        m_pLoginDlg->m_stcLoginStatus.SetWindowText(_T(""));
        m_pLoginDlg->UpdateData(FALSE);
        break;
    case DISKSETTING_PAGE:
        m_pDiskSettingDlg->GetDiskSettingValue();
        break;
    case START_FORMAT_PAGE:
        break;
    case FINISH_FORMAT_PAGE:
        m_pFormatDlg->GetFormatValue();
        m_pFormatDlg->m_stcFormatStatus.SetWindowText(_T(""));
        m_pFormatDlg->m_proFormatStatus.SetPos(0);
        m_pFormatDlg->UpdateData(FALSE);
        break;
    case FINISH_PAGE:
        break;
    }
}