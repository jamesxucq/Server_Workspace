#pragma once

#include "clientcom/clientcom.h"

// CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUT_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CLinkCtrl m_linkAppUpdate;
public:
	afx_msg void OnNMClickAppUpdateSyslink(NMHDR *pNMHDR, LRESULT *pResult);
public:
	DWORD SetAboutValue();
};
