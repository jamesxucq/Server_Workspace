// LoginDlb.cpp : 实现文件
//

#include "stdafx.h"
#include "logiwiza.h"
#include "clientcom/lwizardcom.h"
#include "LoginWizdBzl.h"
#include "LoginDlg.h"
#include "ProxiesDlg.h"

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLoginDlg::IDD, pParent)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVADDR_EDT, m_edtServAddr);
	DDX_Control(pDX, IDC_USERNAME_EDT, m_edtUserName);
	DDX_Control(pDX, IDC_PASSWORD_EDT, m_edtPassword);
	DDX_Control(pDX, IDC_LOCALIDENTI_EDT, m_edtLocalIdenti);
	DDX_Control(pDX, IDC_LOGIN_STAT_STC, m_stcLoginStatus);
	DDX_Control(pDX, IDC_FORGETPWD_SYSLINK, m_linkForgetPwd);
	DDX_Control(pDX, IDC_REGISTUSR_SYSLINK, m_linkRegistUser);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_PROXY_BTN, &CLoginDlg::OnBnClickedProxyBtn)
	ON_NOTIFY(NM_CLICK, IDC_FORGETPWD_SYSLINK, &CLoginDlg::OnNMClickForgetpwdSyslink)
	ON_NOTIFY(NM_CLICK, IDC_REGISTUSR_SYSLINK, &CLoginDlg::OnNMClickRegistusrSyslink)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序

BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
#define GET_SERVA_POINT \
	ServAddress *pServAddress; \
	pServAddress = &(NConfigBzl::GetServParam()->tRegistAddress);
#define GET_AGENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
#define GET_DISPLAY_POINT \
	struct DisplayLink *pDisplayLink; \
	pDisplayLink = NConfigBzl::GetDisplayLink();

	GET_AGENT_POINT
		DWORD dwLocalIdentiLength = MAX_COMPUTERNAME_LENGTH + 1;
	TCHAR szLocalIdenti[MAX_COMPUTERNAME_LENGTH + 1];
	GetComputerName(szLocalIdenti, &dwLocalIdentiLength);
	_tcscpy_s(pClientConfig->szLocalIdenti, szLocalIdenti);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLoginDlg::OnBnClickedProxyBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CProxiesDlg dlg(this);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
}


DWORD CLoginDlg::SetLoginValue()
{
	GET_SERVA_POINT
	TCHAR szAddrTxt[SERVNAME_LENGTH];
	m_edtServAddr.SetWindowText(struti::get_ipaddr(szAddrTxt, pServAddress->sin_addr, pServAddress->sin_port));

	GET_DISPLAY_POINT
	TCHAR szLinkTxt[URI_LENGTH];
	m_linkForgetPwd.SetWindowText(struti::get_url(szLinkTxt, _T("忘记了密码?"), pDisplayLink->szForgetPassword));
	m_linkRegistUser.SetWindowText(struti::get_url(szLinkTxt, _T("新建一个用户"), pDisplayLink->szRegistUser));

	GET_AGENT_POINT
	m_edtUserName.SetWindowText(pClientConfig->szUserName);
	m_edtPassword.SetWindowText(pClientConfig->szPassword);
	m_edtLocalIdenti.SetWindowText(pClientConfig->szLocalIdenti);

	UpdateData(FALSE);
	return 0x00;
}

DWORD CLoginDlg::GetLoginValue()
{
	UpdateData(TRUE);

	GET_SERVA_POINT
	CString csWindowText;
	TCHAR szAddrTxt[SERVNAME_LENGTH];
	m_edtServAddr.GetWindowText(csWindowText);
	struti::split_ipaddr(pServAddress->sin_addr, &pServAddress->sin_port, strcon::cstr_ustr(szAddrTxt, csWindowText));

	GET_AGENT_POINT
	m_edtUserName.GetWindowText(csWindowText);
	_tcscpy_s(pClientConfig->szUserName, csWindowText);
	m_edtPassword.GetWindowText(csWindowText);
	_tcscpy_s(pClientConfig->szPassword, csWindowText);
	NEncoding::RandomSalt(pClientConfig->szSalt);
	m_edtLocalIdenti.GetWindowText(csWindowText);
	_tcscpy_s(pClientConfig->szLocalIdenti, csWindowText);

	return 0x00;
}

void CLoginDlg::OnNMClickForgetpwdSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	PNMLINK pNMLink = (PNMLINK) pNMHDR;
	ShellExecute(NULL, _T("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);  //主要执行语句 

	*pResult = 0;
}

void CLoginDlg::OnNMClickRegistusrSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	PNMLINK pNMLink = (PNMLINK) pNMHDR;
	ShellExecute(NULL, _T("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);  //主要执行语句 

	*pResult = 0;
}
