// FinishDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "logiwiza.h"
#include "FinishDlg.h"


// CFinishDlg �Ի���

IMPLEMENT_DYNAMIC(CFinishDlg, CDialog)

CFinishDlg::CFinishDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFinishDlg::IDD, pParent)
{

}

CFinishDlg::~CFinishDlg()
{
}

void CFinishDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAY_STC, m_stcDisplay);
}


BEGIN_MESSAGE_MAP(CFinishDlg, CDialog)
END_MESSAGE_MAP()


// CFinishDlg ��Ϣ�������

BOOL CFinishDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

DWORD CFinishDlg::SetFinishValue(int CVolStatus) {
	CString csDisplayTxt = _T("");
//
	if(!CVolStatus)
		csDisplayTxt += _T("\n    �û����óɹ�!\n\n");
	else{
		if(0x000000FF & CVolStatus)
			csDisplayTxt += _T("�û���½ʧ��!\n\n");
		else csDisplayTxt += _T("�û���½�ɹ�.\n\n");

		if(0x0000FF00 & CVolStatus)
			csDisplayTxt += _T("ʹ���Ѵ��ڵ�����!\n\n");
		else csDisplayTxt += _T("�½��������ɹ�.\n\n");

		if(0x00FF0000 & CVolStatus)
			csDisplayTxt += _T("�����������ʧ��!\n\n");
		else csDisplayTxt += _T("����������̳ɹ�.\n\n");

		if(0xFF000000 & CVolStatus)
			csDisplayTxt += _T("δ����!\n\n");
	}
	m_stcDisplay.SetWindowTextW(csDisplayTxt);
//
	return 0x00;
}
