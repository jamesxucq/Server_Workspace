// AdvancedDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "OptionsDlg.h"
#include "MoveUserspaceDlg.h"
#include "DowndDiresDlg.h"
#include "SelectiveDlg.h"
#include "AdvancedDlg.h"


// CAdvancedDlg �Ի���

IMPLEMENT_DYNAMIC(CAdvancedDlg, CDialog)

CAdvancedDlg::CAdvancedDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CAdvancedDlg::IDD, pParent)
{

}

CAdvancedDlg::~CAdvancedDlg()
{
}

void CAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOCATION_EDT, m_edtLocation);
    DDX_Control(pDX, IDC_DIRE_SELECTION_BTN, m_btnDireSelection);
    DDX_Control(pDX, IDC_SELECTIVE_STC, m_stcSelective);
    DDX_Control(pDX, IDC_FORCECTRL_BTN, m_btnForceCtrl);
}


BEGIN_MESSAGE_MAP(CAdvancedDlg, CDialog)
    ON_BN_CLICKED(IDC_MOVE_LOCATION_BTN, &CAdvancedDlg::OnBnClickedMoveLocationBtn)
    ON_EN_CHANGE(IDC_LOCATION_EDT, &CAdvancedDlg::OnEnChangeLocationEdt)
    ON_BN_CLICKED(IDC_DIRE_SELECTION_BTN, &CAdvancedDlg::OnBnClickedDireSelectionBtn)
    ON_BN_CLICKED(IDC_FORCECTRL_BTN, &CAdvancedDlg::OnBnClickedForcectrlBtn)
END_MESSAGE_MAP()


// CAdvancedDlg ��Ϣ�������

BOOL CAdvancedDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAdvancedDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
#define APPLY_BTN_ENABLE ((COptionsDlg*)m_pParentWnd)->EnableApplyButton

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CAdvancedDlg::OnBnClickedMoveLocationBtn()
{
	GET_CLIENT_POINT
		CString csSaveDirectory;
	//
	csSaveDirectory = DialogCode::BrowseDirectories(this->m_hWnd, _T("ѡ��Ŀ¼"));
	if(_T("") != csSaveDirectory) {
		TCHAR szNewLocation[MAX_PATH];
		struti::appe_path_ext(szNewLocation, (TCHAR*)(LPCTSTR)csSaveDirectory, _T("\\SDUserSpace"));
		//
		if(pClientConfig->szDriveLetter[0] != szNewLocation[0]) {
			CMoveUserspacelg dlg;
			dlg.SetMoveUserspaceArgu (pClientConfig->szLocation,
				szNewLocation,
				pClientConfig->szUserPool,
				pClientConfig->dwPoolLength,
				pClientConfig->szDriveLetter);
			INT_PTR nResponse = dlg.DoModal();
			if (nResponse == IDOK) {
				// TODO: �ڴ˷��ô����ʱ��
				//  ��ȷ�������رնԻ���Ĵ���
				_tcscpy_s(pClientConfig->szLocation, szNewLocation);
				NLocalBzl::SaveOptionConfig();
				m_edtLocation.SetWindowText(szNewLocation);
			} else if (nResponse == IDCANCEL) {
				// TODO: �ڴ˷��ô����ʱ��
				//  ��ȡ�������رնԻ���Ĵ���
			}		
		} else {
			// �ݴ�
			MessageBox(_T("����ѡ�������Ϊ�ƶ�Ŀ��."), _T("Tip"), MB_OK|MB_ICONWARNING);
		} 
		//
	}
}

void CAdvancedDlg::OnEnChangeLocationEdt()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    // APPLY_BTN_ENABLE(TRUE);
}

void CAdvancedDlg::OnBnClickedDireSelectionBtn()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_btnDireSelection.EnableWindow(FALSE);
    DWORD dwProcessStatus = NExecTransBzl::GetTransmitProcessStatus();
    NLocalBzl::StopTransmitProcess();
    // for test
    /*
    CSelectiveDlg dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    }
    else if (nResponse == IDCANCEL) {
    }
    */
    CDowndDiresDlg dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
        // TODO: �ڴ˷��ô����ʱ��
        //  ��ȷ�������رնԻ���Ĵ���
        switch(NSelectiveBzl::GetDowndThreadStatus()) {
        case DOWND_THREAD_DISCONNECT:
            MessageBox(_T("û����������,���������."), _T("Tip"), MB_OK|MB_ICONWARNING);
            break;
        case DOWND_THREAD_FAULT:
            MessageBox(_T("����������,���������."), _T("Tip"), MB_OK|MB_ICONWARNING);
            break;
        case DOWND_THREAD_SERVBUSY:
            MessageBox(_T("������æ,��ȴ�������."), _T("Tip"), MB_OK|MB_ICONWARNING);
            break;
        case DOWND_THREAD_LOCALBUSY:
            MessageBox(_T("�ͻ���ͬ��æ,��ȴ�������."), _T("Tip"), MB_OK|MB_ICONWARNING);
            break;
        case DOWND_THREAD_SUCCESS:
            CSelectiveDlg dlg;
            INT_PTR nResponse = dlg.DoModal();
            if (nResponse == IDOK) {
            } else if (nResponse == IDCANCEL) {
            }
            break;
        }
    } else if (nResponse == IDCANCEL) {
        // TODO: �ڴ˷��ô����ʱ��
        //  ��ȡ�������رնԻ���Ĵ���
    }
//
    NLocalBzl::ResumeTransmitProcess(dwProcessStatus);
    m_btnDireSelection.EnableWindow(TRUE);
}

DWORD CAdvancedDlg::SetAdvancedValue()
{
    GET_CLIENT_POINT
//
    m_edtLocation.SetWindowText(pClientConfig->szLocation);// SetWindowText();
    m_edtLocation.EnableWindow(FALSE);
//
    UpdateData(FALSE);
    return 0x00;
}

DWORD CAdvancedDlg::GetAdvancedValue()
{
    UpdateData(TRUE);
    return 0x00;
}


void CAdvancedDlg::OnBnClickedForcectrlBtn()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_btnForceCtrl.EnableWindow(FALSE);
	MessageBox(_T("�ͻ�����������Ч."), _T("Tip"), MB_OK|MB_ICONINFORMATION);
	//
    GET_CLIENT_POINT
    _tcscpy_s(pClientConfig->szForceCtrl, _T("slowly"));
    NLocalBzl::SaveOptionConfig();
}
