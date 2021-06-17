// FinishDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "logiwiza.h"
#include "FinishDlg.h"


// CFinishDlg 对话框

IMPLEMENT_DYNAMIC(CFinishDlg, CDialog)

CFinishDlg::CFinishDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFinishDlg::IDD, pParent)
{

}

CFinishDlg::~CFinishDlg()
{
}

void CFinishDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAY_STC, m_stcDisplay);
}


BEGIN_MESSAGE_MAP(CFinishDlg, CDialog)
END_MESSAGE_MAP()


// CFinishDlg 消息处理程序

BOOL CFinishDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

DWORD CFinishDlg::SetFinishValue(int CVolStatus) {
	CString csDisplayTxt = _T("");
//
	if(!CVolStatus)
		csDisplayTxt += _T("\n    用户设置成功!\n\n");
	else{
		if(0x000000FF & CVolStatus)
			csDisplayTxt += _T("用户登陆失败!\n\n");
		else csDisplayTxt += _T("用户登陆成功.\n\n");

		if(0x0000FF00 & CVolStatus)
			csDisplayTxt += _T("使用已存在的数据!\n\n");
		else csDisplayTxt += _T("新建数据区成功.\n\n");

		if(0x00FF0000 & CVolStatus)
			csDisplayTxt += _T("创建虚拟磁盘失败!\n\n");
		else csDisplayTxt += _T("创建虚拟磁盘成功.\n\n");

		if(0xFF000000 & CVolStatus)
			csDisplayTxt += _T("未定义!\n\n");
	}
	m_stcDisplay.SetWindowTextW(csDisplayTxt);
//
	return 0x00;
}
