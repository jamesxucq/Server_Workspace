// OptionsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "ClientDlg.h"
#include "OptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COptionsDlg �Ի���
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


// COptionsDlg ��Ϣ�������

BOOL COptionsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
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
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
//
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void COptionsDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
//
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
//
		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR COptionsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL COptionsDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_dgChildrenDlg.Destroy();
	m_pButtonDlg->DestroyWindow();
	delete  m_pButtonDlg;
//
	return CDialog::DestroyWindow();
}

void COptionsDlg::OnBnClickedOkBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_dgChildrenDlg.GetChildrenValue();
	NLocalBzl::SaveOptionConfig();
//
	OnOK();
	OPTIONS_DLG_OPEN(FALSE);
	NLOCA_RELEASE_EXCLUS(0x01); // add by james 20141220
}

void COptionsDlg::OnBnClickedCancelBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
	OPTIONS_DLG_OPEN(FALSE);
	NLOCA_RELEASE_EXCLUS(0x01); // add by james 20141220
}

void COptionsDlg::OnBnClickedApplyBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	OPTIONS_DLG_OPEN(FALSE);
	NLOCA_RELEASE_EXCLUS(0x01); // add by james 20141220
//
	CDialog::OnClose();
}
