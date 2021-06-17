
// FileSystemDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileSystem.h"
#include "FileSystemDlg.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

#include "./RiverFS/RiverFS.h"
// #include "Logger.h"
// CFileSystemDlg �Ի���




CFileSystemDlg::CFileSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSystemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFileSystemDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CFileSystemDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFileSystemDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFileSystemDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CFileSystemDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CFileSystemDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CFileSystemDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CFileSystemDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CFileSystemDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CFileSystemDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CFileSystemDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CFileSystemDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CFileSystemDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CFileSystemDlg::OnBnClickedButton13)
END_MESSAGE_MAP()


// CFileSystemDlg ��Ϣ�������

BOOL CFileSystemDlg::OnInitDialog()
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

void CFileSystemDlg::OnPaint()
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
HCURSOR CFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileSystemDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(RiverFS::Initialize(_T("D:"), _T("Z:"))) {
		//LOG_TRACE(_T("objAnchor Create False !\nFile: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return;
	}
}

void CFileSystemDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//close the open handle
	RiverFS::Finalize();
}

void CFileSystemDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if(IS_DIRECTORY( _T("Z:\\example"))) {
		int x = 0;
	} else {
		int x = 0;	
	} 
	if(IS_DIRECTORY(_T("Z:\\kccommon.h"))) {
		int x = 0;
	} else {
		int x = 0;	
	} 
}

//#define RIVER_OPERATION_COPY	0x0001
//#define RIVER_OPERATION_MOVE	0x0002
//#define RIVER_OPERATION_ADD	0x0004
FILETIME ftCreationTime;
void CFileSystemDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {_T("Z:\\xxx\\example")};
	TCHAR szExistsPath[MAX_PATH];
	CommonUtility::IsDirectory(&ftCreationTime, _T("Z:\\example"));
	DWORD dwOperation = RiverFS::FolderIdenti(szExistsPath, szFolderPath, &ftCreationTime);

	if(IS_DIRECTORY(szExistsPath)) {
		int x = 0;
	} else {
		int x = 0;	
	}

	if(IS_DIRECTORY(szFolderPath)) {
		int x = 0;
	} else {
		int x = 0;	
	}
}

void CFileSystemDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {_T("Z:\\example\\TTT")};
	CommonUtility::IsDirectory(&ftCreationTime, szFolderPath);
	RiverFS::FolderAppend(szFolderPath, &ftCreationTime);

	if(IS_DIRECTORY(szFolderPath)) {
		int x = 0;
	} else {
		int x = 0;	
	}
}

void CFileSystemDlg::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {_T("Z:\\kingsoft")};
	CommonUtility::IsDirectory(&ftCreationTime, szFolderPath);
	ERASE_FOLDER(szFolderPath);

	if(IS_DIRECTORY(szFolderPath)) {
		int x = 0;
	} else {
		int x = 0;
	}
}

void CFileSystemDlg::OnBnClickedButton7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {_T("Z:\\cache\\xxx")};
	CommonUtility::IsDirectory(&ftCreationTime, _T("Z:\\xxx"));
	RiverFS::FolderRestore(szFolderPath, &ftCreationTime);

	if(IS_DIRECTORY(szFolderPath)) {
		int x = 0;
	} else {
		int x = 0;	
	}
}

void CFileSystemDlg::OnBnClickedButton8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {_T("Z:\\cache\\example")};
	RiverFS::FolderMove(_T("Z:\\xxx\\example"), szFolderPath);

	if(IS_DIRECTORY(szFolderPath)) {
		int x = 0;
	} else {
		int x = 0;	
	}
}

void CFileSystemDlg::OnBnClickedButton9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilePath[MAX_PATH] = {_T("Z:\\example.h")};
	TCHAR szExistsPath[MAX_PATH];
	CommonUtility::IsDirectory(&ftCreationTime, _T("Z:\\example.h"));
	DWORD dwOperation = RiverFS::FindIdenti(szExistsPath, szFilePath, &ftCreationTime);
}

void CFileSystemDlg::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilePath[MAX_PATH] = {_T("Z:\\example.h")};
	RiverFS::FileAppend(szFilePath, &ftCreationTime);
}

void CFileSystemDlg::OnBnClickedButton11()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFilePath[MAX_PATH] = {_T("Z:\\xxx\\example.h")};
	CommonUtility::IsDirectory(&ftCreationTime, _T("Z:\\xxx\\example.h"));
	RiverFS::DeleteFile(szFilePath);
}

void CFileSystemDlg::OnBnClickedButton12()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {_T("Z:\\example.h")};
	CommonUtility::IsDirectory(&ftCreationTime, _T("Z:\\xxx\\example.h"));
	RiverFS::FileRestore(szFolderPath, &ftCreationTime);
}

void CFileSystemDlg::OnBnClickedButton13()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR szFolderPath[MAX_PATH] = {_T("Z:\\example.h")};
	TCHAR szExistsPath[MAX_PATH] = {_T("Z:\\xxx\\example.h")};
	RiverFS::FileMove(szExistsPath, szFolderPath);
}
