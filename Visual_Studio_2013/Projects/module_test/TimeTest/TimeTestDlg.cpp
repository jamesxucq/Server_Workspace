// TimeTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TimeTest.h"
#include "TimeTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTimeTestDlg �Ի���




CTimeTestDlg::CTimeTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTimeTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTimeTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CTimeTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CTimeTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTimeTestDlg ��Ϣ�������

BOOL CTimeTestDlg::OnInitDialog()
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

void CTimeTestDlg::OnPaint()
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
HCURSOR CTimeTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTimeTestDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

#include "strptime.h"
#define TIME_LENGTH				32

void CTimeTestDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������    
	struct tm gm;
	FILETIME filetime;
	char *timestr = "Wed, 15 Nov 1995 04:58:08 GMT";

    memset(&gm, '\0', sizeof (struct tm));
    if (!strptime(timestr, "%a, %d %b %Y %H:%M:%S GMT", &gm)) return;
	SYSTEMTIME st = { 1900+gm.tm_year, 1+gm.tm_mon, gm.tm_wday, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec, 0 };
	if(!SystemTimeToFileTime(&st, &filetime)) return; 

	char timestrx[TIME_LENGTH];
	SYSTEMTIME stx;

	if(!FileTimeToSystemTime(&filetime, &stx)) return;
	struct tm gmt = {stx.wSecond, stx.wMinute, stx.wHour, stx.wDay, stx.wMonth-1, stx.wYear-1900, stx.wDayOfWeek, 0, 0};
	strftime(timestrx, TIME_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", &gmt);

}
