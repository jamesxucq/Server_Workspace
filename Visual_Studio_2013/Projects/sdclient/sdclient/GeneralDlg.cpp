// GeneralDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "MountVolumeBzl.h"

#include "OptionsDlg.h"
#include "GeneralDlg.h"

// CGeneralDlg 对话框

IMPLEMENT_DYNAMIC(CGeneralDlg, CDialog)

CGeneralDlg::CGeneralDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGeneralDlg::IDD, pParent)
{
	m_iMountInde = 0x00;
}

CGeneralDlg::~CGeneralDlg()
{
}

void CGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRIVE_CBO, m_cboDriveRoot);
	DDX_Control(pDX, IDC_SYSMSG_CHK, m_chkSystemMessage);
	DDX_Control(pDX, IDC_AUTORUN_CHK, m_chkAutoRun);
	DDX_Control(pDX, IDC_REMOUNT_BTN, m_btnRemount);
	DDX_Control(pDX, IDC_AUTOUPDATE_CHK, m_chkAutoUpdate);
}


BEGIN_MESSAGE_MAP(CGeneralDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_DRIVE_CBO, &CGeneralDlg::OnCbnSelchangeDriveCbo)
	ON_BN_CLICKED(IDC_SYSMSG_CHK, &CGeneralDlg::OnBnClickedSysmsgChk)
	ON_BN_CLICKED(IDC_AUTORUN_CHK, &CGeneralDlg::OnBnClickedAutorunChk)
	ON_BN_CLICKED(IDC_REMOUNT_BTN, &CGeneralDlg::OnBnClickedRemountBtn)
END_MESSAGE_MAP()


// CGeneralDlg 消息处理程序

BOOL CGeneralDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CGeneralDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
#define APPLY_BTN_ENABLE ((COptionsDlg*)m_pParentWnd)->EnableApplyButton
	m_iMountInde = 0x00;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


DWORD CGeneralDlg::SetGeneralValue()
{
	TCHAR szDriveLetter[8] = _T(" :");
	DWORD dwLogicDrives = GetLogicalDrives();
	GET_CLIENT_POINT
//
	m_cboDriveRoot.ResetContent();
	for (int inde=0; inde<26; inde++) {
		szDriveLetter[0] = 'A' + inde;
		if(!(0x00000001 & (dwLogicDrives>>inde))) {
			if (*szDriveLetter!=_T('A') && *szDriveLetter!=_T('B'))
				m_cboDriveRoot.AddString(szDriveLetter);
		} else if (!_tcscmp(pClientConfig->szDriveLetter, szDriveLetter))
			m_iMountInde = m_cboDriveRoot.AddString(szDriveLetter);
	}				
	m_cboDriveRoot.SetCurSel(m_iMountInde);
	m_btnRemount.EnableWindow(FALSE);
//
	if (!_tcscmp(pClientConfig->szShowBalloonTip, _T("true"))) 
		m_chkSystemMessage.SetCheck(TRUE);
	else m_chkSystemMessage.SetCheck(FALSE);
//
	if (!_tcscmp(pClientConfig->szAutoRun, _T("true"))) 
		m_chkAutoRun.SetCheck(TRUE);
	else m_chkAutoRun.SetCheck(FALSE);
//
	if (!_tcscmp(pClientConfig->szAutoUpdate, _T("true"))) 
		m_chkAutoUpdate.SetCheck(TRUE);
	else m_chkAutoUpdate.SetCheck(FALSE);
//
	UpdateData(FALSE);
	return 0x00;
}

DWORD CGeneralDlg::GetGeneralValue()
{
	UpdateData(TRUE);
	GET_CLIENT_POINT
//
		if (BST_CHECKED == m_chkSystemMessage.GetCheck()) 
			_tcscpy_s(pClientConfig->szShowBalloonTip, _T("true"));
		else _tcscpy_s(pClientConfig->szShowBalloonTip, _T("false"));
//
		if(!SystemCall::CheckApplicaAuto(APPLICATION_NAME)) {
			if(BST_UNCHECKED == m_chkAutoRun.GetCheck()) {
				_tcscpy_s(pClientConfig->szAutoRun, _T("false"));
				if(SystemCall::DeliApplicaAuto(APPLICATION_NAME)) 
					return ((DWORD)-1);
			}
		} else {
			if(BST_CHECKED == m_chkAutoRun.GetCheck()) {
				_tcscpy_s(pClientConfig->szAutoRun, _T("true"));	
				if(SystemCall::AddiApplicaAutoEx(APPLICATION_NAME)) 
					return ((DWORD)-1);
			}
		}
//
		if (BST_CHECKED == m_chkAutoUpdate.GetCheck()) 
			_tcscpy_s(pClientConfig->szAutoUpdate, _T("true"));
		else _tcscpy_s(pClientConfig->szAutoUpdate, _T("query"));
//
		return 0x00;
}

void CGeneralDlg::OnCbnSelchangeDriveCbo()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	GET_CLIENT_POINT
//
	CString csWindowText;
	m_cboDriveRoot.GetWindowText(csWindowText);
//
	if(_tcscmp(pClientConfig->szDriveLetter, csWindowText)) 
		m_btnRemount.EnableWindow(TRUE);
	else m_btnRemount.EnableWindow(FALSE);
}

void CGeneralDlg::OnBnClickedSysmsgChk()
{
	// TODO: 在此添加控件通知处理程序代码
	APPLY_BTN_ENABLE(TRUE);
}

void CGeneralDlg::OnBnClickedAutorunChk()
{
	// TODO: 在此添加控件通知处理程序代码
	APPLY_BTN_ENABLE(TRUE);
}


void CGeneralDlg::OnBnClickedRemountBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	GET_CLIENT_POINT
//
	CString csWindowText;
	m_cboDriveRoot.GetWindowText(csWindowText);
//
	m_btnRemount.EnableWindow(FALSE);
	DWORD dwProcessStatus = NExecTransBzl::GetTransmitProcessStatus();
	NLocalBzl::StopTransmitProcess();
//
	DWORD dwMountValue = NMountVolumeBzl::RemountVolume(pClientConfig->szDriveLetter, (TCHAR*)(LPCTSTR)csWindowText);
	if(dwMountValue) {
		switch(dwMountValue)
		{
		case ERR_UNMOUNT:
			MessageBox(_T("有文件打开,请关闭后重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
			break;
		case ERR_EXIST_MOUNT:
			MessageBox(_T("挂载出错,请退出程序重试."), _T("Tip"), MB_OK|MB_ICONERROR);
			break;
		case ERR_NEW_MOUNT:
			MessageBox(_T("重新挂载出错,请退出程序重试."), _T("Tip"), MB_OK|MB_ICONERROR);
			break;
		case ERR_FAULT:
			MessageBox(_T("程序内部出错,请退出程序重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
			break;
		default:
			break;
		}
		m_cboDriveRoot.SetCurSel(m_iMountInde);
		m_btnRemount.EnableWindow(TRUE);
	} else { // ERR_SUCCESS
		_tcscpy_s(pClientConfig->szDriveLetter, csWindowText);
		NLocalBzl::SaveOptionConfig();
		m_iMountInde = m_cboDriveRoot.GetCurSel();
	}
// _LOG_DEBUG(_T("Mount Value:%d"), dwMountValue);
	//
	NLocalBzl::ResumeTransmitProcess(dwProcessStatus);
}

