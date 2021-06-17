#pragma once
#include "afxwin.h"


// CAdvancedDlg 对话框

class CAdvancedDlg : public CDialog
{
	DECLARE_DYNAMIC(CAdvancedDlg)

public:
	CAdvancedDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAdvancedDlg();

	// 对话框数据
	enum { IDD = IDD_ADVANCED_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CEdit m_edtLocation;
	CButton m_btnDireSelection;
	CStatic m_stcSelective;
	CButton m_btnForceCtrl;
public:
	afx_msg void OnEnChangeLocationEdt();
	afx_msg void OnBnClickedMoveLocationBtn();
	afx_msg void OnBnClickedDireSelectionBtn();
	afx_msg void OnBnClickedForcectrlBtn();
public:
	DWORD SetAdvancedValue();
	DWORD GetAdvancedValue();
};
