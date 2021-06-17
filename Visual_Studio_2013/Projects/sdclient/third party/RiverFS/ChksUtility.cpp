#include "StdAfx.h"

#include "CommonUtility.h"
#include "Sha1.h"
// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "ChksList.h"
#include "ChksUtility.h"

//
#define MAX_UPDATE_NUM	256

//
#define CREAT_TEMP_FILE(FILE_NAME) { \
    static TCHAR szTempPath[MAX_PATH]; \
    if(_T(':') != szTempPath[1]) \
		GetTempPath(MAX_PATH, szTempPath); \
    GetTempFileName(szTempPath, _T("sdclient"), 0, FILE_NAME); \
}

HANDLE ChksUtility::ChksFileHandle() {
    TCHAR szFileName[MAX_PATH];
    //
	CREAT_TEMP_FILE(szFileName)
    HANDLE hFileChks = CreateFile( szFileName,
                                   GENERIC_WRITE | GENERIC_READ,
                                   NULL, /* FILE_SHARE_READ */
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                   NULL);
    if( INVALID_HANDLE_VALUE == hFileChks ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return INVALID_HANDLE_VALUE;
    }
    //
    return hFileChks;
}

VOID ChksMD5(TCHAR *label, unsigned char *md5sum) {
    TCHAR chksum_str[64];
    for (int inde=0; inde<16; inde++)
        _stprintf_s (chksum_str+inde*2, 33, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
_LOG_DEBUG(_T("%s:%s"), label, chksum_str);
}

HANDLE ChksUtility::CreatChksFile(HANDLE hFileRiver, const TCHAR *szFileName) {
    HANDLE hFileChks = CreateFile( szFileName,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if( INVALID_HANDLE_VALUE == hFileChks ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return INVALID_HANDLE_VALUE;
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileChks, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError()) {
        CloseHandle( hFileChks );
        return INVALID_HANDLE_VALUE;
    }
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
	unsigned int chks_tatol;
	CHKS_LENGTH(chks_tatol, qwFileSize)
    //
    struct riv_chks rchks;
    DWORD wlen;
    for (unsigned int inde = 0; inde < chks_tatol; inde++) {
        rchks.offset = ((unsigned __int64)inde) << 22;
        comutil::chunk_chks(rchks.md5_chks, hFileChks, qwFileSize, rchks.offset);
// _LOG_DEBUG(_T("rchks.offset: %llu chunk:%d"), rchks.offset, rchks.offset>>22);
// ChksMD5(_T("rchks.md5_chks"), rchks.md5_chks);
        if(!WriteFile(hFileRiver, &rchks, sizeof(struct riv_chks), &wlen, NULL))
            break;
    }
    //
    if(INVALID_HANDLE_VALUE != hFileChks) {
        CloseHandle( hFileChks );
        hFileChks = INVALID_HANDLE_VALUE;
    }
    //
    return hFileRiver;
}

VOID ChksUtility::FileSha1Chks(unsigned char *sha1_chks, HANDLE hRiveChks) {
    DWORD dwResult = SetFilePointer(hRiveChks, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return;
    //
    struct riv_chks rchks;
    sha1_ctx cx[1];
    DWORD dwReadSize;
    //
    sha1_begin(cx);
    while(ReadFile(hRiveChks, &rchks, sizeof(struct riv_chks), &dwReadSize, NULL) && 0<dwReadSize) {
        sha1_hash(rchks.md5_chks, MD5_DIGEST_LEN, cx);
    }
// ChksMD5(_T("1 sha1_chks"), sha1_chks);
    sha1_end(sha1_chks, cx);
// ChksMD5(_T("2 sha1_chks"), sha1_chks);
}

VOID ChksUtility::FileSha1Chks(unsigned char *sha1_chks, HANDLE hChksList, ULONG lListInde) {
    struct ChksEntry tChksEntry;
    sha1_ctx cx[1];
    //
	tChksEntry.sibling = lListInde;
	sha1_begin(cx);
	while (INVA_INDE_VALU != tChksEntry.sibling) {
_LOG_DEBUG(_T("read chks lListPosit:%u"), tChksEntry.sibling); // disable by james 20140115
		ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
		sha1_hash(tChksEntry.szFileChks, MD5_DIGEST_LEN, cx);
	}
// ChksMD5(_T("1 sha1_chks"), sha1_chks);
	sha1_end(sha1_chks, cx);
// ChksMD5(_T("2 sha1_chks"), sha1_chks);
}

//
DWORD ChksUtility::FillIdleEntry(HANDLE hChksList) {
    struct ChksEntry tChksEntry;
    DWORD wlen;
    //
    INIT_CHKS_ENTRY(tChksEntry)
    ULONG lIdleOffset = SetFilePointer(hChksList, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!WriteFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &wlen, NULL))
        return ((DWORD)-1);
    //
    return 0x00;
}

ULONG ChksUtility::FindIdleEntry(HANDLE hChksList) {
    struct ChksEntry tFillEntry, tChksEntry;
    ULONG lIdleOffset, lIdleInde = INVA_INDE_VALU;
    DWORD slen, wlen;
    //
    lIdleOffset = SetFilePointer(hChksList, -(LONG)sizeof(struct ChksEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        lIdleInde = tFillEntry.recycled;
        if(INVA_INDE_VALU == ReadNode(hChksList, &tChksEntry, lIdleInde))
            return INVA_INDE_VALU;
// _LOG_DEBUG(_T("idle entry:%u sibling:%u recycled:%u"), lIdleInde, tChksEntry.sibling, tChksEntry.recycled);
        if(INVA_INDE_VALU != tChksEntry.sibling) {
            tFillEntry.recycled = tChksEntry.sibling;
            ModifyNode(hChksList, tChksEntry.sibling, LIST_INDE_RECYCLED, tChksEntry.recycled);
			tChksEntry.sibling = INVA_INDE_VALU;
        } else tFillEntry.recycled = tChksEntry.recycled;
        //
        memset(&tChksEntry.ftLastWrite, 0, sizeof(FILETIME));
        if(INVA_INDE_VALU == WriteNode(hChksList, &tChksEntry, lIdleInde))
            return INVA_INDE_VALU;
        //
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        OverLapped.Offset = lIdleOffset;
        if(!WriteFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
            return INVA_INDE_VALU;
    } else {
        if(FillIdleEntry(hChksList)) return INVA_INDE_VALU;
        lIdleInde = lIdleOffset / sizeof(struct ChksEntry);
    }
    //
    return lIdleInde;
}

ULONG ChksUtility::AddIdleEntry(HANDLE hChksList, ULONG lListInde) {
    struct ChksEntry tFillEntry;
    DWORD slen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(hChksList, -(LONG)sizeof(struct ChksEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    ModifyNode(hChksList, lListInde, LIST_INDE_RECYCLED, tFillEntry.recycled);
    //
    tFillEntry.recycled = lListInde;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    // FlushFileBuffers(hChksList);
    return lListInde;
}

ULONG ChksUtility::ReadNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListInde * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, pChksEntry, sizeof(struct ChksEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lListInde;
}

ULONG ChksUtility::WriteNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListInde) {
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListInde * sizeof(struct ChksEntry);
    if(!WriteFile(hChksList, pChksEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lListInde;
}

ULONG ChksUtility::ModifyNode(HANDLE hChksList, ULONG lListInde, DWORD dwIndeType, ULONG lNextInde) {
    DWORD slen, wlen;
    struct ChksEntry tChksEntry;
    ULONG lExistInde = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListInde * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    switch(dwIndeType) {
    case LIST_INDE_SIBLING:
        lExistInde = tChksEntry.sibling;
        tChksEntry.sibling = lNextInde;
        break;
    case LIST_INDE_RECYCLED:
        lExistInde = tChksEntry.recycled;
        tChksEntry.recycled = lNextInde;
        break;
    }
    //
    if(!WriteFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistInde;
}

DWORD ChksUtility::ListLength(HANDLE hChksList, ULONG lListHinde) { // for test
    struct ChksEntry tChksEntry;
    unsigned int inde;
    //
// _LOG_DEBUG(_T("list length alloc block:"));
    tChksEntry.sibling = lListHinde;
    for(inde = 0; INVA_INDE_VALU != tChksEntry.sibling; inde++) {
// _LOG_DEBUG(_T(" %u"), tChksEntry.sibling);
        ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
    }
    //
    return inde;
}

DWORD ChksUtility::ListEntry(HANDLE hChksList, ULONG lListHinde) { // for test
    struct ChksEntry tChksEntry;
    unsigned int inde;
    //
// _LOG_DEBUG(_T("block info:"));
    tChksEntry.sibling = lListHinde;
    for(inde = 0; INVA_INDE_VALU != tChksEntry.sibling; inde++) {
// _LOG_DEBUG(_T(" inde:%u"), tChksEntry.sibling);
        ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
// _LOG_DEBUG(_T(" last_write:%lld"), tChksEntry.ftLastWrite);
    }
    //
    return inde;
}

VOID ChksUtility::ChksIdleEntry(HANDLE hChksList) { // for test
    struct ChksEntry tFillEntry, tChksEntry;
    ULONG lIdleOffset, lIdleInde = INVA_INDE_VALU;
    DWORD slen;
    //
    lIdleOffset = SetFilePointer(hChksList, -(LONG)sizeof(struct ChksEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ;
    if(!ReadFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return ;
    }
    //
// _LOG_DEBUG(_T("idle list alloc block:"));
    ULONG lRecyclInde;
    for(lRecyclInde = tFillEntry.recycled; INVA_INDE_VALU != ReadNode(hChksList, &tChksEntry, lRecyclInde); ) {
// _LOG_DEBUG(_T(" r:%u s:%u"), lRecyclInde, tChksEntry.sibling);
        lRecyclInde = tChksEntry.recycled;
        for(; INVA_INDE_VALU != ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);) {
            if(INVA_INDE_VALU != tChksEntry.sibling) {
// _LOG_DEBUG(_T(" s:%u"), tChksEntry.sibling);
            }
        }
    }
}

ULONG FindSibliEntry(HANDLE hChksList, ULONG lListInde) {
    DWORD slen;
    struct ChksEntry tChksEntry;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListInde * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return tChksEntry.sibling;
}

ULONG ChksUtility::ResetListLength(HANDLE hChksList, ULONG lListHinde, DWORD dwOldLength, DWORD dwNewLength) {
    int diff_clen = dwNewLength - dwOldLength;
    int inde;
    ULONG eind, nind, sibli;
    //
// _LOG_DEBUG(_T("dwOldLength:%u dwNewLength:%u diff_clen:%d"), dwOldLength, dwNewLength, diff_clen);
    if(0 < diff_clen) { // make long than old
_LOG_DEBUG(_T("make long than old"));
        for (nind = lListHinde; INVA_INDE_VALU != (sibli = FindSibliEntry(hChksList, nind)); nind = sibli);
// _LOG_DEBUG(_T("nind:%lu sibli:%lu"), nind, sibli);
        //
        eind = FindIdleEntry(hChksList);
        if (INVA_INDE_VALU == nind) lListHinde = eind;
        ModifyNode(hChksList, nind, LIST_INDE_SIBLING, eind);
// _LOG_DEBUG(_T("nind:%lu eind:%lu"), nind, eind);
        nind = eind;
        //
        for (inde = 0x01; diff_clen > inde; inde++) {
            eind = FindIdleEntry(hChksList);
            // fprintf(stderr, "nind:%llu eind:%llu\n", nind, eind);
            ModifyNode(hChksList, nind, LIST_INDE_SIBLING, eind);
// _LOG_DEBUG(_T("nind:%lu eind:%lu"), nind, eind);
            nind = eind;
        }
    } else if(0 > diff_clen) { // make short
_LOG_DEBUG(_T("make short than old"));
        nind = INVA_INDE_VALU;
        for (sibli = lListHinde, inde = 0x00; (int)dwNewLength > inde; inde++) {
            nind = sibli;
            sibli = FindSibliEntry(hChksList, sibli);
// _LOG_DEBUG(_T("inde:%d nind:%lu sibli:%lu"), inde, nind, sibli);
        }
        AddIdleEntry(hChksList, sibli);
        if(INVA_INDE_VALU == nind) lListHinde = INVA_INDE_VALU;
        else ModifyNode(hChksList, nind, LIST_INDE_SIBLING, INVA_INDE_VALU);
    }
    //
    return lListHinde;
}

ULONG ChksUtility::ModifyNode(HANDLE hChksList, ULONG lListInde, unsigned char *md5_chks, FILETIME *ftLastWrite) {
    DWORD slen, wlen;
    struct ChksEntry tChksEntry;
    ULONG lNextInde = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListInde * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    lNextInde = tChksEntry.sibling;
    memcpy(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
    memcpy(tChksEntry.szFileChks, md5_chks, MD5_DIGEST_LEN);
    //
    if(!WriteFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lNextInde;
}

//
/*
DWORD ChksUtility::ValidChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD dwListPosit, FILETIME *ftLastWrite) {
    struct ChksEntry tChksEntry;
    ULONG lListInde;
    DWORD inde;
    //
    if (INVA_INDE_VALU == lListHinde) return 0x00;
    tChksEntry.sibling = lListHinde;
    for (inde = 0x00; dwListPosit > inde; inde++) {
// _LOG_DEBUG(_T("--- tChksEntry.sibling:%u"), tChksEntry.sibling); // disable by james 20140115
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListInde) return 0x00;
    }
    //
    DWORD dwValidTatol = 0x00;
    while(INVA_INDE_VALU != tChksEntry.sibling) {
// _LOG_DEBUG(_T("+++ tChksEntry.sibling:%u"), tChksEntry.sibling); // disable by james 20140115
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (memcmp(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME))) {
            ++dwValidTatol;
        }
// _LOG_DEBUG(_T("dwValidTatol:%u"), dwValidTatol); // disable by james 20140115
    }
    //
    return dwValidTatol;
}
*/

DWORD ChksUtility::FileChksRelay(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, WIN32_FIND_DATA *fileInfo) {
    struct ChksEntry tChksEntry;
    ULONG lListInde;
    DWORD inde, update_tatol = 0x00;
    //
    if (INVA_INDE_VALU == lListHinde) return ((DWORD)-1);
    HANDLE hChunk = CreateFile( szFilePath,
                                /* GENERIC_WRITE | */ GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if( INVALID_HANDLE_VALUE == hChunk ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    unsigned __int64 qwFileSize = fileInfo->nFileSizeLow;
    qwFileSize += ((unsigned __int64)fileInfo->nFileSizeHigh) << 32;
    tChksEntry.sibling = lListHinde;
    for (inde = 0; INVA_INDE_VALU != tChksEntry.sibling; inde++) {
        if(MAX_UPDATE_NUM < update_tatol) {
            if(INVALID_HANDLE_VALUE != hChunk) {
                CloseHandle( hChunk );
                hChunk = INVALID_HANDLE_VALUE;
            }
            return 0x02;
        }
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (memcmp(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME))) {
            update_tatol++;
_LOG_DEBUG(_T("1 update chunk chks. lListInde:%u"), lListInde);
            comutil::chunk_chks(tChksEntry.szFileChks, hChunk, qwFileSize, ((unsigned __int64)inde) << 22);
			memcpy(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME));
            WriteNode(hChksList, &tChksEntry, lListInde);
            // fprintf(stderr, "update chks, lListInde:%llu tChksEntry.time_stamp:%ld\n", lListInde, tChksEntry.time_stamp);
        }
    }
    //
    if(INVALID_HANDLE_VALUE != hChunk) {
        CloseHandle( hChunk );
        hChunk = INVALID_HANDLE_VALUE;
    }
	return update_tatol? 0x01: 0x00;
}

DWORD ChksUtility::FileChksUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, WIN32_FIND_DATA *fileInfo) {
    struct ChksEntry tChksEntry;
    ULONG lListInde;
    DWORD inde, update_tatol = 0x00;
    //
    if (INVA_INDE_VALU == lListHinde) return ((DWORD)-1);
    HANDLE hChunk = CreateFile( szFilePath,
                                /* GENERIC_WRITE | */ GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if( INVALID_HANDLE_VALUE == hChunk ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    unsigned __int64 qwFileSize = fileInfo->nFileSizeLow;
    qwFileSize += ((unsigned __int64)fileInfo->nFileSizeHigh) << 32;
    tChksEntry.sibling = lListHinde;
    for (inde = 0; INVA_INDE_VALU != tChksEntry.sibling; inde++) {
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (memcmp(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME))) {
            update_tatol++;
_LOG_DEBUG(_T("1 update chunk chks. lListInde:%u"), lListInde);
            comutil::chunk_chks(tChksEntry.szFileChks, hChunk, qwFileSize, ((unsigned __int64)inde) << 22);
			memcpy(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME));
            WriteNode(hChksList, &tChksEntry, lListInde);
            // fprintf(stderr, "update chks, lListInde:%llu tChksEntry.time_stamp:%ld\n", lListInde, tChksEntry.time_stamp);
        }
    }
    //
    if(INVALID_HANDLE_VALUE != hChunk) {
        CloseHandle( hChunk );
        hChunk = INVALID_HANDLE_VALUE;
    }
	return update_tatol? 0x01: 0x00;
}

DWORD ChksUtility::FileChunkRelay(HANDLE hChksList, ULONG *lIndePosit, const TCHAR *szFilePath, ULONG lListHinde, DWORD dwListPosit, WIN32_FIND_DATA *fileInfo) {
    struct ChksEntry tChksEntry;
    ULONG lListInde;
    DWORD inde, update_tatol = 0x00;
    //
_LOG_DEBUG(_T("file chunk update. lListHinde:%lu"), lListHinde);
    if (INVA_INDE_VALU == lListHinde) return ((DWORD)-1);
    tChksEntry.sibling = lListHinde;
    for (inde = 0; dwListPosit > inde; inde++) {
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListInde) return ((DWORD)-1);
    }
    //
    HANDLE hChunk = CreateFile( szFilePath,
                                /* GENERIC_WRITE | */ GENERIC_READ,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if( INVALID_HANDLE_VALUE == hChunk ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    unsigned __int64 qwFileSize = fileInfo->nFileSizeLow;
    qwFileSize += ((unsigned __int64)fileInfo->nFileSizeHigh) << 32;
    if(lIndePosit) *lIndePosit = tChksEntry.sibling;
    for (; INVA_INDE_VALU != tChksEntry.sibling; inde++) {
        if(MAX_UPDATE_NUM < update_tatol) {
            if(INVALID_HANDLE_VALUE != hChunk) {
                CloseHandle( hChunk );
                hChunk = INVALID_HANDLE_VALUE;
            }
            return 0x02;
        }
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (memcmp(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME))) {
            update_tatol++;
_LOG_DEBUG(_T("2 update chunk chks. lListInde:%u"), lListInde);
            comutil::chunk_chks(tChksEntry.szFileChks, hChunk, qwFileSize, ((unsigned __int64)inde) << 22);
			memcpy(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME));
            WriteNode(hChksList, &tChksEntry, lListInde);
// _LOG_DEBUG(_T("update chks, lListInde:%lu tChksEntry.time_stamp:%ld\n"), lListInde, tChksEntry.time_stamp);
        }
    }
    //
    if(INVALID_HANDLE_VALUE != hChunk) {
        CloseHandle( hChunk );
        hChunk = INVALID_HANDLE_VALUE;
    }
	return update_tatol? 0x01: 0x00;
}

DWORD ChksUtility::FileRiveChks(HANDLE hRiveChks, HANDLE hChksList, ULONG lListPosit) {
    struct ChksEntry tChksEntry;
    struct riv_chks rchks;
    DWORD wlen, inde;
    //
    tChksEntry.sibling = lListPosit;
    for (inde = 0x00; INVA_INDE_VALU != tChksEntry.sibling; inde++) {
// _LOG_DEBUG(_T("read chks lListPosit:%u"), tChksEntry.sibling);
        if (INVA_INDE_VALU == ReadNode(hChksList, &tChksEntry, tChksEntry.sibling))
            return ((DWORD)-1);
        memcpy(rchks.md5_chks, tChksEntry.szFileChks, MD5_DIGEST_LEN);
        rchks.offset = ((unsigned __int64)inde) << 22;
        // memcpy(&pRiveChks[inde], &rchks, sizeof(struct riv_chks));
        if(!WriteFile(hRiveChks, &rchks, sizeof(struct riv_chks), &wlen, NULL))
            return ((DWORD)-1);
    }
    return 0x00;
}

ULONG ChksUtility::ChunkChksUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, DWORD dwListPosit, WIN32_FIND_DATA *fileInfo) {
    struct ChksEntry tChksEntry;
    ULONG lListInde;
    DWORD inde;
    //
    if (INVA_INDE_VALU == lListHinde) return INVA_INDE_VALU;
    tChksEntry.sibling = lListHinde;
    for (inde = 0x00; dwListPosit >= inde; inde++) {
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListInde) return INVA_INDE_VALU;
    }
    //
    if (memcmp(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME))) {
// _LOG_DEBUG(_T("update chunk chks. dwListPosit:%u"), dwListPosit);
        HANDLE hChunk = CreateFile( szFilePath,
                                    /* GENERIC_WRITE | */ GENERIC_READ,
                                    NULL, /* FILE_SHARE_READ */
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
        if( INVALID_HANDLE_VALUE == hChunk ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
            return INVA_INDE_VALU;
        }
        //
        unsigned __int64 qwFileSize = fileInfo->nFileSizeLow;
        qwFileSize += ((unsigned __int64)fileInfo->nFileSizeHigh) << 32;
        comutil::chunk_chks(tChksEntry.szFileChks, hChunk, qwFileSize, ((unsigned __int64)dwListPosit) << 22);
		memcpy(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME));
        WriteNode(hChksList, &tChksEntry, lListInde);
        // fprintf(stderr, "update chks, lListPosit:%u tChksEntry.time_stamp:%ld\n", lListPosit, tChksEntry.time_stamp);
        if(INVALID_HANDLE_VALUE != hChunk) {
            CloseHandle( hChunk );
            hChunk = INVALID_HANDLE_VALUE;
        }
    }
    //
    return lListInde;
}

DWORD ChksUtility::ChunkRiveChks(struct riv_chks *pRiveChks, HANDLE hChksList, ULONG lListPosit) {
    struct ChksEntry tChksEntry;
    struct riv_chks rchks;
    //
// _LOG_DEBUG(_T("read chks lListPosit:%u"), lListPosit);
    if (INVA_INDE_VALU == ReadNode(hChksList, &tChksEntry, lListPosit))
        return ((DWORD)-1);
    memcpy(rchks.md5_chks, tChksEntry.szFileChks, MD5_DIGEST_LEN);
    rchks.offset = ((unsigned __int64)lListPosit) << 22; // lListPosit * CHUNK_SIZE;
    memcpy(pRiveChks, &rchks, sizeof(struct riv_chks));
    //
    return 0x00;
}

ULONG ChksUtility::ChunkLawiUpdate(HANDLE hChksList, ULONG lListHinde, DWORD dwListPosit, FILETIME *ftLastWrite) {
    struct ChksEntry tChksEntry;
    ULONG lListInde;
    DWORD inde;
    //
    if (INVA_INDE_VALU == lListHinde) return INVA_INDE_VALU;
    tChksEntry.sibling = lListHinde;
    for (inde = 0x00; dwListPosit >= inde; inde++) {
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListInde) return INVA_INDE_VALU;
    }
    //
    if (memcmp(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME))) {
_LOG_DEBUG(_T("update chunk last write. dwListPosit:%u lw:%s"), dwListPosit, comutil::ftim_unis(ftLastWrite));
		memcpy(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
        WriteNode(hChksList, &tChksEntry, lListInde);
        // fprintf(stderr, "update last write, lListPosit:%u tChksEntry.time_stamp:%ld\n", lListPosit, tChksEntry.time_stamp);
    }
    //
    return lListInde;
}

ULONG ChksUtility::ChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD dwListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite) {
    struct ChksEntry tChksEntry;
    ULONG lListInde;
    DWORD inde;
    //
    if (INVA_INDE_VALU == lListHinde) return INVA_INDE_VALU;
    tChksEntry.sibling = lListHinde;
    for (inde = 0x00; dwListPosit >= inde; inde++) {
        lListInde = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListInde) return INVA_INDE_VALU;
    }
    //
_LOG_DEBUG(_T("update chunk last write. dwListPosit:%u lw:%s"), dwListPosit, comutil::ftim_unis(ftLastWrite));
    memcpy(tChksEntry.szFileChks, md5_chks, MD5_DIGEST_LEN);
	memcpy(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
    WriteNode(hChksList, &tChksEntry, lListInde);
    // fprintf(stderr, "update last write, lListPosit:%u tChksEntry.time_stamp:%ld\n", lListPosit, tChksEntry.time_stamp);
    //
    return lListInde;
}

VOID ChksUtility::FileChksCopy(HANDLE hChksList, ULONG lToHinde, ULONG lFromHinde) {
    struct ChksEntry tChksEntry;
	//
	ULONG lNextInde = lToHinde;
    tChksEntry.sibling = lFromHinde;
    while (INVA_INDE_VALU != tChksEntry.sibling) {
		if(INVA_INDE_VALU == ReadNode(hChksList, &tChksEntry, tChksEntry.sibling))
			break;
		//
		lNextInde = ChksUtility::ModifyNode(hChksList, lNextInde, tChksEntry.szFileChks, &tChksEntry.ftLastWrite);
    }
}