// MoveUsrdatDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "sdclient.h"
#include "ExecuteTransmitBzl.h"
#include "LocalBzl.h"
#include "MoveUserspaceBzl.h"

#include "MoveUserspaceDlg.h"


// CMoveUserspacelg 对话框

IMPLEMENT_DYNAMIC(CMoveUserspacelg, CDialog)

CMoveUserspacelg::CMoveUserspacelg(CWnd* pParent /*=NULL*/)
    : CDialog(CMoveUserspacelg::IDD, pParent)
    ,m_szExistLocation(NULL)
    ,m_szNewLocation(NULL)
    ,m_szFileName(NULL)
    ,m_dwFileSize(0)
    ,m_szLetter(NULL)
{
}

CMoveUserspacelg::~CMoveUserspacelg()
{
}

void CMoveUserspacelg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDOK, m_btnOk);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_MOVE_STATUS_STC, m_stcMoveStatus);
}


BEGIN_MESSAGE_MAP(CMoveUserspacelg, CDialog)
    ON_BN_CLICKED(IDOK, &CMoveUserspacelg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CMoveUserspacelg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMoveUserspacelg 消息处理程序

BOOL CMoveUserspacelg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CMoveUserspacelg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_btnOk.EnableWindow(FALSE);
    m_btnCancel.EnableWindow(FALSE);
    DWORD dwProcessStatus = NExecTransBzl::GetTransmitProcessStatus();
    NLocalBzl::StopTransmitProcess();

    int move_result = NMoveSpaceBzl::MoveUserspace(this, m_szExistLocation,
                      m_szNewLocation, m_szFileName,
                      m_dwFileSize, m_szLetter);

    NLocalBzl::ResumeTransmitProcess(dwProcessStatus);
    m_btnOk.EnableWindow(TRUE);
    m_btnCancel.EnableWindow(TRUE);

    switch(move_result) {
    case ERR_SUCCESS:
        break;
    case ERR_UNMOUNT:
        MessageBox(_T("有文件打开,请关闭后重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    case ERR_COPY_DATA:
        MessageBox(_T("移动文件出错,请检查磁盘空间."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    case ERR_FREE_SPACE:
        MessageBox(_T("目标磁盘没有足够的空间."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    case ERR_NEW_MOUNT:
    case ERR_EXIST_MOUNT:
    case ERR_FAULT:
// _LOG_DEBUG(_T("MoveUserspace result:%d"), move_result); // disable by james 20140410
        MessageBox(_T("程序内部出错,请关闭后重试."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    default:
        break;
    }
    if(ERR_SUCCESS != move_result) {
        TCHAR szDisplsyStatus[MID_TEXT_LEN];
        _stprintf_s(szDisplsyStatus, _T("移动数据成功.\n\n        %s"), m_szNewLocation);
        m_stcMoveStatus.SetWindowText(szDisplsyStatus);
        return;
    }
//
    OnOK();
}

void CMoveUserspacelg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}

bool CMoveUserspacelg::SetMoveUserspaceArgu (TCHAR *szExistLocation, TCHAR *szNewLocation,TCHAR *szFileName, DWORD dwFileSize,  TCHAR *szDriveLetter)
{
    m_szExistLocation = szExistLocation;
    m_szNewLocation = szNewLocation;
    m_szFileName = szFileName;
    m_dwFileSize = dwFileSize;
    m_szLetter = szDriveLetter;

    return true;
}
BOOL CMoveUserspacelg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    TCHAR szDisplsyStatus[MID_TEXT_LEN];

    _stprintf_s(szDisplsyStatus, _T("是否确定移动数据到新位置?\n\n        %s"), m_szNewLocation);
    m_stcMoveStatus.SetWindowText(szDisplsyStatus);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
