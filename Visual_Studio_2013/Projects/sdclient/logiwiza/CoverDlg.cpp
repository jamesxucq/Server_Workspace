// StartDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "logiwiza.h"
#include "CoverDlg.h"


// CCoverDlg �Ի���

IMPLEMENT_DYNAMIC(CCoverDlg, CDialog)

CCoverDlg::CCoverDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCoverDlg::IDD, pParent)
{

}

CCoverDlg::~CCoverDlg()
{
}

void CCoverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCoverDlg, CDialog)
	//	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCoverDlg ��Ϣ�������

BOOL CCoverDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// void CCoverDlg::OnClose()
// {
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//
//	// CDialog::OnClose();
//	return ;
// }

// void CCoverDlg::OnOK()
// {
//	// TODO: �ڴ����ר�ô����/����û���
//
//	CDialog::OnOK();
// }
