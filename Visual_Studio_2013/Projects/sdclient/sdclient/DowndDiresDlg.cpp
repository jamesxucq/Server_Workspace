// DowndDiresDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sdclient.h"

#include "ExecuteTransmitBzl.h"

#include "DowndDiresDlg.h"


// CDowndDiresDlg �Ի���

IMPLEMENT_DYNAMIC(CDowndDiresDlg, CDialog)

CDowndDiresDlg::CDowndDiresDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDowndDiresDlg::IDD, pParent)
{
}

CDowndDiresDlg::~CDowndDiresDlg()
{
}

void CDowndDiresDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDowndDiresDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CDowndDiresDlg::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDowndDiresDlg ��Ϣ�������

BOOL CDowndDiresDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

UINT DowndDiresInfoThread(LPVOID lpParam);
const UINT uiDowndTimerID = 0x0001;
BOOL CDowndDiresDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	// NSelectiveBzl::StartDowndThread((LPVOID)this);
	NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_WAITING);
	CWinThread *pDowndThread = AfxBeginThread(DowndDiresInfoThread, (LPVOID)NULL);
	SetTimer(uiDowndTimerID, 2048, NULL);
//
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDowndDiresDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(uiDowndTimerID);
//
	OnCancel();
}


void CDowndDiresDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(DOWND_THREAD_ACTIVE != NSelectiveBzl::GetDowndThreadStatus()) {
		KillTimer(uiDowndTimerID);
		OnOK();
	}
//
	CDialog::OnTimer(nIDEvent);
}


UINT DowndDiresInfoThread(LPVOID lpParam)
{
	NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_ACTIVE);
	DWORD downd_status = NSelectiveBzl::PerfromDirectoriesDownd();
// DWORD downd_status = DOWND_VALUE_SUCCESS;
// _LOG_DEBUG(_T("eeeeeeeeeeeeeeeee"));
	switch(downd_status) {
	case DOWND_VALUE_QUERYERR:
	case DOWND_VALUE_FAILED:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_FAULT);
		break;
	case DOWND_VALUE_SERVBUSY:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_SERVBUSY);
		break;
	case DOWND_VALUE_SUCCESS:
// NSelectiveBzl::InitDiresTreeTest(); // for test
		NSelectiveBzl::PrepaDisplayTView();
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_SUCCESS);
		break;
	case DOWND_VALUE_DISCONN:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_DISCONNECT);
		break;
	case DOWND_PROECSS_BUSY:
		NSelectiveBzl::SetDowndThreadStatus(DOWND_THREAD_LOCALBUSY);
		break;
	}
_LOG_DEBUG(_T("downd_status:%08x"), downd_status); // disable by james 20130410
//
	return 0;
}
