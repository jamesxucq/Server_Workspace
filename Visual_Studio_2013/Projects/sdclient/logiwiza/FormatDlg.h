// FormatDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CFormatDlg �Ի���
class CFormatDlg : public CDialog
{
	// ����
public:
	CFormatDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_FORMAT_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	// ���ɵ���Ϣӳ�亯��
	DECLARE_MESSAGE_MAP()
	// ʵ��
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CStatic m_stcLocation;
	CStatic m_stcDriveRoot;
	CStatic m_stcDiskLabel;
	CStatic m_stcFormatStatus;
public:
	DWORD SetFormatValue();
	DWORD GetFormatValue();
public:
	afx_msg void OnNMCustomdrawProgressBar(NMHDR *pNMHDR, LRESULT *pResult);
	CProgressCtrl m_proFormatStatus;
};
