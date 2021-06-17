// BandwidthDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "sdclient.h"
#include "clientcom/clientcom.h"
#include "OptionsDlg.h"
#include "BandwidthDlg.h"


// CBandwidthDlg 对话框

IMPLEMENT_DYNAMIC(CBandwidthDlg, CDialog)

CBandwidthDlg::CBandwidthDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CBandwidthDlg::IDD, pParent)
    ,m_rdoDownd(0)
    ,m_rdoUpload(0)
    ,m_iFlagDownd(0)
    ,m_iFlagUpload(0)
{

}

CBandwidthDlg::~CBandwidthDlg()
{
}

void CBandwidthDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DOWNRATE_EDT, m_edtDowndRate);
    DDX_Control(pDX, IDC_UPLOADRATE_EDT, m_edtUploadRate);
    DDX_Radio(pDX, IDC_DOWNLIMIT_RDO, m_rdoDownd);
    DDX_Radio(pDX, IDC_UPLOADLIMIT_RDO, m_rdoUpload);
}


BEGIN_MESSAGE_MAP(CBandwidthDlg, CDialog)
    ON_BN_CLICKED(IDC_DOWNLIMIT_RDO, &CBandwidthDlg::OnBnClickedDowndRdo)
    ON_BN_CLICKED(IDC_DOWNRATE_RDO, &CBandwidthDlg::OnBnClickedDowndRdo)
    ON_BN_CLICKED(IDC_UPLOADLIMIT_RDO, &CBandwidthDlg::OnBnClickedUploadRdo)
    ON_BN_CLICKED(IDC_UPLOADAUTO_RDO, &CBandwidthDlg::OnBnClickedUploadRdo)
    ON_BN_CLICKED(IDC_UPLOADRATE_RDO, &CBandwidthDlg::OnBnClickedUploadRdo)
    ON_EN_CHANGE(IDC_DOWNRATE_EDT, &CBandwidthDlg::OnEnChangeEdt)
    ON_EN_CHANGE(IDC_UPLOADRATE_EDT, &CBandwidthDlg::OnEnChangeEdt)
END_MESSAGE_MAP()


// CBandwidthDlg 消息处理程序

BOOL CBandwidthDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CBandwidthDlg::OnInitDialog()
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

DWORD CBandwidthDlg::SetBandwidthValue()
{
    GET_NETWORK_POINT
    CString csWindowText;
    //
    csWindowText.Format(_T("%d"), pNetworkConf->dwDowndRate);
    m_edtDowndRate.SetWindowText(csWindowText);
    if (!_tcscmp(pNetworkConf->szDowndLimit, _T("true"))) {
        m_rdoDownd = 1;
        m_edtDowndRate.EnableWindow(TRUE);
    } else if (!_tcscmp(pNetworkConf->szDowndLimit, _T("false"))) {
        m_rdoDownd = 0;
        m_edtDowndRate.EnableWindow(FALSE);
    }
    //
    csWindowText.Format(_T("%d"), pNetworkConf->dwUploadRate);
    m_edtUploadRate.SetWindowText(csWindowText);
    if (!_tcscmp(pNetworkConf->szUploadLimit, _T("auto"))) {
        m_rdoUpload = 1;
        m_edtUploadRate.EnableWindow(FALSE);
    } else if (!_tcscmp(pNetworkConf->szUploadLimit, _T("true"))) {
        m_rdoUpload = 2;
        m_edtUploadRate.EnableWindow(TRUE);
    } else if (!_tcscmp(pNetworkConf->szUploadLimit, _T("false"))) {
        m_rdoUpload = 0;
        m_edtUploadRate.EnableWindow(FALSE);
    }
    //
    m_iFlagDownd = m_rdoDownd;
    m_iFlagUpload = m_rdoUpload;
    UpdateData(FALSE);
//
    return 0x00;
}

DWORD CBandwidthDlg::GetBandwidthValue()
{
    UpdateData(TRUE);
//
    GET_NETWORK_POINT
    CString csWindowText;
    if (0 == m_rdoDownd) _tcscpy_s(pNetworkConf->szDowndLimit, _T("false"));
    else {
        _tcscpy_s(pNetworkConf->szDowndLimit, _T("true"));
        m_edtDowndRate.GetWindowText(csWindowText);
        pNetworkConf->dwDowndRate = _tstoi(csWindowText);
    }
//
    if (0 == m_rdoUpload) _tcscpy_s(pNetworkConf->szUploadLimit, _T("false"));
    else if (1 == m_rdoUpload) _tcscpy_s(pNetworkConf->szUploadLimit, _T("auto"));
    else if (2 == m_rdoUpload) {
        _tcscpy_s(pNetworkConf->szUploadLimit, _T("true"));
        m_edtUploadRate.GetWindowText(csWindowText);
        pNetworkConf->dwUploadRate = _tstoi(csWindowText);
    }
//
    return 0x00;
}

void CBandwidthDlg::OnBnClickedDowndRdo()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);
//
    if (m_iFlagDownd == m_rdoDownd) 
		return;
    else m_iFlagDownd = m_rdoDownd;
//
    if (0 == m_rdoDownd) m_edtDowndRate.EnableWindow(FALSE);
    else m_edtDowndRate.EnableWindow(TRUE);
    // m_edtUploadRate;
    //((COptionsDlg*)m_pParentWnd)->m_btnApply.EnableWindow(TRUE);
    APPLY_BTN_ENABLE(TRUE);
}

void CBandwidthDlg::OnBnClickedUploadRdo()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData(TRUE);
//
    if (m_iFlagUpload == m_rdoUpload) 
		return;
    else m_iFlagUpload = m_rdoUpload;
//
    if (0==m_rdoUpload || 1==m_rdoUpload) m_edtUploadRate.EnableWindow(FALSE);
    else m_edtUploadRate.EnableWindow(TRUE);
//
    //((COptionsDlg*)m_pParentWnd)->m_btnApply.EnableWindow(TRUE);
    APPLY_BTN_ENABLE(TRUE);
}

void CBandwidthDlg::OnEnChangeEdt()
{
    // TODO: 在此添加控件通知处理程序代码
    //((COptionsDlg*)m_pParentWnd)->m_btnApply.EnableWindow(TRUE);
    APPLY_BTN_ENABLE(TRUE);
}
