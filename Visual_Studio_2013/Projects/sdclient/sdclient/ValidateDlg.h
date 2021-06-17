#pragma once


// CValidateDlg 对话框

class CValidateDlg : public CDialog
{
	DECLARE_DYNAMIC(CValidateDlg)

public:
	CValidateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CValidateDlg();

	// 对话框数据
	enum { IDD = IDD_VALIDATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	CEdit m_edtPassword;
	CButton m_btnValidate;
public:
	afx_msg void OnBnClickedValidateBtn();
	afx_msg void OnEnChangePasswordEdt();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
