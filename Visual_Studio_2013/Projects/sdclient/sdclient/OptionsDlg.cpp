// OptionsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "ClientDlg.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COptionsDlg 对话框
COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(COptionsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APPLY_BTN, m_btnApply);
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OK_BTN, &COptionsDlg::OnBnClickedOkBtn)
	ON_BN_CLICKED(IDC_CANCEL_BTN, &COptionsDlg::OnBnClickedCancelBtn)
	ON_BN_CLICKED(IDC_APPLY_BTN, &COptionsDlg::OnBnClickedApplyBtn)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序

BOOL COptionsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}
//
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
//
	// TODO: 在此添加额外的初始化代码
	// NLOCA_PASS_EXCLUS; // add by james 20141220
#define OPTIONS_DLG_OPEN ((CClientDlg*)m_pParentWnd)->OptionsDlgOpen
	OPTIONS_DLG_OPEN(TRUE);
//
	// Init the subdlg
	CRect rc;
	GetDlgItem(IDC_BUTTON_POS)->GetWindowRect(rc);
	ScreenToClient(&rc);
	m_pButtonDlg = new CButtonDlg(this);
	m_pButtonDlg->Create(IDD_BUTTON_DLG, this);
	m_pButtonDlg->MoveWindow(&rc);
//
	GetDlgItem(IDC_CHILDREN_POS)->GetWindowRect(rc);
	ScreenToClient(&rc);
	if(!m_dgChildrenDlg.Create(&rc, this)) 
		return TRUE;
	m_dgChildrenDlg.SetChildrenValue();
//
	m_btnApply.EnableWindow(FALSE);
	RefreshWindow(ENUM_GENERAL);
//
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COptionsDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // 用于绘制的设备上下文
//
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
//
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR COptionsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL COptionsDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dgChildrenDlg.Destroy();
	m_pButtonDlg->DestroyWindow();
	delete  m_pButtonDlg;
//
	return CDialog::DestroyWindow();
}

void COptionsDlg::OnBnClickedOkBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_dgChildrenDlg.GetChildrenValue();
	NLocalBzl::SaveOptionConfig();
//
	OnOK();
	OPTIONS_DLG_OPEN(FALSE);
	NLOCA_RELEASE_EXCLUS(0x01); // add by james 20141220
}

void COptionsDlg::OnBnClickedCancelBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
	OPTIONS_DLG_OPEN(FALSE);
	NLOCA_RELEASE_EXCLUS(0x01); // add by james 20141220
}

void COptionsDlg::OnBnClickedApplyBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_btnApply.EnableWindow(FALSE);
//
	m_dgChildrenDlg.GetChildrenValue();
	NLocalBzl::SaveOptionConfig();
}


void COptionsDlg::RefreshWindow(DWORD dwSubDlgNum)
{
	m_pButtonDlg->DrawBtnBitmap(dwSubDlgNum);
	m_dgChildrenDlg.DisplayWindow(dwSubDlgNum); 
}

void COptionsDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OPTIONS_DLG_OPEN(FALSE);
	NLOCA_RELEASE_EXCLUS(0x01); // add by james 20141220
//
	CDialog::OnClose();
}
