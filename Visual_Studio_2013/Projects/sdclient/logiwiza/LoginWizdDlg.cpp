// LoginWizdDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "logiwiza.h"

#include "clientcom/lwizardcom.h"
#include "LoginWizdBzl.h"
#include "LoginWizdDlg.h"


// CLoginWizdDlg �Ի���

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


// CLoginWizdDlg ��Ϣ�������

BOOL CLoginWizdDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
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
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, true);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
//
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
	// �쳣: OCX ����ҳӦ���� FALSE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLoginWizdDlg::OnPaint()
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
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLoginWizdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLoginWizdDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_dgChildrenDlg.Destroy();
	m_pButtonDlg->DestroyWindow();
	delete  m_pButtonDlg;
//
	return CDialog::DestroyWindow();
}

void CLoginWizdDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(IDNO == MessageBox(_T("û����ɴ���,�Ƿ��˳�?"), _T("Tip"), 
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

