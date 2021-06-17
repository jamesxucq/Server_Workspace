// SelectiveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"

#include "SelectiveBzl.h"

#include "SelectiveDlg.h"


// CSelectiveDlg 对话框

IMPLEMENT_DYNAMIC(CSelectiveDlg, CDialog)

CSelectiveDlg::CSelectiveDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSelectiveDlg::IDD, pParent)
{
}

CSelectiveDlg::~CSelectiveDlg()
{
}

void CSelectiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADVANCE_MODE_BTN, m_btnAdvanceMode);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_SELECTIVE_TREE, m_tvDiresTView);
}


BEGIN_MESSAGE_MAP(CSelectiveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSelectiveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSelectiveDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ADVANCE_MODE_BTN, &CSelectiveDlg::OnBnClickedAdvanceModeBtn)
END_MESSAGE_MAP()


// CSelectiveDlg 消息处理程序

BOOL CSelectiveDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}
//
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSelectiveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//
	// TODO:  在此添加额外的初始化
	m_imgState.Create(IDB_CHECK_STATE,13, 1, RGB(255,255,255));
	m_imgList.Create(IDB_FOLDER_LIST,16, 1, RGB(255,255,255));
//
	m_tvDiresTView.SetImageList(&m_imgList, TVSIL_NORMAL);
	m_tvDiresTView.SetImageList(&m_imgState, TVSIL_STATE);
	// m_tvDiresTView.SetBkColor(RGB(0, 250, 255)); // 设置m_mytree的背景色
//
	if(NORMAL_SELECTIVE == NSelectiveBzl::GetSelectiveDlgType()) {
		m_btnAdvanceMode.EnableWindow(TRUE);
		m_btnAdvanceMode.ShowWindow(SW_SHOW);
// _LOG_DEBUG(_T("normal selective"));
		//
		m_tvDiresTView.RegistDisplayTView(NSelectiveBzl::GetDiresTViewHmap());
		m_tvDiresTView.RedrawTreeView(NORMAL_TVIEW);
	} else {
		m_btnAdvanceMode.EnableWindow(FALSE);
		m_btnAdvanceMode.ShowWindow(SW_HIDE);
// _LOG_DEBUG(_T("advance selective"));
		//
		m_tvDiresTView.RegistDisplayTView(NSelectiveBzl::GetDiresTViewHmap());
		m_tvDiresTView.RedrawTreeView(ADVANCED_TVIEW);
	}
//
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSelectiveDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	NSelectiveBzl::FinishDirectoriesTView(&m_tvDiresTView);
	NSelectiveBzl::FinalizeEnviro(&m_tvDiresTView);
	OnOK();
}

void CSelectiveDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	NSelectiveBzl::FinalizeEnviro(&m_tvDiresTView);
	OnCancel();
}

void CSelectiveDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	NSelectiveBzl::FinalizeEnviro(&m_tvDiresTView);
	CDialog::OnClose();
}

void CSelectiveDlg::OnBnClickedAdvanceModeBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_btnAdvanceMode.EnableWindow(FALSE);
	m_btnAdvanceMode.ShowWindow(SW_HIDE);
	//
	m_tvDiresTView.RegistDisplayTView(NSelectiveBzl::GetDiresTViewHmap());
	m_tvDiresTView.RedrawTreeView(ADVANCED_TVIEW);
}
