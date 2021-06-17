#include "StdAfx.h"
#include "CommonUtility.h"

#include "RiverUtility.h"
#include "FolderUtility.h"
#include "../Logger.h"
// #include "../../clientcom/utility/ulog.h"


VOID NFolderVec::DeleFolderVec(FolderVec *pFolderVec) {
    vector <struct FolderShadow *>::iterator iter;
    //
    // if(pFolderVec->empty()) return;
    for(iter=pFolderVec->begin(); pFolderVec->end()!=iter; ++iter)
        DeleNode(*iter);
    //
    pFolderVec->clear();
}

VOID NFolderVec::DeleFolderHmap(FolderHmap *pFolderHmap) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    // if(pFilesHmap->empty()) return;
    for(river_iter=pFolderHmap->begin(); pFolderHmap->end()!=river_iter; ++river_iter)
        DeleNode(river_iter->second);
    //
    pFolderHmap->clear();
}

struct FolderShadow *NFolderVec::AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, ULONG lFolderInde) {
    struct FolderShadow *shadow;
    // SYSTEMTIME ftLastWrite;
    //
    shadow = (struct FolderShadow *)malloc(sizeof(struct FolderShadow));
    if(!shadow) return NULL;
    //
    INIT_FOLDER_SHADOW(shadow)
    shadow->szFolderName = _tcsdup(pFolderEntry->szFolderName);
	memcpy(&shadow->ftCreatTime, &pFolderEntry->ftCreatTime, sizeof(FILETIME));
    shadow->parent = (struct FolderShadow *)pFolderEntry->parent;
    shadow->child = (struct FolderShadow *)pFolderEntry->child;
    shadow->sibling = (struct FolderShadow *)pFolderEntry->sibling;
    shadow->inde = lFolderInde;
    shadow->leaf_inde = pFolderEntry->leaf_inde;
    //
    // pFolderHmap->insert(std::pair<wstring, struct FolderShadow *>(szDirectory, shadow));
    pFolderVec->push_back(shadow);

    return shadow;
}

struct FolderShadow *NFolderVec::AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
    struct FolderShadow *shadow;
    TCHAR szDirectory[MAX_PATH];
    // SYSTEMTIME ftLastWrite;
    //
    shadow = (struct FolderShadow *)malloc(sizeof(struct FolderShadow));
    if(!shadow) return NULL;
    INIT_FOLDER_SHADOW(shadow)
    //
    comutil::full_name(szDirectory, szPathString, pFindData->cFileName);
    shadow->szFolderName = _tcsdup(szDirectory);
	memcpy(&shadow->ftCreatTime, &pFindData->ftCreationTime, sizeof(FILETIME));
    //
    pFolderVec->push_back(shadow);
    //
    return shadow;
}

struct FolderShadow *NFolderVec::DeleFromVec(FolderVec *pFolderVec, struct FolderShadow *shadow) {
    vector <struct FolderShadow *>::iterator iter;
    struct FolderShadow *fshado = NULL;
    //
    for(iter=pFolderVec->begin(); pFolderVec->end()!=iter; ++iter) {
        if(shadow == *iter) {
            pFolderVec->erase(iter);
            fshado = shadow;
            break;
        }
    }
    //
    return fshado;
}

//
DWORD FolderIndex::InitalFolderInde(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, FolderVec *pFolderVec, HANDLE hRiverFolder) {
    if(INVALID_HANDLE_VALUE == hRiverFolder) return ((DWORD)-1);
    //
    if(!LoadRiverFolder(pFolderVec, hRiverFolder, ROOT_INDE_VALU))
        return ((DWORD)-1);
    if(FolderIndex::BuildingIndex(pRiverHmap, pIndeHmap, pFolderVec))
        return ((DWORD)-1);
    //
    return 0x00;
}

VOID FolderIndex::FinalFolderInde(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, FolderVec *pFolderVec) {
    pRiverHmap->clear();
    pIndeHmap->clear();
    NFolderVec::DeleFolderVec(pFolderVec);
}

static inline VOID FillChildrenParent(struct FolderShadow *child, struct FolderShadow *parent) {
    while(child) {
        child->parent = parent;
        child = child->sibling;
    }
}

struct FolderShadow *FolderIndex::LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, ULONG lFolderInde) {
    struct FolderShadow *travel, *sibling, *child;
    struct FolderEntry tFolderEntry;
    //
    if(INVA_INDE_VALU == lFolderInde) return NULL;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderInde))
        return NULL;
    //
    child = LoadRiverFolder(pFolderVec, hRiverFolder, tFolderEntry.child);
    sibling = LoadRiverFolder(pFolderVec, hRiverFolder, tFolderEntry.sibling);
    //
    tFolderEntry.child = (DWORD)child;
    tFolderEntry.sibling = (DWORD)sibling;
    //
    travel = NFolderVec::AddNewNode(pFolderVec, &tFolderEntry, lFolderInde);
    if(child) FillChildrenParent(child, travel);
