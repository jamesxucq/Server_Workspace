// DowndDiresDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"

#include "ExecuteTransmitBzl.h"

#include "DowndDiresDlg.h"


// CDowndDiresDlg 对话框

IMPLEMENT_DYNAMIC(CDowndDiresDlg, CDialog)

CDowndDiresDlg::CDowndDiresDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDowndDiresDlg::IDD, pParent)
{
}

CDowndDiresDlg::~CDowndDiresDlg()
{
}

void CDowndDiresDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDowndDiresDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDowndDiresDlg::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDowndDiresDlg 消息处理程序

BOOL CDowndDiresDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

UINT DowndDiresInfoThread(LPVOID lpParam);
const UINT uiDowndTimerID = 0x0001;
BOOL CDowndDiresDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// NSelectiveBzl::StartDowndThread((LPVOID)this);
	NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_WAITING);
	CWinThread *pDowndThread = AfxBeginThread(DowndDiresInfoThread, (LPVOID)NULL);
	SetTimer(uiDowndTimerID, 2048, NULL);
//
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDowndDiresDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(uiDowndTimerID);
//
	OnCancel();
}


void CDowndDiresDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(DOWND_THREAD_ACTIVE != NSelectiveBzl::GetDowndThreadStatus()) {
		KillTimer(uiDowndTimerID);
		OnOK();
	}
//
	CDialog::OnTimer(nIDEvent);
}


UINT DowndDiresInfoThread(LPVOID lpParam)
{
	NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_ACTIVE);
	DWORD downd_status = NSelectiveBzl::PerfromDirectoriesDownd();
// DWORD downd_status = DOWND_VALUE_SUCCESS;
// _LOG_DEBUG(_T("eeeeeeeeeeeeeeeee"));
	switch(downd_status) {
	case DOWND_VALUE_QUERYERR:
	case DOWND_VALUE_FAILED:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_FAULT);
		break;
	case DOWND_VALUE_SERVBUSY:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_SERVBUSY);
		break;
	case DOWND_VALUE_SUCCESS:
// NSelectiveBzl::InitDiresTreeTest(); // for test
		NSelectiveBzl::PrepaDisplayTView();
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_SUCCESS);
		break;
	case DOWND_VALUE_DISCONN:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_DISCONNECT);
		break;
	case DOWND_PROECSS_BUSY:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_LOCALBUSY);
		break;
	}
_LOG_DEBUG(_T("downd_status:%08x"), downd_status); // disable by james 20130410
//
	return 0;
}
