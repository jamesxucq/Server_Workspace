#pragma once
#include "afxwin.h"


// CAdvancedDlg �Ի���

class CAdvancedDlg : public CDialog
{
	DECLARE_DYNAMIC(CAdvancedDlg)

public:
	CAdvancedDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAdvancedDlg();

	// �Ի�������
	enum { IDD = IDD_ADVANCED_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
