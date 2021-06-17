#include "StdAfx.h"
#include "CommonUtility.h"

#include "RiverUtility.h"
#include "FolderUtility.h"
// #include "../../clientcom/utility/ulog.h"


VOID NFolderVec::DeleteFolderVec(FolderVec *pFolderVec)
{
    vector <struct FolderShadow *>::iterator iter;
    //
    // if(pFolderVec->empty()) return;
    for(iter=pFolderVec->begin(); iter!=pFolderVec->end(); ++iter)
        DeleteNode(*iter);
    //
    pFolderVec->clear();
}

VOID NFolderVec::DeleteFolderHmap(FolderHmap *pFolderHmap)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    // if(pFilesHmap->empty()) return;
    for(river_iter=pFolderHmap->begin(); river_iter!=pFolderHmap->end(); ++river_iter)
        DeleteNode(river_iter->second);
    //
    pFolderHmap->clear();
}

struct FolderShadow *NFolderVec::AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, ULONG lFolderIndex)
{
    struct FolderShadow *shadow;
    //SYSTEMTIME ftLastWrite;
    //
    shadow = (struct FolderShadow *)malloc(sizeof(struct FolderShadow));
    if(!shadow) return NULL;
    //
    INIT_FOLDER_SHADOW(shadow)
    shadow->szFolderName = _tcsdup(pFolderEntry->szFolderName);
    shadow->ftCreationTime = pFolderEntry->ftCreationTime;
    shadow->parent = (struct FolderShadow *)pFolderEntry->parent;
    shadow->child = (struct FolderShadow *)pFolderEntry->child;
    shadow->sibling = (struct FolderShadow *)pFolderEntry->sibling;
    shadow->inde = lFolderIndex;
    shadow->leaf_inde = pFolderEntry->leaf_inde;
    //
    // pFolderHmap->insert(std::pair<wstring, struct FolderShadow *>(szDirectory, shadow));
    pFolderVec->push_back(shadow);

    return shadow;
}

struct FolderShadow *NFolderVec::AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData)
{
    struct FolderShadow *shadow;
    TCHAR szDirectory[MAX_PATH];
    //SYSTEMTIME ftLastWrite;
    //
    shadow = (struct FolderShadow *)malloc(sizeof(struct FolderShadow));
    if(!shadow) return NULL;
    INIT_FOLDER_SHADOW(shadow)
    //
    CommonUtility::full_name(szDirectory, szPathString, pFindData->cFileName);
    shadow->szFolderName = _tcsdup(szDirectory);
    shadow->ftCreationTime = pFindData->ftCreationTime;
    //
    pFolderVec->push_back(shadow);
    //
    return shadow;
}

struct FolderShadow *NFolderVec::DeleteFromVec(FolderVec *pFolderVec, struct FolderShadow *shadow)
{
    vector <struct FolderShadow *>::iterator iter;
    struct FolderShadow *fshado = NULL;
    //
    for(iter=pFolderVec->begin(); iter!=pFolderVec->end(); ++iter) {
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
DWORD FolderIndex::InitalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec, HANDLE hRiverFolder)
{
    if(INVALID_HANDLE_VALUE == hRiverFolder) return ((DWORD)-1);
    //
    if(!LoadRiverFolder(pFolderVec, hRiverFolder, ROOT_INDE_VALU))
        return ((DWORD)-1);
    if(FolderIndex::BuildingIndex(pRiverHmap, pIsonymHmap, pIndexHmap, pFolderVec))
        return ((DWORD)-1);
    //
    return 0;
}

VOID FolderIndex::FinalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec)
{
    pRiverHmap->clear();
    pIsonymHmap->clear();
    pIndexHmap->clear();
    NFolderVec::DeleteFolderVec(pFolderVec);
}

static inline VOID FillChildrenParent(struct FolderShadow *child, struct FolderShadow *parent)
{
    while(child) {
        child->parent = parent;
        child = child->sibling;
    }
}

