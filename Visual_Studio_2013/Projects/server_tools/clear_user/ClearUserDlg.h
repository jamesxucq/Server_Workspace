// ClearUserDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CClearUserDlg �Ի���
class CClearUserDlg : public CDialog
{
	// ����
public:
	CClearUserDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_CLEARUSER_DIALOG };

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
	CEdit m_edtUID;
	int m_rdoClearType;
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedConnectBtn();
	afx_msg void OnBnClickedClearBtn();
	afx_msg void OnBnClickedCancel();
};