// _LOG_DEBUG(_T("travel:%08X inde:%d sibling:%08X FolderName:%s"), travel, travel->inde, sibling, travel->szFolderName);
    //
    return travel;
}

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, struct FolderShadow *shadow) {
    pRiverHmap->insert(FolderHmap::value_type(shadow->szFolderName, shadow));
    pIndeHmap->insert(IndexHmap::value_type(shadow->inde, shadow));
    //
    return 0x00;
}

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, FolderVec *pFolderVec) {
    vector <struct FolderShadow *>::iterator iter;
    //
    for(iter = pFolderVec->begin(); pFolderVec->end() != iter; ++iter) {
        pRiverHmap->insert(FolderHmap::value_type((*iter)->szFolderName, *iter));
        pIndeHmap->insert(IndexHmap::value_type((*iter)->inde, *iter));
    }
    //
    return 0x00;
}

DWORD FolderIndex::RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistPath, struct FolderShadow *shadow) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    river_iter = pRiverHmap->find(szExistPath);
    if (pRiverHmap->end() != river_iter) pRiverHmap->erase(river_iter);
    //
    // pRiverHmap->insert(std::pair<wstring, struct FolderShadow *>(shadow->szFolderName, shadow));
    pRiverHmap->insert(FolderHmap::value_type(shadow->szFolderName, shadow));
    //
    return 0x00;
}

DWORD FolderIndex::IndexDele(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, struct FolderShadow *shadow) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    //
    river_iter = pRiverHmap->find(shadow->szFolderName);
    if (pRiverHmap->end() != river_iter) pRiverHmap->erase(river_iter);
    //
    inde_iter = pIndeHmap->find(shadow->inde);
    if (pIndeHmap->end() != inde_iter) pIndeHmap->erase(inde_iter);
//
    return 0x00;
}

DWORD FolderUtility::FillIdleEntry(HANDLE hRiverFolder) {
    struct FolderEntry tFolderEntry;
    DWORD wlen;
    //
    INIT_FOLDER_ENTRY(tFolderEntry)
    ULONG lIdleOffset = SetFilePointer(hRiverFolder, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, NULL))
        return ((DWORD)-1);
    //
    return 0x00;
}

static ULONG FindNextEntry(HANDLE hRiverFolder, ULONG lNextInde) {
    struct FolderEntry tFolderEntry;
    ULONG lLeafInde;
// _LOG_DEBUG(_T("---x lNextInde:%d"), lNextInde);
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lNextInde))
        return INVA_INDE_VALU;
    if(INVA_INDE_VALU == tFolderEntry.child && INVA_INDE_VALU == tFolderEntry.recycled)
        return lNextInde;
    //
    lLeafInde = FindNextEntry(hRiverFolder, tFolderEntry.child);
// _LOG_DEBUG(_T("---1 lLeafInde:%d"), lLeafInde);
    if(INVA_INDE_VALU != lLeafInde) {
        if(lLeafInde == tFolderEntry.child) {
            tFolderEntry.child = INVA_INDE_VALU;
            FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lNextInde);
        }
// _LOG_DEBUG(_T("---2 re lLeafInde:%d"), lLeafInde);
        return lLeafInde;
    }
    //
    lLeafInde = FindNextEntry(hRiverFolder, tFolderEntry.recycled);
// _LOG_DEBUG(_T("---3 lLeafInde:%d"), lLeafInde);
    if(INVA_INDE_VALU != lLeafInde) {
        if(lLeafInde == tFolderEntry.recycled) {
            tFolderEntry.recycled = INVA_INDE_VALU;
            FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lNextInde);
        }
// _LOG_DEBUG(_T("---4 re lLeafInde:%d"), lLeafInde);
        return lLeafInde;
    }
    //
// _LOG_DEBUG(_T("---5 re lLeafInde:INVA_INDE_VALU"));
    return INVA_INDE_VALU;
}

