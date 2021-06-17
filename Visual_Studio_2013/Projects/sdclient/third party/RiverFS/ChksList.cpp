#include "StdAfx.h"

#include "CommonUtility.h"
// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "ChksUtility.h"
#include "ChksList.h"

//

CChksList::CChksList(void):
    m_hChksList(INVALID_HANDLE_VALUE)
{
    MKZEO(m_szChksList);
}

CChksList::~CChksList(void)
{
}
//
CChksList objChksList;
//
DWORD CChksList::Initialize(const TCHAR *szLocation) {
    comutil::get_name(m_szChksList, szLocation, FILES_CHKS_DEFAULT);
    m_hChksList = CreateFile( m_szChksList,
                              GENERIC_WRITE | GENERIC_READ,
                              /*NULL,*/ FILE_SHARE_READ, // share test 
                              NULL,
                              OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                              NULL);
    if( INVALID_HANDLE_VALUE == m_hChksList ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    return 0x00;
}

DWORD CChksList::Finalize() {
    if(INVALID_HANDLE_VALUE != m_hChksList) {
        CloseHandle( m_hChksList );
        m_hChksList = INVALID_HANDLE_VALUE;
    }
    //
    return 0x00;
}
//
ULONG CChksList::FillEntryList(ULONG lListHinde, DWORD *dwListLength, DWORD dwNewLength) {
    // DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
    ULONG lChksHinde = INVA_INDE_VALU;
    if(*dwListLength != dwNewLength) {
_LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength); // disable by james 20140115
        lChksHinde = ChksUtility::ResetListLength(m_hChksList, lListHinde, *dwListLength, dwNewLength);
        *dwListLength = dwNewLength;
    } else lChksHinde = lListHinde;
    //
// ChksUtility::ListEntry(m_hChksList, lChksHinde); // for test
    return lChksHinde;
}

ULONG CChksList::InsEntryList(HANDLE hRiveChks, ULONG lListHinde, DWORD dwListLength, DWORD dwNewLength, FILETIME *ftLastWrite) {
    // DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
    ULONG lChksHinde = INVA_INDE_VALU;
    if(dwListLength != dwNewLength) {
// _LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength); // disable by james 20140115
        lChksHinde = ChksUtility::ResetListLength(m_hChksList, lListHinde, dwListLength, dwNewLength);
    } else lChksHinde = lListHinde;
    //
    if(INVA_INDE_VALU != lChksHinde) {
        SetFilePointer(hRiveChks, 0, NULL, FILE_BEGIN);
        struct riv_chks rchks;
        ULONG lNextInde = lChksHinde;
        DWORD inde, dwReadSize;
        for(inde = 0; dwNewLength > inde; inde++) {
            if(!ReadFile(hRiveChks, &rchks, sizeof(struct riv_chks), &dwReadSize, NULL) || !dwReadSize)
                break;
            lNextInde = ChksUtility::ModifyNode(m_hChksList, lNextInde, rchks.md5_chks, ftLastWrite);
        }
    }
    //
// ChksUtility::ListEntry(m_hChksList, lChksHinde); // for test
    return lChksHinde;
}

ULONG CChksList::EntryListCopy(ULONG lListHinde, DWORD dwNewLength) {
    ULONG lChksHinde = INVA_INDE_VALU;
// _LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength); // disable by james 20140115
    lChksHinde = ChksUtility::ResetListLength(m_hChksList, INVA_INDE_VALU, 0x00, dwNewLength);
	ChksUtility::FileChksCopy(m_hChksList, lChksHinde, lListHinde);
    //
// ChksUtility::ListEntry(m_hChksList, lChksHinde); // for test
    return lChksHinde;
}

