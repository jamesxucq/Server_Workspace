#pragma once
#include "afxwin.h"


// CFinishDlg 对话框

class CFinishDlg : public CDialog
{
	DECLARE_DYNAMIC(CFinishDlg)

public:
	CFinishDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFinishDlg();

	// 对话框数据
	enum { IDD = IDD_FINISH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	DWORD SetFinishValue(int CVolStatus);
	CStatic m_stcDisplay;
};
