// FormatDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CFormatDlg 对话框
class CFormatDlg : public CDialog
{
	// 构造
public:
	CFormatDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_FORMAT_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	// 生成的消息映射函数
	DECLARE_MESSAGE_MAP()
	// 实现
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CStatic m_stcLocation;
	CStatic m_stcDriveRoot;
	CStatic m_stcDiskLabel;
	CStatic m_stcFormatStatus;
public:
	DWORD SetFormatValue();
	DWORD GetFormatValue();
public:
	afx_msg void OnNMCustomdrawProgressBar(NMHDR *pNMHDR, LRESULT *pResult);
	CProgressCtrl m_proFormatStatus;
};
