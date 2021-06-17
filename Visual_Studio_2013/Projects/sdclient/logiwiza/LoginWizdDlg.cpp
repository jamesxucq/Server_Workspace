// LoginWizdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "logiwiza.h"

#include "clientcom/lwizardcom.h"
#include "LoginWizdBzl.h"
#include "LoginWizdDlg.h"


// CLoginWizdDlg 对话框

IMPLEMENT_DYNAMIC(CLoginWizdDlg, CDialog)

CLoginWizdDlg::CLoginWizdDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLoginWizdDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLoginWizdDlg::~CLoginWizdDlg()
{
}

void CLoginWizdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginWizdDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLoginWizdDlg 消息处理程序

BOOL CLoginWizdDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLoginWizdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, true);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
//
	// TODO:  在此添加额外的初始化
	if(LoginWizdBzl::Initialize()) AFX_PRINT_ERRCODE(ERR_LOAD_CONFIG);
	DWORD dwProgramExecuteType = LoginWizdBzl::LoginExecuteType();
//
	CRect rc;
	GetDlgItem(IDC_BUTTON_POS)->GetWindowRect(rc);
	ScreenToClient(&rc);
	m_pButtonDlg = new CButtonDlg(this);
	m_pButtonDlg->Create(IDD_BUTTON_DLG, this);
	m_pButtonDlg->MoveWindow(&rc);
//
	GetDlgItem(IDC_CHILDREN_POS)->GetWindowRect(rc);
	ScreenToClient(&rc);
	if(!m_dgChildrenDlg.Create(&rc, this)) return TRUE;
//
	if(PROGRAM_FIRSTRUN == dwProgramExecuteType)
		LoginWizdBzl::RefreshWindowNormal(COVER_PAGE);
	else if(PROGRAM_NORMALRUN == dwProgramExecuteType)
		LoginWizdBzl::RefreshWindowNormal(LOGIN_PAGE);
//
	return true;  // return true unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLoginWizdDlg::OnPaint()
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
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLoginWizdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLoginWizdDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dgChildrenDlg.Destroy();
	m_pButtonDlg->DestroyWindow();
	delete  m_pButtonDlg;
//
	return CDialog::DestroyWindow();
}

void CLoginWizdDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(IDNO == MessageBox(_T("没有完成创建,是否退出?"), _T("Tip"), 
		MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2)) return;	
//
	CDialog::OnClose();
}

HWND CLoginWizdDlg::DisplayWindowNormal(DWORD dwWizardNumbe)
{
	m_pButtonDlg->DrawBtnNormalBitmap(dwWizardNumbe);
	return m_dgChildrenDlg.DisplayNormalWindow(dwWizardNumbe);
}

void CLoginWizdDlg::DisplayWindowAction(DWORD dwWizardNumbe)
{
	m_dgChildrenDlg.DisplayActionWindow(dwWizardNumbe);
	m_pButtonDlg->DrawBtnActionBitmap(dwWizardNumbe);
}

void CLoginWizdDlg::DisplayWindowFinish(DWORD dwWizardNumbe)
{
	m_dgChildrenDlg.DisplayFinishWindow(dwWizardNumbe);
	m_pButtonDlg->DrawBtnFinishBitmap(dwWizardNumbe);
}

void CLoginWizdDlg::ExitApplica()
{
	OnOK();
}

