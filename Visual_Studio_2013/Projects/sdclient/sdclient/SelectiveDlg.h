#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "clientcom/clientcom.h"

// CSelectiveDlg �Ի���

class CSelectiveDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectiveDlg)

public:
	CSelectiveDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelectiveDlg();

	// �Ի�������
	enum { IDD = IDD_SELECTIVE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
private:
	CImageList m_imgList;
	CImageList m_imgState;
public:
	CAdvaTView m_tvDiresTView;
	CButton m_btnAdvanceMode;
	CButton m_btnOk;
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	afx_msg void OnBnClickedAdvanceModeBtn();
};
