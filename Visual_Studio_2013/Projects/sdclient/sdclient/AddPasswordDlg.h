#pragma once
#include "afxwin.h"


// CAddPasswordDlg �Ի���

class CAddPasswordDlg : public CDialog
{
    DECLARE_DYNAMIC(CAddPasswordDlg)

public:
    CAddPasswordDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CAddPasswordDlg();

    // �Ի�������
    enum { IDD = IDD_ADD_PASSWD_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
    CEdit m_edtNewPassword;
    CEdit m_edtValPassword;
    CStatic m_stcNewPassword;
    CStatic m_stcValPassword;
private:
    TCHAR *m_szNewPassword;
    TCHAR *m_szSaltValue;
public:
    afx_msg void OnBnClickedOk();
public:
    bool SetPassword(TCHAR *szNewPassword, TCHAR *szSaltValue);
    afx_msg void OnBnClickedCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnEnSetfocusNewpwdAddEdt();
    afx_msg void OnEnSetfocusChkpwdAddEdt();
};
