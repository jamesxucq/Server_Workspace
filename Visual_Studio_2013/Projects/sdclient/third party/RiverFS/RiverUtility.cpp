#include "StdAfx.h"
#include "CommonUtility.h"

// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "ChksUtility.h"
#include "ChksList.h"
#include "FolderUtility.h"
#include "RiverUtility.h"

DWORD EntryUtility::FillIdleEntry(HANDLE hFilesRiver) {
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
    return 0x00;
}

ULONG EntryUtility::FindIdleEntry(HANDLE hFilesRiver, ULONG *lListHinde, DWORD *dwListLength) {
    struct FileEntry tFillEntry, tFileEntry;
    ULONG lIdleOffset, lIdleInde = INVA_INDE_VALU;
    DWORD slen, wlen;
    //
    lIdleOffset = SetFilePointer(hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        lIdleInde = tFillEntry.recycled;
        if(INVA_INDE_VALU == ReadNode(hFilesRiver, &tFileEntry, lIdleInde))
            return INVA_INDE_VALU;
        //
        *lListHinde = tFileEntry.list_hinde;
        *dwListLength = tFileEntry.chkslen;
        if(INVA_INDE_VALU != tFileEntry.sibling) {
            tFillEntry.recycled = tFileEntry.sibling;
            ModifyNode(hFilesRiver, tFileEntry.sibling, FILE_INDE_RECYCLED, tFileEntry.recycled);
        } else tFillEntry.recycled = tFileEntry.recycled;
        //
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        OverLapped.Offset = lIdleOffset;
        if(!WriteFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
            return INVA_INDE_VALU;
    } else {
        if(FillIdleEntry(hFilesRiver)) return INVA_INDE_VALU;
		*lListHinde = INVA_INDE_VALU;
        *dwListLength = 0x00;
        lIdleInde = lIdleOffset / sizeof(struct FileEntry);
    }
    //
    return lIdleInde;
}

ULONG EntryUtility::AddIdleEntry(HANDLE hFilesRiver, ULONG lFileInde) {
    struct FileEntry tFillEntry;
    DWORD slen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    ModifyNode(hFilesRiver, lFileInde, FILE_INDE_AIDLE, tFillEntry.recycled);
    //
    tFillEntry.recycled = lFileInde;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;

    // FlushFileBuffers(hFilesRiver);
    return lFileInde;
}

ULONG EntryUtility::ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileInde * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lFileInde;
}

ULONG EntryUtility::WriteNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde) {
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileInde * sizeof(struct FileEntry);
    if(!WriteFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

ULONG EntryUtility::ModifyNode(HANDLE hFilesRiver, ULONG lFileInde, DWORD dwIndeType, ULONG lNextInde) {
    DWORD slen, wlen;
    struct FileEntry tFileEntry;
    ULONG lExistInde = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileInde * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    switch(dwIndeType) {
    case FILE_INDE_ISONYM: // FILE_INDE_RECYCLED
        lExistInde = tFileEntry.isonym_node; // recycled
        tFileEntry.isonym_node = lNextInde; // recycled
        break;
    case FILE_INDE_SIBLING:
        lExistInde = tFileEntry.sibling;
        tFileEntry.sibling = lNextInde;
        break;
    case FILE_INDE_AIDLE:
		tFileEntry.sibling = INVA_INDE_VALU;
        lExistInde = tFileEntry.recycled;
        tFileEntry.recycled = lNextInde;
        break;
    }
    //
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistInde;
}

ULONG EntryUtility::VoidNode(HANDLE hFilesRiver, ULONG lFileInde) {
    DWORD slen, wlen;
    struct FileEntry tFileEntry;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileInde * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
	MKZEO(tFileEntry.szFileName);
    memset(&tFileEntry.szFileChks, '\0', SHA1_DIGEST_LEN);
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

//
ULONG RiverUtility::InitializeEntry(HANDLE hRiverFolder, const TCHAR *szRootPath) {
    struct FolderEntry tFolderEntry;
    ULONG lFolderInde = ROOT_INDE_VALU;
    //
    INIT_FOLDER_ENTRY(tFolderEntry)
    _tcscpy_s(tFolderEntry.szFolderName, MAX_PATH, szRootPath);
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderInde))
        return INVA_INDE_VALU;
    //
    return lFolderInde;
}

DWORD RiverUtility::FillIdleEntry(HANDLE hChksList, HANDLE hFilesRiver, HANDLE hRiverFolder) {
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
    return 0x00;
}

ULONG RiverUtility::InsFolderLeaf(HANDLE hRiverFolder, ULONG lFolderInde, ULONG lChildInde) {
    struct FolderEntry tFolderEntry;
    ULONG lFileInde;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderInde))
        return INVA_INDE_VALU;
    //
    lFileInde = tFolderEntry.leaf_inde;
    tFolderEntry.leaf_inde = lChildInde;
    //
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderInde))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

