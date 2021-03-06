// ProxiesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "OptionsDlg.h"
#include "ProxiesDlg.h"


// CProxiesDlg 对话框

IMPLEMENT_DYNAMIC(CProxiesDlg, CDialog)

CProxiesDlg::CProxiesDlg(CWnd* pParent /*=NULL*/)
    :CDialog(CProxiesDlg::IDD, pParent)
    ,m_rdoProxy(0)
    ,m_iFlagRadio(0)
{

}

CProxiesDlg::~CProxiesDlg()
{
}

void CProxiesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TYPE_CBO, m_cboType);
    DDX_Control(pDX, IDC_SERV_EDT, m_edtAddress);
    DDX_Control(pDX, IDC_PORT_EDT, m_edtPort);
    DDX_Control(pDX, IDC_PROXY_UNAME_EDT, m_edtUserName);
    DDX_Control(pDX, IDC_PROXY_PASSWD_EDT, m_edtPassword);
    DDX_Control(pDX, IDC_AUTH_CHK, m_chkProxyAuth);
    DDX_Radio(pDX, IDC_NOPROXY_RDO, m_rdoProxy);
    DDX_Control(pDX, IDC_TYPE_STC, m_stcType);
    DDX_Control(pDX, IDC_SERV_STC, m_stcAddress);
    DDX_Control(pDX, IDC_PORT_STC, m_stcPort);
    DDX_Control(pDX, IDC_UNAME_STC, m_stcUserName);
    DDX_Control(pDX, IDC_PASSWD_STC, m_stcPassword);
}


BEGIN_MESSAGE_MAP(CProxiesDlg, CDialog)
    ON_BN_CLICKED(IDC_NOPROXY_RDO, &CProxiesDlg::OnBnClickedProxyRdo)
    ON_BN_CLICKED(IDC_AUTOPROXY_RDO, &CProxiesDlg::OnBnClickedProxyRdo)
    ON_BN_CLICKED(IDC_MANUALPROXY_RDO, &CProxiesDlg::OnBnClickedProxyRdo)
    ON_BN_CLICKED(IDC_AUTH_CHK, &CProxiesDlg::OnBnClickedAuthChk)
    ON_CBN_SELCHANGE(IDC_TYPE_CBO, &CProxiesDlg::OnEnChangeEx)
    ON_EN_CHANGE(IDC_SERV_EDT, &CProxiesDlg::OnEnChangeEx)
    ON_EN_CHANGE(IDC_PORT_EDT, &CProxiesDlg::OnEnChangeEx)
    ON_EN_CHANGE(IDC_PROXY_UNAME_EDT, &CProxiesDlg::OnEnChangeEx)
    ON_EN_CHANGE(IDC_PROXY_PASSWD_EDT, &CProxiesDlg::OnEnChangeEx)
END_MESSAGE_MAP()


// CProxiesDlg 消息处理程序

BOOL CProxiesDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CProxiesDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
#define GET_NETWORK_POINT \
	struct NetworkConfig *pNetworkConf; \
	pNetworkConf = NConfigBzl::GetNetworkConfig();
#define APPLY_BTN_ENABLE ((COptionsDlg*)m_pParentWnd)->EnableApplyButton

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

DWORD CProxiesDlg::SetProxieszValue()
{
    GET_NETWORK_POINT
    CString csWindowText;

    m_cboType.EnableWindow(FALSE);
    m_edtAddress.EnableWindow(FALSE);
    m_edtPort.EnableWindow(FALSE);
    m_chkProxyAuth.EnableWindow(FALSE);
    m_edtPassword.EnableWindow(FALSE);
    m_edtUserName.EnableWindow(FALSE);
    //
    m_stcType.EnableWindow(FALSE);
    m_stcAddress.EnableWindow(FALSE);
    m_stcPort.EnableWindow(FALSE);
    m_stcUserName.EnableWindow(FALSE);
    m_stcPassword.EnableWindow(FALSE);

    //
    m_cboType.SetWindowText(pNetworkConf->szProxyType);
    m_edtAddress.SetWindowText(pNetworkConf->tAddress.sin_addr);
    csWindowText.Format(_T("%d"), pNetworkConf->tAddress.sin_port);
    m_edtPort.SetWindowText(csWindowText);
    m_edtPassword.SetWindowText(pNetworkConf->szPassword);
    m_edtUserName.SetWindowText(pNetworkConf->szUserName);

    //
    if (!_tcscmp(pNetworkConf->szProxyEnable, _T("true"))) {
        m_rdoProxy = 2;

        m_cboType.EnableWindow(TRUE);
        m_edtAddress.EnableWindow(TRUE);
        m_edtPort.EnableWindow(TRUE);
        m_chkProxyAuth.EnableWindow(TRUE);
        m_chkProxyAuth.SetCheck(FALSE);

        if (!_tcscmp(pNetworkConf->szProxyAuth, _T("true"))) {
            m_chkProxyAuth.SetCheck(TRUE);
            m_edtPassword.EnableWindow(TRUE);
            m_edtUserName.EnableWindow(TRUE);
        }
    } else if (!_tcscmp(pNetworkConf->szProxyEnable, _T("auto"))) m_rdoProxy = 1;
    else if (!_tcscmp(pNetworkConf->szProxyEnable, _T("false"))) m_rdoProxy = 0;

    //
    m_iFlagRadio = m_rdoProxy;
    UpdateData(FALSE);

    return 0x00;
}

