// AboutDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "AboutDlg.h"

#include "OptionsDlg.h"

// CAboutDlg 对话框

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APP_UPDATE_SYSLINK, m_linkAppUpdate);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_APP_UPDATE_SYSLINK, &CAboutDlg::OnNMClickAppUpdateSyslink)
END_MESSAGE_MAP()


// CAboutDlg 消息处理程序

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
#define GET_DISPLAY_POINT \
	struct DisplayLink *pDisplayLink; \
	pDisplayLink = NConfigBzl::GetDisplayLink();
#define APPLY_BTN_ENABLE ((COptionsDlg*)m_pParentWnd)->EnableApplyButton

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

DWORD CAboutDlg::SetAboutValue()
{
	GET_DISPLAY_POINT

	TCHAR szLinkTxt[URI_LENGTH];
	m_linkAppUpdate.SetWindowText(struti::get_url(szLinkTxt, COMPANY_HOME_LINK, pDisplayLink->szCompanyLink));

	UpdateData(FALSE);
	return 0x00;
}

void CAboutDlg::OnNMClickAppUpdateSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	PNMLINK pNMLink = (PNMLINK) pNMHDR;
	ShellExecute(NULL, _T("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);  //主要执行语句 

	*pResult = 0;
}
