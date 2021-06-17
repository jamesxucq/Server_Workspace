#pragma once
#include "afxwin.h"
#include "resource.h"

// CCoverDlg 对话框

class CCoverDlg : public CDialog
{
	DECLARE_DYNAMIC(CCoverDlg)

public:
	CCoverDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCoverDlg();

	// 对话框数据
	enum { IDD = IDD_COVER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	//	afx_msg void OnClose();
protected:
	//	virtual void OnOK();
};
