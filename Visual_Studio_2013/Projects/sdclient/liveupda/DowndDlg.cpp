
// DowndDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "liveupda.h"
#include "DowndBzl.h"
#include "DowndDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDowndDlg 对话框

CDowndDlg::CDowndDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDowndDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDowndDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDowndDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// }}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDowndDlg 消息处理程序
UINT LiveDowndThread(LPVOID lpParam);
const UINT uiDowndTimerID = 0x0001;
BOOL CDowndDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 在此添加额外的初始化代码
	NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_WAITING);
	CWinThread *pDowndThread = AfxBeginThread(LiveDowndThread, (LPVOID)NULL);
	SetTimer(uiDowndTimerID, 2048, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDowndDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDowndDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CDowndDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(WM_NCPAINT == message) ShowWindow(SW_HIDE); 
//
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDowndDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(DOWND_THREAD_ACTIVE != NDowndBzl::GetDowndThreadStatus()) {
		KillTimer(uiDowndTimerID);
		OnOK();
	}
//
	CDialog::OnTimer(nIDEvent);
}

UINT LiveDowndThread(LPVOID lpParam)
{
	NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_ACTIVE);
	DWORD downd_status = NDowndBzl::PerfromDownload();
	switch(downd_status) {
	case DOWND_VALUE_SUCCESS:
		NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_SUCCESS);
		break;
	case DOWND_VALUE_FAILED:
	case DOWND_LOAD_FILE:
	case DOWND_VALUE_UPDATE:
		NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_FAULT);
		break;
	}
// _LOG_DEBUG(_T("downd_status:%d"), downd_status);
//
	return 0;
}
