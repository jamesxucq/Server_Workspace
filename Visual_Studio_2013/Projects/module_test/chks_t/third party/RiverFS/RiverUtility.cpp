#include "StdAfx.h"
#include "CommonUtility.h"

// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "ChksUtility.h"
#include "ChksList.h"
#include "FolderUtility.h"
#include "RiverUtility.h"

DWORD EntryUtility::FillIdleEntry(HANDLE hFilesRiver)
{
    struct FileEntry tFileEntry;
    DWORD wlen;
    //
    INIT_FILE_ENTRY(tFileEntry)
    ULONG lIdleOffset = SetFilePointer(hFilesRiver, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, NULL))
        return ((DWORD)-1);
    //
    return 0;
}

ULONG EntryUtility::FindIdleEntry(ULONG *lListHinde, DWORD *iListLength, HANDLE hFilesRiver)
{
    struct FileEntry tFillEntry, tFileEntry;
    ULONG lIdleOffset, lIdleIndex = 0;
    DWORD rlen, wlen;
    //
    lIdleOffset = SetFilePointer(hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    //
    if(!ReadFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        lIdleIndex = tFillEntry.recycled;
        if(INVA_INDE_VALU == ReadNode(hFilesRiver, &tFileEntry, lIdleIndex))
            return INVA_INDE_VALU;
        //
        *lListHinde = tFileEntry.list_hinde;
        *iListLength = tFileEntry.chkslen;
        if(INVA_INDE_VALU != tFileEntry.sibling) {
            tFillEntry.recycled = tFileEntry.sibling;
            ModifyNode(hFilesRiver, tFileEntry.sibling, FILE_INDEX_RECYCLED, tFileEntry.recycled);
        } else tFillEntry.recycled = tFileEntry.recycled;
        //
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        OverLapped.Offset = lIdleOffset;
        if(!WriteFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
            return INVA_INDE_VALU;
    } else {
        if(FillIdleEntry(hFilesRiver)) return INVA_INDE_VALU;
        lIdleIndex = lIdleOffset / sizeof(struct FileEntry);
    }
    //
    return lIdleIndex;
}

ULONG EntryUtility::AddIdleEntry(HANDLE hFilesRiver, ULONG lFileIndex)
{
    struct FileEntry tFillEntry;
    DWORD rlen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    //
    if(!ReadFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    ModifyNode(hFilesRiver, lFileIndex, FILE_INDEX_RECYCLED, tFillEntry.recycled);
    //
    tFillEntry.recycled = lFileIndex;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;

    // FlushFileBuffers(hFilesRiver);
    return lFileIndex;
}

ULONG EntryUtility::ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileIndex)
{
    DWORD rlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lFileIndex;
}

ULONG EntryUtility::WriteNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileIndex)
{
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
    if(!WriteFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFileIndex;
}

ULONG EntryUtility::ModifyNode(HANDLE hFilesRiver, ULONG lFileIndex, DWORD dwIndexType, ULONG lNextIndex)
{
    DWORD rlen, wlen;
    struct FileEntry tFileEntry;
    ULONG lExistsIndex = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    switch(dwIndexType) {
    case FILE_INDEX_ISONYM: // FILE_INDEX_RECYCLED
        lExistsIndex = tFileEntry.isonym_node; // recycled
        tFileEntry.isonym_node = lNextIndex; // recycled
        break;
    case FILE_INDEX_SIBLING:
        lExistsIndex = tFileEntry.sibling;
        tFileEntry.sibling = lNextIndex;
        break;
    }
    //
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistsIndex;
}

ULONG EntryUtility::VoidNode(HANDLE hFilesRiver, ULONG lFileIndex)
{
    DWORD rlen, wlen;
    struct FileEntry tFileEntry;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    memset(&tFileEntry.szFileChks, '\0', SHA1_DIGEST_LEN);
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFileIndex;
}

//
ULONG RiverUtility::InitializeEntry(HANDLE hRiverFolder, const TCHAR *szRootPath)
{
    struct FolderEntry tFolderEntry;
    ULONG lFolderIndex = ROOT_INDE_VALU;
    //
    INIT_FOLDER_ENTRY(tFolderEntry)
    _tcscpy_s(tFolderEntry.szFolderName, MAX_PATH, szRootPath);
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderIndex))
        return INVA_INDE_VALU;
    //
    return lFolderIndex;
}

