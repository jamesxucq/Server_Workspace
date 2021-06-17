
// RiverTDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RiverT.h"
#include "RiverTDlg.h"

#include "RiverFS/RiverFS.h"
#include "WatcherBzl.h"

// CRiverTDlg �Ի���
CRiverTDlg::CRiverTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRiverTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRiverTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRiverTDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CRiverTDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CRiverTDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CRiverTDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CRiverTDlg ��Ϣ�������

BOOL CRiverTDlg::OnInitDialog()
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

void CRiverTDlg::OnPaint()
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
HCURSOR CRiverTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRiverTDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CRiverTDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(RiverFS::Initialize(_T("D:"), _T("Z:"))) {
		//LOG_TRACE(_T("objAnchor Create False !\nFile: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return;
	}
	NWatcherBzl::Initialize(_T("Z:"));
}

void CRiverTDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//close the open handle
	RiverFS::Finalize();
	NWatcherBzl::Finalize();
}
