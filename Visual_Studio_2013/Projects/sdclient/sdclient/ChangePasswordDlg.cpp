// ModifyClientPWD.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sdclient.h"
#include "SecurityDlg.h"
#include "ChangePasswordDlg.h"


// CChangePasswordDlg �Ի���

IMPLEMENT_DYNAMIC(CChangePasswordDlg, CDialog)

CChangePasswordDlg::CChangePasswordDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CChangePasswordDlg::IDD, pParent)
    ,m_szOldPassword(NULL)
    ,m_szNewPassword(NULL)
    ,m_szSaltValue(NULL)
{

}

CChangePasswordDlg::~CChangePasswordDlg()
{
}

void CChangePasswordDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NEWPWD_CHG_EDT, m_edtNewPassword);
    DDX_Control(pDX, IDC_VALPWD_CHG_EDT, m_edtValPassword);
    DDX_Control(pDX, IDC_OLDPWD_CHG_EDT, m_edtOldPassword);
    DDX_Control(pDX, IDC_OLDPWD_CHG_STC, m_stcOldPassword);
    DDX_Control(pDX, IDC_NEWPWD_CHG_STC, m_stcNewPassword);
    DDX_Control(pDX, IDC_VALPWD_CHG_STC, m_stcValPassword);
}


BEGIN_MESSAGE_MAP(CChangePasswordDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CChangePasswordDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CChangePasswordDlg::OnBnClickedCancel)
    ON_EN_SETFOCUS(IDC_OLDPWD_CHG_EDT, &CChangePasswordDlg::OnEnSetfocusOldpwdChangeEdt)
    ON_EN_SETFOCUS(IDC_NEWPWD_CHG_EDT, &CChangePasswordDlg::OnEnSetfocusNewpwdChangeEdt)
    ON_EN_SETFOCUS(IDC_VALPWD_CHG_EDT, &CChangePasswordDlg::OnEnSetfocusChkpwdChangeEdt)
END_MESSAGE_MAP()


// CChangePasswordDlg ��Ϣ�������

BOOL CChangePasswordDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

bool CChangePasswordDlg::SetPassword(TCHAR *szNewPassword, TCHAR *szOldPassword, TCHAR *szSaltValue)
{
    m_szOldPassword = szOldPassword;
    m_szNewPassword = szNewPassword;
    m_szSaltValue = szSaltValue;

    return true;
}
void CChangePasswordDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    TCHAR szOldPassword[PASSWORD_LENGTH];
    TCHAR szNewPassword[PASSWORD_LENGTH];
    TCHAR szValPassword[PASSWORD_LENGTH];

    UpdateData(TRUE);
    m_edtOldPassword.GetWindowText(szOldPassword, PASSWORD_LENGTH);
    m_edtNewPassword.GetWindowText(szNewPassword, PASSWORD_LENGTH);
    m_edtValPassword.GetWindowText(szValPassword, PASSWORD_LENGTH);

    TCHAR szPWDChks[MD5_TEXT_LENGTH];
    NEncoding::PWDChks(szPWDChks, szOldPassword, m_szSaltValue);
    if(_tcscmp(szPWDChks, m_szOldPassword)) {
        m_stcOldPassword.ShowWindow(SW_SHOW);
        return ;
    }
    if(_tcslen(szNewPassword) < 6) {
        m_stcNewPassword.ShowWindow(SW_SHOW);
        return ;
    }
    if(_tcscmp(szNewPassword, szValPassword)) {
        m_stcValPassword.ShowWindow(SW_SHOW);
        return ;
    }
    // _tcscpy_s(m_szNewPassword, PASSWORD_LENGTH, szNewPassword);
    NEncoding::PWDChks(m_szNewPassword, szNewPassword, m_szSaltValue);

    OnOK();
}

void CChangePasswordDlg::OnBnClickedCancel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnCancel();
}

BOOL CChangePasswordDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    m_stcOldPassword.ShowWindow(SW_HIDE);
    m_stcNewPassword.ShowWindow(SW_HIDE);
    m_stcValPassword.ShowWindow(SW_HIDE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CChangePasswordDlg::OnEnSetfocusOldpwdChangeEdt()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_stcOldPassword.ShowWindow(SW_HIDE);

    UpdateData(FALSE);
}

void CChangePasswordDlg::OnEnSetfocusNewpwdChangeEdt()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_stcNewPassword.ShowWindow(SW_HIDE);

    UpdateData(FALSE);
}

void CChangePasswordDlg::OnEnSetfocusChkpwdChangeEdt()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_stcValPassword.ShowWindow(SW_HIDE);

    UpdateData(FALSE);
}
