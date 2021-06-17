// ShellCacheTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ShellCacheTest.h"
#include "ShellCacheTestDlg.h"

#include "CacheLinker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CShellCacheTestDlg �Ի���




CShellCacheTestDlg::CShellCacheTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShellCacheTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShellCacheTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShellCacheTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CShellCacheTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CShellCacheTestDlg ��Ϣ�������

BOOL CShellCacheTestDlg::OnInitDialog()
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

void CShellCacheTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CShellCacheTestDlg::OnPaint()
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
HCURSOR CShellCacheTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CShellCacheTestDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Open SDCache Pipe
	OCmdCacheLinker.Create();

	if(!OCmdCacheLinker.CacheClear(FILE_STATUS_UNKNOWN)) 
		return;
	if(!OCmdCacheLinker.SetCacheWatchingDirectory(_T("E:\\"))) 
		return;
//	if(!DisplayFilesAttribShell(vpFileAttrib, sDriveRoot)) 
//		return;
//	if(DisplayFilesVecShell(vpLastFilesVec, sDriveRoot)) 
//		return;

/*
#define		FILE_STATUS_SYNCING						0x00000001
//////////////////////////////////////////////////////////////�¶���ʾ������С
#define		FILE_STATUS_DELETE						0x00000002
#define		FILE_STATUS_MODIFIED					0x00000004
#define		FILE_STATUS_ADDED						0x00000008
//////////////////////////////////////////////////////////////�¶���ʾ�������
#define		FILE_STATUS_CONFLICT					0x00000010 //����0
	//���������µ�ͼ��̳���
#define		FILE_STATUS_LOCKED						0x00000020
#define		FILE_STATUS_READONLY					0x00000040
#define		FILE_STATUS_FORBID						0x00000080
//////////////////////////////////////////////////////////////
#define		FILE_STATUS_NORMAL						0x00001000
#define		FILE_STATUS_DIRECTORY					0x00002000
*/

	OCmdCacheLinker.CacheFileInsert(_T("E:\\1.txt"), FILE_STATUS_SYNCING);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\2.txt"), FILE_STATUS_DELETE);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\3.txt"), FILE_STATUS_MODIFIED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\4.txt"), FILE_STATUS_ADDED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\5.txt"), FILE_STATUS_CONFLICT);
	// OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_LOCKED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\7.txt"), FILE_STATUS_READONLY);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\8.txt"), FILE_STATUS_FORBID);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\9.txt"), FILE_STATUS_NORMAL);
	// OCmdCacheLinker.CacheErase(_T("E:\\5.txt"), FILE_STATUS_NORMAL);

	OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_MODIFIED);
	OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_SYNCING);
	// OCmdCacheLinker.CacheFileInsert(_T("E:\\6.txt"), FILE_STATUS_NORMAL);
	// OCmdCacheLinker.CacheErase(_T("E:\\6.txt"), FILE_STATUS_NORMAL);
	OCmdCacheLinker.CacheErase(_T("E:\\6.txt"), FILE_STATUS_SYNCING);


	if(!OCmdCacheLinker.SetCacheWatchingDirectory(_T(""))) {
		//TRACE( _T("set cache watch path failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);	
	}
	if(!OCmdCacheLinker.CacheClear(FILE_STATUS_UNKNOWN)) {
		//TRACE( _T("set cache clear failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);		
	}

	//Exit the Cache Close SDCache Pipe
	OCmdCacheLinker.Destroy();
}
