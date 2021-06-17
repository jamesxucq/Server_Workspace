
// DowndDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "liveupda.h"
#include "DowndBzl.h"
#include "DowndDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDowndDlg �Ի���

CDowndDlg::CDowndDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDowndDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDowndDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDowndDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// }}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDowndDlg ��Ϣ�������
UINT LiveDowndThread(LPVOID lpParam);
const UINT uiDowndTimerID = 0x0001;
BOOL CDowndDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_WAITING);
	CWinThread *pDowndThread = AfxBeginThread(LiveDowndThread, (LPVOID)NULL);
	SetTimer(uiDowndTimerID, 2048, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDowndDlg::OnPaint()
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
HCURSOR CDowndDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CDowndDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(WM_NCPAINT == message) ShowWindow(SW_HIDE); 
//
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CDowndDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(DOWND_THREAD_ACTIVE != NDowndBzl::GetDowndThreadStatus()) {
		KillTimer(uiDowndTimerID);
		OnOK();
	}
//
	CDialog::OnTimer(nIDEvent);
}

UINT LiveDowndThread(LPVOID lpParam)
{
	NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_ACTIVE);
	DWORD downd_status = NDowndBzl::PerfromDownload();
	switch(downd_status) {
	case DOWND_VALUE_SUCCESS:
		NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_SUCCESS);
		break;
	case DOWND_VALUE_FAILED:
	case DOWND_LOAD_FILE:
	case DOWND_VALUE_UPDATE:
		NDowndBzl::SetDowndThreadStatus(DOWND_THREAD_FAULT);
		break;
	}
// _LOG_DEBUG(_T("downd_status:%d"), downd_status);
//
	return 0;
}
