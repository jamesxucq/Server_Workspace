#pragma once
#include "afxwin.h"


// CFinishDlg �Ի���

class CFinishDlg : public CDialog
{
	DECLARE_DYNAMIC(CFinishDlg)

public:
	CFinishDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFinishDlg();

	// �Ի�������
	enum { IDD = IDD_FINISH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	DWORD SetFinishValue(int CVolStatus);
	CStatic m_stcDisplay;
};
