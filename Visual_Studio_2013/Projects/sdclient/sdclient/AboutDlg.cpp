// AboutDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sdclient.h"
#include "AboutDlg.h"

#include "OptionsDlg.h"

// CAboutDlg �Ի���

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APP_UPDATE_SYSLINK, m_linkAppUpdate);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_APP_UPDATE_SYSLINK, &CAboutDlg::OnNMClickAppUpdateSyslink)
END_MESSAGE_MAP()


// CAboutDlg ��Ϣ�������

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
#define GET_DISPLAY_POINT \
	struct DisplayLink *pDisplayLink; \
	pDisplayLink = NConfigBzl::GetDisplayLink();
#define APPLY_BTN_ENABLE ((COptionsDlg*)m_pParentWnd)->EnableApplyButton

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

DWORD CAboutDlg::SetAboutValue()
{
	GET_DISPLAY_POINT

	TCHAR szLinkTxt[URI_LENGTH];
	m_linkAppUpdate.SetWindowText(struti::get_url(szLinkTxt, COMPANY_HOME_LINK, pDisplayLink->szCompanyLink));

	UpdateData(FALSE);
	return 0x00;
}

void CAboutDlg::OnNMClickAppUpdateSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PNMLINK pNMLink = (PNMLINK) pNMHDR;
	ShellExecute(NULL, _T("open"), pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);  //��Ҫִ����� 

	*pResult = 0;
}