ULONG RiverUtility::InsFileItem(HANDLE hFilesRiver, ULONG lFileInde, ULONG lSibliInde, ULONG lParentInde, ULONG lListInde, TCHAR *szFileName, unsigned __int64 qwFileSize, FILETIME *ftCreatTime, DWORD chks_tatol) {
    struct FileEntry tFileEntry;
    //
    INIT_FILE_ENTRY(tFileEntry)
    tFileEntry.sibling = lSibliInde;
    tFileEntry.parent = lParentInde;
    _tcscpy_s(tFileEntry.szFileName, MAX_PATH, szFileName);
	tFileEntry.qwFileSize = qwFileSize;
// _LOG_DEBUG(_T("river ins file:%s"), szFileName);
	memcpy(&tFileEntry.ftCreatTime, ftCreatTime, sizeof(FILETIME));
    tFileEntry.list_hinde = lListInde;
    tFileEntry.chkslen = chks_tatol;
    //
    if(INVA_INDE_VALU == EntryUtility::WriteNode(hFilesRiver, &tFileEntry, lFileInde))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

ULONG RiverUtility::InsFolderItem(HANDLE hRiverFolder, ULONG lFolderInde, ULONG lParentInde, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
    struct FolderEntry tFolderEntry;
    ULONG lChildInde;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lParentInde))
        return INVA_INDE_VALU;
    //
    lChildInde = tFolderEntry.child;
    tFolderEntry.child = lFolderInde;
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lParentInde))
        return INVA_INDE_VALU;
    //
    INIT_FOLDER_ENTRY(tFolderEntry)
	memcpy(&tFolderEntry.ftCreatTime, &pFileData->ftCreationTime, sizeof(FILETIME));
    tFolderEntry.sibling = lChildInde;
    tFolderEntry.parent = lParentInde;
    comutil::full_name(tFolderEntry.szFolderName, szPathString, pFileData->cFileName);
// _LOG_DEBUG(_T("river ins folder:%s"), tFolderEntry.szFolderName);
    //
    if(INVA_INDE_VALU == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderInde))
        return INVA_INDE_VALU;
    //
    return lFolderInde;
}

ULONG RiverUtility::InsFileIsonym(HANDLE hFilesRiver, ULONG lFileInde, ULONG lIsonymInde) {
    struct FileEntry tFileEntry;
    DWORD slen, wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileInde * sizeof(struct FileEntry);
    //
    if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    tFileEntry.isonym_node = lIsonymInde;
    if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

ULONG RiverUtility::InsChksListItem(HANDLE hChksList, DWORD chks_tatol) {
    struct ChksEntry tChksEntry;
    //
    if(0x00 == chks_tatol) return INVA_INDE_VALU;
    //
    ULONG lListInde = SetFilePointer(hChksList, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lListInde && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    lListInde /= sizeof(struct ChksEntry);
    //
    INIT_CHKS_ENTRY(tChksEntry)
    ULONG lListHinde = lListInde;
    unsigned int inde;
// _LOG_DEBUG(_T("lListInde:"));
    for(inde = 1; chks_tatol > inde; lListInde++, inde++) {
// _LOG_DEBUG(_T("%u "), lListInde);
        tChksEntry.sibling = lListInde + 1;
        if(INVA_INDE_VALU == ChksUtility::WriteNode(hChksList, &tChksEntry, lListInde))
            return INVA_INDE_VALU;
    }
    //
// _LOG_DEBUG(_T("%u "), lListInde);
    tChksEntry.sibling = INVA_INDE_VALU;
    if(INVA_INDE_VALU == ChksUtility::WriteNode(hChksList, &tChksEntry, lListInde))
        return INVA_INDE_VALU;
    //
    return lListHinde;
}

//
static ULONG FindSibliNext(HANDLE hFilesRiver, ULONG lFileInde) {
	struct FileEntry tFileEntry;
	//
	if(INVA_INDE_VALU == EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileInde))
		return INVA_INDE_VALU;
	//
	return tFileEntry.sibling;
}

static ULONG FindRecycledNext(HANDLE hFilesRiver, ULONG lFileInde) {
	struct FileEntry tFileEntry;
	//
	if(INVA_INDE_VALU == EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileInde))
		return INVA_INDE_VALU;
	//
	return tFileEntry.recycled;
}

ULONG EntryUtility::FindRecyEntry(HANDLE hFilesRiver, ULONG lSibliInde, ULONG lFileInde) {
	ULONG lIdleInde;
	ULONG lFoundInde = INVA_INDE_VALU;
	//
	ULONG lIdleOffset = SetFilePointer(hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
		return INVA_INDE_VALU;
	//
_LOG_DEBUG(_T("xxx 3"));
	ULONG lFindInde;
	lFindInde = lIdleInde = lIdleOffset / sizeof(struct FileEntry);
	while(INVA_INDE_VALU != (lFindInde=FindRecycledNext(hFilesRiver, lFindInde))) {
		if(lSibliInde == lFindInde) {
			while(INVA_INDE_VALU != (lFindInde=FindSibliNext(hFilesRiver, lFindInde))) {
				if(lFileInde == lFindInde) {
					lFoundInde = lFileInde;
					break;
				}
			}
			break;
		}
	}
_LOG_DEBUG(_T("xxx 4"));
	//
	if(INVA_INDE_VALU == lFoundInde) {
		lFindInde = lIdleInde;
		while(INVA_INDE_VALU != (lFindInde=FindRecycledNext(hFilesRiver, lFindInde))) {
			lIdleInde = lFindInde;
			while(INVA_INDE_VALU != (lIdleInde=FindSibliNext(hFilesRiver, lIdleInde))) {
				if(lFileInde == lIdleInde) {
					lFoundInde = lFileInde;
					break;
				}
			}
			if(INVA_INDE_VALU != lFoundInde) 
				break;
		}
	}
_LOG_DEBUG(_T("xxx 5"));
	//
	return lFoundInde;
}