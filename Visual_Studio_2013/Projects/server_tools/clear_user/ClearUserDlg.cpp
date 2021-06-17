// ClearUserDlg.cpp : 实现文件
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


// CClearUserDlg 对话框
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


// CClearUserDlg 消息处理程序

BOOL CClearUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	_tcscpy_s(ONetworkConf.szProxyStatus, _T("false"));
	OCommand.Create(&ONetworkConf, &OSession);
	log_init(_T(".\\clear_user.log"), log_lvl_max);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClearUserDlg::OnPaint()
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
HCURSOR CClearUserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClearUserDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OCommand.Destroy();
//
	CDialog::OnClose();
}

void CClearUserDlg::OnBnClickedConnectBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	// m_stcCommand.SetWindowText(_T("正在连接..."));
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
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_stcCommand.SetWindowText(_T("正在清除..."));
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
	// TODO: 在此添加控件通知处理程序代码
	log_fini();
	OnCancel();
}