#define FILE_SIZE_INFO(szFileName) \
    WIN32_FIND_DATA fileInfo; \
    HANDLE hFind; \
    unsigned __int64 qwFileSize = 0x00; \
    hFind = FindFirstFile(szFileName, &fileInfo); \
    if(INVALID_HANDLE_VALUE != hFind) { \
        qwFileSize = fileInfo.nFileSizeLow; \
        qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32; \
    } \
    FindClose(hFind);

DWORD CChksList::FileSha1Relay(unsigned char *szSha1Chks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength) {
    DWORD dwRiveValue = 0x00;
    FILE_SIZE_INFO(szFileName)
	DWORD dwNewLength;
	CHKS_LENGTH(dwNewLength, qwFileSize)
    if(*dwListLength != dwNewLength) {
_LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength);
        *lListHinde = ChksUtility::ResetListLength(m_hChksList, *lListHinde, *dwListLength, dwNewLength);
        *dwListLength = dwNewLength;
        dwRiveValue |= 0x01;
    }
    // DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
	switch(ChksUtility::FileChksRelay(m_hChksList, szFileName, *lListHinde, &fileInfo)) {
    case 0x00:
        ChksUtility::FileSha1Chks(szSha1Chks, m_hChksList, *lListHinde);
        break;
    case 0x01:
		dwRiveValue |= 0x01;
        ChksUtility::FileSha1Chks(szSha1Chks, m_hChksList, *lListHinde);
        break;
    case 0x02:
        dwRiveValue |= 0x02;
        break;
    case ((DWORD)-1):
		ChksUtility::FileSha1Chks(szSha1Chks, m_hChksList, *lListHinde);
        dwRiveValue |= 0x04;
        break;
	}
_LOG_DEBUG(_T("file sha1 relay, dwRiveValue:%d"), dwRiveValue);
    return dwRiveValue;
}

DWORD CChksList::FileSha1Update(unsigned char *szSha1Chks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength) {
    DWORD dwRiveValue = 0x00;
    FILE_SIZE_INFO(szFileName)
	DWORD dwNewLength;
	CHKS_LENGTH(dwNewLength, qwFileSize)
    if(*dwListLength != dwNewLength) {
_LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength);
        *lListHinde = ChksUtility::ResetListLength(m_hChksList, *lListHinde, *dwListLength, dwNewLength);
        *dwListLength = dwNewLength;
        dwRiveValue |= 0x01;
    }
    // DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
	switch(ChksUtility::FileChksUpdate(m_hChksList, szFileName, *lListHinde, &fileInfo)) {
    case 0x00:
        ChksUtility::FileSha1Chks(szSha1Chks, m_hChksList, *lListHinde);
        break;
    case 0x01:
		dwRiveValue |= 0x01;
        ChksUtility::FileSha1Chks(szSha1Chks, m_hChksList, *lListHinde);
        break;
    case ((DWORD)-1):
		ChksUtility::FileSha1Chks(szSha1Chks, m_hChksList, *lListHinde);
        dwRiveValue |= 0x02;
        break;
	}
_LOG_DEBUG(_T("file sha1 update, dwRiveValue:%d"), dwRiveValue);
    return dwRiveValue;
}

DWORD CChksList::FileSha1Chks(unsigned char *szSha1Chks, ULONG lListHinde) {
_LOG_DEBUG(_T("create file sha1, lListHinde:%lu"), lListHinde);
	ChksUtility::FileSha1Chks(szSha1Chks, m_hChksList, lListHinde);
	return 0x00;
}

