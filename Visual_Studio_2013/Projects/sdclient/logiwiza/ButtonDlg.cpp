// ButtonDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "logiwiza.h"
#include "LoginWizdDlg.h"
#include "LoginWizdBzl.h"

#include "ButtonDlg.h"


// CButtonDlg 对话框

IMPLEMENT_DYNAMIC(CButtonDlg, CDialog)

CButtonDlg::CButtonDlg(CWnd* pParent /*=NULL*/)
: CDialog(CButtonDlg::IDD, pParent)
{

}

CButtonDlg::~CButtonDlg()
{
}

void CButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEXT, m_btnNext);
	DDX_Control(pDX, IDC_PREV, m_btnPrev);
}


BEGIN_MESSAGE_MAP(CButtonDlg, CDialog)
	ON_BN_CLICKED(IDC_NEXT, &CButtonDlg::OnBnClickedNext)
	ON_BN_CLICKED(IDC_PREV, &CButtonDlg::OnBnClickedPrev)
END_MESSAGE_MAP()

BOOL CButtonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// CButtonDlg 消息处理程序

BOOL CButtonDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CButtonDlg::OnBnClickedNext()
{
	// TODO: 在此添加控件通知处理程序代码
	LoginWizdBzl::PerformNextRefresh();
}

void CButtonDlg::OnBnClickedPrev()
{
	// TODO: 在此添加控件通知处理程序代码
	LoginWizdBzl::PerformPrevRefresh();
}

void CButtonDlg::DrawBtnNormalBitmap(DWORD dwFocusBtnNum) {
	// TODO: 在此添加控件通知处理程序代码
	switch (dwFocusBtnNum) {
	case COVER_PAGE:
		m_btnPrev.SetWindowText(_T("<上一步"));
		m_btnNext.SetWindowText(_T("下一步>"));
		m_btnPrev.ShowWindow(SW_HIDE);
		break;
	case LOGIN_PAGE:
		m_btnPrev.SetWindowText(_T("<上一步"));
		m_btnNext.SetWindowText(_T("登录>"));
		m_btnPrev.ShowWindow(SW_SHOW);
		m_btnPrev.EnableWindow(FALSE);
		break;
	case DISKSETTING_PAGE:
		m_btnPrev.SetWindowText(_T("<上一步"));
		m_btnNext.SetWindowText(_T("下一步>"));
		m_btnPrev.EnableWindow(TRUE);
		break;
	case START_FORMAT_PAGE:
		m_btnPrev.SetWindowText(_T("<上一步"));
		m_btnNext.SetWindowText(_T("创建>"));
		m_btnPrev.EnableWindow(TRUE);
		break;
	case FINISH_FORMAT_PAGE:
		m_btnPrev.SetWindowText(_T("<上一步"));
		m_btnNext.SetWindowText(_T("下一步>"));
		m_btnPrev.EnableWindow(TRUE);
		break;
	case FINISH_PAGE:
		m_btnPrev.SetWindowText(_T("<上一步"));
		m_btnNext.SetWindowText(_T("完成>"));
		m_btnPrev.ShowWindow(SW_HIDE);
		break;
	}
}


void CButtonDlg::DrawBtnActionBitmap(DWORD dwFocusBtnNum) {
	// TODO: 在此添加控件通知处理程序代码
	switch (dwFocusBtnNum) {
	case COVER_PAGE:
		m_btnPrev.EnableWindow(FALSE);
		m_btnNext.EnableWindow(FALSE);
		break;
	case LOGIN_PAGE:
		m_btnPrev.EnableWindow(FALSE);
		m_btnNext.EnableWindow(FALSE);
		break;
	case DISKSETTING_PAGE:
		m_btnPrev.EnableWindow(FALSE);
		m_btnNext.EnableWindow(FALSE);
		break;
	case START_FORMAT_PAGE:
		m_btnPrev.EnableWindow(FALSE);
		m_btnNext.EnableWindow(FALSE);
		break;
	case FINISH_FORMAT_PAGE:
		m_btnPrev.EnableWindow(FALSE);
		m_btnNext.EnableWindow(FALSE);
		break;
	case FINISH_PAGE:
		m_btnPrev.EnableWindow(FALSE);
		m_btnNext.EnableWindow(FALSE);
		break;
	}
}

void CButtonDlg::DrawBtnFinishBitmap(DWORD dwFocusBtnNum) {
	// TODO: 在此添加控件通知处理程序代码
	switch (dwFocusBtnNum) {
	case COVER_PAGE:
		m_btnPrev.EnableWindow(TRUE);
		m_btnNext.EnableWindow(TRUE);
		break;
	case LOGIN_PAGE:
		m_btnPrev.EnableWindow(FALSE);	
		m_btnNext.EnableWindow(TRUE);
		break;
	case DISKSETTING_PAGE:
		m_btnPrev.EnableWindow(TRUE);	
		m_btnNext.EnableWindow(TRUE);	
		break;
	case START_FORMAT_PAGE:
		m_btnPrev.EnableWindow(FALSE);
		m_btnNext.EnableWindow(FALSE);
		break;
	case FINISH_FORMAT_PAGE:
		m_btnPrev.EnableWindow(TRUE);
		m_btnNext.EnableWindow(TRUE);
		break;
	case FINISH_PAGE:
		m_btnPrev.EnableWindow(TRUE);
		m_btnNext.EnableWindow(TRUE);
		break;
	}
}
