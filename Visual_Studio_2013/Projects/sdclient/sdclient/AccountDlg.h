#pragma once
#include "afxwin.h"


// CAccountDlg �Ի���

class CAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CAccountDlg)

public:
	CAccountDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAccountDlg();

	// �Ի�������
	enum { IDD = IDD_ACCOUNT_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CButton m_btnUnlink;
	CStatic m_stcUserName;
	CStatic m_stcLocalIdenti;
	// CStatic m_stcVersion;
public:
	afx_msg void OnBnClickedUnlinkBtn();
public:
	DWORD SetAccountValue();
	// bool GetAccountValue(); // modify by james 20101205
};
