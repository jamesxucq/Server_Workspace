#pragma once

#include "SelectiveBzl.h"

// CDowndDiresDlg �Ի���

class CDowndDiresDlg : public CDialog
{
	DECLARE_DYNAMIC(CDowndDiresDlg)

public:
	CDowndDiresDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDowndDiresDlg();

	// �Ի�������
	enum { IDD = IDD_DOWND_DIRES_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};