ULONG FolderUtility::FindIdleEntry(HANDLE hRiverFolder) {
    struct FolderEntry tFillEntry;
    ULONG lIdleOffset, lIdleInde = INVA_INDE_VALU;
    DWORD slen, wlen;
    //
    lIdleOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    //
// _LOG_DEBUG(_T("tFillEntry.recycled:%d"), tFillEntry.recycled);
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        lIdleInde = FindNextEntry(hRiverFolder, tFillEntry.recycled);
        //
        if(tFillEntry.recycled == lIdleInde) {
            tFillEntry.recycled = INVA_INDE_VALU;
            OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
            OverLapped.Offset = lIdleOffset;
            if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
                return INVA_INDE_VALU;
        }
    } else {
        if(FillIdleEntry(hRiverFolder)) return INVA_INDE_VALU;
        lIdleInde = lIdleOffset / sizeof(struct FolderEntry);
    }
    //
// _LOG_DEBUG(_T("lIdleInde:%d"), lIdleInde);
    return lIdleInde;
}

ULONG FolderUtility::AddIdleEntry(HANDLE hRiverFolder, ULONG lIdleInde) {
    struct FolderEntry tFillEntry;
    DWORD slen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    ModifyNode(hRiverFolder, lIdleInde, FODE_INDE_RECYCLED, tFillEntry.recycled);
    //
    tFillEntry.recycled = lIdleInde;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lIdleInde;
}

ULONG FolderUtility::ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderInde * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, pFolderEntry, sizeof(struct FolderEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lFolderInde;
}

ULONG FolderUtility::WriteNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderInde) {
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderInde * sizeof(struct FolderEntry);
    if(!WriteFile(hRiverFolder, pFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFolderInde;
}

ULONG FolderUtility::ModifyNode(HANDLE hRiverFolder, ULONG lFolderInde, DWORD dwIndeType, ULONG lNextInde) {
    struct FolderEntry tFolderEntry;
    ULONG lExistInde = INVA_INDE_VALU;
    DWORD slen, wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderInde * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    switch(dwIndeType) {
    case FODE_INDE_SIBLING: // FODE_INDE_RECYCLED
        lExistInde = tFolderEntry.sibling; // recycled
        tFolderEntry.sibling = lNextInde; // recycled
        break;
    case FODE_INDE_LEAF:
        lExistInde = tFolderEntry.leaf_inde;
        tFolderEntry.leaf_inde = lNextInde;
        break;
    case FODE_INDE_CHILD:
        lExistInde = tFolderEntry.child;
        tFolderEntry.child = lNextInde;
        break;
    }
    //
    if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistInde;
}

DWORD FolderUtility::CTimeUpdate(HANDLE hRiverFolder, ULONG lFolderInde, FILETIME *ftCreatTime) {
    struct FolderEntry tFolderEntry;
    DWORD slen, wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderInde * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return ((DWORD)-1);
    }
    //
	if(memcmp(ftCreatTime, &tFolderEntry.ftCreatTime, sizeof(FILETIME))) {
		memcpy(&tFolderEntry.ftCreatTime, ftCreatTime, sizeof(FILETIME));
		if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
			return ((DWORD)-1);	
	}
    //
    return 0x00;
}


ULONG FolderUtility::VoidNode(HANDLE hRiverFolder, ULONG lFolderInde) {
    DWORD slen, wlen;
    struct FolderEntry tFolderEntry;
    ULONG lExistInde = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderInde * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    MKZEO(tFolderEntry.szFolderName);
    memset(&tFolderEntry.ftCreatTime, '\0', sizeof(FILETIME));
    if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistInde;
}

ULONG FolderUtility::ReadRecyEntry(HANDLE hRiverFolder, struct FolderEntry *pRecyEntry, ULONG lFolderInde) {
    struct FolderEntry tFillEntry;
    ULONG lIdleOffset, lIdleInde = INVA_INDE_VALU;
    DWORD slen;
    //
    lIdleOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVA_INDE_VALU;
    }
    //
_LOG_DEBUG(_T("xxx 2"));
    if(INVA_INDE_VALU != tFillEntry.recycled) {
		lIdleInde = tFillEntry.recycled;
_LOG_DEBUG(_T("lIdleInde:%d lFolderInde:%d"), lIdleInde, lFolderInde);
		do {
			if(lFolderInde == lIdleInde) {
				if((INVA_INDE_VALU==ReadNode(hRiverFolder, pRecyEntry, lFolderInde)) 
					|| ISZEO(pRecyEntry->szFolderName))
					lIdleInde = INVA_INDE_VALU;
				break;
			}
			lIdleInde = FindNextEntry(hRiverFolder, lIdleInde);
_LOG_DEBUG(_T("lIdleInde:%d"), lIdleInde);
		} while(INVA_INDE_VALU != lIdleInde);
    }
_LOG_DEBUG(_T("xxx 8"));
    //
    return lIdleInde;
}