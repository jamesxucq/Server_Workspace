// chks_tDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "chks_t.h"
#include "chks_tDlg.h"

#include "./third party/RiverFS/RiverFS.h"
#include "./third party/RiverFS/sha1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cchks_tDlg 对话框




Cchks_tDlg::Cchks_tDlg(CWnd* pParent /*=NULL*/)
    : CDialog(Cchks_tDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cchks_tDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cchks_tDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &Cchks_tDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// Cchks_tDlg 消息处理程序

BOOL Cchks_tDlg::OnInitDialog()
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

void Cchks_tDlg::OnPaint()
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
HCURSOR Cchks_tDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

#define MD5_DIGEST_LEN					16
#define MD5_TEXT_LENGTH					33
VOID ChksMD5(TCHAR *label, unsigned char *md5sum)
{
    TCHAR chksum_str[64];

    for (int i=0; i<MD5_DIGEST_LEN; i++)
        _stprintf_s (chksum_str+i*2, MD5_TEXT_LENGTH, _T("%02x"), md5sum[i]);
    chksum_str[32] = _T('\0');

    TRACE(_T("%s:%s\n"), label, chksum_str);
}

VOID ChksSha1(TCHAR *label, unsigned char *sha1sum)
{
    TCHAR chksum_str[256];

    for (int i=0; i<20; i++)
        _stprintf_s (chksum_str+i*2, 64, _T("%02x"), sha1sum[i]);
    chksum_str[40] = _T('\0');

    TRACE(_T("%s:%s\n"), label, chksum_str);
}

void print_chunk_chks(TCHAR *szFilePath, DWORD chk_posi)
{

    unsigned char md5_chks[MD5_DIGEST_LEN];

    HANDLE hChunk = CreateFile( szFilePath,
                                /* GENERIC_WRITE | */ GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if( hChunk == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed: %d"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return;
    }
    unsigned __int64 qwFileSize = CommonUtility::FileSize(szFilePath);
    CommonUtility::chunk_chks(md5_chks, hChunk, qwFileSize, ((unsigned __int64)chk_posi) << 22);

    if(hChunk != INVALID_HANDLE_VALUE) {
        CloseHandle( hChunk );
        hChunk = INVALID_HANDLE_VALUE;
    }

    ChksMD5(_T("chunk chks"), md5_chks);
}

HANDLE BuildVerifyFile(TCHAR *szVerifyFile)
{
    HANDLE hFileVerify;
    TCHAR szDirectory[MAX_PATH];
    TCHAR szFileName[MAX_PATH];
//
    GetTempPath(MAX_PATH, szDirectory);
    GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
    hFileVerify = CreateFile( szFileName,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                              NULL);
    if( hFileVerify == INVALID_HANDLE_VALUE ) {
        return INVALID_HANDLE_VALUE;
    }
    if(szVerifyFile) _tcscpy_s(szVerifyFile, MAX_PATH, szFileName);
//
    return hFileVerify;
}

void printf_file_chks_local(TCHAR *szFilePath)
{

    unsigned char md5_chks[MD5_DIGEST_LEN];

    HANDLE hChunk = CreateFile( szFilePath,
                                /* GENERIC_WRITE | */ GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if( hChunk == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed: %d"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return;
    }
    unsigned __int64 qwFileSize = CommonUtility::FileSize(szFilePath);
    DWORD iNewLength = (DWORD)(qwFileSize >> 22); /* new_size / CHUNK_SIZE; */ \
    if (POW2N_MOD(qwFileSize, CHUNK_SIZE)) iNewLength++;
    for(unsigned __int64 i=0; i < iNewLength; i++) {
        CommonUtility::chunk_chks(md5_chks, hChunk, qwFileSize, ((unsigned __int64)i) << 22);
        ChksMD5(_T("chunk chks"), md5_chks);
    }

    if(hChunk != INVALID_HANDLE_VALUE) {
        CloseHandle( hChunk );
        hChunk = INVALID_HANDLE_VALUE;
    }

}

void printf_file_chks(HANDLE hRiveChks)
{
    DWORD result = SetFilePointer(hRiveChks, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
        TRACE(_T("error\n"));
    }

    struct riv_chks tRiveChks;
    DWORD slen;
    for(;;) {
        if(!ReadFile(hRiveChks, &tRiveChks, sizeof(struct riv_chks), &slen, NULL) || slen <= 0) {
            break;
        }
        ChksMD5(_T("chunk chks"), tRiveChks.md5_chks);
    }
}

void printf_file_sha1(TCHAR *szFilePath)
{
    unsigned char szFileChks[SHA1_DIGEST_LEN];
    unsigned char md5_chks[MD5_DIGEST_LEN];

    HANDLE hChunk = CreateFile( szFilePath,
                                /* GENERIC_WRITE | */ GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if( hChunk == INVALID_HANDLE_VALUE ) {
        TRACE( _T("create file failed: %d"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return;
    }
    unsigned __int64 qwFileSize = CommonUtility::FileSize(szFilePath);
    DWORD iNewLength = (DWORD)(qwFileSize >> 22); /* new_size / CHUNK_SIZE; */ \
    if (POW2N_MOD(qwFileSize, CHUNK_SIZE)) iNewLength++;
    sha1_ctx cx[1];
    sha1_begin(cx);
    for(unsigned __int64 i=0; i < iNewLength; i++) {
        CommonUtility::chunk_chks(md5_chks, hChunk, qwFileSize, ((unsigned __int64)i) << 22);
        // ChksMD5(_T("chunk chks"), md5_chks);
        sha1_hash(md5_chks, MD5_DIGEST_LEN, cx);
    }
    sha1_end(szFileChks, cx);

    if(hChunk != INVALID_HANDLE_VALUE) {
        CloseHandle( hChunk );
        hChunk = INVALID_HANDLE_VALUE;
    }
    ChksSha1(_T("file sha1"), szFileChks);
}

void Cchks_tDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    // OnOK();
    RiverFS::Initialize(_T("E:\\TEST DATA\\rive"), _T("Z:"));
    //
    /*    unsigned char szFileChks[SHA1_DIGEST_LEN];
        struct FileID tFileID;
        RiverFS::InitFileID(&tFileID, _T("Z:\\UNIXfavicon.pdf"));
        memcpy(szFileChks, tFileID.szFileChks, SHA1_DIGEST_LEN);
        ChksSha1(_T("file sha1"), szFileChks); */
    //
// #define CloseFileID(tFileID) CloseHandle(tFileID.hRiveChks)
    //
//	FILETIME ftLastWrite = {0, 1};
//	unsigned char md5_chks[MD5_DIGEST_LEN] = {1};
    // RiverFS::ChunkWriteUpdate(_T("Z:\\xUNIXfavicon.pdf"), 2, &ftLastWrite);
    // RiverFS::ChunkWriteUpdate(_T("Z:\\xUNIXfavicon.pdf"), 1, &ftLastWrite);
//	RiverFS::ChunkUpdate(_T("\\live\\UNIXfavicon.pdf"), 1, md5_chks, &ftLastWrite, _T("Z:"));
//	RiverFS::ChunkUpdate(_T("\\live\\user_pool\\xxx\\UNIXfavicon.pdf"), 2, md5_chks, &ftLastWrite, _T("Z:"));
    //
    /*    RiverFS::FileSha1(szFileChks, _T("Z:\\UNIXfavicon.pdf"));
        ChksSha1(_T("file sha1"), szFileChks);
        printf_file_sha1(_T("Z:\\UNIXfavicon.pdf"));*/

    //
    // RiverFS::ValidFileChks(_T("Z:\\UNIXfavicon.pdf"), 0); // 0:pass 1:error

    //
    /*    HANDLE hRiveChks = BuildVerifyFile(NULL);
        RiverFS::FileChks(hRiveChks, _T("Z:\\UNIXfavicon.pdf"), 0);
        printf_file_chks(hRiveChks);
        CloseHandle( hRiveChks );
        printf_file_chks_local(_T("Z:\\UNIXfavicon.pdf"));*/
    //
    /*    struct riv_chks rhks;
        RiverFS::ChunkChks(&rhks, _T("Z:\\UNIXfavicon.pdf"), 6);
        ChksMD5(_T("chunk chks"), rhks.md5_chks);
        print_chunk_chks(_T("Z:\\UNIXfavicon.pdf"), 6);*/
    //
    // TCHAR szExistsFile[260];
    // DWORD x = RiverFS::FileIdenti(szExistsFile, &tFileID);
    struct FileID tFileID;
    RiverFS::InitFileID(&tFileID, _T("Z:\\UNIXfavicon.pdf"), TRUE);
    RiverFS::FileAppend(&tFileID);


    CloseFileID(tFileID);
    //
    RiverFS::Finalize();
}
