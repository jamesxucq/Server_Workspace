#pragma once

#include "clientcom/clientcom.h"

#include "AddPasswordDlg.h"
#include "ChangePasswordDlg.h"
#include "afxcmn.h"

// CSecurityDlg �Ի���

class CSecurityDlg : public CDialog
{
    DECLARE_DYNAMIC(CSecurityDlg)

public:
    CSecurityDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CSecurityDlg();

    // �Ի�������
    enum { IDD = IDD_SECURITY_PAGE_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
public:
    TCHAR m_szNewPassword[PASSWORD_LENGTH];
    TCHAR m_szOldPassword[PASSWORD_LENGTH];
    TCHAR m_szSaltValue[SALT_TEXT_LEN];
public:
    CLinkCtrl m_linkChangeUserpwd;
    CButton m_chkClientProtect;
    CButton m_btnPassword;
public:
    afx_msg void OnBnClickedPasswordChk();
    afx_msg void OnBnClickedClientPwdBtn();
    afx_msg void OnNMClickChangeUserpwdSyslink(NMHDR *pNMHDR, LRESULT *pResult);
public:
    DWORD SetSecurityValue();
    DWORD GetSecurityValue();
};
