// LocalBzl.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "ApplicaBzl.h"
#include "LocalBzl.h"

#include "ClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
const UINT WM_ADDICON_TOTASKBAR = ::RegisterWindowMessage(_T("TaskbarCreated"));

#define EXIT_FAIL_DELAY 3500

// CClientDlg �Ի���
CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pOptionsDlg = NULL;
	m_bShowTrayNotify = TRUE; // zxn
	m_bOptionsOpened = FALSE;
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotify)
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_SUSPEND, &CClientDlg::OnAppSuspend)
	ON_COMMAND(ID_OPTIONS_OPEN, &CClientDlg::OnOptionsOpen)
	ON_COMMAND(ID_OPEN_ROOTDIR, &CClientDlg::OnRootDirectoryOpen)
	ON_COMMAND(ID_PROCESS_STATUS, &CClientDlg::OnPausedResume)
	ON_REGISTERED_MESSAGE(WM_ADDICON_TOTASKBAR, &CClientDlg::OnAddIconToTaskbar)
END_MESSAGE_MAP()

// CClientDlg ��Ϣ�������

BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}
//
	return CDialog::PreTranslateMessage(pMsg);
}

//
// Handle notification from tray icon: display a message.
//
LRESULT CClientDlg::OnTrayNotify(WPARAM uID,LPARAM lEvent)
{
	if (m_bShowTrayNotify) 
	{
		static LPCSTR MouseMessages[] = { "WM_MOUSEMOVE",
			"WM_LBUTTONDOWN", "WM_LBUTTONUP", "WM_LBUTTONDBLCLK",
			"WM_RBUTTONDOWN", "WM_RBUTTONUP", "WM_RBUTTONDBLCLK",
			"WM_MBUTTONDOWN", "WM_MBUTTONUP", "WM_MBUTTONDBLCLK" };
		switch(uID) {
		case WM_MOUSEMOVE:
			break;	
		case WM_RBUTTONDBLCLK:
			NLocalBzl::OpenRootDirectorise(this->m_hWnd);
			break;				
		default:
			break;		
		}
	}
//
	return 0;
}

void CClientDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (NLocalBzl::GetShutdownStatus()) CDialog::OnClose();
	else ShowWindow(SW_HIDE);
}

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	if(NApplicaBzl::ApplicaInitialize(this)) {
		AFX_PRINT_ERRCODE(ERR_INIT_ENV);
		return true;
	}
	_LOG_INFO(_T("initial the application Ok"));
	return true;  // ���ǽ��������õ��ؼ������򷵻� true
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::OnAppSuspend()
{
	// TODO: �ڴ���������������
	//
	static LONG lAppSuspend;
	//
	if(!m_bOptionsOpened && !InterlockedCompareExchange(&lAppSuspend, 0x0001, 0x0000)) {
		_LOG_INFO(_T("suspend the application"));
		//
		DWORD dwSuspendPrepare = 0x01;
		if(!NExecTransBzl::GetTransmitProcessStatus()) { // ͬ���߳�,����ͬ��
			if(IDYES == MessageBox(_T("����ͬ��,�Ƿ�ֹͣͬ�����˳�?"), _T("Tip"), MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2)) {
				NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, _T("����ֹͣͬ��,���Ժ�."));
				NLocalBzl::KillTransmitProcess(); // modify by james 20120106
			} else {
				dwSuspendPrepare = 0x00;
				InterlockedExchange(&lAppSuspend, 0x00);
			}
		} else NLocalBzl::StopTransmitProcess(); // ͬ���߳�,����״̬

		DWORD dwFinalValue;
		if(dwSuspendPrepare && NExecTransBzl::GetTransmitProcessStatus()){
			_LOG_INFO(_T("previous eixt the application"));
			NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, _T("�����˳�,���Ժ�..."));
			if(!(dwFinalValue = NApplicaBzl::ApplicaFinalize())) { 
				// �˳�������������ɹ�
				NLocalBzl::SetShutdownStatus(TRUE);		// really exit
				SendMessage(WM_CLOSE);
			} else {  // �˳������������ʧ��,���³�ʼ��
				if(NApplicaBzl::ApplicaInitialize(this)) {
					PRINT_ERRCODE(this->m_hWnd, ERR_INIT_ENV);
				}
				if(-1 == dwFinalValue) PRINT_ERRCODE(this->m_hWnd, ERR_FINAL_ENV);
				else if(0x01 == dwFinalValue) NLocalBzl::BalloonTip(BALLOON_TIP_EXCEPTION, _T("���ļ���,��رպ�����."));
				//
				Sleep(EXIT_FAIL_DELAY);
				InterlockedExchange(&lAppSuspend, 0x00);
				_LOG_INFO(_T("reinit the application, dwFinalValue:%d"), dwFinalValue);
			}
		}
	}
	//
}

void CClientDlg::OnOptionsOpen()
{
	// TODO: �ڴ���������������
_LOG_DEBUG(_T("options open!")); //
	if(NLOCA_PASS_EXCLUS && !m_bOptionsOpened) {
		if(m_pOptionsDlg) {
			m_pOptionsDlg->DestroyWindow();
			delete m_pOptionsDlg;
		}
		m_pOptionsDlg = new COptionsDlg(this);
		m_pOptionsDlg->Create(IDD_OPTIONS_DLG, this);
		m_pOptionsDlg->ShowWindow(SW_SHOW);
	}
}

void CClientDlg::OnRootDirectoryOpen()
{
	// TODO: �ڴ���������������
	NLocalBzl::OpenRootDirectorise(this->m_hWnd);
}

void CClientDlg::OnPausedResume()
{
	// TODO: �ڴ���������������
	NLocalBzl::PausedResumeTransmitProcess();
}


LRESULT CClientDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	switch(message) {
	case WM_QUERYENDSESSION:
		if(!NExecTransBzl::GetTransmitProcessStatus()) NLocalBzl::KillTransmitProcess();
		else NLocalBzl::StopTransmitProcess();
		break;
	case WM_ENDSESSION:
		NApplicaBzl::ApplicaFinalize();
		break;
	case WM_NCPAINT:
		ShowWindow(SW_HIDE); 
		break;
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CClientDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	if(!m_bOptionsOpened) {
		if(m_pOptionsDlg) {
			m_pOptionsDlg->DestroyWindow();
			delete m_pOptionsDlg;
		}
	}
	return CDialog::DestroyWindow();
}

LRESULT CClientDlg::OnAddIconToTaskbar(WPARAM wParam, LPARAM lParam) {
	NLocalBzl::AddIconToTaskbar(this);
	return 0L;
}