DWORD CChksList::FileChksRelay(HANDLE hRiveChks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit) {
    DWORD dwRiveValue = 0x00;
	//
    FILE_SIZE_INFO(szFileName)
	DWORD dwNewLength;
	CHKS_LENGTH(dwNewLength, qwFileSize)
_LOG_DEBUG(_T("reset list length %u-%u lListHinde:%lu"), *dwListLength, dwNewLength, *lListHinde); // disable by james 20140115
    if(*dwListLength != dwNewLength) {
        *lListHinde = ChksUtility::ResetListLength(m_hChksList, *lListHinde, *dwListLength, dwNewLength);
        *dwListLength = dwNewLength;
        dwRiveValue |= 0x01;
_LOG_DEBUG(_T("reset list length %u-%u lNewHinde:%lu"), *dwListLength, dwNewLength, *lListHinde);
    }
// DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
// ChksUtility::ChksIdleEntry(m_hChksList); // for test
    ULONG lIndePosit;
    switch(ChksUtility::FileChunkRelay(m_hChksList, &lIndePosit, szFileName, *lListHinde, dwListPosit, &fileInfo)) {
    case 0x00:
        ChksUtility::FileRiveChks(hRiveChks, m_hChksList, lIndePosit);
        break;
    case 0x01:
        dwRiveValue |= 0x01;
        ChksUtility::FileRiveChks(hRiveChks, m_hChksList, lIndePosit);
        break;
    case 0x02:
        dwRiveValue |= 0x02;
        break;
    case ((DWORD)-1):
        dwRiveValue |= 0x04;
        break;
    }
_LOG_DEBUG(_T("leave rivfs file, dwRiveValue:%d"), dwRiveValue);
    return dwRiveValue;
}

DWORD CChksList::ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit) {
    DWORD dwRiveValue = 0x00;
    FILE_SIZE_INFO(szFileName)
	DWORD dwNewLength;
	CHKS_LENGTH(dwNewLength, qwFileSize)
    if(*dwListLength != dwNewLength) {
_LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength);
        *lListHinde = ChksUtility::ResetListLength(m_hChksList, *lListHinde, *dwListLength, dwNewLength);
        *dwListLength = dwNewLength;
        dwRiveValue = 0x01;
    }
    // DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
    //
    ULONG lIndePosit = ChksUtility::ChunkChksUpdate(m_hChksList, szFileName, *lListHinde, dwListPosit, &fileInfo);
    ChksUtility::ChunkRiveChks(pRiveChks, m_hChksList, lIndePosit);
    // fprintf(stderr, "leave rivfs chunk");
    return dwRiveValue;
}

ULONG  CChksList::ChunkLawiUpdate(ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit, DWORD chunk_length, FILETIME *ftLastWrite) {
    DWORD dwRiveValue = 0x00;
    DWORD dwNewLength = dwListPosit;
	if(chunk_length) dwNewLength++;
    if(*dwListLength < dwNewLength) {
_LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength);
        *lListHinde = ChksUtility::ResetListLength(m_hChksList, *lListHinde, *dwListLength, dwNewLength);
        *dwListLength = dwNewLength;
        dwRiveValue = 0x01;
    }
// DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
    //
    ULONG lIndePosit = ChksUtility::ChunkLawiUpdate(m_hChksList, *lListHinde, dwListPosit, ftLastWrite);
    // fprintf(stderr, "leave rivfs chunk");
    return dwRiveValue;
}

ULONG  CChksList::ChunkUpdate(ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit, DWORD chunk_length, unsigned char *md5_chks, FILETIME *ftLastWrite) {
    DWORD dwRiveValue = 0x00;
    DWORD dwNewLength = dwListPosit;
	if(chunk_length) dwNewLength++;
    if(*dwListLength < dwNewLength) {
_LOG_DEBUG(_T("reset list length %u-%u"), *dwListLength, dwNewLength);
        *lListHinde = ChksUtility::ResetListLength(m_hChksList, *lListHinde, *dwListLength, dwNewLength);
        *dwListLength = dwNewLength;
        dwRiveValue = 0x01;
    }
// DWORD dwOldLength = ChksUtility::ListLength(m_hChksList, lListHinde); // for test
    //
    ULONG lIndePosit = ChksUtility::ChunkUpdate(m_hChksList, *lListHinde, dwListPosit, md5_chks, ftLastWrite);
    // fprintf(stderr, "leave rivfs chunk");
    return dwRiveValue;
}