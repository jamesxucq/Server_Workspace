// FormatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "logiwiza.h"
#include "clientcom/lwizardcom.h"
#include "LoginWizdBzl.h"
#include "FormatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFormatDlg 对话框
CFormatDlg::CFormatDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFormatDlg::IDD, pParent)
{

}

void CFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOCATION_STC, m_stcLocation);
	DDX_Control(pDX, IDC_DRIVE_STC, m_stcDriveRoot);
	DDX_Control(pDX, IDC_LABEL_STC, m_stcDiskLabel);
	DDX_Control(pDX, IDC_FORMAT_STAT_STC, m_stcFormatStatus);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_proFormatStatus);
}

BEGIN_MESSAGE_MAP(CFormatDlg, CDialog)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS_BAR, &CFormatDlg::OnNMCustomdrawProgressBar)
END_MESSAGE_MAP()


// CFormatDlg 消息处理程序

BOOL CFormatDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}
//
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CFormatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO: 在此添加额外的初始化代码
#define GET_AGENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
	m_proFormatStatus.SetRange(0, 64);
//
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


DWORD CFormatDlg::SetFormatValue()
{
	GET_AGENT_POINT
//
		m_stcLocation.SetWindowText(pClientConfig->szLocation);
	m_stcDriveRoot.SetWindowText(pClientConfig->szDriveLetter);
	m_stcDiskLabel.SetWindowText(pClientConfig->szDiskLabel);
//
	UpdateData(FALSE);
	return 0x00;
}

DWORD CFormatDlg::GetFormatValue()
{
	UpdateData(TRUE);
//
	return 0x00;
}


void CFormatDlg::OnNMCustomdrawProgressBar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
