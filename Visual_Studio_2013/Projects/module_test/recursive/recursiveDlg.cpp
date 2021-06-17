// recursiveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "recursive.h"
#include "recursiveDlg.h"


#include "FilesVec.h"
#include "RecursiveUtility.h"


// CrecursiveDlg 对话框




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


// CrecursiveDlg 消息处理程序

BOOL CrecursiveDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CrecursiveDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialog::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
    // TODO: 在此添加控件通知处理程序代码

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
