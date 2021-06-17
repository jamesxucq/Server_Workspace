// DiskSettiongDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "logiwiza.h"
#include "clientcom/lwizardcom.h"
#include "LoginWizdBzl.h"
#include "DiskSettingDlg.h"


// CDiskSettingDlg 对话框

IMPLEMENT_DYNAMIC(CDiskSettingDlg, CDialog)

CDiskSettingDlg::CDiskSettingDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDiskSettingDlg::IDD, pParent)
{
}

CDiskSettingDlg::~CDiskSettingDlg() {
}

void CDiskSettingDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOCATION_EDT, m_edtLocation);
	DDX_Control(pDX, IDC_CHKSPACE_PROMPT_STC, m_stcChkspacePrompt);
	DDX_Control(pDX, IDC_CHKSPACE_TEXT_STC, m_stcChkspaceText);
	DDX_Control(pDX, IDC_ATTENTION_STC, m_stcAttentionText);
}


BEGIN_MESSAGE_MAP(CDiskSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_PATH_SELECT_BTN, &CDiskSettingDlg::OnBnClickedPathSelectBtn)
	ON_EN_CHANGE(IDC_LOCATION_EDT, &CDiskSettingDlg::OnEnChangeLocationEdt)
END_MESSAGE_MAP()


BOOL CDiskSettingDlg::PreTranslateMessage(MSG* pMsg) {
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CDiskSettingDlg::OnBnClickedPathSelectBtn() {
	// TODO: 在此添加控件通知处理程序代码
	CString csSelectDirectory;
	TCHAR szSaveDirectory[MAX_PATH];
//
	csSelectDirectory = DialogCode::BrowseDirectories(this->m_hWnd, _T("选择目录"));
	if(_T("") != csSelectDirectory){
		// modify by james 20101202
		struti::appe_path_ext(szSaveDirectory, (TCHAR*)(LPCTSTR)csSelectDirectory, _T("\\SDUserSpace"));
		m_edtLocation.SetWindowText(szSaveDirectory);
	}
}

BOOL CDiskSettingDlg::OnInitDialog() {
	CDialog::OnInitDialog();
//
	// TODO: 在此添加额外的初始化
#define GET_CLIENT_POINT \
	struct ClientConfig *pClientConfig; \
	pClientConfig = NConfigBzl::GetClientConfig();
//
	GET_CLIENT_POINT
	_tcscpy_s(pClientConfig->szDiskLabel, _T("同步网盘"));
	if(!_tcscmp(pClientConfig->szLocation, _T(""))) _tcscpy_s(pClientConfig->szLocation, NFileUtility::ModuleBaseName());
//
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

DWORD CDiskSettingDlg::SetDiskSettingValue() {
	GET_CLIENT_POINT
	m_edtLocation.SetWindowText(pClientConfig->szLocation);
//
	// if(!_tcscmp(pClientConfig->szDriveLetter, _T(""))) { // disable by james 20120320
	TCHAR szDriveLetter[8] = _T(" :");
	DWORD dwLogicDrives = GetLogicalDrives();
	for (int inde=0; inde<26; inde++) {
		if(!(0x00000001 & (dwLogicDrives>>inde))) {
			szDriveLetter[0] = 'A' + inde;
			if (*szDriveLetter!=_T('A') && *szDriveLetter!=_T('B')) {
				_tcscpy_s(pClientConfig->szDriveLetter, szDriveLetter);
				break;
			}
		}
	}
	//
	TCHAR szDisplayTxt[MID_TEXT_LEN];
	_stprintf_s(szDisplayTxt, 
		_T("创建一个同步盘的本地空间.目录所在磁盘为NTFS文件格式,且不能小于%dG的空闲空间."),
		pClientConfig->dwPoolLength);
	m_stcAttentionText.SetWindowText(szDisplayTxt);
//
	UpdateData(FALSE);
	return 0x00;
}

DWORD CDiskSettingDlg::GetDiskSettingValue() {
	UpdateData(TRUE);
//
	GET_CLIENT_POINT
	CString csWindowText;
	m_edtLocation.GetWindowText(csWindowText); 
	_tcscpy_s(pClientConfig->szLocation, csWindowText);
//
	return 0x00;
}

void CDiskSettingDlg::OnEnChangeLocationEdt() {
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
//
	CString csWindowText;
	m_edtLocation.GetWindowText(csWindowText);

	if(!LoginWizdBzl::ValidateUserspaceExistEx((TCHAR*)(LPCTSTR)csWindowText)) {
		m_stcChkspacePrompt.EnableWindow(TRUE);
		m_stcChkspaceText.EnableWindow(TRUE);
	} else {		
		m_stcChkspacePrompt.EnableWindow(FALSE);
		m_stcChkspaceText.EnableWindow(FALSE);	
	}
}
