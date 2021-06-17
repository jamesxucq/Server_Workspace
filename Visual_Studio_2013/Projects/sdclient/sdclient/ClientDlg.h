// LocalBzl.h : 头文件
//

#pragma once

#include "clientcom/clientcom.h"
#include "LocalBzl.h"
#include "TRANSThread.h"
#include "OptionsDlg.h"
#include "afxwin.h"

// CClientDlg 对话框
class CClientDlg : public CDialog
{
	// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_CLIENT_DLG };

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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	COptionsDlg *m_pOptionsDlg;
	BOOL m_bOptionsOpened;
public:
	void OptionsDlgOpen(BOOL bOpenDlg) {m_bOptionsOpened = bOpenDlg; }
private:
	// BOOL		m_bShowBalloonTip;	// display info in main window
	BOOL		m_bShowTrayNotify;	// display info in main window
public:
	LRESULT OnTrayNotify(WPARAM wParam,LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnAppSuspend();
	afx_msg void OnOptionsOpen();
	afx_msg void OnRootDirectoryOpen();
	afx_msg void OnPausedResume();
	afx_msg LRESULT OnAddIconToTaskbar(WPARAM wParam, LPARAM lParam);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL DestroyWindow();
};
