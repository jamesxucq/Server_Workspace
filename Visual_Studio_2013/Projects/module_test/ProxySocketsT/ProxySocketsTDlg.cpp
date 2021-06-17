// ProxySocketsTDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CProxySocketsTDlg �Ի���




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


// CProxySocketsTDlg ��Ϣ�������

BOOL CProxySocketsTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CProxySocketsTDlg::OnPaint()
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
