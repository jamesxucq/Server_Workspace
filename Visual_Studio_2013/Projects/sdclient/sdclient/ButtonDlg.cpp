#include "StdAfx.h"
#include "OptionsDlg.h"
#include "ButtonDlg.h"


// CButtonDlg 对话框

IMPLEMENT_DYNAMIC(CButtonDlg, CDialog)

CButtonDlg::CButtonDlg(CWnd* pParent /*=NULL*/)
: CDialog(CButtonDlg::IDD, pParent)
{
	DISPLAY_FLAGS_NORMAL(m_dwDisplayBtnFlags);
}

CButtonDlg::~CButtonDlg()
{
}

void CButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GENERAL_BTN, m_btnGeneral);
	DDX_Control(pDX, IDC_ACCOUNT_BTN, m_btnAccount);
	DDX_Control(pDX, IDC_BANDWIDTH_BTN, m_btnBandwidth);
	DDX_Control(pDX, IDC_PROXIES_BTN, m_btnProxies);
	DDX_Control(pDX, IDC_SECURITY_BTN, m_btnSecurity);
	DDX_Control(pDX, IDC_ADVANCED_BTN, m_btnAdvanced);
	DDX_Control(pDX, IDC_ABOUT_BTN, m_btnAbout);
}

BEGIN_MESSAGE_MAP(CButtonDlg, CDialog)
	ON_BN_CLICKED(IDC_GENERAL_BTN, &CButtonDlg::OnBnClickedGeneralBtn)
	ON_BN_CLICKED(IDC_ACCOUNT_BTN, &CButtonDlg::OnBnClickedAccountBtn)
	ON_BN_CLICKED(IDC_BANDWIDTH_BTN, &CButtonDlg::OnBnClickedBandwidthBtn)
	ON_BN_CLICKED(IDC_PROXIES_BTN, &CButtonDlg::OnBnClickedProxiesBtn)
	ON_BN_CLICKED(IDC_SECURITY_BTN, &CButtonDlg::OnBnClickedSecurityBtn)
	ON_BN_CLICKED(IDC_ADVANCED_BTN, &CButtonDlg::OnBnClickedAdvancedBtn)
	ON_BN_CLICKED(IDC_ABOUT_BTN, &CButtonDlg::OnBnClickedAboutBtn)
END_MESSAGE_MAP()


// CGeneralDlg 消息处理程序

BOOL CButtonDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN) {
		if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CButtonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
#define REFRESH_WINDOW ((COptionsDlg*)m_pParentWnd)->RefreshWindow

	// init options button
	m_btnGeneral.SetImgs(IDB_NGENERAL, IDB_FGENERAL, IDB_SGENERAL);
	m_btnAccount.SetImgs(IDB_NACCOUNT, IDB_FACCOUNT, IDB_SACCOUNT);
	m_btnBandwidth.SetImgs(IDB_NBANDWIDTH, IDB_FBANDWIDTH, IDB_SBANDWIDTH);
	m_btnProxies.SetImgs(IDB_NPROXIES, IDB_FPROXIES, IDB_SPROXIES);
	m_btnSecurity.SetImgs(IDB_NSECURITY, IDB_FSECURITY, IDB_SSECURITY);
	m_btnAdvanced.SetImgs(IDB_NADVANCED, IDB_FADVANCED, IDB_SADVANCED);
	m_btnAbout.SetImgs(IDB_NABOUT, IDB_FABOUT, IDB_SABOUT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CButtonDlg::OnBnClickedGeneralBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	REFRESH_WINDOW(ENUM_GENERAL);
}

void CButtonDlg::OnBnClickedAccountBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	REFRESH_WINDOW(ENUM_ACCOUNT);
}

void CButtonDlg::OnBnClickedBandwidthBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	REFRESH_WINDOW(ENUM_BANDWIDTH);
}

void CButtonDlg::OnBnClickedProxiesBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	REFRESH_WINDOW(ENUM_PROXIES);
}

void CButtonDlg::OnBnClickedSecurityBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	REFRESH_WINDOW(ENUM_SECURITY);
}

void CButtonDlg::OnBnClickedAdvancedBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	REFRESH_WINDOW(ENUM_ADVANCED);
}

void CButtonDlg::OnBnClickedAboutBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	REFRESH_WINDOW(ENUM_ABOUT);
}

void CButtonDlg::DrawBtnBitmap(DWORD dwFocusBtnNum)
{
	// TODO: 在此添加控件通知处理程序代码
	if(GENERAL_CHECK_FOCUSE(m_dwDisplayBtnFlags)) {
		m_btnGeneral.DrawImage(NORMAL);
		GENERAL_SHOW_NORMAL(m_dwDisplayBtnFlags);
	} else if(ACCOUNT_CHECK_FOCUSE(m_dwDisplayBtnFlags)) {
		m_btnAccount.DrawImage(NORMAL);
		ACCOUNT_SHOW_NORMAL(m_dwDisplayBtnFlags);
	} else if(BANDWIDTH_CHECK_FOCUSE(m_dwDisplayBtnFlags)) {
		m_btnBandwidth.DrawImage(NORMAL);
		BANDWIDTH_SHOW_NORMAL(m_dwDisplayBtnFlags);
	} else if(PROXIES_CHECK_FOCUSE(m_dwDisplayBtnFlags)) {
		m_btnProxies.DrawImage(NORMAL);
		PROXIES_SHOW_NORMAL(m_dwDisplayBtnFlags);
	} else if(SECURITY_CHECK_FOCUSE(m_dwDisplayBtnFlags)) {
		m_btnSecurity.DrawImage(NORMAL);
		SECURITY_SHOW_NORMAL(m_dwDisplayBtnFlags);
	} else if(ADVANCED_CHECK_FOCUSE(m_dwDisplayBtnFlags)) {
		m_btnAdvanced.DrawImage(NORMAL);
		ADVANCED_SHOW_NORMAL(m_dwDisplayBtnFlags);
	} else if(ABOUT_CHECK_FOCUSE(m_dwDisplayBtnFlags)) {
		m_btnAbout.DrawImage(NORMAL);
		ABOUT_SHOW_NORMAL(m_dwDisplayBtnFlags);
	}
//
	switch (dwFocusBtnNum) {
	case ENUM_GENERAL:
		m_btnGeneral.DrawImage(SELECT);
		GENERAL_SHOW_FOCUSE(m_dwDisplayBtnFlags);
		break;
	case ENUM_ACCOUNT:
		m_btnAccount.DrawImage(SELECT);
		ACCOUNT_SHOW_FOCUSE(m_dwDisplayBtnFlags);
		break;
	case ENUM_BANDWIDTH:
		m_btnBandwidth.DrawImage(SELECT);
		BANDWIDTH_SHOW_FOCUSE(m_dwDisplayBtnFlags);
		break;
	case ENUM_PROXIES:
		m_btnProxies.DrawImage(SELECT);
		PROXIES_SHOW_FOCUSE(m_dwDisplayBtnFlags);
		break;
	case ENUM_SECURITY:
		m_btnSecurity.DrawImage(SELECT);
		SECURITY_SHOW_FOCUSE(m_dwDisplayBtnFlags);
		break;
	case ENUM_ADVANCED:
		m_btnAdvanced.DrawImage(SELECT);
		ADVANCED_SHOW_FOCUSE(m_dwDisplayBtnFlags);
		break;
	case ENUM_ABOUT:
		m_btnAbout.DrawImage(SELECT);
		ABOUT_SHOW_FOCUSE(m_dwDisplayBtnFlags);
		break;
	default:
		break;
	}
//
	Invalidate(FALSE);
}

