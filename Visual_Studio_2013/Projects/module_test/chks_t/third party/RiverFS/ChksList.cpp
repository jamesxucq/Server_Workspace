#include "StdAfx.h"

#include "CommonUtility.h"
// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "ChksUtility.h"
#include "ChksList.h"

//

#define MAX_UPDATE_NUM	64
//

CChksList::CChksList(void)
{
}

CChksList::~CChksList(void)
{
}
//
CChksList objChksList;
//
DWORD CChksList::Initialize(const TCHAR *szLocation)
{
    CommonUtility::get_name(m_szChksList, szLocation, FILES_CHKS_DEFAULT);
    m_hChksList = CreateFile( m_szChksList,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                              NULL);
    if( INVALID_HANDLE_VALUE == m_hChksList ) {
        TRACE( _T("create file failed: %d\n"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    return 0;
}

DWORD CChksList::Finalize()
{
    if(INVALID_HANDLE_VALUE != m_hChksList) {
        CloseHandle( m_hChksList );
        m_hChksList = INVALID_HANDLE_VALUE;
    }
    //
    return 0;
}
//
ULONG CChksList::FillEntryList(ULONG lListHinde, DWORD *iListLength, DWORD iNewLength)
{
    // DWORD iOldLength = ChksUtility::ListLength(m_hChksList, lListHinde);
    ULONG lChksHinde = INVA_INDE_VALU;
    if(*iListLength != iNewLength) {
        TRACE(_T("reset list length %u-%u\n"), *iListLength, iNewLength);
        lChksHinde = ChksUtility::ResetListLength(m_hChksList, lListHinde, *iListLength, iNewLength);
        *iListLength = iNewLength;
    } else lChksHinde = lListHinde;
    //
// ChksUtility::ListEntry(m_hChksList, lChksHinde); // for test
    return lChksHinde;
}

ULONG CChksList::InsEntryList(HANDLE hRiveChks, ULONG lListHinde, DWORD *iListLength, FILETIME *ftLastWrite)
{
    DWORD iNewLength;
    if(NULL == hRiveChks) iNewLength =  0x00;
    else iNewLength = GetFileSize(hRiveChks, NULL) / sizeof(struct riv_chks);
    // DWORD iOldLength = ChksUtility::ListLength(m_hChksList, lListHinde);
    ULONG lChksHinde = INVA_INDE_VALU;
    if(*iListLength != iNewLength) {
        TRACE(_T("reset list length %u-%u\n"), *iListLength, iNewLength);
        lChksHinde = ChksUtility::ResetListLength(m_hChksList, lListHinde, *iListLength, iNewLength);
        *iListLength = iNewLength;
    } else lChksHinde = lListHinde;
    //
    if(INVA_INDE_VALU != lChksHinde) {
        SetFilePointer(hRiveChks, 0, NULL, FILE_BEGIN);
        struct riv_chks rchks;
        ULONG lNextIndex = lChksHinde;
        DWORD ind, dwReadSize;
        for(ind = 0; iNewLength > ind; ind++) {
            if(!ReadFile(hRiveChks, &rchks, sizeof(struct riv_chks), &dwReadSize, NULL) || dwReadSize <= 0)
                break;
            lNextIndex = ChksUtility::ModifyNode(m_hChksList, lNextIndex, rchks.md5_chks, ftLastWrite);
        }
    }
    //
// ChksUtility::ListEntry(m_hChksList, lChksHinde); // for test
    return lChksHinde;
}

#define file_info(szFileName) \
    WIN32_FIND_DATA fileInfo; \
    HANDLE hFind; \
    unsigned __int64 qwFileSize = 0; \
    hFind = FindFirstFile(szFileName, &fileInfo); \
    if(INVALID_HANDLE_VALUE != hFind) { \
        qwFileSize = fileInfo.nFileSizeLow; \
        qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32; \
    } \
    FindClose(hFind);

#define cchks_length(new_size) \
        DWORD iNewLength = (DWORD)(new_size >> 22); /* new_size / CHUNK_SIZE; */ \
        if (POW2N_MOD(new_size, CHUNK_SIZE)) iNewLength++;

DWORD CChksList::FileSha1(unsigned char *pSha1Chks, TCHAR *szFileName, ULONG lListHinde, DWORD *iListLength)
{
    DWORD dwRiveValue = 0x00;
    file_info(szFileName)
    cchks_length(qwFileSize)
    if(*iListLength != iNewLength) {
        TRACE(_T("reset list length %u-%u\n"), *iListLength, iNewLength);
        ChksUtility::ResetListLength(m_hChksList, lListHinde, *iListLength, iNewLength);
        *iListLength = iNewLength;
        dwRiveValue = 0x01;
    }
    // DWORD iOldLength = ChksUtility::ListLength(m_hChksList, lListHinde);
    //
    ChksUtility::FileChunkUpdate(m_hChksList, szFileName, lListHinde, 0, &fileInfo);
    ChksUtility::FileSha1Chks(pSha1Chks, m_hChksList, lListHinde);
    // printf("leave rivfs file\n");
    return dwRiveValue;
}

DWORD CChksList::ValidFileChks(TCHAR *szFileName, ULONG lListHinde, DWORD iListLength, DWORD iListPosit)
{
    file_info(szFileName)
    cchks_length(qwFileSize)
    //
    DWORD iValidLength = ChksUtility::ValidChunkUpdate(m_hChksList, lListHinde, iListPosit, &fileInfo.ftLastWriteTime);
    TRACE(_T("iValidLength:%u iNewLength:%u iListLength:%u\n"), iValidLength, iNewLength, iListLength);
    iValidLength += (iNewLength - iListLength);
    // _LOG_TRACE(_T("iValidLength:%u\n"), iValidLength);
    //
    return MAX_UPDATE_NUM < iValidLength;
}

DWORD CChksList::FileChks(HANDLE hRiveChks, TCHAR *szFileName, ULONG lListHinde, DWORD *iListLength, DWORD iListPosit)
{
    DWORD dwRiveValue = 0x00;
    file_info(szFileName)
    cchks_length(qwFileSize)
    if(*iListLength != iNewLength) {
        TRACE(_T("reset list length %u-%u\n"), *iListLength, iNewLength);
        ChksUtility::ResetListLength(m_hChksList, lListHinde, *iListLength, iNewLength);
        *iListLength = iNewLength;
        dwRiveValue = 0x01;
    }
    // DWORD iOldLength = ChksUtility::ListLength(m_hChksList, lListHinde);
// ChksUtility::ChksIdleEntry(m_hChksList);
    //
    ULONG lIndexPosit = ChksUtility::FileChunkUpdate(m_hChksList, szFileName, lListHinde, iListPosit, &fileInfo);
    ChksUtility::FileRiveChks(hRiveChks, m_hChksList, lIndexPosit);
    // printf("leave rivfs file\n");
    return dwRiveValue;
}

DWORD CChksList::ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, ULONG lListHinde, DWORD *iListLength, DWORD iListPosit)
{
    DWORD dwRiveValue = 0x00;
    file_info(szFileName)
    cchks_length(qwFileSize)
    if(*iListLength != iNewLength) {
        TRACE(_T("reset list length %u-%u\n"), *iListLength, iNewLength);
        ChksUtility::ResetListLength(m_hChksList, lListHinde, *iListLength, iNewLength);
        *iListLength = iNewLength;
        dwRiveValue = 0x01;
    }
    // DWORD iOldLength = ChksUtility::ListLength(m_hChksList, lListHinde);
    //
    ULONG lIndexPosit = ChksUtility::ChunkChksUpdate(m_hChksList, szFileName, lListHinde, iListPosit, &fileInfo);
    ChksUtility::ChunkRiveChks(pRiveChks, m_hChksList, lIndexPosit);
    // printf("leave rivfs chunk\n");
    return dwRiveValue;
}

ULONG  CChksList::ChunkLastWrite(ULONG lListHinde, DWORD *iListLength, DWORD iListPosit, FILETIME *ftLastWrite)
{
    DWORD dwRiveValue = 0x00;
    DWORD iNewLength = 1 + iListPosit;
    if(*iListLength < iNewLength) {
        TRACE(_T("reset list length %u-%u\n"), *iListLength, iNewLength);
        ChksUtility::ResetListLength(m_hChksList, lListHinde, *iListLength, iNewLength);
        *iListLength = iNewLength;
        dwRiveValue = 0x01;
    }
// DWORD iOldLength = ChksUtility::ListLength(m_hChksList, lListHinde);
    //
    ULONG lIndexPosit = ChksUtility::ChunkWriteUpdate(m_hChksList, lListHinde, iListPosit, ftLastWrite);
    // printf("leave rivfs chunk\n");
    return dwRiveValue;
}

ULONG  CChksList::ChunkUpdate(ULONG lListHinde, DWORD *iListLength, DWORD iListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite)
{
    DWORD dwRiveValue = 0x00;
    DWORD iNewLength = 1 + iListPosit;
    if(*iListLength < iNewLength) {
        TRACE(_T("reset list length %u-%u\n"), *iListLength, iNewLength);
        ChksUtility::ResetListLength(m_hChksList, lListHinde, *iListLength, iNewLength);
        *iListLength = iNewLength;
        dwRiveValue = 0x01;
    }
// DWORD iOldLength = ChksUtility::ListLength(m_hChksList, lListHinde);
    //
    ULONG lIndexPosit = ChksUtility::ChunkUpdate(m_hChksList, lListHinde, iListPosit, md5_chks, ftLastWrite);
    // printf("leave rivfs chunk\n");
    return dwRiveValue;
}