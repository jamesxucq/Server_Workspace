#pragma once
#include "afxwin.h"


// CMoveUserspacelg 对话框

class CMoveUserspacelg : public CDialog
{
    DECLARE_DYNAMIC(CMoveUserspacelg)

public:
    CMoveUserspacelg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CMoveUserspacelg();

    // 对话框数据
    enum { IDD = IDD_MOVE_USERSPACE_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    CButton m_btnOk;
    CButton m_btnCancel;
    CStatic m_stcMoveStatus;
private:
    TCHAR *m_szExistLocation;
    TCHAR *m_szNewLocation;
    TCHAR *m_szFileName;
    DWORD m_dwFileSize;
    TCHAR *m_szLetter;
public:
    bool SetMoveUserspaceArgu (TCHAR *szExistLocation, TCHAR *szNewLocation,TCHAR *szFileName, DWORD dwFileSize,  TCHAR *szDriveLetter);
    virtual BOOL OnInitDialog();
};