DWORD CProxiesDlg::GetProxieszValue()
{
    UpdateData(TRUE);

    GET_NETWORK_POINT
    CString csWindowText;
    if (0 == m_rdoProxy) _tcscpy_s(pNetworkConf->szProxyEnable, _T("false"));
    else if (1 == m_rdoProxy) _tcscpy_s(pNetworkConf->szProxyEnable, _T("auto"));
    else {
        _tcscpy_s(pNetworkConf->szProxyEnable, _T("true"));
        m_cboType.GetWindowText(csWindowText);
        _tcscpy_s(pNetworkConf->szProxyType, csWindowText);
        m_edtAddress.GetWindowText(csWindowText);
        _tcscpy_s(pNetworkConf->tAddress.sin_addr, csWindowText);
        m_edtPort.GetWindowText(csWindowText);
        pNetworkConf->tAddress.sin_port = _tstoi(csWindowText);
        if (BST_CHECKED == m_chkProxyAuth.GetCheck()) {
            _tcscpy_s(pNetworkConf->szProxyAuth, _T("true"));
            m_edtUserName.GetWindowText(csWindowText);
            _tcscpy_s(pNetworkConf->szUserName, csWindowText);
            m_edtPassword.GetWindowText(csWindowText);
            _tcscpy_s(pNetworkConf->szPassword, csWindowText);
            NEncoding::RandomSalt(pNetworkConf->szSalt);
        } else _tcscpy_s(pNetworkConf->szProxyAuth, _T("false"));
    }

    return 0x00;
}

void CProxiesDlg::OnBnClickedProxyRdo()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);

    if (m_iFlagRadio == m_rdoProxy) 
		return;
    else m_iFlagRadio = m_rdoProxy;

    if(0==m_rdoProxy || 1==m_rdoProxy) {
        m_cboType.EnableWindow(FALSE);
        m_edtAddress.EnableWindow(FALSE);
        m_edtPort.EnableWindow(FALSE);
        m_chkProxyAuth.EnableWindow(FALSE);
        m_edtUserName.EnableWindow(FALSE);
        m_edtPassword.EnableWindow(FALSE);
        //
        m_stcType.EnableWindow(FALSE);
        m_stcAddress.EnableWindow(FALSE);
        m_stcPort.EnableWindow(FALSE);
        m_stcUserName.EnableWindow(FALSE);
        m_stcPassword.EnableWindow(FALSE);
    } else { //2==m_rdoProxy
        m_cboType.EnableWindow(TRUE);
        m_edtAddress.EnableWindow(TRUE);
        m_edtPort.EnableWindow(TRUE);
        m_stcType.EnableWindow(TRUE);
        m_stcAddress.EnableWindow(TRUE);
        m_stcPort.EnableWindow(TRUE);
        m_chkProxyAuth.EnableWindow(TRUE);
        if (BST_CHECKED == m_chkProxyAuth.GetCheck()) {
            m_edtUserName.EnableWindow(TRUE);
            m_edtPassword.EnableWindow(TRUE);
            m_stcUserName.EnableWindow(TRUE);
            m_stcPassword.EnableWindow(TRUE);
        }
    }

    APPLY_BTN_ENABLE(TRUE);
}

void CProxiesDlg::OnBnClickedAuthChk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);

    if (BST_CHECKED == m_chkProxyAuth.GetCheck()) {
        m_edtUserName.EnableWindow(TRUE);
        m_edtPassword.EnableWindow(TRUE);
        m_stcUserName.EnableWindow(TRUE);
        m_stcPassword.EnableWindow(TRUE);
    } else {
        m_edtUserName.EnableWindow(FALSE);
        m_edtPassword.EnableWindow(FALSE);
        m_stcUserName.EnableWindow(FALSE);
        m_stcPassword.EnableWindow(FALSE);
    }

    APPLY_BTN_ENABLE(TRUE);
}

void CProxiesDlg::OnEnChangeEx()
{
    // TODO:  在此添加控件通知处理程序代码
    APPLY_BTN_ENABLE(TRUE);
}

