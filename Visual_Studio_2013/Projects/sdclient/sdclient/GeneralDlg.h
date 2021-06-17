#pragma once
#include "afxwin.h"


// CGeneralDlg 对话框

class CGeneralDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeneralDlg)

public:
	CGeneralDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGeneralDlg();

	// 对话框数据
	enum { IDD = IDD_GENERAL_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CComboBox m_cboDriveRoot;
	CButton m_btnRemount;
	CButton m_chkSystemMessage;
	CButton m_chkAutoRun;
	CButton m_chkAutoUpdate;
public:
	afx_msg void OnCbnSelchangeDriveCbo();
	afx_msg void OnBnClickedRemountBtn();
	afx_msg void OnBnClickedSysmsgChk();
	afx_msg void OnBnClickedAutorunChk();
private:
	int m_iMountInde;
public:
	DWORD SetGeneralValue();
	DWORD GetGeneralValue();
};
