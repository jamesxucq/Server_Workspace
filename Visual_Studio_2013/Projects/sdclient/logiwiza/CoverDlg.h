#pragma once
#include "afxwin.h"
#include "resource.h"

// CCoverDlg �Ի���

class CCoverDlg : public CDialog
{
	DECLARE_DYNAMIC(CCoverDlg)

public:
	CCoverDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCoverDlg();

	// �Ի�������
	enum { IDD = IDD_COVER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	//	afx_msg void OnClose();
protected:
	//	virtual void OnOK();
};
