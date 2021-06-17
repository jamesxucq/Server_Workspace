#pragma once


// CButtonDlg �Ի���

class CButtonDlg : public CDialog
{
	DECLARE_DYNAMIC(CButtonDlg)

public:
	CButtonDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CButtonDlg();

	// �Ի�������
	enum { IDD = IDD_BUTTON_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CButton m_btnNext;
	CButton m_btnPrev;
public:
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedPrev();
public:
	void DrawBtnNormalBitmap(DWORD dwFocusBtnNum);
	void DrawBtnActionBitmap(DWORD dwFocusBtnNum);
	void DrawBtnFinishBitmap(DWORD dwFocusBtnNum);
};


