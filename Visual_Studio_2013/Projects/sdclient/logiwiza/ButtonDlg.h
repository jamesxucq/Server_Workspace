#pragma once


// CButtonDlg 对话框

class CButtonDlg : public CDialog
{
	DECLARE_DYNAMIC(CButtonDlg)

public:
	CButtonDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CButtonDlg();

	// 对话框数据
	enum { IDD = IDD_BUTTON_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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


