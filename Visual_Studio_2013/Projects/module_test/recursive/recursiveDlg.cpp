// recursiveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "recursive.h"
#include "recursiveDlg.h"


#include "FilesVec.h"
#include "RecursiveUtility.h"


// CrecursiveDlg �Ի���




CrecursiveDlg::CrecursiveDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CrecursiveDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CrecursiveDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CrecursiveDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, &CrecursiveDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CrecursiveDlg ��Ϣ�������

BOOL CrecursiveDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������

    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CrecursiveDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ����������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CrecursiveDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

#define RECURSIVE_FILE_ENTRY   _T("recursive_file.acd")

HANDLE BuildCacheFile(TCHAR *szCacheFile, TCHAR *szFileName)
{
    static TCHAR szCacheDirectory[MAX_PATH];
    TCHAR szBuildFile[MAX_PATH];
    //
    if(_T(':') != szCacheDirectory[1]) {
        if(!GetEnvironmentVariable(_T("APPDATA"), szCacheDirectory, MAX_PATH))
            return NULL;
        _tcscat_s(szCacheDirectory, _T("\\sdclient"));
        CreateDirectory(szCacheDirectory, NULL);
    }
    _stprintf_s(szBuildFile, MAX_PATH, _T("%s\\%s"), szCacheDirectory, szFileName);
    if(szCacheFile) _tcscpy_s(szCacheFile, MAX_PATH, szBuildFile);
    //
    HANDLE hFileCache = CreateFile( szBuildFile,
                                    GENERIC_WRITE | GENERIC_READ,
                                    NULL, /* FILE_SHARE_READ */
                                    NULL,
                                    OPEN_ALWAYS, // CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_TEMPORARY, // | FILE_FLAG_DELETE_ON_CLOSE,
                                    NULL);
    if( INVALID_HANDLE_VALUE == hFileCache ) {
        TRACE( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    //
    return hFileCache;
}

int xread_latt(struct attent *latt, HANDLE hEntryFile, unsigned __int64 offset)
{
    LONG lFileSizeHigh = (DWORD)(offset>>32);
    DWORD dwResult = SetFilePointer(hEntryFile, (DWORD)(offset&0xFFFFFFFF), &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return -1;
    }
    DWORD rlen;
    if(!ReadFile(hEntryFile, latt, sizeof(struct attent), &rlen, NULL) || 0 >= rlen) {
        return -1;
    }
    return 0;
}

void print_latt(HANDLE hEntryFile)
{
    unsigned __int64 offset;
    struct attent latt;

    int ind;
    for(ind = 0; ; ind++) {
        offset = sizeof(struct attent) * ind;
        if(xread_latt(&latt, hEntryFile, offset)) break;
        TRACE(_T("file_name:%s\n"), latt.file_name);
    }
}

void CrecursiveDlg::OnBnClickedButton1()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������

// 0:ok 0x01:error 0x02:processing 0x03:not_found
    TCHAR szEntryFile[MAX_PATH];
    HANDLE hEntryFile = BuildCacheFile(szEntryFile, RECURSIVE_FILE_ENTRY);
    //
    DWORD x = RecursiveUtility::RecursiveFile(hEntryFile, _T("Z:"));
    //
    if(!x) {
        print_latt(hEntryFile);
        //
		if(INVALID_HANDLE_VALUE != hEntryFile) {
			CloseHandle( hEntryFile );
			hEntryFile = INVALID_HANDLE_VALUE;
		}
        DeleteFile(szEntryFile);
    }
    if(INVALID_HANDLE_VALUE != hEntryFile) CloseHandle( hEntryFile );
    //
}
