// AnchorTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AnchorTest.h"
#include "AnchorTestDlg.h"

#include "Logger.h"
#include "AnchorBzl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAnchorTestDlg 对话框




CAnchorTestDlg::CAnchorTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnchorTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnchorTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAnchorTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_TEST_BTN, &CAnchorTestDlg::OnBnClickedTestBtn)
END_MESSAGE_MAP()


// CAnchorTestDlg 消息处理程序

BOOL CAnchorTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	log_init(_T("e:\\anchor.log"), log_lvl_max);
	CAnchorBzl::Create(_T("~\\SyncingCtrl.flag"), _T("~\\anchor_index.ndx"), _T("~\\anchor_data.dat"), _T("e:"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAnchorTestDlg::OnPaint()
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
HCURSOR CAnchorTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void PrintFilesVec(FilesAttribVec *vpFilesAttVec)  // for debug 20110222
{
	vector <struct FileAttrib *>::iterator iter;

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		LOG_DEBUG(_T("-----------------------------------------------"));
		LOG_DEBUG(_T("--------------------------- filename:%s mod_type:%c"), (*iter)->filename, (*iter)->mod_type);
	}
}

void CAnchorTestDlg::OnBnClickedTestBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	FilesAttribVec vFastsyncVec;
	DWORD iLastAnchor = CAnchorBzl::GetLastAnchAndLock();
	LOG_DEBUG(_T("the last anchor is:%d"), iLastAnchor);
	if(!iLastAnchor) {
		LOG_DEBUG(_T("add new anchor"));
		CAnchorBzl::AddNewAnchAndUnlock(++iLastAnchor);
	}

	CAnchorBzl::AnchFilesVecInsert(_T("C:\\1.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\2.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\3.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\4.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\5.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\6.txt"), 'A');

	iLastAnchor = CAnchorBzl::GetLastAnchAndLock();
	LOG_DEBUG(_T("the last anchor is:%d"), iLastAnchor);
	//CAnchorBzl::UnlockAnchorFile();

	CAnchorBzl::GetFastsyncFilesVecByAnchor(&vFastsyncVec, iLastAnchor);
	PrintFilesVec(&vFastsyncVec);
	CFilesVec::DeleteFilesVec(&vFastsyncVec);

	LOG_DEBUG(_T("add new anchor"));
	CAnchorBzl::AddNewAnchAndUnlock(++iLastAnchor);

	CAnchorBzl::GetFastsyncFilesVecByAnchor(&vFastsyncVec, 1);
	PrintFilesVec(&vFastsyncVec);
	CFilesVec::DeleteFilesVec(&vFastsyncVec);
}

BOOL CAnchorTestDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	CAnchorBzl::Destroy();
	log_fini();

	return CDialog::DestroyWindow();
}
