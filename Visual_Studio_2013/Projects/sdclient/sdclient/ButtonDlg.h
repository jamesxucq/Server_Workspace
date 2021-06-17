#pragma once
#include "afxwin.h"

#include "clientcom/clientcom.h"
#include "Resource.h"

// CGeneralDlg 对话框

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
private:
	DWORD m_dwDisplayBtnFlags;
public:
	CButtonST m_btnGeneral;
	CButtonST m_btnAccount;
	CButtonST m_btnBandwidth;
	CButtonST m_btnProxies;
	CButtonST m_btnSecurity;
	CButtonST m_btnAdvanced;
	CButtonST m_btnAbout;
public:
	afx_msg void OnBnClickedGeneralBtn();
	afx_msg void OnBnClickedAccountBtn();
	afx_msg void OnBnClickedBandwidthBtn();
	afx_msg void OnBnClickedProxiesBtn();
	afx_msg void OnBnClickedSecurityBtn();
	afx_msg void OnBnClickedAdvancedBtn();
	afx_msg void OnBnClickedAboutBtn();
public:
	void DrawBtnBitmap(DWORD dwFocusBtnNum);
};

#define DISPLAY_FLAGS_NORMAL(FLAGS)				FLAGS = 0;

#define GENERAL_SHOW_FOCUSE(FLAGS)				FLAGS |= 0x00000001
#define ACCOUNT_SHOW_FOCUSE(FLAGS)				FLAGS |= 0x00000002
#define BANDWIDTH_SHOW_FOCUSE(FLAGS)			FLAGS |= 0x00000004
#define PROXIES_SHOW_FOCUSE(FLAGS)				FLAGS |= 0x00000008
#define SECURITY_SHOW_FOCUSE(FLAGS)				FLAGS |= 0x00000010
#define ADVANCED_SHOW_FOCUSE(FLAGS)				FLAGS |= 0x00000020
#define ABOUT_SHOW_FOCUSE(FLAGS)				FLAGS |= 0x00000040

#define GENERAL_SHOW_NORMAL(FLAGS)				FLAGS &= 0xFFFFFFFE
#define ACCOUNT_SHOW_NORMAL(FLAGS)				FLAGS &= 0xFFFFFFFD
#define BANDWIDTH_SHOW_NORMAL(FLAGS)			FLAGS &= 0xFFFFFFFB
#define PROXIES_SHOW_NORMAL(FLAGS)				FLAGS &= 0xFFFFFFF7
#define SECURITY_SHOW_NORMAL(FLAGS)				FLAGS &= 0xFFFFFFEF
#define ADVANCED_SHOW_NORMAL(FLAGS)				FLAGS &= 0xFFFFFFDF
#define ABOUT_SHOW_NORMAL(FLAGS)				FLAGS &= 0xFFFFFFBF

#define GENERAL_CHECK_FOCUSE(FLAGS)				(FLAGS & 0x00000001)
#define ACCOUNT_CHECK_FOCUSE(FLAGS)				(FLAGS & 0x00000002)
#define BANDWIDTH_CHECK_FOCUSE(FLAGS)			(FLAGS & 0x00000004)
#define PROXIES_CHECK_FOCUSE(FLAGS)				(FLAGS & 0x00000008)
#define SECURITY_CHECK_FOCUSE(FLAGS)			(FLAGS & 0x00000010)
#define ADVANCED_CHECK_FOCUSE(FLAGS)			(FLAGS & 0x00000020)
#define ABOUT_CHECK_FOCUSE(FLAGS)				(FLAGS & 0x00000040)
