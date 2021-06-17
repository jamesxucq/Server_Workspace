// ClearUserDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ClearUser.h"
#include "ClearUserDlg.h"

#include "ErrorPrint.h"
#include "Logger.h"
#include "Session.h"
#include "Command.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClearUserDlg �Ի���
CClearUserDlg::CClearUserDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClearUserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClearUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMMAND_STC, m_stcCommand);
	DDX_Control(pDX, IDC_CONNECT_STC, m_stcConnect);
	DDX_Control(pDX, IDC_ADMIN_EDT, m_edtAdmin);
	DDX_Control(pDX, IDC_PASSWD_EDT, m_edtPassword);
	DDX_Control(pDX, IDC_ADDRESS_EDT, m_edtAddress);
	DDX_Control(pDX, IDC_UID_EDT, m_edtUID);
	DDX_Radio(pDX, IDC_SINGLEUSR_RDO, m_rdoClearType);
	//DDX_Control(pDX, IDC_SINGLEUSR_RDO, m_rdoClearType);
}

BEGIN_MESSAGE_MAP(CClearUserDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CLEAR_BTN, &CClearUserDlg::OnBnClickedClearBtn)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CONNECT_BTN, &CClearUserDlg::OnBnClickedConnectBtn)
	ON_BN_CLICKED(IDCANCEL, &CClearUserDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CClearUserDlg ��Ϣ�������

BOOL CClearUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	_tcscpy_s(ONetworkConf.szProxyStatus, _T("false"));
	OCommand.Create(&ONetworkConf, &OSession);
	log_init(_T(".\\clear_user.log"), log_lvl_max);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClearUserDlg::OnPaint()
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
HCURSOR CClearUserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClearUserDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	OCommand.Destroy();
//
	CDialog::OnClose();
}

void CClearUserDlg::OnBnClickedConnectBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	// m_stcCommand.SetWindowText(_T("��������..."));
//
	struct CacheParam tCacheParam;
	CString csAddress, csAdmin, csPassword;
//
	m_edtAddress.GetWindowText(csAddress);
	m_edtAdmin.GetWindowText(csAdmin);
	m_edtPassword.GetWindowText(csPassword);
	if(!csPassword.GetLength() || !csAdmin.GetLength() || !csAddress.GetLength()) {
		NErrorPrint::AfxPrintError();
		return;
	}
	char address_txt[SERVNAME_LENGTH];
	striconv::unicode_ansi(address_txt, (wchar_t*)(LPCTSTR)csAddress);
	striutil::address_split(&tCacheParam.Address, address_txt);
	_tcscpy_s(tCacheParam.szAdmin, csAdmin.GetLength()+1, (wchar_t*)(LPCTSTR)csAdmin);	
	_tcscpy_s(tCacheParam.szPassword, csPassword.GetLength()+1, (wchar_t*)(LPCTSTR)csPassword);
//
	char list_xml[BODY_BUFF_LEN];
	if(EXECU_TYPE_SUCCESS == OCommand.OptionList(list_xml, LIST_TYPE_ADMIN, &tCacheParam)) 
		m_stcConnect.SetWindowText(_T("OK"));
	else m_stcConnect.SetWindowText(_T("FAILED"));
//
	UpdateData(FALSE);
}

void CClearUserDlg::OnBnClickedClearBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_stcCommand.SetWindowText(_T("�������..."));
//
	DWORD dwClearType = CLEAR_TYPE_SINGLE;
	if(0 == m_rdoClearType) dwClearType = CLEAR_TYPE_SINGLE;
	else if(1 == m_rdoClearType) dwClearType = CLEAR_TYPE_ALLUSRS;
//
	struct CacheParam tCacheParam;
	CString csAddress, csAdmin, csPassword;
//
	m_edtAddress.GetWindowText(csAddress);
	m_edtAdmin.GetWindowText(csAdmin);
	m_edtPassword.GetWindowText(csPassword);
	if(!csPassword.GetLength() || !csAdmin.GetLength() || !csAddress.GetLength()) {
		NErrorPrint::AfxPrintError();
		return;
	}
	char address_txt[SERVNAME_LENGTH];
	striconv::unicode_ansi(address_txt, (wchar_t*)(LPCTSTR)csAddress);
	striutil::address_split(&tCacheParam.Address, address_txt);
	_tcscpy_s(tCacheParam.szAdmin, csAdmin.GetLength()+1, (wchar_t*)(LPCTSTR)csAdmin);	
	_tcscpy_s(tCacheParam.szPassword, csPassword.GetLength()+1, (wchar_t*)(LPCTSTR)csPassword);
//
	char uid[MIN_STRING_LEN];
	if(CLEAR_TYPE_SINGLE == dwClearType) {
		CString csUID;
		m_edtUID.GetWindowText(csUID);
		striconv::unicode_ansi(uid, (wchar_t*)(LPCTSTR)csUID);
		if(!csUID.GetLength()) {
			NErrorPrint::AfxPrintError();
			return;
		}
	} else strcpy_s(uid, "*");
//
	DWORD dwClearStatus = OCommand.OptionClear(dwClearType, uid, &tCacheParam);
	switch(dwClearStatus) {
		case EXECU_TYPE_UNKNOWN:
		case EXECU_TYPE_FAILED:
			m_stcCommand.SetWindowText(_T("FAILED"));
			break;
		case EXECU_TYPE_SUCCESS:
			m_stcCommand.SetWindowText(_T("OK"));
			break;
		case EXECU_TYPE_EXCEPTION:
			m_stcCommand.SetWindowText(_T("EXCEPTION"));
			break;
		case EXECU_TYPE_CONFLICT:
			m_stcCommand.SetWindowText(_T("CONFLICT"));
			break;	
	}
//
	UpdateData(FALSE);
} 
void CClearUserDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	log_fini();
	OnCancel();
}
