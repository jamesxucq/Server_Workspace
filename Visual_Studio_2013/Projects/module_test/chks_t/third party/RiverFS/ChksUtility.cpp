#include "StdAfx.h"

#include "CommonUtility.h"
#include "Sha1.h"
// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "ChksList.h"
#include "ChksUtility.h"
//
HANDLE ChksUtility::ChksFileHandle()
{
    TCHAR szDirectory[MAX_PATH];
    TCHAR szFileName[MAX_PATH];
    //
    GetTempPath(MAX_PATH, szDirectory);
    GetTempFileName(szDirectory, _T("sdclient"), 0, szFileName);
    //
    HANDLE hFileChks = CreateFile( szFileName,
                                   GENERIC_WRITE | GENERIC_READ,
                                   NULL, /* FILE_SHARE_READ */
                                   NULL,
                                   CREATE_ALWAYS,
                                   FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                   NULL);
    if( INVALID_HANDLE_VALUE == hFileChks ) {
        TRACE( _T("create file failed: %d\n"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return INVALID_HANDLE_VALUE;
    }
    //
    return hFileChks;
}

HANDLE ChksUtility::CreatChksFile(HANDLE hFileRiver, const TCHAR *szFileName)
{
    HANDLE hFileChks = CreateFile( szFileName,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
    if( INVALID_HANDLE_VALUE == hFileChks ) {
        TRACE( _T("create file failed: %d\n"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return INVALID_HANDLE_VALUE;
    }
    DWORD dwFileSizeHigh;
    unsigned __int64 qwFileSize = GetFileSize(hFileChks, &dwFileSizeHigh);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    unsigned int chks_tally = (unsigned int)(qwFileSize >> 22); // (vlength / CHUNK_SIZE);
    if (POW2N_MOD(qwFileSize, CHUNK_SIZE)) chks_tally++;
    //
    struct riv_chks rchks;
    DWORD wlen;
    for (unsigned int inde = 0; inde < chks_tally; inde++) {
        rchks.offset = inde * CHUNK_SIZE;
        CommonUtility::chunk_chks(rchks.md5_chks, hFileChks, qwFileSize, rchks.offset);
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

VOID ChksUtility::FileSha1Chks(unsigned char *sha1_chks, HANDLE hRiveChks)
{
    struct riv_chks rchks;
    sha1_ctx cx[1];
    DWORD dwReadSize;
    //
    DWORD dwResult = SetFilePointer(hRiveChks, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return;
    //
    sha1_begin(cx);
    while(ReadFile(hRiveChks, &rchks, sizeof(struct riv_chks), &dwReadSize, NULL) && 0 < dwReadSize) {
        sha1_hash(rchks.md5_chks, MD5_DIGEST_LEN, cx);
    }
    sha1_end(sha1_chks, cx);
}

VOID ChksUtility::FileSha1Chks(unsigned char *sha1_chks, HANDLE hChksList, ULONG lListIndex)
{
    struct ChksEntry tChksEntry;
    sha1_ctx cx[1];
    //
    tChksEntry.sibling = lListIndex;
    sha1_begin(cx);
    while (INVA_INDE_VALU != tChksEntry.sibling) {
        TRACE(_T("read chks lListPosit:%u\n"), tChksEntry.sibling);
        ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        sha1_hash(tChksEntry.szFileChks, MD5_DIGEST_LEN, cx);
    }
    sha1_end(sha1_chks, cx);
}

//
DWORD ChksUtility::FillIdleEntry(HANDLE hChksList)
{
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
    return 0;
}

ULONG ChksUtility::FindIdleEntry(HANDLE hChksList)
{
    struct ChksEntry tFillEntry, tChksEntry;
    ULONG lIdleOffset, lIdleIndex = 0;
    DWORD rlen, wlen;
    //
    lIdleOffset = SetFilePointer(hChksList, -(LONG)sizeof(struct ChksEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    //
    if(!ReadFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        lIdleIndex = tFillEntry.recycled;
        if(INVA_INDE_VALU == ReadNode(hChksList, &tChksEntry, lIdleIndex))
            return INVA_INDE_VALU;
        // TRACE(_T("idle entry:%u sibling:%u recycled:%u\n"), lIdleIndex, tChksEntry.sibling, tChksEntry.recycled);
        memset(&tChksEntry.ftLastWrite, 0, sizeof(FILETIME));
        if(INVA_INDE_VALU == WriteNode(hChksList, &tChksEntry, lIdleIndex))
            return INVA_INDE_VALU;
        //
        if(INVA_INDE_VALU != tChksEntry.sibling) {
            tFillEntry.recycled = tChksEntry.sibling;
            ModifyNode(hChksList, tChksEntry.sibling, LIST_INDEX_RECYCLED, tChksEntry.recycled);
        } else tFillEntry.recycled = tChksEntry.recycled;
        //
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        OverLapped.Offset = lIdleOffset;
        if(!WriteFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
            return INVA_INDE_VALU;
    } else {
        if(FillIdleEntry(hChksList)) return INVA_INDE_VALU;
        lIdleIndex = lIdleOffset / sizeof(struct ChksEntry);
    }
    //
    return lIdleIndex;
}

ULONG ChksUtility::AddIdleEntry(HANDLE hChksList, ULONG lListIndex)
{
    struct ChksEntry tFillEntry;
    DWORD rlen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(hChksList, -(LONG)sizeof(struct ChksEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    //
    if(!ReadFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    ModifyNode(hChksList, lListIndex, LIST_INDEX_RECYCLED, tFillEntry.recycled);
    //
    tFillEntry.recycled = lListIndex;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    // FlushFileBuffers(hChksList);
    return lListIndex;
}

ULONG ChksUtility::ReadNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListIndex)
{
    DWORD rlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListIndex * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, pChksEntry, sizeof(struct ChksEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lListIndex;
}

ULONG ChksUtility::WriteNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListIndex)
{
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListIndex * sizeof(struct ChksEntry);
    if(!WriteFile(hChksList, pChksEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lListIndex;
}

ULONG ChksUtility::ModifyNode(HANDLE hChksList, ULONG lListIndex, DWORD dwIndexType, ULONG lNextIndex)
{
    DWORD rlen, wlen;
    struct ChksEntry tChksEntry;
    ULONG lExistsIndex = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListIndex * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    switch(dwIndexType) {
    case LIST_INDEX_SIBLING:
        lExistsIndex = tChksEntry.sibling;
        tChksEntry.sibling = lNextIndex;
        break;
    case LIST_INDEX_RECYCLED:
        lExistsIndex = tChksEntry.recycled;
        tChksEntry.recycled = lNextIndex;
        break;
    }
    //
    if(!WriteFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistsIndex;
}

DWORD ChksUtility::ListLength(HANDLE hChksList, ULONG lListHinde)
{
    struct ChksEntry tChksEntry;
    unsigned int ind;
    //
    TRACE(_T("list length alloc block:"));
    tChksEntry.sibling = lListHinde;
    for(ind = 0; INVA_INDE_VALU != tChksEntry.sibling; ind++) {
        TRACE(_T(" %u"), tChksEntry.sibling);
        ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
    }
    TRACE(_T("\n"));
    //
    return ind;
}

DWORD ChksUtility::ListEntry(HANDLE hChksList, ULONG lListHinde) // for test
{
    struct ChksEntry tChksEntry;
    unsigned int ind;
    //
    TRACE(_T("block info:"));
    tChksEntry.sibling = lListHinde;
    for(ind = 0; INVA_INDE_VALU != tChksEntry.sibling; ind++) {
        TRACE(_T(" inde:%u"), tChksEntry.sibling);
        ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        TRACE(_T(" last_write:%lld"), tChksEntry.ftLastWrite);
    }
    TRACE(_T("\n"));
    //
    return ind;
}

VOID ChksUtility::ChksIdleEntry(HANDLE hChksList)
{
    struct ChksEntry tFillEntry, tChksEntry;
    ULONG lIdleOffset, lIdleIndex = 0;
    DWORD rlen;
    //
    lIdleOffset = SetFilePointer(hChksList, -(LONG)sizeof(struct ChksEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ;
    //
    if(!ReadFile(hChksList, &tFillEntry, sizeof(struct ChksEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return ;
    }
    //
    TRACE(_T("idle list alloc block:"));
    ULONG lRecyclIndex;
    for(lRecyclIndex = tFillEntry.recycled; INVA_INDE_VALU != ReadNode(hChksList, &tChksEntry, lRecyclIndex); ) {
        TRACE(_T(" r:%u s:%u"), lRecyclIndex, tChksEntry.sibling);
        lRecyclIndex = tChksEntry.recycled;
        for(; INVA_INDE_VALU != ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);) {
            if(INVA_INDE_VALU != tChksEntry.sibling)
                TRACE(_T(" s:%u"), tChksEntry.sibling);
        }
    }
    TRACE(_T("\n"));
}

ULONG FindSibliEntry(HANDLE hChksList, ULONG lListIndex)
{
    DWORD rlen;
    struct ChksEntry tChksEntry;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListIndex * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return tChksEntry.sibling;
}

ULONG ChksUtility::ResetListLength(HANDLE hChksList, ULONG lListHinde, DWORD iOldLength, DWORD iNewLength)
{
    int diff_clen = iNewLength - iOldLength;
    int ind;
    ULONG eind, nind, sibli;
    //
    if(0 < diff_clen) { // make long then old
        for (nind = lListHinde; INVA_INDE_VALU != (sibli = FindSibliEntry(hChksList, nind)); nind = sibli);
        //
        eind = FindIdleEntry(hChksList);
        if (INVA_INDE_VALU == nind) lListHinde = eind;
        ModifyNode(hChksList, nind, LIST_INDEX_SIBLING, eind);
        nind = eind;
        //
        for (ind = 0x01; diff_clen > ind; ind++) {
            eind = FindIdleEntry(hChksList);
            // printf("nind:%llu eind:%llu\n", nind, eind);
            ModifyNode(hChksList, nind, LIST_INDEX_SIBLING, eind);
            nind = eind;
        }
    } else if(0 > diff_clen) { // make short
        nind = INVA_INDE_VALU;
        for (sibli = lListHinde, ind = 0x00; (int)iNewLength > ind; ind++) {
            nind = sibli;
            sibli = FindSibliEntry(hChksList, sibli);
        }
        AddIdleEntry(hChksList, sibli);
        if(INVA_INDE_VALU == nind) lListHinde = INVA_INDE_VALU;
        else ModifyNode(hChksList, nind, LIST_INDEX_SIBLING, INVA_INDE_VALU);
    }
    //
    return lListHinde;
}

ULONG ChksUtility::ModifyNode(HANDLE hChksList, ULONG lListIndex, unsigned char *md5_chks, FILETIME *ftLastWrite)
{
    DWORD rlen, wlen;
    struct ChksEntry tChksEntry;
    ULONG lNextIndex = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lListIndex * sizeof(struct ChksEntry);
    if(!ReadFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    lNextIndex = tChksEntry.sibling;
    memcpy(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
    memcpy(tChksEntry.szFileChks, md5_chks, MD5_DIGEST_LEN);
    //
    if(!WriteFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lNextIndex;
}
//
DWORD ChksUtility::ValidChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD iListPosit, FILETIME *ftLastWrite)
{
    struct ChksEntry tChksEntry;
    ULONG lListIndex;
    DWORD ind;
    //
    if (INVA_INDE_VALU == lListHinde) return 0x00;
    tChksEntry.sibling = lListHinde;
    for (ind = 0; iListPosit > ind; ind++) {
        lListIndex = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListIndex) return 0x00;
    }
    //
    DWORD dwValidTally = 0x00;
    while(INVA_INDE_VALU != tChksEntry.sibling) {
        lListIndex = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (memcmp(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME))) {
            ++dwValidTally;
        }
    }
    //
    return dwValidTally;
}

ULONG ChksUtility::FileChunkUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, DWORD iListPosit, WIN32_FIND_DATA *fileInfo)
{
    struct ChksEntry tChksEntry;
    ULONG lListIndex;
    DWORD ind;
    //
    if (INVA_INDE_VALU == lListHinde) return INVA_INDE_VALU;
    tChksEntry.sibling = lListHinde;
    for (ind = 0; iListPosit > ind; ind++) {
        lListIndex = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListIndex) return INVA_INDE_VALU;
    }
    //
    HANDLE hChunk = CreateFile( szFilePath,
                                /* GENERIC_WRITE | */ GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if( INVALID_HANDLE_VALUE == hChunk ) {
        TRACE( _T("create file failed: %d\n"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return INVA_INDE_VALU;
    }
    //
    unsigned __int64 qwFileSize = fileInfo->nFileSizeLow;
    qwFileSize += ((unsigned __int64)fileInfo->nFileSizeHigh) << 32;
    ULONG lIndexPosit = tChksEntry.sibling;
    for (; INVA_INDE_VALU != tChksEntry.sibling; ind++) {
        lListIndex = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (memcmp(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME))) {
            TRACE(_T("update chunk chks. lListIndex:%u\n"), lListIndex);
            CommonUtility::chunk_chks(tChksEntry.szFileChks, hChunk, qwFileSize, ((unsigned __int64)ind) << 22);
            tChksEntry.ftLastWrite = fileInfo->ftLastWriteTime;
            WriteNode(hChksList, &tChksEntry, lListIndex);
            // printf("update chks, lListIndex:%llu tChksEntry.time_stamp:%ld\n", lListIndex, tChksEntry.time_stamp);
        }
    }
    //
    if(INVALID_HANDLE_VALUE != hChunk) {
        CloseHandle( hChunk );
        hChunk = INVALID_HANDLE_VALUE;
    }
    return lIndexPosit;
}

DWORD ChksUtility::FileRiveChks(HANDLE hRiveChks, HANDLE hChksList, ULONG lListPosit)
{
    struct ChksEntry tChksEntry;
    struct riv_chks rchks;
    DWORD wlen, ind;
    //
    tChksEntry.sibling = lListPosit;
    for (ind = 0; INVA_INDE_VALU != tChksEntry.sibling; ind++) {
        TRACE(_T("read chks lListPosit:%u\n"), tChksEntry.sibling);
        if (INVA_INDE_VALU == ReadNode(hChksList, &tChksEntry, tChksEntry.sibling))
            return 0x01;
        memcpy(rchks.md5_chks, tChksEntry.szFileChks, MD5_DIGEST_LEN);
        rchks.offset = ind << 22;
        // memcpy(&pRiveChks[ind], &rchks, sizeof(struct riv_chks));
        if(!WriteFile(hRiveChks, &rchks, sizeof(struct riv_chks), &wlen, NULL))
            return 0x01;
    }
    return 0x00;
}

ULONG ChksUtility::ChunkChksUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, DWORD iListPosit, WIN32_FIND_DATA *fileInfo)
{
    struct ChksEntry tChksEntry;
    ULONG lListIndex;
    DWORD ind;
    //
    if (INVA_INDE_VALU == lListHinde) return INVA_INDE_VALU;
    tChksEntry.sibling = lListHinde;
    for (ind = 0; iListPosit >= ind; ind++) {
        lListIndex = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListIndex) return INVA_INDE_VALU;
    }
    //
    if (memcmp(&tChksEntry.ftLastWrite, &fileInfo->ftLastWriteTime, sizeof(FILETIME))) {
        TRACE(_T("update chunk chks. iListPosit:%u\n"), iListPosit);
        HANDLE hChunk = CreateFile( szFilePath,
                                    /* GENERIC_WRITE | */ GENERIC_READ,
                                    NULL, /* FILE_SHARE_READ */
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
        if( INVALID_HANDLE_VALUE == hChunk ) {
            TRACE( _T("create file failed: %d\n"), GetLastError() );
            TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
            return INVA_INDE_VALU;
        }
        //
        unsigned __int64 qwFileSize = fileInfo->nFileSizeLow;
        qwFileSize += ((unsigned __int64)fileInfo->nFileSizeHigh) << 32;
        CommonUtility::chunk_chks(tChksEntry.szFileChks, hChunk, qwFileSize, ((unsigned __int64)iListPosit) << 22);
        tChksEntry.ftLastWrite = fileInfo->ftLastWriteTime;
        WriteNode(hChksList, &tChksEntry, lListIndex);
        // printf("update chks, lListPosit:%u tChksEntry.time_stamp:%ld\n", lListPosit, tChksEntry.time_stamp);
        //
        if(INVALID_HANDLE_VALUE != hChunk) {
            CloseHandle( hChunk );
            hChunk = INVALID_HANDLE_VALUE;
        }
    }
    //
    return lListIndex;
}

DWORD ChksUtility::ChunkRiveChks(struct riv_chks *pRiveChks, HANDLE hChksList, ULONG lListPosit)
{
    struct ChksEntry tChksEntry;
    struct riv_chks rchks;
    //
    TRACE(_T("read chks lListPosit:%u\n"), lListPosit);
    if (INVA_INDE_VALU == ReadNode(hChksList, &tChksEntry, lListPosit))
        return 0x01;
    memcpy(rchks.md5_chks, tChksEntry.szFileChks, MD5_DIGEST_LEN);
    rchks.offset = 0;
    memcpy(pRiveChks, &rchks, sizeof(struct riv_chks));
    //
    return 0x00;
}

ULONG ChksUtility::ChunkWriteUpdate(HANDLE hChksList, ULONG lListHinde, DWORD iListPosit, FILETIME *ftLastWrite)
{
    struct ChksEntry tChksEntry;
    ULONG lListIndex;
    DWORD ind;
    //
    if (INVA_INDE_VALU == lListHinde) return INVA_INDE_VALU;
    tChksEntry.sibling = lListHinde;
    for (ind = 0; iListPosit >= ind; ind++) {
        lListIndex = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListIndex) return INVA_INDE_VALU;
    }
    //
    if (memcmp(&tChksEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME))) {
        TRACE(_T("update chunk last write. iListPosit:%u\n"), iListPosit);
        tChksEntry.ftLastWrite = *ftLastWrite;
        WriteNode(hChksList, &tChksEntry, lListIndex);
        // printf("update last write, lListPosit:%u tChksEntry.time_stamp:%ld\n", lListPosit, tChksEntry.time_stamp);
    }
    //
    return lListIndex;
}

ULONG ChksUtility::ChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD iListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite)
{
    struct ChksEntry tChksEntry;
    ULONG lListIndex;
    DWORD ind;
    //
    if (INVA_INDE_VALU == lListHinde) return INVA_INDE_VALU;
    tChksEntry.sibling = lListHinde;
    for (ind = 0; iListPosit >= ind; ind++) {
        lListIndex = ReadNode(hChksList, &tChksEntry, tChksEntry.sibling);
        if (INVA_INDE_VALU == lListIndex) return INVA_INDE_VALU;
    }
    //
    TRACE(_T("update chunk last write. iListPosit:%u\n"), iListPosit);
    memcpy(tChksEntry.szFileChks, md5_chks, MD5_DIGEST_LEN);
    tChksEntry.ftLastWrite = *ftLastWrite;
    WriteNode(hChksList, &tChksEntry, lListIndex);
    // printf("update last write, lListPosit:%u tChksEntry.time_stamp:%ld\n", lListPosit, tChksEntry.time_stamp);
    //
    return lListIndex;
}