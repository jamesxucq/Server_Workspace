// ButtonDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "logiwiza.h"
#include "LoginWizdDlg.h"
#include "LoginWizdBzl.h"

#include "ButtonDlg.h"


// CButtonDlg �Ի���

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// CButtonDlg ��Ϣ�������

BOOL CButtonDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CButtonDlg::OnBnClickedNext()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LoginWizdBzl::PerformNextRefresh();
}

void CButtonDlg::OnBnClickedPrev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LoginWizdBzl::PerformPrevRefresh();
}

void CButtonDlg::DrawBtnNormalBitmap(DWORD dwFocusBtnNum) {
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	switch (dwFocusBtnNum) {
	case COVER_PAGE:
		m_btnPrev.SetWindowText(_T("<��һ��"));
		m_btnNext.SetWindowText(_T("��һ��>"));
		m_btnPrev.ShowWindow(SW_HIDE);
		break;
	case LOGIN_PAGE:
		m_btnPrev.SetWindowText(_T("<��һ��"));
		m_btnNext.SetWindowText(_T("��¼>"));
		m_btnPrev.ShowWindow(SW_SHOW);
		m_btnPrev.EnableWindow(FALSE);
		break;
	case DISKSETTING_PAGE:
		m_btnPrev.SetWindowText(_T("<��һ��"));
		m_btnNext.SetWindowText(_T("��һ��>"));
		m_btnPrev.EnableWindow(TRUE);
		break;
	case START_FORMAT_PAGE:
		m_btnPrev.SetWindowText(_T("<��һ��"));
		m_btnNext.SetWindowText(_T("����>"));
		m_btnPrev.EnableWindow(TRUE);
		break;
	case FINISH_FORMAT_PAGE:
		m_btnPrev.SetWindowText(_T("<��һ��"));
		m_btnNext.SetWindowText(_T("��һ��>"));
		m_btnPrev.EnableWindow(TRUE);
		break;
	case FINISH_PAGE:
		m_btnPrev.SetWindowText(_T("<��һ��"));
		m_btnNext.SetWindowText(_T("���>"));
		m_btnPrev.ShowWindow(SW_HIDE);
		break;
	}
}


void CButtonDlg::DrawBtnActionBitmap(DWORD dwFocusBtnNum) {
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
