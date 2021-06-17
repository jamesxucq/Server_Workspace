#pragma once
#include "afxwin.h"


// CLoginDlg �Ի���

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginDlg();

	// �Ի�������
	enum { IDD = IDD_LOGIN_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CEdit m_edtServAddr;
	CEdit m_edtUserName;
	CEdit m_edtPassword;
	CEdit m_edtLocalIdenti;
	CStatic m_stcLoginStatus;
	CLinkCtrl m_linkForgetPwd;
	CLinkCtrl m_linkRegistUser;
public:
	afx_msg void OnBnClickedProxyBtn();
	afx_msg void OnNMClickForgetpwdSyslink(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickRegistusrSyslink(NMHDR *pNMHDR, LRESULT *pResult);
public:
	DWORD SetLoginValue();
	DWORD GetLoginValue();
};
