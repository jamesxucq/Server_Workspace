// AdminDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAdminDlg 对话框
class CAdminDlg : public CDialog
{
// 构造
public:
	CAdminDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ADMIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_stcCommand;
	CStatic m_stcConnect;
	CEdit m_edtAdmin;
	CEdit m_edtPassword;
	CEdit m_edtAddress;
	CEdit m_edtShowXml;
	CEdit m_edtAddminAddress;
public:
	afx_msg void OnBnClickedConnectBtn();
	afx_msg void OnBnClickedRefreshBtn();
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedClearBtn();
	afx_msg void OnBnClickedCancel();
};
