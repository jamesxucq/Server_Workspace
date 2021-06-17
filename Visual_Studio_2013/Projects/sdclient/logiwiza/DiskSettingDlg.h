#pragma once
#include "afxwin.h"
#include "resource.h"

// CDiskSettingDlg 对话框

class CDiskSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CDiskSettingDlg)

public:
	CDiskSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiskSettingDlg();

	// 对话框数据
	enum { IDD = IDD_DISKSETTING_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedPathSelectBtn();
	afx_msg void OnEnChangeLocationEdt();
public:
	CEdit m_edtLocation;
	CStatic m_stcChkspacePrompt;
	CStatic m_stcChkspaceText;
	CStatic m_stcAttentionText;
public:
	virtual BOOL OnInitDialog();
public:
	DWORD SetDiskSettingValue();
	DWORD GetDiskSettingValue();
};

