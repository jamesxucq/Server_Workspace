// AdminDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAdminDlg �Ի���
class CAdminDlg : public CDialog
{
// ����
public:
	CAdminDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ADMIN_DIALOG };

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
	CStatic m_stcCommand;
	CStatic m_stcConnect;
	CEdit m_edtAdmin;
	CEdit m_edtPassword;
	CEdit m_edtAddress;
	CEdit m_edtShowXml;
	CEdit m_edtAddminAddress;
public:
	afx_msg void OnBnClickedConnectBtn();
	afx_msg void OnBnClickedRefreshBtn();
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedClearBtn();
	afx_msg void OnBnClickedCancel();
};