#pragma once
#include "afxwin.h"
#include "resource.h"

// CDiskSettingDlg �Ի���

class CDiskSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CDiskSettingDlg)

public:
	CDiskSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDiskSettingDlg();

	// �Ի�������
	enum { IDD = IDD_DISKSETTING_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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

