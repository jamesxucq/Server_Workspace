#include "StdAfx.h"

#include "ChksAddi.h"

#define FILE_ATTRIB_DEFAULT				_T("~\\file_attri.tmp")
#define CHKS_LIST_DEFAULT				_T("~\\chks_list.tmp")

CChksAddi::CChksAddi(void):
m_hFileChks(INVALID_HANDLE_VALUE),
m_hChksList(INVALID_HANDLE_VALUE)
{
}

CChksAddi::~CChksAddi(void) {
}

class CChksAddi objChksAddi;

//
DWORD CChksAddi::Initialize(const TCHAR *szLocation) {
	TCHAR szFileChks[MAX_PATH];
    comutil::get_name(szFileChks, szLocation, FILE_ATTRIB_DEFAULT);
    m_hFileChks = CreateFile( szFileChks,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                              NULL);
    if( INVALID_HANDLE_VALUE == m_hFileChks ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
//
    comutil::get_name(szFileChks, szLocation, CHKS_LIST_DEFAULT);
    m_hChksList = CreateFile( szFileChks,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                              NULL);
    if( INVALID_HANDLE_VALUE == m_hChksList ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
		CloseHandle( m_hFileChks );
        return ((DWORD)-1);
    }
    //
    return 0x00;
}

VOID CChksAddi::Finalize() {
    if(INVALID_HANDLE_VALUE != m_hChksList) {
        CloseHandle( m_hChksList );
        m_hChksList = INVALID_HANDLE_VALUE;
    }
//
    if(INVALID_HANDLE_VALUE != m_hFileChks) {
        CloseHandle( m_hFileChks );
        m_hFileChks = INVALID_HANDLE_VALUE;
    }
    //
}

//
ULONG CChksAddi::AppFileAttri(struct FileID *pFileID) {
	ULONG ulFileInde = INVA_INDE_VALU;
	DWORD dwChksLen;
	CHKS_LENGTH(dwChksLen, pFileID->qwFileSize)
	//
	ULONG ulListHinde = NAddiUtili::AppChksList(m_hChksList, pFileID->hRiveChks, dwChksLen);
	ulFileInde = NAddiUtili::AppFileChks(m_hFileChks, pFileID, ulListHinde, dwChksLen);
	//
	return ulFileInde;
}

DWORD CChksAddi::GetFileAttri(struct FileID *pFileID, ULONG ulFileInde) {
	ULONG ulListHinde = INVA_INDE_VALU;
	DWORD dwChksLen = 0x00;
	//
	pFileID->hRiveChks = ChksUtility::ChksFileHandle();
	if(INVA_INDE_VALU != NAddiUtili::GetFileChks(m_hFileChks, pFileID, &ulListHinde, &dwChksLen, ulFileInde))
		NAddiUtili::GetChksList(m_hChksList, pFileID->hRiveChks, ulListHinde, dwChksLen);
	//
	return 0x00;
}

DWORD CChksAddi::GetFileAttri(unsigned __int64 *qwFileSize, FILETIME *ftCreatTime, unsigned char *szSha1Chks, ULONG ulFileInde) {
    struct FileEnti tFileEnti;
    DWORD slen;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = ulFileInde * sizeof(struct FileEnti);
    if(!ReadFile(m_hFileChks, &tFileEnti, sizeof(struct FileEnti), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return ((DWORD)-1);
    }
	*qwFileSize = tFileEnti.qwFileSize;
	if(ftCreatTime) memcpy(ftCreatTime, &tFileEnti.ftCreatTime, sizeof(FILETIME));
	if(szSha1Chks) memcpy(szSha1Chks, tFileEnti.szFileChks, SHA1_DIGEST_LEN);
	//
	return 0x00;
}


DWORD CChksAddi::GetFileChks(char *szSha1Chks, ULONG ulFileInde) {
    struct FileEnti tFileEnti;
    DWORD slen;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = ulFileInde * sizeof(struct FileEnti);
    if(!ReadFile(m_hFileChks, &tFileEnti, sizeof(struct FileEnti), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return ((DWORD)-1);
    }
	memcpy(szSha1Chks, tFileEnti.szFileChks, SHA1_DIGEST_LEN);
	//
	return 0x00;
}

ULONG CChksAddi::AppFolderAttri(struct FileID *pFolderID) {
	struct FileEnti tFileEnti;
	ULONG ulFileInde = INVA_INDE_VALU;
	//
	_tcscpy_s(tFileEnti.szFileName, MAX_PATH, pFolderID->szFileName);
	memcpy(&tFileEnti.ftCreatTime, &pFolderID->ftCreatTime, sizeof(FILETIME));
	memcpy(&tFileEnti.ftLastWrite, &pFolderID->ftLastWrite, sizeof(FILETIME));
	//
    DWORD dwWritenSize;
    ULONG lIdleOffset = SetFilePointer(m_hFileChks, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
	ulFileInde = lIdleOffset / sizeof(struct FileEnti);
    if(!WriteFile(m_hFileChks, &tFileEnti, sizeof(struct FileEnti), &dwWritenSize, NULL))
        return INVA_INDE_VALU;
    //
	return ulFileInde;
}

DWORD CChksAddi::GetFolderAttri(struct FileID *pFolderID, ULONG ulFolderInde) {
    struct FileEnti tFileEnti;
    DWORD slen;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = ulFolderInde * sizeof(struct FileEnti);
    if(!ReadFile(m_hFileChks, &tFileEnti, sizeof(struct FileEnti), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return ((DWORD)-1);
    }
	//
	_tcscpy_s(pFolderID->szFileName, MAX_PATH, tFileEnti.szFileName);
	memcpy(&pFolderID->ftCreatTime, &tFileEnti.ftCreatTime, sizeof(FILETIME));
	memcpy(&pFolderID->ftLastWrite, &tFileEnti.ftLastWrite, sizeof(FILETIME));
	//
	return 0x00;

}

//
ULONG NAddiUtili::AppChksList(HANDLE hChksList, HANDLE hRiveChks, DWORD dwChksLen) {
	ULONG ulListHinde = INVA_INDE_VALU;
	//
    ULONG ulOffset = SetFilePointer(hChksList, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==ulOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
	ulListHinde = ulOffset / sizeof(struct ChksEnti);
    ulOffset = SetFilePointer(hRiveChks, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==ulOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
	//
	struct ChksEnti ckent;
	struct riv_chks rchks;
    DWORD inde, dwWritenSize, dwReadSize;
    for(inde = 0x00; dwChksLen > inde; inde++) {
        if(!ReadFile(hRiveChks, &rchks, sizeof(struct riv_chks), &dwReadSize, NULL) || !dwReadSize)
            break;
		ckent.offset = rchks.offset;
		memcpy(ckent.md5_chks, rchks.md5_chks, MD5_DIGEST_LEN);
		if(!WriteFile(hChksList, &ckent, sizeof(struct ChksEnti), &dwWritenSize, NULL))
			break;
    }
//
	return ulListHinde;
}

ULONG NAddiUtili::AppFileChks(HANDLE hFileChks, struct FileID *pFileID, ULONG ulListHinde, DWORD dwChksLen) {
	struct FileEnti tFileEnti;
	ULONG ulFileInde = INVA_INDE_VALU;
	//
	_tcscpy_s(tFileEnti.szFileName, MAX_PATH, pFileID->szFileName);
	tFileEnti.qwFileSize = pFileID->qwFileSize;
	memcpy(&tFileEnti.ftCreatTime, &pFileID->ftCreatTime, sizeof(FILETIME));
	memcpy(&tFileEnti.ftLastWrite, &pFileID->ftLastWrite, sizeof(FILETIME));
	memcpy(tFileEnti.szFileChks, &pFileID->szFileChks, SHA1_DIGEST_LEN);
	tFileEnti.list_hinde = ulListHinde;
	tFileEnti.chkslen = dwChksLen;
	//
    DWORD dwWritenSize;
    ULONG lIdleOffset = SetFilePointer(hFileChks, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
	ulFileInde = lIdleOffset / sizeof(struct FileEnti);
    if(!WriteFile(hFileChks, &tFileEnti, sizeof(struct FileEnti), &dwWritenSize, NULL))
        return INVA_INDE_VALU;
    //
	return ulFileInde;
}

DWORD NAddiUtili::GetChksList(HANDLE hChksList, HANDLE hRiveChks, ULONG ulListHinde, DWORD dwChksLen) {
	ULONG ulChksOffset = ulListHinde * sizeof(struct ChksEnti);
	ULONG ulOffset = SetFilePointer(hChksList, ulChksOffset, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==ulOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    ulOffset = SetFilePointer(hRiveChks, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==ulOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
//
	struct ChksEnti ckent;
	struct riv_chks rchks;
    DWORD inde, dwWritenSize, dwReadSize;
    for(inde = 0x00; dwChksLen > inde; inde++) {
        if(!ReadFile(hChksList, &ckent, sizeof(struct ChksEnti), &dwReadSize, NULL) || !dwReadSize)
            break;
		rchks.offset = ckent.offset;
		memcpy(rchks.md5_chks, ckent.md5_chks, MD5_DIGEST_LEN);
		if(!WriteFile(hRiveChks, &rchks, sizeof(struct riv_chks), &dwWritenSize, NULL))
			break;
    }
	//
	return 0x00;
}

DWORD NAddiUtili::GetFileChks(HANDLE hFileChks, struct FileID *pFileID, ULONG *ulListHinde, DWORD *dwChksLen, ULONG ulFileInde) {
    struct FileEnti tFileEnti;
    DWORD slen;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = ulFileInde * sizeof(struct FileEnti);
    if(!ReadFile(hFileChks, &tFileEnti, sizeof(struct FileEnti), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return ((DWORD)-1);
    }
	//
	_tcscpy_s(pFileID->szFileName, MAX_PATH, tFileEnti.szFileName);
	pFileID->qwFileSize = tFileEnti.qwFileSize;
	memcpy(&pFileID->ftCreatTime, &tFileEnti.ftCreatTime, sizeof(FILETIME));
	memcpy(&pFileID->ftLastWrite, &tFileEnti.ftLastWrite, sizeof(FILETIME));
	memcpy(&pFileID->szFileChks, tFileEnti.szFileChks, SHA1_DIGEST_LEN);
	*ulListHinde = tFileEnti.list_hinde;
	*dwChksLen = tFileEnti.chkslen;
	//
	return 0x00;
}
