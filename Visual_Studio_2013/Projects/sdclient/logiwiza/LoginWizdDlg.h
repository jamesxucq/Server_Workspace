#pragma once

#include "afxwin.h"

#include "ChildrenDlg.h"
#include "ButtonDlg.h"

// CLoginWizdDlg �Ի���

class CLoginWizdDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginWizdDlg)

public:
	CLoginWizdDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginWizdDlg();

	// �Ի�������
	enum { IDD = IDD_WIZARD_DLG };
protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
private:
	class CChildrenDlg m_dgChildrenDlg;
	class CButtonDlg *m_pButtonDlg;
public:
	virtual BOOL DestroyWindow();
	afx_msg void OnClose();
public:
	HWND DisplayWindowNormal(DWORD dwWizardNumbe);
	void DisplayWindowAction(DWORD dwWizardNumbe);
	void DisplayWindowFinish(DWORD dwWizardNumbe);
public:
	void ExitApplica();
};

enum wizard_pages {
	UNUSE_PAGE = 1,
	COVER_PAGE,
	LOGIN_PAGE,
	DISKSETTING_PAGE,
	START_FORMAT_PAGE,
	FINISH_FORMAT_PAGE,
	FINISH_PAGE
};