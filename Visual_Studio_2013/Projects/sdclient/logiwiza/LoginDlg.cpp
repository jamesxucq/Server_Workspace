// LoginDlb.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "logiwiza.h"
#include "clientcom/lwizardcom.h"
#include "LoginWizdBzl.h"
#include "LoginDlg.h"
#include "ProxiesDlg.h"

// CLoginDlg �Ի���

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


// CLoginDlg ��Ϣ�������

BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CLoginDlg::OnBnClickedProxyBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CProxiesDlg dlg(this);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
}


DWORD CLoginDlg::SetLoginValue()
{
	GET_SERVA_POINT
	TCHAR szAddrTxt[SERVNAME_LENGTH];
	m_edtServAddr.SetWindowText(struti::get_ipaddr(szAddrTxt, pServAddress->sin_addr, pServAddress->sin_port));

	GET_DISPLAY_POINT
	TCHAR szLinkTxt[URI_LENGTH];
	m_linkForgetPwd.SetWindowText(struti::get_url(szLinkTxt, _T("����������?"), pDisplayLink->szForgetPassword));
	m_linkRegistUser.SetWindowText(struti::get_url(szLinkTxt, _T("�½�һ���û�"), pDisplayLink->szRegistUser));

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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PNMLINK pNMLink = (PNMLINK) pNMHDR;
	ShellExecute(NULL, _T("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);  //��Ҫִ����� 

	*pResult = 0;
}

void CLoginDlg::OnNMClickRegistusrSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PNMLINK pNMLink = (PNMLINK) pNMHDR;
	ShellExecute(NULL, _T("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);  //��Ҫִ����� 

	*pResult = 0;
}
