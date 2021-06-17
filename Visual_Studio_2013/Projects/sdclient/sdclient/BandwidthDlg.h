#pragma once


// CBandwidthDlg 对话框

class CBandwidthDlg : public CDialog
{
	DECLARE_DYNAMIC(CBandwidthDlg)

public:
	CBandwidthDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBandwidthDlg();

	// 对话框数据
	enum { IDD = IDD_BANDWIDTH_PAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
public:
	CEdit m_edtDowndRate;
	CEdit m_edtUploadRate;
	int m_rdoDownd;
	int m_rdoUpload;
private:
	int m_iFlagDownd;
	int m_iFlagUpload;
public:
	afx_msg void OnBnClickedDowndRdo();
	afx_msg void OnBnClickedUploadRdo();
	afx_msg void OnEnChangeEdt();
public:
	DWORD SetBandwidthValue();
	DWORD GetBandwidthValue();
};
