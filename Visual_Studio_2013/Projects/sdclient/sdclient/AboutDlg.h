#pragma once

#include "clientcom/clientcom.h"

// CAboutDlg 对话框

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUT_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CLinkCtrl m_linkAppUpdate;
public:
	afx_msg void OnNMClickAppUpdateSyslink(NMHDR *pNMHDR, LRESULT *pResult);
public:
	DWORD SetAboutValue();
};
