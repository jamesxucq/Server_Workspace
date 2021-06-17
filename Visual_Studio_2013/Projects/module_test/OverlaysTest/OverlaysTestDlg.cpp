
// OverlaysTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OverlaysTest.h"
#include "OverlaysTestDlg.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#endif


// COverlaysTestDlg 对话框




COverlaysTestDlg::COverlaysTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlaysTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COverlaysTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COverlaysTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &COverlaysTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &COverlaysTestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &COverlaysTestDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// COverlaysTestDlg 消息处理程序

BOOL COverlaysTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COverlaysTestDlg::OnPaint()
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
HCURSOR COverlaysTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "CacheLinker.h"

void COverlaysTestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	// Open SDCache Pipe
	objCommandLinker.Initialize();

	if(objCommandLinker.CacheClear(FILE_STATUS_UNKNOWN)) 
		return ;
	if(objCommandLinker.SetCacheWatchingDirectory(_T("Z:"))) 
		return ;
}

void COverlaysTestDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(objCommandLinker.SetCacheWatchingDirectory(_T(""))) {
		//TRACE( _T("set cache watch path failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);	
	}
	if(objCommandLinker.CacheClear(FILE_STATUS_UNKNOWN)) {
		//TRACE( _T("set cache clear failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);		
	}

	//Exit the Cache Close SDCache Pipe
	objCommandLinker.Finalize();

}

void COverlaysTestDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

	//	objCommandLinker.CacheFileInsert(_T("H:\\4.txt"), FILE_STATUS_ADDED);
	//	objCommandLinker.CacheFileInsert(_T("H:\\5.txt"), FILE_STATUS_CONFLICT);
	//	objCommandLinker.CacheFileInsert(_T("H:\\4.txt"), FILE_STATUS_SYNCING);
	//	objCommandLinker.CacheErase(_T("H:\\4.txt"), FILE_STATUS_SYNCING);
	//	objCommandLinker.CacheErase(_T("H:\\4.txt"), FILE_STATUS_NORMAL);

}

