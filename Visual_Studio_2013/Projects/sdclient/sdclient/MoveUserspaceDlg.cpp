// MoveUsrdatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include "sdclient.h"
#include "ExecuteTransmitBzl.h"
#include "LocalBzl.h"
#include "MoveUserspaceBzl.h"

#include "MoveUserspaceDlg.h"


// CMoveUserspacelg �Ի���

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


// CMoveUserspacelg ��Ϣ�������

BOOL CMoveUserspacelg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(pMsg->message==WM_KEYDOWN) {
        if(pMsg->wParam==VK_ESCAPE || pMsg-> wParam==VK_RETURN)
            return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CMoveUserspacelg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
        MessageBox(_T("���ļ���,��رպ�����."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    case ERR_COPY_DATA:
        MessageBox(_T("�ƶ��ļ�����,������̿ռ�."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    case ERR_FREE_SPACE:
        MessageBox(_T("Ŀ�����û���㹻�Ŀռ�."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    case ERR_NEW_MOUNT:
    case ERR_EXIST_MOUNT:
    case ERR_FAULT:
// _LOG_DEBUG(_T("MoveUserspace result:%d"), move_result); // disable by james 20140410
        MessageBox(_T("�����ڲ�����,��رպ�����."), _T("Tip"), MB_OK|MB_ICONWARNING);
        break;
    default:
        break;
    }
    if(ERR_SUCCESS != move_result) {
        TCHAR szDisplsyStatus[MID_TEXT_LEN];
        _stprintf_s(szDisplsyStatus, _T("�ƶ����ݳɹ�.\n\n        %s"), m_szNewLocation);
        m_stcMoveStatus.SetWindowText(szDisplsyStatus);
        return;
    }
//
    OnOK();
}

void CMoveUserspacelg::OnBnClickedCancel()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    TCHAR szDisplsyStatus[MID_TEXT_LEN];

    _stprintf_s(szDisplsyStatus, _T("�Ƿ�ȷ���ƶ����ݵ���λ��?\n\n        %s"), m_szNewLocation);
    m_stcMoveStatus.SetWindowText(szDisplsyStatus);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}
