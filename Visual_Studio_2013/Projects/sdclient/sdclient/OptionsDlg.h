// OptionsDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "LocalBzl.h"
#include "ButtonDlg.h"
#include "ChildrenDlg.h"


enum OptionPage
{	
	ENUM_GENERAL = 1,
	ENUM_ACCOUNT,
	ENUM_BANDWIDTH,
	ENUM_PROXIES,
	ENUM_SECURITY,
	ENUM_ADVANCED,
	ENUM_ABOUT
};

// COptionsDlg 对话框
class COptionsDlg : public CDialog
{
	// 构造
public:
	COptionsDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_OPTIONS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	// 实现
	DECLARE_MESSAGE_MAP()
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	virtual BOOL DestroyWindow();
private:
	CButton m_btnApply;
public:
	afx_msg void OnBnClickedOkBtn();
	afx_msg void OnBnClickedCancelBtn();
	afx_msg void OnBnClickedApplyBtn();
private:
	class CButtonDlg *m_pButtonDlg;
	class CChildrenDlg m_dgChildrenDlg;
public:
	void RefreshWindow(DWORD dwSubDlgNum = ENUM_GENERAL);
	BOOL EnableApplyButton(BOOL bEnable = TRUE) { return m_btnApply.EnableWindow(bEnable); }
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};
