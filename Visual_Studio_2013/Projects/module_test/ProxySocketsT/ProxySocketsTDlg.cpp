// ProxySocketsTDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProxySocketsT.h"
#include "ProxySocketsTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "NormalSockets.h"
#include "SdSocketsBzl.h"
#include "proxysockets.h"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProxySocketsTDlg 对话框




CProxySocketsTDlg::CProxySocketsTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProxySocketsTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProxySocketsTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProxySocketsTDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CProxySocketsTDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CProxySocketsTDlg 消息处理程序

BOOL CProxySocketsTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CProxySocketsTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CProxySocketsTDlg::OnPaint()
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
HCURSOR CProxySocketsTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void FILL_MSG_SOCKETS(struct MsgSocketsArgu *pMsgSocketsArgu) { 
	/////////////////////////////////////////////////////////////////////////////
	strcpy_s((pMsgSocketsArgu)->sRegisHost, "192.168.1.103"); 
	(pMsgSocketsArgu)->iRegisPort = 8080; 
	strcpy_s((pMsgSocketsArgu)->sAuthHost, "192.168.1.103");
	(pMsgSocketsArgu)->iAuthPort = 8089; 
	/////////////////////////////////////////////////////////////////////////////
	_tcscpy_s((pMsgSocketsArgu)->sDownloadLimit, _T("")); 
	(pMsgSocketsArgu)->iDownloadRate = 128; 
	_tcscpy_s((pMsgSocketsArgu)->sUploadLimit, _T("")); 
	(pMsgSocketsArgu)->iUploadRate = 128; 
	_tcscpy_s((pMsgSocketsArgu)->sProxyStatus, _T("true")); 

	strcpy_s((pMsgSocketsArgu)->sServer, "192.168.1.3"); 
	// _tcscpy_s((pMsgSocketsArgu)->sType, _T("HTTP")); 
	// _tcscpy_s((pMsgSocketsArgu)->sType, _T("HTTPS")); 
	// (pMsgSocketsArgu)->iPort = 808; 

	// _tcscpy_s((pMsgSocketsArgu)->sType, _T("SOCKS4")); 
	_tcscpy_s((pMsgSocketsArgu)->sType, _T("SOCKS5")); 
	(pMsgSocketsArgu)->iPort = 1080; 

	// _tcscpy_s((pMsgSocketsArgu)->sProxyAuth, _T("false")); 
	_tcscpy_s((pMsgSocketsArgu)->sProxyAuth, _T("true")); 
	strcpy_s((pMsgSocketsArgu)->sUserName, "james"); 
	strcpy_s((pMsgSocketsArgu)->sPassword, "123456"); 
}  


#define REGISTER	_T("Register Sdtp/1.1\r\n")
bool BuildRegistSend(wchar_t *sdtp_buffer)
{
	wchar_t TextBuff[TEXT_BUFF_SIZE];

	_tcscpy_s(TextBuff, REGISTER);
	_tcscat_s(TextBuff, _T("User-Name:xujie@163.com\r\n"));
	_tcscat_s(TextBuff, _T("Password:123456\r\n"));
	_tcscat_s(TextBuff, _T("Client-Name:xxxx\r\n\r\n"));

	_tcscpy_s(sdtp_buffer, TEXT_BUFF_SIZE, TextBuff);

	return true;
}

void CProxySocketsTDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	struct MsgSocketsArgu stMsgSocketsArgu;
	// CONVERT_MSG_SOCKETS(&stMsgSocketsArgu, pLocalConf);
	FILL_MSG_SOCKETS(&stMsgSocketsArgu);

	CMsgSocketsBzl *pQuerySocketsBzl = NSdSocketsBzl::Factory(&stMsgSocketsArgu);

	pQuerySocketsBzl->SetAddress(stMsgSocketsArgu.sRegisHost, stMsgSocketsArgu.iRegisPort);


	wchar_t		head_buffer[HEAD_BUFF_LEN];
	char m_bitsrecv_buffer[RECV_BUFF_SIZE];

	BuildRegistSend(head_buffer);

	NStringCon::unicode_utf8(m_bitsrecv_buffer, head_buffer);
	int retValue = pQuerySocketsBzl->SendReceive(m_bitsrecv_buffer, m_bitsrecv_buffer);
	NStringCon::utf8_unicode(head_buffer, m_bitsrecv_buffer);


	NSdSocketsBzl::DestroyObject(pQuerySocketsBzl);
	pQuerySocketsBzl = NULL;
}
