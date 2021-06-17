#pragma once


// CValidateDlg �Ի���

class CValidateDlg : public CDialog
{
	DECLARE_DYNAMIC(CValidateDlg)

public:
	CValidateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CValidateDlg();

	// �Ի�������
	enum { IDD = IDD_VALIDATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
