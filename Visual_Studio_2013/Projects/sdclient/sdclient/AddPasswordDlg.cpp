// AddClientPWD.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "SecurityDlg.h"
#include "AddPasswordDlg.h"


// CAddPasswordDlg 对话框

IMPLEMENT_DYNAMIC(CAddPasswordDlg, CDialog)

CAddPasswordDlg::CAddPasswordDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CAddPasswordDlg::IDD, pParent)
    ,m_szNewPassword(NULL)
    ,m_szSaltValue(NULL)
{

}

CAddPasswordDlg::~CAddPasswordDlg()
{
}

void CAddPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NEWPWD_ADD_EDT, m_edtNewPassword);
    DDX_Control(pDX, IDC_VALPWD_ADD_EDT, m_edtValPassword);
    DDX_Control(pDX, IDC_NEWPWD_ADD_STC, m_stcNewPassword);
    DDX_Control(pDX, IDC_VALPWD_ADD_STC, m_stcValPassword);
}


BEGIN_MESSAGE_MAP(CAddPasswordDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CAddPasswordDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CAddPasswordDlg::OnBnClickedCancel)
    ON_EN_SETFOCUS(IDC_NEWPWD_ADD_EDT, &CAddPasswordDlg::OnEnSetfocusNewpwdAddEdt)
    ON_EN_SETFOCUS(IDC_VALPWD_ADD_EDT, &CAddPasswordDlg::OnEnSetfocusChkpwdAddEdt)
END_MESSAGE_MAP()


// CAddPasswordDlg 消息处理程序

BOOL CAddPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CAddPasswordDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    TCHAR szNewPassword[PASSWORD_LENGTH];
    TCHAR szValPassword[PASSWORD_LENGTH];

    UpdateData(TRUE);
    m_edtNewPassword.GetWindowText(szNewPassword, PASSWORD_LENGTH);
    m_edtValPassword.GetWindowText(szValPassword, PASSWORD_LENGTH);
    if(_tcslen(szNewPassword) < 6) {
        m_stcNewPassword.ShowWindow(SW_SHOW);
        return ;
    }
    if(_tcscmp(szNewPassword, szValPassword)) {
        m_stcValPassword.ShowWindow(SW_SHOW);
        return ;
    }

    // _tcscpy_s(m_szNewPassword, PASSWORD_LENGTH, szNewPassword);
    NEncoding::RandomSalt(m_szSaltValue);
    NEncoding::PWDChks(m_szNewPassword, szNewPassword, m_szSaltValue);

    OnOK();
}

bool CAddPasswordDlg::SetPassword(TCHAR *szNewPassword, TCHAR *szSaltValue)
{
    m_szNewPassword = szNewPassword;
    m_szSaltValue = szSaltValue;

    return true;
}
void CAddPasswordDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}

BOOL CAddPasswordDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_stcNewPassword.ShowWindow(SW_HIDE);
    m_stcValPassword.ShowWindow(SW_HIDE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CAddPasswordDlg::OnEnSetfocusNewpwdAddEdt()
{
    // TODO: 在此添加控件通知处理程序代码
    m_stcNewPassword.ShowWindow(SW_HIDE);

    UpdateData(FALSE);
}

void CAddPasswordDlg::OnEnSetfocusChkpwdAddEdt()
{
    // TODO: 在此添加控件通知处理程序代码
    m_stcValPassword.ShowWindow(SW_HIDE);

    UpdateData(FALSE);
}
