// ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"

#include "ErrorPrint.h"
#include "Logger.h"
#include "Session.h"
#include "Command.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CServerDlg 对话框
CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT_STC, m_stcConnect);
	DDX_Control(pDX, IDC_ADMIN_EDT, m_edtAdmin);
	DDX_Control(pDX, IDC_PASSWD_EDT, m_edtPassword);
	DDX_Control(pDX, IDC_ADDRESS_EDT, m_edtAddress);
	DDX_Control(pDX, IDC_WORKER_EDT, m_edtServer);
	DDX_Control(pDX, IDC_PORT_EDT, m_edtPort);
	DDX_Control(pDX, IDC_COMMAND_STC, m_stcCommand);
	DDX_Control(pDX, IDC_SHOWXML_EDT, m_edtShowXml);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT_BTN, &CServerDlg::OnBnClickedConnectBtn)
	ON_BN_CLICKED(IDC_REFRESH_BTN, &CServerDlg::OnBnClickedRefreshBtn)
	ON_BN_CLICKED(IDC_ADD_BTN, &CServerDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_CLEAR_BTN, &CServerDlg::OnBnClickedClearBtn)
	ON_BN_CLICKED(IDCANCEL, &CServerDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	_tcscpy_s(ONetworkConf.szProxyStatus, _T("false"));
	OCommand.Create(&ONetworkConf, &OSession);
	log_init(_T(".\\server.log"), log_lvl_max);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::OnBnClickedConnectBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
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

void CServerDlg::OnBnClickedRefreshBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
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
	wchar_t szListXml[BODY_BUFF_LEN];
	if(EXECU_TYPE_SUCCESS == OCommand.OptionList(list_xml, LIST_TYPE_WORKER, &tCacheParam)) {
		m_stcCommand.SetWindowText(_T("OK"));
		striconv::ansi_unicode(szListXml, list_xml);
		m_edtShowXml.SetWindowText(szListXml);
	} else {
		m_edtShowXml.SetWindowText(_T(""));
		m_stcCommand.SetWindowText(_T("FAILED"));
	}
//
	UpdateData(FALSE);
}

void CServerDlg::OnBnClickedAddBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
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
//
	char address_txt[SERVNAME_LENGTH];
	striconv::unicode_ansi(address_txt, (wchar_t*)(LPCTSTR)csAddress);
	striutil::address_split(&tCacheParam.Address, address_txt);
	_tcscpy_s(tCacheParam.szAdmin, csAdmin.GetLength()+1, (wchar_t*)(LPCTSTR)csAdmin);	
	_tcscpy_s(tCacheParam.szPassword, csPassword.GetLength()+1, (wchar_t*)(LPCTSTR)csPassword);
//
	CString csServer, csPort;
	m_edtServer.GetWindowText(csServer);
	m_edtPort.GetWindowText(csPort);
	if(!csServer.GetLength() || !csPort.GetLength()) {
		NErrorPrint::AfxPrintError();
		return;
	}
	char serv_server[SERVNAME_LENGTH];
	striconv::unicode_ansi(serv_server, (wchar_t*)(LPCTSTR)csServer);
	char port_txt[SERVNAME_LENGTH];
	striconv::unicode_ansi(port_txt, (wchar_t*)(LPCTSTR)csPort);
	int port_array[128];
	striutil::port_split(port_array, 128, port_txt);
	//
	char serv_address[SERVNAME_LENGTH];
	for(int inde = 0; port_array[inde]; inde++) {
		sprintf_s(serv_address, "%s:%d", serv_server, port_array[inde]);
		if(EXECU_TYPE_SUCCESS != OCommand.OptionAdd(ADD_TYPE_WORKER, serv_address, &tCacheParam)) {
			m_stcCommand.SetWindowText(_T("FAILED"));
			break;
		} else m_stcCommand.SetWindowText(_T("OK"));
	}
	//
	UpdateData(FALSE);
}

void CServerDlg::OnBnClickedClearBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
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
	CString csServer, csPort;
	m_edtServer.GetWindowText(csServer);
	m_edtPort.GetWindowText(csPort);
	if(!csServer.GetLength() || !csPort.GetLength()) {
		NErrorPrint::AfxPrintError();
		return;
	}
	char serv_server[SERVNAME_LENGTH];
	striconv::unicode_ansi(serv_server, (wchar_t*)(LPCTSTR)csServer);
	char port_txt[SERVNAME_LENGTH];
	striconv::unicode_ansi(port_txt, (wchar_t*)(LPCTSTR)csPort);
	int port_array[128];
	striutil::port_split(port_array, 128, port_txt);
	//
	char serv_address[SERVNAME_LENGTH];
	for(int inde = 0; port_array[inde]; inde++) {
		sprintf_s(serv_address, "%s:%d", serv_server, port_array[inde]);
		DWORD dwClearStatus = OCommand.OptionClear(CLEAR_TYPE_WORKER, serv_address, &tCacheParam);
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
	}
	//
	UpdateData(FALSE);
}

void CServerDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	log_fini();
	OnCancel();
}