DWORD RiverUtility::FillIdleEntry(HANDLE hChksList, HANDLE hFilesRiver, HANDLE hRiverFolder)
{
    DWORD wlen;
    //
    struct ChksEntry tChksEntry;
    INIT_CHKS_ENTRY(tChksEntry)
    ULONG lIdleOffset = SetFilePointer(hChksList, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!WriteFile(hChksList, &tChksEntry, sizeof(struct ChksEntry), &wlen, NULL))
        return ((DWORD)-1);
    //
    struct FileEntry tFileEntry;
    INIT_FILE_ENTRY(tFileEntry)
    lIdleOffset = SetFilePointer(hFilesRiver, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, NULL))
        return ((DWORD)-1);
    //
    struct FolderEntry tFolderEntry;
    INIT_FOLDER_ENTRY(tFolderEntry)
    lIdleOffset = SetFilePointer(hRiverFolder, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, NULL))
        return ((DWORD)-1);
    //
    return 0;
}

ULONG RiverUtility::InsFolderLeaf(HANDLE hRiverFolder, ULONG lFolderIndex, ULONG lChildIndex)
{
    struct FolderEntry tFolderEntry;
    ULONG lFileIndex;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderIndex))
        return INVA_INDE_VALU;
    //
    lFileIndex = tFolderEntry.leaf_inde;
    tFolderEntry.leaf_inde = lChildIndex;
    //
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderIndex))
        return INVA_INDE_VALU;
    //
    return lFileIndex;
}

ULONG RiverUtility::InsFileItem(HANDLE hFilesRiver, ULONG lFileIndex, ULONG lSiblingIndex, ULONG lParentIndex, ULONG lListIndex, TCHAR *szFileName, DWORD chks_tally)
{
    struct FileEntry tFileEntry;
    //
    INIT_FILE_ENTRY(tFileEntry)
    tFileEntry.sibling = lSiblingIndex;
    tFileEntry.parent = lParentIndex;
    _tcscpy_s(tFileEntry.szFileName, MAX_PATH, szFileName);
    tFileEntry.list_hinde = lListIndex;
    tFileEntry.chkslen = chks_tally;
    //
    if(INVA_INDE_VALU == EntryUtility::WriteNode(hFilesRiver, &tFileEntry, lFileIndex))
        return INVA_INDE_VALU;
    //
    return lFileIndex;
}

ULONG RiverUtility::InsFolderItem(HANDLE hRiverFolder, ULONG lFolderIndex, ULONG lParentIndex, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData)
{
    struct FolderEntry tFolderEntry;
    ULONG lChildIndex;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lParentIndex))
        return INVA_INDE_VALU;
    //
    lChildIndex = tFolderEntry.child;
    tFolderEntry.child = lFolderIndex;
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lParentIndex))
        return INVA_INDE_VALU;
    //
    INIT_FOLDER_ENTRY(tFolderEntry)
    tFolderEntry.ftCreationTime = pFileData->ftCreationTime;
    tFolderEntry.sibling = lChildIndex;
    tFolderEntry.parent = lParentIndex;
    CommonUtility::full_name(tFolderEntry.szFolderName, szPathString, pFileData->cFileName);
    //
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderIndex))
        return INVA_INDE_VALU;
    //
    return lFolderIndex;
}

ULONG RiverUtility::InsFileIsonym(HANDLE hFilesRiver, ULONG lFileIndex, ULONG lIsonymIndex)
{
    struct FileEntry tFileEntry;
    DWORD rlen, wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
    //
    if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, &OverLapped)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    tFileEntry.isonym_node = lIsonymIndex;
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFileIndex;
}

ULONG RiverUtility::InsChksListItem(HANDLE hChksList, DWORD chks_tally)
{
    struct ChksEntry tChksEntry;
    //
    ULONG lListIndex = SetFilePointer(hChksList, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lListIndex && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    lListIndex /= sizeof(struct ChksEntry);
    //
    INIT_CHKS_ENTRY(tChksEntry)
    ULONG lListHinde = lListIndex;
    unsigned int ind;
    TRACE(_T("lListIndex:"));
    for(ind = 1; chks_tally > ind; lListIndex++, ind++) {
        TRACE(_T("%u "), lListIndex);
        tChksEntry.sibling = lListIndex + 1;
        if(INVA_INDE_VALU == ChksUtility::WriteNode(hChksList, &tChksEntry, lListIndex))
            return INVA_INDE_VALU;
    }
    //
    TRACE(_T("%u "), lListIndex);
    tChksEntry.sibling = INVA_INDE_VALU;
    if(INVA_INDE_VALU == ChksUtility::WriteNode(hChksList, &tChksEntry, lListIndex))
        return INVA_INDE_VALU;
    TRACE(_T("\n"));
    //
    return lListHinde;
}
