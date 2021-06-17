// ShellCacheTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShellCacheTest.h"
#include "ShellCacheTestDlg.h"

#include "CacheLinker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CShellCacheTestDlg 对话框




CShellCacheTestDlg::CShellCacheTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShellCacheTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShellCacheTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShellCacheTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CShellCacheTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CShellCacheTestDlg 消息处理程序

BOOL CShellCacheTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShellCacheTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CShellCacheTestDlg::OnPaint()
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
HCURSOR CShellCacheTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CShellCacheTestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	// Open SDCache Pipe
	OCmdCacheLinker.Create();

	if(!OCmdCacheLinker.CacheClear(FILE_STATUS_UNKNOWN)) 
		return;
	if(!OCmdCacheLinker.SetCacheWatchingDirectory(_T("E:\\"))) 
		return;
//	if(!DisplayFilesAttribShell(vpFileAttrib, sDriveRoot)) 
//		return;
//	if(DisplayFilesVecShell(vpLastFilesVec, sDriveRoot)) 
//		return;

/*
#define		FILE_STATUS_SYNCING						0x00000001
//////////////////////////////////////////////////////////////下段显示优先最小
#define		FILE_STATUS_DELETE						0x00000002
#define		FILE_STATUS_MODIFIED					0x00000004
#define		FILE_STATUS_ADDED						0x00000008
//////////////////////////////////////////////////////////////下段显示优先最大
#define		FILE_STATUS_CONFLICT					0x00000010 //优先0
	//以下有向下的图标继承性
#define		FILE_STATUS_LOCKED						0x00000020
#define		FILE_STATUS_READONLY					0x00000040
#define		FILE_STATUS_FORBID						0x00000080
//////////////////////////////////////////////////////////////
#define		FILE_STATUS_NORMAL						0x00001000
#define		FILE_STATUS_DIRECTORY					0x00002000
*/

	OCmdCacheLinker.CacheFileInsert(_T("E:\\1.txt"), FILE_STATUS_SYNCING);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\2.txt"), FILE_STATUS_DELETE);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\3.txt"), FILE_STATUS_MODIFIED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\4.txt"), FILE_STATUS_ADDED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\5.txt"), FILE_STATUS_CONFLICT);
	// OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_LOCKED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\7.txt"), FILE_STATUS_READONLY);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\8.txt"), FILE_STATUS_FORBID);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\9.txt"), FILE_STATUS_NORMAL);
	// OCmdCacheLinker.CacheErase(_T("E:\\5.txt"), FILE_STATUS_NORMAL);

	OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_MODIFIED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_SYNCING);
	// OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_NORMAL);
	// OCmdCacheLinker.CacheErase(_T("E:\\6.txt"), FILE_STATUS_NORMAL);
	OCmdCacheLinker.CacheErase(_T("E:\\6.txt"), FILE_STATUS_SYNCING);


	if(!OCmdCacheLinker.SetCacheWatchingDirectory(_T(""))) {
		//TRACE( _T("set cache watch path failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);	
	}
	if(!OCmdCacheLinker.CacheClear(FILE_STATUS_UNKNOWN)) {
		//TRACE( _T("set cache clear failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);		
	}

	//Exit the Cache Close SDCache Pipe
	OCmdCacheLinker.Destroy();
}
