// AddClientPWD.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sdclient.h"
#include "SecurityDlg.h"
#include "AddPasswordDlg.h"


// CAddPasswordDlg �Ի���

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


// CAddPasswordDlg ��Ϣ�������

BOOL CAddPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CAddPasswordDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnCancel();
}

BOOL CAddPasswordDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    m_stcNewPassword.ShowWindow(SW_HIDE);
    m_stcValPassword.ShowWindow(SW_HIDE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CAddPasswordDlg::OnEnSetfocusNewpwdAddEdt()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_stcNewPassword.ShowWindow(SW_HIDE);

    UpdateData(FALSE);
}

void CAddPasswordDlg::OnEnSetfocusChkpwdAddEdt()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_stcValPassword.ShowWindow(SW_HIDE);

    UpdateData(FALSE);
}
