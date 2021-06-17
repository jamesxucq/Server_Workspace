#pragma once
#include "afxwin.h"


// CAccountDlg 对话框

class CAccountDlg : public CDialog
{
	DECLARE_DYNAMIC(CAccountDlg)

public:
	CAccountDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAccountDlg();

	// 对话框数据
	enum { IDD = IDD_ACCOUNT_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
