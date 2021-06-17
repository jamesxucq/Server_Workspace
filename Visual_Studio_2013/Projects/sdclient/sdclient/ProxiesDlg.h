#pragma once
#include "afxwin.h"

// CProxiesDlg �Ի���

class CProxiesDlg : public CDialog
{
    DECLARE_DYNAMIC(CProxiesDlg)

public:
    CProxiesDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CProxiesDlg();

    // �Ի�������
    enum { IDD = IDD_PROXIES_PAGE_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
public:
    CComboBox m_cboType;
    CEdit m_edtAddress;
    CEdit m_edtPort;
    CEdit m_edtUserName;
    CEdit m_edtPassword;
    CButton m_chkProxyAuth;
    int m_rdoProxy;
private:
    int m_iFlagRadio;
public:
    afx_msg void OnBnClickedProxyRdo();
    afx_msg void OnBnClickedAuthChk();
    afx_msg void OnEnChangeEx();
public:
    DWORD SetProxieszValue();
    DWORD GetProxieszValue();
public:
    CStatic m_stcType;
    CStatic m_stcAddress;
    CStatic m_stcPort;
    CStatic m_stcUserName;
    CStatic m_stcPassword;
};
