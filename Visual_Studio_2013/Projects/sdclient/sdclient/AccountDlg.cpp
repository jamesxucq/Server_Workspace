// AccountDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "OptionsDlg.h"
#include "AccountDlg.h"


// CAccountDlg 对话框

IMPLEMENT_DYNAMIC(CAccountDlg, CDialog)

CAccountDlg::CAccountDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAccountDlg::IDD, pParent)
{

}

CAccountDlg::~CAccountDlg()
{
}

void CAccountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UNLINK_BTN, m_btnUnlink);
	DDX_Control(pDX, IDC_UNAME_STC, m_stcUserName);
	DDX_Control(pDX, IDC_PCNAME_STC, m_stcLocalIdenti);
	// DDX_Control(pDX, IDC_VERSION_STC, m_stcVersion);
}


BEGIN_MESSAGE_MAP(CAccountDlg, CDialog)
	ON_BN_CLICKED(IDC_UNLINK_BTN, &CAccountDlg::OnBnClickedUnlinkBtn)
END_MESSAGE_MAP()

// CAccountDlg 消息处理程序

BOOL CAccountDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAccountDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAccountDlg::OnBnClickedUnlinkBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	GET_CLIENT_POINT
	if(NLocalBzl::AccountUnlink()) MessageBox(_T("连接服务器出错."), _T("Tip"), MB_OK|MB_ICONERROR);
	else {
		_tcscpy_s(pClientConfig->szAccountLinked, _T("false"));
		_tcscpy_s(pClientConfig->szForceCtrl, _T("slowly"));
		NLocalBzl::SaveOptionConfig();
		NLocalBzl::SwitchToLoginWizard();
	}
}

DWORD CAccountDlg::SetAccountValue()
{
	GET_CLIENT_POINT
	TCHAR szPrompt[MID_TEXT_LEN];

	_stprintf_s(szPrompt, _T("同步网盘连接到 (%s)"), pClientConfig->szLocalIdenti);
	m_stcLocalIdenti.SetWindowText(szPrompt);

	_stprintf_s(szPrompt, _T("用户帐户: %s"), pClientConfig->szUserName);
	m_stcUserName.SetWindowText(szPrompt);

	UpdateData(FALSE);
	return 0x00;
}

/* // modify by james 20101205
bool CAccountDlg::GetAccountValue()
{ return true; }
*/