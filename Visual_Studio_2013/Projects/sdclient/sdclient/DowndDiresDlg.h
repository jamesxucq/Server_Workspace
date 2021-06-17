#pragma once

#include "SelectiveBzl.h"

// CDowndDiresDlg 对话框

class CDowndDiresDlg : public CDialog
{
	DECLARE_DYNAMIC(CDowndDiresDlg)

public:
	CDowndDiresDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDowndDiresDlg();

	// 对话框数据
	enum { IDD = IDD_DOWND_DIRES_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};