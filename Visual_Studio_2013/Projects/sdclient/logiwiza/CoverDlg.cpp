// StartDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "logiwiza.h"
#include "CoverDlg.h"


// CCoverDlg 对话框

IMPLEMENT_DYNAMIC(CCoverDlg, CDialog)

CCoverDlg::CCoverDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCoverDlg::IDD, pParent)
{

}

CCoverDlg::~CCoverDlg()
{
}

void CCoverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCoverDlg, CDialog)
	//	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCoverDlg 消息处理程序

BOOL CCoverDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// void CCoverDlg::OnClose()
// {
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	// CDialog::OnClose();
//	return ;
// }

// void CCoverDlg::OnOK()
// {
//	// TODO: 在此添加专用代码和/或调用基类
//
//	CDialog::OnOK();
// }
