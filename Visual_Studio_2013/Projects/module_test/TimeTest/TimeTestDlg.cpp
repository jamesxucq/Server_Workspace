// TimeTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TimeTest.h"
#include "TimeTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTimeTestDlg 对话框




CTimeTestDlg::CTimeTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTimeTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTimeTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTimeTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CTimeTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTimeTestDlg 消息处理程序

BOOL CTimeTestDlg::OnInitDialog()
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

void CTimeTestDlg::OnPaint()
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
HCURSOR CTimeTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTimeTestDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

#include "strptime.h"
#define TIME_LENGTH				32

void CTimeTestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码    
	struct tm gm;
	FILETIME filetime;
	char *timestr = "Wed, 15 Nov 1995 04:58:08 GMT";

    memset(&gm, '\0', sizeof (struct tm));
    if (!strptime(timestr, "%a, %d %b %Y %H:%M:%S GMT", &gm)) return;
	SYSTEMTIME st = { 1900+gm.tm_year, 1+gm.tm_mon, gm.tm_wday, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec, 0 };
	if(!SystemTimeToFileTime(&st, &filetime)) return; 

	char timestrx[TIME_LENGTH];
	SYSTEMTIME stx;

	if(!FileTimeToSystemTime(&filetime, &stx)) return;
	struct tm gmt = {stx.wSecond, stx.wMinute, stx.wHour, stx.wDay, stx.wMonth-1, stx.wYear-1900, stx.wDayOfWeek, 0, 0};
	strftime(timestrx, TIME_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", &gmt);

}
