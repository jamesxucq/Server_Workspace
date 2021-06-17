// ValidateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "sdclient.h"
#include "LocalBzl.h"

#include "ValidateDlg.h"


// CValidateDlg �Ի���

IMPLEMENT_DYNAMIC(CValidateDlg, CDialog)

CValidateDlg::CValidateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CValidateDlg::IDD, pParent)
{

}

CValidateDlg::~CValidateDlg()
{
}

void CValidateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASSWORD_EDT, m_edtPassword);
	DDX_Control(pDX, IDC_VALIDATE_BTN, m_btnValidate);
}


BEGIN_MESSAGE_MAP(CValidateDlg, CDialog)
	ON_BN_CLICKED(IDC_VALIDATE_BTN, &CValidateDlg::OnBnClickedValidateBtn)
	ON_EN_CHANGE(IDC_PASSWORD_EDT, &CValidateDlg::OnEnChangePasswordEdt)
END_MESSAGE_MAP()


// CValidateDlg ��Ϣ�������
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
/*
if(VK_ESCAPE==pMsg->wParam || VK_RETURN==pMsg->wParam)
	return TRUE;
*/
BOOL CValidateDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(WM_KEYDOWN == pMsg->message) {
		if(VK_RETURN == pMsg->wParam) {
			GET_CLIENT_POINT
			CString csPassword;
			m_edtPassword.GetWindowText(csPassword); 
			TCHAR szPWDChks[MD5_TEXT_LENGTH];
			NEncoding::PWDChks(szPWDChks, (TCHAR*)(LPCTSTR)csPassword, pClientConfig->szSaltValue);
			if(_tcscmp(szPWDChks, pClientConfig->szLocalPassword)) 
				return TRUE;
		} if(VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}
//
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CValidateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CValidateDlg::OnBnClickedValidateBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_edtPassword.SetWindowText(_T(""));
//
	CDialog::OnOK();
}

void CValidateDlg::OnEnChangePasswordEdt()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GET_CLIENT_POINT
		CString csPassword;
//
	m_edtPassword.GetWindowText(csPassword);
	// if(!_tcsicmp(szPassword, pClientConfig->szLocalPassword)) m_btnValidate.EnableWindow(true); 
	TCHAR szPWDChks[MD5_TEXT_LENGTH];
	NEncoding::PWDChks(szPWDChks, (TCHAR*)(LPCTSTR)csPassword, pClientConfig->szSaltValue);
	if(!_tcscmp(szPWDChks, pClientConfig->szLocalPassword)) m_btnValidate.EnableWindow(TRUE); 
	else m_btnValidate.EnableWindow(FALSE); 
}


void CValidateDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_edtPassword.SetWindowText(_T(""));
//
	CDialog::OnOK();
}