struct FolderShadow *FolderIndex::LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, ULONG lFolderIndex)
{
    struct FolderShadow *travel, *sibling, *child;
    struct FolderEntry tFolderEntry;
    //
    if(INVA_INDE_VALU == lFolderIndex) return NULL;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderIndex))
        return NULL;
    //
    child = LoadRiverFolder(pFolderVec, hRiverFolder, tFolderEntry.child);
    sibling = LoadRiverFolder(pFolderVec, hRiverFolder, tFolderEntry.sibling);
    //
    tFolderEntry.child = (DWORD)child;
    tFolderEntry.sibling = (DWORD)sibling;
    //
    travel = NFolderVec::AddNewNode(pFolderVec, &tFolderEntry, lFolderIndex);
    if(child) FillChildrenParent(child, travel);
    //
    return travel;
}

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    pRiverHmap->insert(FolderHmap::value_type(shadow->szFolderName, shadow));
    pIndexHmap->insert(IndexHmap::value_type(shadow->inde, shadow));
    //
    wchar_t *folder = _tcsrchr(shadow->szFolderName, _T('\\'));
    if(folder) {
        river_iter = pIsonymHmap->find(++folder);
        if(river_iter != pIsonymHmap->end()) {
            struct FolderShadow *fshado = river_iter->second;
            while(fshado->isonym_node) fshado = fshado->isonym_node;
            fshado->isonym_node = shadow;
        } else pIsonymHmap->insert(FolderHmap::value_type(folder, shadow));
    }
    //
    return 0;
}

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec)
{
    vector <struct FolderShadow *>::iterator iter;
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    for(iter = pFolderVec->begin(); iter != pFolderVec->end(); ++iter) {
        //
        pRiverHmap->insert(FolderHmap::value_type((*iter)->szFolderName, *iter));
        pIndexHmap->insert(IndexHmap::value_type((*iter)->inde, *iter));
        //
        wchar_t *folder = _tcsrchr((*iter)->szFolderName, _T('\\'));
        if(folder && _T('')!=*(++folder)) {
            river_iter = pIsonymHmap->find(folder);
            if(river_iter != pIsonymHmap->end()) {
                struct FolderShadow *shadow = river_iter->second;
                while(shadow->isonym_node) shadow = shadow->isonym_node;
                shadow->isonym_node = *iter;
            } else pIsonymHmap->insert(FolderHmap::value_type(folder, *iter));
        }
        //
    }
    //
    return 0;
}

DWORD FolderIndex::RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistsPath, struct FolderShadow *shadow)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    river_iter = pRiverHmap->find(szExistsPath);
    if (river_iter != pRiverHmap->end()) pRiverHmap->erase(river_iter);
    //
    // pRiverHmap->insert(std::pair<wstring, struct FolderShadow *>(shadow->szFolderName, shadow));
    pRiverHmap->insert(FolderHmap::value_type(shadow->szFolderName, shadow));
    //
    return 0;
}

DWORD FolderIndex::IndexDelete(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    struct FolderShadow *prev, *fshado;
    //
    river_iter = pRiverHmap->find(shadow->szFolderName);
    if (river_iter != pRiverHmap->end()) pRiverHmap->erase(river_iter);
    //
    inde_iter = pIndexHmap->find(shadow->inde);
    if (inde_iter != pIndexHmap->end()) pIndexHmap->erase(inde_iter);
    //
    wchar_t *folder = _tcsrchr(shadow->szFolderName, _T('\\'));
    river_iter = pIsonymHmap->find(++folder);
    if (river_iter != pIsonymHmap->end()) {
        fshado = river_iter->second;
        if(shadow == fshado) {
            pIsonymHmap->erase(river_iter);
            if(shadow->isonym_node)
                pIsonymHmap->insert(FolderHmap::value_type(folder, shadow->isonym_node));
        } else {
            prev = fshado;
            while(fshado = fshado->isonym_node) {
                if(shadow == fshado) {
                    prev->isonym_node = fshado->isonym_node;
                    break;
                }
                prev = fshado;
            }
        }
    }
    //
    return 0;
}

DWORD FolderUtility::FillIdleEntry(HANDLE hRiverFolder)
{
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
    return 0;
}

