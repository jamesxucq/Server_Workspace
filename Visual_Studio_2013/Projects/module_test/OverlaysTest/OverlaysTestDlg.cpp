
// OverlaysTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OverlaysTest.h"
#include "OverlaysTestDlg.h"

#ifdef _DEBUG
// #define new DEBUG_NEW
#endif


// COverlaysTestDlg �Ի���




COverlaysTestDlg::COverlaysTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlaysTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COverlaysTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COverlaysTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &COverlaysTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &COverlaysTestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &COverlaysTestDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// COverlaysTestDlg ��Ϣ�������

BOOL COverlaysTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void COverlaysTestDlg::OnPaint()
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
HCURSOR COverlaysTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "CacheLinker.h"

void COverlaysTestDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// Open SDCache Pipe
	objCommandLinker.Initialize();

	if(objCommandLinker.CacheClear(FILE_STATUS_UNKNOWN)) 
		return ;
	if(objCommandLinker.SetCacheWatchingDirectory(_T("Z:"))) 
		return ;
}

void COverlaysTestDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(objCommandLinker.SetCacheWatchingDirectory(_T(""))) {
		//TRACE( _T("set cache watch path failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);	
	}
	if(objCommandLinker.CacheClear(FILE_STATUS_UNKNOWN)) {
		//TRACE( _T("set cache clear failed.\n") );
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);		
	}

	//Exit the Cache Close SDCache Pipe
	objCommandLinker.Finalize();

}

void COverlaysTestDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//	objCommandLinker.CacheFileInsert(_T("H:\\4.txt"), FILE_STATUS_ADDED);
	//	objCommandLinker.CacheFileInsert(_T("H:\\5.txt"), FILE_STATUS_CONFLICT);
	//	objCommandLinker.CacheFileInsert(_T("H:\\4.txt"), FILE_STATUS_SYNCING);
	//	objCommandLinker.CacheErase(_T("H:\\4.txt"), FILE_STATUS_SYNCING);
	//	objCommandLinker.CacheErase(_T("H:\\4.txt"), FILE_STATUS_NORMAL);

}

