// AnchorTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AnchorTest.h"
#include "AnchorTestDlg.h"

#include "Logger.h"
#include "AnchorBzl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAnchorTestDlg �Ի���




CAnchorTestDlg::CAnchorTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnchorTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAnchorTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAnchorTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_TEST_BTN, &CAnchorTestDlg::OnBnClickedTestBtn)
END_MESSAGE_MAP()


// CAnchorTestDlg ��Ϣ�������

BOOL CAnchorTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	log_init(_T("e:\\anchor.log"), log_lvl_max);
	CAnchorBzl::Create(_T("~\\SyncingCtrl.flag"), _T("~\\anchor_index.ndx"), _T("~\\anchor_data.dat"), _T("e:"));

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAnchorTestDlg::OnPaint()
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
HCURSOR CAnchorTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void PrintFilesVec(FilesAttribVec *vpFilesAttVec)  // for debug 20110222
{
	vector <struct FileAttrib *>::iterator iter;

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		LOG_DEBUG(_T("-----------------------------------------------"));
		LOG_DEBUG(_T("--------------------------- filename:%s mod_type:%c"), (*iter)->filename, (*iter)->mod_type);
	}
}

void CAnchorTestDlg::OnBnClickedTestBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	FilesAttribVec vFastsyncVec;
	DWORD iLastAnchor = CAnchorBzl::GetLastAnchAndLock();
	LOG_DEBUG(_T("the last anchor is:%d"), iLastAnchor);
	if(!iLastAnchor) {
		LOG_DEBUG(_T("add new anchor"));
		CAnchorBzl::AddNewAnchAndUnlock(++iLastAnchor);
	}

	CAnchorBzl::AnchFilesVecInsert(_T("C:\\1.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\2.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\3.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\4.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\5.txt"), 'A');
	CAnchorBzl::AnchFilesVecInsert(_T("C:\\6.txt"), 'A');

	iLastAnchor = CAnchorBzl::GetLastAnchAndLock();
	LOG_DEBUG(_T("the last anchor is:%d"), iLastAnchor);
	//CAnchorBzl::UnlockAnchorFile();

	CAnchorBzl::GetFastsyncFilesVecByAnchor(&vFastsyncVec, iLastAnchor);
	PrintFilesVec(&vFastsyncVec);
	CFilesVec::DeleteFilesVec(&vFastsyncVec);

	LOG_DEBUG(_T("add new anchor"));
	CAnchorBzl::AddNewAnchAndUnlock(++iLastAnchor);

	CAnchorBzl::GetFastsyncFilesVecByAnchor(&vFastsyncVec, 1);
	PrintFilesVec(&vFastsyncVec);
	CFilesVec::DeleteFilesVec(&vFastsyncVec);
}

BOOL CAnchorTestDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	CAnchorBzl::Destroy();
	log_fini();

	return CDialog::DestroyWindow();
}
