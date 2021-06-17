// SecurityDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "SecurityDlg.h"

#include "OptionsDlg.h"

// CSecurityDlg 对话框

IMPLEMENT_DYNAMIC(CSecurityDlg, CDialog)

CSecurityDlg::CSecurityDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSecurityDlg::IDD, pParent)
{
    MKZEO(m_szNewPassword);
    MKZEO(m_szOldPassword);
    MKZEO(m_szSaltValue);
}

CSecurityDlg::~CSecurityDlg()
{
}

void CSecurityDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PASSWORD_CHK, m_chkClientProtect);
    DDX_Control(pDX, IDC_CLIENT_PWD_BTN, m_btnPassword);
    DDX_Control(pDX, IDC_CHANGE_USERPWD_SYSLINK, m_linkChangeUserpwd);
}


BEGIN_MESSAGE_MAP(CSecurityDlg, CDialog)
    ON_BN_CLICKED(IDC_PASSWORD_CHK, &CSecurityDlg::OnBnClickedPasswordChk)
    ON_BN_CLICKED(IDC_CLIENT_PWD_BTN, &CSecurityDlg::OnBnClickedClientPwdBtn)
    ON_NOTIFY(NM_CLICK, IDC_CHANGE_USERPWD_SYSLINK, &CSecurityDlg::OnNMClickChangeUserpwdSyslink)
END_MESSAGE_MAP()


// CSecurityDlg 消息处理程序

BOOL CSecurityDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSecurityDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
#define GET_DISPLAY_POINT \
	struct DisplayLink *pDisplayLink; \
	pDisplayLink = NConfigBzl::GetDisplayLink();

#define APPLY_BTN_ENABLE ((COptionsDlg*)m_pParentWnd)->EnableApplyButton

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

DWORD CSecurityDlg::SetSecurityValue()
{
    GET_CLIENT_POINT
    GET_DISPLAY_POINT

    _tcscpy_s(m_szNewPassword, pClientConfig->szLocalPassword);
    _tcscpy_s(m_szOldPassword, pClientConfig->szLocalPassword);
    _tcscpy_s(m_szSaltValue, pClientConfig->szSaltValue);

    TCHAR szLinkTxt[URI_LENGTH];
    m_linkChangeUserpwd.SetWindowText(struti::get_url(szLinkTxt, _T("更改用户密码"), pDisplayLink->szChangePassword));
    if (!_tcscmp(pClientConfig->szProtected, _T("true"))) {
        m_btnPassword.SetWindowText(_T("更改口令"));
        m_chkClientProtect.SetCheck(TRUE);
        m_btnPassword.EnableWindow(TRUE);
    } else {
        m_btnPassword.SetWindowText(_T("设置口令"));
        m_chkClientProtect.SetCheck(FALSE);
        m_btnPassword.EnableWindow(FALSE);
    }

    UpdateData(FALSE);
    return 0x00;
}

DWORD CSecurityDlg::GetSecurityValue()
{
    UpdateData(TRUE);
    GET_CLIENT_POINT

    if (BST_CHECKED == m_chkClientProtect.GetCheck()) {
        _tcscpy_s(pClientConfig->szProtected, _T("true"));
        // NEncoding::RandomSalt(pClientConfig->szSaltValue);
        // NEncoding::PWDChks(pClientConfig->szLocalPassword, m_szNewPassword, pClientConfig->szSaltValue);
        _tcscpy_s(pClientConfig->szLocalPassword, m_szNewPassword);
    } else {
        _tcscpy_s(pClientConfig->szLocalPassword, _T(""));
        _tcscpy_s(pClientConfig->szProtected, _T("false"));
    }
    return 0x00;
}

void CSecurityDlg::OnBnClickedPasswordChk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);

    GET_CLIENT_POINT
    if (BST_CHECKED == m_chkClientProtect.GetCheck()) {
        CAddPasswordDlg dlg;
        dlg.SetPassword(m_szNewPassword, m_szSaltValue);
        INT_PTR nResponse = dlg.DoModal();
        if (nResponse == IDOK) {
            _tcscpy_s(pClientConfig->szProtected, _T("true"));
            _tcscpy_s(pClientConfig->szSaltValue, MIN_TEXT_LEN, m_szSaltValue);
            _tcscpy_s(pClientConfig->szLocalPassword, MD5_TEXT_LENGTH, m_szNewPassword);
            NLocalBzl::SaveOptionConfig();

            m_btnPassword.SetWindowText(_T("更改口令"));
            m_btnPassword.EnableWindow(TRUE);
        } else if (nResponse == IDCANCEL) {
            //  “取消”来关闭对话框的代码
            m_chkClientProtect.SetCheck(FALSE);
            m_btnPassword.EnableWindow(FALSE);
        }
    } else {
        APPLY_BTN_ENABLE(TRUE);
        m_btnPassword.EnableWindow(FALSE);
    }

    UpdateData(FALSE);
}

void CSecurityDlg::OnBnClickedClientPwdBtn()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);

    GET_CLIENT_POINT
    if(!_tcscmp(pClientConfig->szProtected, _T("true"))) {
        CChangePasswordDlg dlg;
        dlg.SetPassword(m_szNewPassword, m_szOldPassword, m_szSaltValue);
        INT_PTR nResponse = dlg.DoModal();
        if (nResponse == IDOK) {
            _tcscpy_s(pClientConfig->szLocalPassword, MD5_TEXT_LENGTH, m_szNewPassword);
            NLocalBzl::SaveOptionConfig();
        } else if (nResponse == IDCANCEL) {
            //  “取消”来关闭对话框的代码
        }
    }
}

void CSecurityDlg::OnNMClickChangeUserpwdSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    PNMLINK pNMLink = (PNMLINK) pNMHDR;
    ShellExecute(NULL, _T("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);  //主要执行语句

    *pResult = 0;
}
