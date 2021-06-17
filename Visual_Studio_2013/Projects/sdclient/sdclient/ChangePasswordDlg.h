#pragma once
#include "afxwin.h"


// CChangePasswordDlg �Ի���

class CChangePasswordDlg : public CDialog
{
    DECLARE_DYNAMIC(CChangePasswordDlg)

public:
    CChangePasswordDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CChangePasswordDlg();

    // �Ի�������
    enum { IDD = IDD_CHG_PASSWD_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
    CEdit m_edtNewPassword;
    CEdit m_edtValPassword;
    CEdit m_edtOldPassword;
    CStatic m_stcOldPassword;
    CStatic m_stcNewPassword;
    CStatic m_stcValPassword;
private:
    TCHAR *m_szOldPassword;
    TCHAR *m_szNewPassword;
    TCHAR *m_szSaltValue;
public:
    bool SetPassword(TCHAR *szNewPassword, TCHAR *szOldPassword, TCHAR *szSaltValue);
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnEnSetfocusOldpwdChangeEdt();
    afx_msg void OnEnSetfocusNewpwdChangeEdt();
    afx_msg void OnEnSetfocusChkpwdChangeEdt();
};
