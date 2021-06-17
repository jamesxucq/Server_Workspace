// ValidateDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "sdclient.h"
#include "LocalBzl.h"

#include "ValidateDlg.h"


// CValidateDlg 对话框

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


// CValidateDlg 消息处理程序
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
/*
if(VK_ESCAPE==pMsg->wParam || VK_RETURN==pMsg->wParam)
	return TRUE;
*/
BOOL CValidateDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
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

	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CValidateDlg::OnBnClickedValidateBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_edtPassword.SetWindowText(_T(""));
//
	CDialog::OnOK();
}

void CValidateDlg::OnEnChangePasswordEdt()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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
	// TODO: 在此添加专用代码和/或调用基类
	m_edtPassword.SetWindowText(_T(""));
//
	CDialog::OnOK();
}
