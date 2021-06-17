// LocalBzl.cpp : 实现文件
//

#include "stdafx.h"

#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "ApplicaBzl.h"
#include "LocalBzl.h"

#include "ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
const UINT WM_ADDICON_TOTASKBAR = ::RegisterWindowMessage(_T("TaskbarCreated"));

#define EXIT_FAIL_DELAY 3500

// CClientDlg 对话框
CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pOptionsDlg = NULL;
	m_bShowTrayNotify = TRUE; // zxn
	m_bOptionsOpened = FALSE;
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotify)
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_SUSPEND, &CClientDlg::OnAppSuspend)
	ON_COMMAND(ID_OPTIONS_OPEN, &CClientDlg::OnOptionsOpen)
	ON_COMMAND(ID_OPEN_ROOTDIR, &CClientDlg::OnRootDirectoryOpen)
	ON_COMMAND(ID_PROCESS_STATUS, &CClientDlg::OnPausedResume)
	ON_REGISTERED_MESSAGE(WM_ADDICON_TOTASKBAR, &CClientDlg::OnAddIconToTaskbar)
END_MESSAGE_MAP()

// CClientDlg 消息处理程序

BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}
//
	return CDialog::PreTranslateMessage(pMsg);
}

//
// Handle notification from tray icon: display a message.
//
LRESULT CClientDlg::OnTrayNotify(WPARAM uID,LPARAM lEvent)
{
	if (m_bShowTrayNotify) 
	{
		static LPCSTR MouseMessages[] = { "WM_MOUSEMOVE",
			"WM_LBUTTONDOWN", "WM_LBUTTONUP", "WM_LBUTTONDBLCLK",
			"WM_RBUTTONDOWN", "WM_RBUTTONUP", "WM_RBUTTONDBLCLK",
			"WM_MBUTTONDOWN", "WM_MBUTTONUP", "WM_MBUTTONDBLCLK" };
		switch(uID) {
		case WM_MOUSEMOVE:
			break;	
		case WM_RBUTTONDBLCLK:
			NLocalBzl::OpenRootDirectorise(this->m_hWnd);
			break;				
		default:
			break;		
		}
	}
//
	return 0;
}

void CClientDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (NLocalBzl::GetShutdownStatus()) CDialog::OnClose();
	else ShowWindow(SW_HIDE);
}

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 在此添加额外的初始化代码
	if(NApplicaBzl::ApplicaInitialize(this)) {
		AFX_PRINT_ERRCODE(ERR_INIT_ENV);
		return true;
	}
	_LOG_INFO(_T("initial the application Ok"));
	return true;  // 除非将焦点设置到控件，否则返回 true
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::OnAppSuspend()
{
	// TODO: 在此添加命令处理程序代码
	//
	static LONG lAppSuspend;
	//
	if(!m_bOptionsOpened && !InterlockedCompareExchange(&lAppSuspend, 0x0001, 0x0000)) {
		_LOG_INFO(_T("suspend the application"));
		//
		DWORD dwSuspendPrepare = 0x01;
		if(!NExecTransBzl::GetTransmitProcessStatus()) { // 同步线程,正在同步
			if(IDYES == MessageBox(_T("正在同步,是否停止同步后退出?"), _T("Tip"), MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2)) {
				NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, _T("正在停止同步,请稍候."));
				NLocalBzl::KillTransmitProcess(); // modify by james 20120106
			} else {
				dwSuspendPrepare = 0x00;
				InterlockedExchange(&lAppSuspend, 0x00);
			}
		} else NLocalBzl::StopTransmitProcess(); // 同步线程,其他状态

		DWORD dwFinalValue;
		if(dwSuspendPrepare && NExecTransBzl::GetTransmitProcessStatus()){
			_LOG_INFO(_T("previous eixt the application"));
			NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, _T("正在退出,请稍候..."));
			if(!(dwFinalValue = NApplicaBzl::ApplicaFinalize())) { 
				// 退出程序结束环境成功
				NLocalBzl::SetShutdownStatus(TRUE);		// really exit
				SendMessage(WM_CLOSE);
			} else {  // 退出程序结束环境失败,重新初始化
				if(NApplicaBzl::ApplicaInitialize(this)) {
					PRINT_ERRCODE(this->m_hWnd, ERR_INIT_ENV);
				}
				if(-1 == dwFinalValue) PRINT_ERRCODE(this->m_hWnd, ERR_FINAL_ENV);
				else if(0x01 == dwFinalValue) NLocalBzl::BalloonTip(BALLOON_TIP_EXCEPTION, _T("有文件打开,请关闭后重试."));
				//
				Sleep(EXIT_FAIL_DELAY);
				InterlockedExchange(&lAppSuspend, 0x00);
				_LOG_INFO(_T("reinit the application, dwFinalValue:%d"), dwFinalValue);
			}
		}
	}
	//
}

void CClientDlg::OnOptionsOpen()
{
	// TODO: 在此添加命令处理程序代码
_LOG_DEBUG(_T("options open!")); //
	if(NLOCA_PASS_EXCLUS && !m_bOptionsOpened) {
		if(m_pOptionsDlg) {
			m_pOptionsDlg->DestroyWindow();
			delete m_pOptionsDlg;
		}
		m_pOptionsDlg = new COptionsDlg(this);
		m_pOptionsDlg->Create(IDD_OPTIONS_DLG, this);
		m_pOptionsDlg->ShowWindow(SW_SHOW);
	}
}

void CClientDlg::OnRootDirectoryOpen()
{
	// TODO: 在此添加命令处理程序代码
	NLocalBzl::OpenRootDirectorise(this->m_hWnd);
}

void CClientDlg::OnPausedResume()
{
	// TODO: 在此添加命令处理程序代码
	NLocalBzl::PausedResumeTransmitProcess();
}


LRESULT CClientDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message) {
	case WM_QUERYENDSESSION:
		if(!NExecTransBzl::GetTransmitProcessStatus()) NLocalBzl::KillTransmitProcess();
		else NLocalBzl::StopTransmitProcess();
		break;
	case WM_ENDSESSION:
		NApplicaBzl::ApplicaFinalize();
		break;
	case WM_NCPAINT:
		ShowWindow(SW_HIDE); 
		break;
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CClientDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	if(!m_bOptionsOpened) {
		if(m_pOptionsDlg) {
			m_pOptionsDlg->DestroyWindow();
			delete m_pOptionsDlg;
		}
	}
	return CDialog::DestroyWindow();
}

LRESULT CClientDlg::OnAddIconToTaskbar(WPARAM wParam, LPARAM lParam) {
	NLocalBzl::AddIconToTaskbar(this);
	return 0L;
}