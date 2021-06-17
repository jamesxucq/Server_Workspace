// LocalBzl.h : ͷ�ļ�
//

#pragma once

#include "clientcom/clientcom.h"
#include "LocalBzl.h"
#include "TRANSThread.h"
#include "OptionsDlg.h"
#include "afxwin.h"

// CClientDlg �Ի���
class CClientDlg : public CDialog
{
	// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_CLIENT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