static ULONG FindNextEntry(HANDLE hRiverFolder, ULONG lNextIndex)
{
    struct FolderEntry tFolderEntry;
    ULONG lLeafIndex;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lNextIndex))
        return INVA_INDE_VALU;
    if(INVA_INDE_VALU == tFolderEntry.child && INVA_INDE_VALU == tFolderEntry.recycled)
        return lNextIndex;
    //
    lLeafIndex = FindNextEntry(hRiverFolder, tFolderEntry.child);
    if(INVA_INDE_VALU != lLeafIndex) {
        if(lLeafIndex == tFolderEntry.child) {
            tFolderEntry.child = INVA_INDE_VALU;
            FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lNextIndex);
        }
        return lLeafIndex;
    }
    //
    lLeafIndex = FindNextEntry(hRiverFolder, tFolderEntry.recycled);
    if(INVA_INDE_VALU != lLeafIndex) {
        if(lLeafIndex == tFolderEntry.recycled) {
            tFolderEntry.recycled = INVA_INDE_VALU;
            FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lNextIndex);
        }
        return lLeafIndex;
    }
    //
    return INVA_INDE_VALU;
}

ULONG FolderUtility::FindIdleEntry(HANDLE hRiverFolder)
{
    struct FolderEntry tFillEntry;
    ULONG lIdleOffset, lIdleIndex = 0;
    DWORD rlen, wlen;
    //
    lIdleOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;

    if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        lIdleIndex = FindNextEntry(hRiverFolder, tFillEntry.recycled);
        //
        if(tFillEntry.recycled == lIdleIndex) {
            tFillEntry.recycled = INVA_INDE_VALU;
            OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
            OverLapped.Offset = lIdleOffset;
            if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
                return INVA_INDE_VALU;
        }
    } else {
        if(FillIdleEntry(hRiverFolder)) return INVA_INDE_VALU;
        lIdleIndex = lIdleOffset / sizeof(struct FolderEntry);
    }
    //
    return lIdleIndex;
}

ULONG FolderUtility::AddIdleEntry(HANDLE hRiverFolder, ULONG lIdleIndex)
{
    struct FolderEntry tFillEntry;
    DWORD rlen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    ModifyNode(hRiverFolder, lIdleIndex, FODE_INDEX_RECYCLED, tFillEntry.recycled);
    //
    tFillEntry.recycled = lIdleIndex;
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lIdleIndex;
}

ULONG FolderUtility::ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderIndex)
{
    DWORD rlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, pFolderEntry, sizeof(struct FolderEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lFolderIndex;
}

ULONG FolderUtility::WriteNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderIndex)
{
    DWORD wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
    if(!WriteFile(hRiverFolder, pFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lFolderIndex;
}

ULONG FolderUtility::ModifyNode(HANDLE hRiverFolder, ULONG lFolderIndex, DWORD dwIndexType, ULONG lNextIndex)
{
    struct FolderEntry tFolderEntry;
    ULONG lExistsIndex = INVA_INDE_VALU;
    DWORD rlen, wlen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    switch(dwIndexType) {
    case FODE_INDEX_CHILD:
        lExistsIndex = tFolderEntry.child;
        tFolderEntry.child = lNextIndex;
        break;
    case FODE_INDEX_SIBLING: // FODE_INDEX_RECYCLED
        lExistsIndex = tFolderEntry.sibling; // recycled
        tFolderEntry.sibling = lNextIndex; // recycled
        break;
    case FODE_INDEX_LEAF:
        lExistsIndex = tFolderEntry.leaf_inde;
        tFolderEntry.leaf_inde = lNextIndex;
        break;
    }
    //
    if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistsIndex;
}

ULONG FolderUtility::VoidNode(HANDLE hRiverFolder, ULONG lFolderIndex)
{
    DWORD rlen, wlen;
    struct FolderEntry tFolderEntry;
    ULONG lExistsIndex = INVA_INDE_VALU;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &rlen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    memset(&tFolderEntry.ftCreationTime, '\0', sizeof(FILETIME));
    if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lExistsIndex;
}