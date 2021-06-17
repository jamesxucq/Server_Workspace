// AdvancedDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "OptionsDlg.h"
#include "MoveUserspaceDlg.h"
#include "DowndDiresDlg.h"
#include "SelectiveDlg.h"
#include "AdvancedDlg.h"


// CAdvancedDlg 对话框

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


// CAdvancedDlg 消息处理程序

BOOL CAdvancedDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CAdvancedDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
#define APPLY_BTN_ENABLE ((COptionsDlg*)m_pParentWnd)->EnableApplyButton

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CAdvancedDlg::OnBnClickedMoveLocationBtn()
{
	GET_CLIENT_POINT
		CString csSaveDirectory;
	//
	csSaveDirectory = DialogCode::BrowseDirectories(this->m_hWnd, _T("选择目录"));
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
				// TODO: 在此放置处理何时用
				//  “确定”来关闭对话框的代码
				_tcscpy_s(pClientConfig->szLocation, szNewLocation);
				NLocalBzl::SaveOptionConfig();
				m_edtLocation.SetWindowText(szNewLocation);
			} else if (nResponse == IDCANCEL) {
				// TODO: 在此放置处理何时用
				//  “取消”来关闭对话框的代码
			}		
		} else {
			// 容错
			MessageBox(_T("不能选择挂载盘为移动目的."), _T("Tip"), MB_OK|MB_ICONWARNING);
		} 
		//
	}
}

void CAdvancedDlg::OnEnChangeLocationEdt()
{
    // TODO: 在此添加控件通知处理程序代码
    // APPLY_BTN_ENABLE(TRUE);
}

void CAdvancedDlg::OnBnClickedDireSelectionBtn()
{
    // TODO: 在此添加控件通知处理程序代码
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
        // TODO: 在此放置处理何时用
        //  “确定”来关闭对话框的代码
        switch(NSelectiveBzl::GetDowndThreadStatus()) {
        case DOWND_THREAD_DISCONNECT:
            MessageBox(_T("没有网络连接,请检查后重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
            break;
        case DOWND_THREAD_FAULT:
            MessageBox(_T("服务器出错,请检查后重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
            break;
        case DOWND_THREAD_SERVBUSY:
            MessageBox(_T("服务器忙,请等待后重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
            break;
        case DOWND_THREAD_LOCALBUSY:
            MessageBox(_T("客户端同步忙,请等待后重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
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
        // TODO: 在此放置处理何时用
        //  “取消”来关闭对话框的代码
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
    // TODO: 在此添加控件通知处理程序代码
    m_btnForceCtrl.EnableWindow(FALSE);
	MessageBox(_T("客户端重启后生效."), _T("Tip"), MB_OK|MB_ICONINFORMATION);
	//
    GET_CLIENT_POINT
    _tcscpy_s(pClientConfig->szForceCtrl, _T("slowly"));
    NLocalBzl::SaveOptionConfig();
}
