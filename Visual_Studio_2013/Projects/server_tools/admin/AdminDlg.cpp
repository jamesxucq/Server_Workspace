// AdminDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Admin.h"
#include "AdminDlg.h"

#include "ErrorPrint.h"
#include "Logger.h"
#include "Session.h"
#include "Command.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAdminDlg 对话框
CAdminDlg::CAdminDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdminDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAdminDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT_STC, m_stcConnect);
	DDX_Control(pDX, IDC_ADMIN_EDT, m_edtAdmin);
	DDX_Control(pDX, IDC_PASSWD_EDT, m_edtPassword);
	DDX_Control(pDX, IDC_ADDRESS_EDT, m_edtAddress);
	DDX_Control(pDX, IDC_ADMIN_ADDR_EDT, m_edtAddminAddress);
	DDX_Control(pDX, IDC_COMMAND_STC, m_stcCommand);
	DDX_Control(pDX, IDC_SHOWXML_EDT, m_edtShowXml);
}

BEGIN_MESSAGE_MAP(CAdminDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CONNECT_BTN, &CAdminDlg::OnBnClickedConnectBtn)
	ON_BN_CLICKED(IDC_REFRESH_BTN, &CAdminDlg::OnBnClickedRefreshBtn)
	ON_BN_CLICKED(IDC_ADD_BTN, &CAdminDlg::OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_CLEAR_BTN, &CAdminDlg::OnBnClickedClearBtn)
	ON_BN_CLICKED(IDCANCEL, &CAdminDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAdminDlg 消息处理程序

BOOL CAdminDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	_tcscpy_s(ONetworkConf.szProxyStatus, _T("false"));
	OCommand.Create(&ONetworkConf, &OSession);
	log_init(_T(".\\admin.log"), log_lvl_max);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAdminDlg::OnPaint()
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
HCURSOR CAdminDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAdminDlg::OnBnClickedConnectBtn()
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

void CAdminDlg::OnBnClickedRefreshBtn()
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
//
	char list_xml[BODY_BUFF_LEN];
	wchar_t szListXml[BODY_BUFF_LEN];
	if(EXECU_TYPE_SUCCESS == OCommand.OptionList(list_xml, LIST_TYPE_ADMIN, &tCacheParam)) {
		m_stcCommand.SetWindowText(_T("OK"));

		m_edtShowXml.SetLimitText(2048);
		striconv::ansi_unicode(szListXml, list_xml);
		m_edtShowXml.SetWindowText(szListXml);
	} else  {
		m_edtShowXml.SetWindowText(_T(""));
		m_stcCommand.SetWindowText(_T("FAILED"));
	}
//
	UpdateData(FALSE);
}

void CAdminDlg::OnBnClickedAddBtn()
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
	CString csAdminAddress;
	m_edtAddminAddress.GetWindowText(csAdminAddress);	
	if(!csAdminAddress.GetLength()) {
		NErrorPrint::AfxPrintError();
		return;
	}
	char admin_address[SERVNAME_LENGTH];
	striconv::unicode_ansi(admin_address, (wchar_t*)(LPCTSTR)csAdminAddress);
//
	if(EXECU_TYPE_SUCCESS == OCommand.OptionAdd(ADD_TYPE_ADMIN, admin_address, &tCacheParam)) 
		m_stcCommand.SetWindowText(_T("OK"));
	else m_stcCommand.SetWindowText(_T("FAILED"));
//
	UpdateData(FALSE);
}

void CAdminDlg::OnBnClickedClearBtn()
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
	CString csAdminAddress;
	m_edtAddminAddress.GetWindowText(csAdminAddress);	
	if(!csAdminAddress.GetLength()) {
		NErrorPrint::AfxPrintError();
		return;
	}
	char admin_address[SERVNAME_LENGTH];
	striconv::unicode_ansi(admin_address, (wchar_t*)(LPCTSTR)csAdminAddress);
//
	DWORD dwClearStatus = OCommand.OptionClear(CLEAR_TYPE_ADMIN, admin_address, &tCacheParam);
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

void CAdminDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	log_fini();
	OnCancel();
}
