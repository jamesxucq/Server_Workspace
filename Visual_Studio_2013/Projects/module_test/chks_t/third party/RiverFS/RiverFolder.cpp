#include "StdAfx.h"
#include "CommonUtility.h"

// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "FilesRiver.h"
#include "RiverFolder.h"

CRiverFolder::CRiverFolder(void)
    :m_hRiverFolder(INVALID_HANDLE_VALUE)
{
    MKZERO(m_szRiverFolder);
}

CRiverFolder::~CRiverFolder(void)
{
}

CRiverFolder objRiverFolder;

DWORD CRiverFolder::Initialize(const TCHAR *szLocation)
{
    CommonUtility::get_name(m_szRiverFolder, szLocation, RIVER_FOLDER_DEFAULT);
    m_hRiverFolder = CreateFile( m_szRiverFolder,
                                 GENERIC_WRITE | GENERIC_READ,
                                 NULL, /* FILE_SHARE_READ */
                                 NULL,
                                 OPEN_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                 NULL);
    if( INVALID_HANDLE_VALUE == m_hRiverFolder ) {
        TRACE( _T("create file failed: %d\n"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    if(FolderIndex::InitalFolderIndex(&m_mRiverFolder, &m_mIsonymFolder,
                                      &m_mFolderIndex, &m_vRiverFolder, m_hRiverFolder))
        return ((DWORD)-1);
    //
    return 0;
}

DWORD CRiverFolder::Finalize()
{
    FolderIndex::FinalFolderIndex(&m_mRiverFolder, &m_mIsonymFolder,
                                  &m_mFolderIndex, &m_vRiverFolder);
    //
    if(INVALID_HANDLE_VALUE != m_hRiverFolder) {
        CloseHandle( m_hRiverFolder );
        m_hRiverFolder = INVALID_HANDLE_VALUE;
    }

    return 0;
}

DWORD CRiverFolder::IsDirectory(const TCHAR *szFolderPath)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(river_iter != m_mRiverFolder.end()) return 0x01;
    //
    return 0;
}

DWORD CRiverFolder::InsEntry(const TCHAR *szFolderPath, FILETIME *ftCreationTime, FolderShadow *pshado)
{
    struct FolderEntry tFolderEntry;
    //
    ULONG lIdleIndex = FolderUtility::FindIdleEntry(m_hRiverFolder);
    ULONG lChildIndex = FolderUtility::ModifyNode(m_hRiverFolder, pshado->inde, FODE_INDEX_CHILD, lIdleIndex);
    //
    INIT_FOLDER_ENTRY(tFolderEntry)
    _tcscpy_s(tFolderEntry.szFolderName, szFolderPath);
    tFolderEntry.ftCreationTime = *ftCreationTime;
    tFolderEntry.parent = pshado->inde;
    tFolderEntry.child = INVA_INDE_VALU;
    tFolderEntry.sibling = lChildIndex;
    if(INVA_INDE_VALU == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, lIdleIndex))
        return ((DWORD)-1);
    //
    tFolderEntry.parent = (DWORD)pshado;
    tFolderEntry.child = NULL;
    tFolderEntry.sibling = (DWORD)FolderShadowPtr(lChildIndex);
    struct FolderShadow *shadow = NFolderVec::AddNewNode(&m_vRiverFolder, &tFolderEntry, lIdleIndex);
    if(!shadow) return ((DWORD)-1);
    pshado->child = shadow;
    FolderIndex::BuildingIndex(&m_mRiverFolder, &m_mIsonymFolder, &m_mFolderIndex, shadow);
    //
    return 0;
}

VOID CRiverFolder::DelFolderEntry(struct FolderShadow *shadow)
{
    FolderIndex::IndexDelete(&m_mRiverFolder, &m_mIsonymFolder, &m_mFolderIndex, shadow);
    NFolderVec::DeleteFromVec(&m_vRiverFolder, shadow);
    //
    objFilesRiver.DelFolderEntry(shadow->leaf_inde);
    NFolderVec::DeleteNode(shadow);
}

VOID CRiverFolder::EntryDelete(struct FolderShadow *shadow)
{
    if(!shadow) return;
    //
    EntryDelete(shadow->child);
    EntryDelete(shadow->sibling);
    //
    DelFolderEntry(shadow);
}

DWORD CRiverFolder::EntryEraseRecu(const TCHAR *szFolderPath)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    struct FolderEntry tFolderEntry;
    struct FolderShadow *fshado, *prev_shado;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(river_iter != m_mRiverFolder.end()) {
        fshado = river_iter->second;
        FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, fshado->inde);
        if(fshado == fshado->parent->child) {
            FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FODE_INDEX_CHILD, tFolderEntry.sibling);
            fshado->parent->child = fshado->sibling;
        } else {
            prev_shado = fshado->parent->child;
            while(prev_shado->sibling != fshado) prev_shado = prev_shado->sibling;
            //
            if(fshado->sibling)
                FolderUtility::ModifyNode(m_hRiverFolder, prev_shado->inde, FODE_INDEX_SIBLING, fshado->sibling->inde);
            else FolderUtility::ModifyNode(m_hRiverFolder, prev_shado->inde, FODE_INDEX_SIBLING, INVA_INDE_VALU);
            prev_shado->sibling = fshado->sibling;
        }
        //
        if(INVA_INDE_VALU == FolderUtility::AddIdleEntry(m_hRiverFolder, fshado->inde))
            return ((DWORD)-1);
        //
        EntryDelete(fshado->child);
        DelFolderEntry(fshado);
        //
    }
    //
    return 0;
}

static ULONG FindLastEntry(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, const TCHAR *szFolderPath, FILETIME *ftCreationTime)
{
    struct FolderEntry tFillEntry;
    ULONG lIdleIndex;
    DWORD rlen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    //
    if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    lIdleIndex = tFillEntry.recycled;
    //
    struct FolderEntry tIdleEntry;
    FolderUtility::ReadNode(hRiverFolder, &tIdleEntry, lIdleIndex);
    if(pFolderEntry) memcpy(pFolderEntry, &tIdleEntry, sizeof(struct FolderEntry));
    //
    wchar_t *fileName1 = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    wchar_t *fileName2 = _tcsrchr(tIdleEntry.szFolderName, _T('\\'));
    if(!memcmp(&tIdleEntry.ftCreationTime, ftCreationTime, sizeof(FILETIME)) && !_tcscmp(fileName1, fileName2)) {
        tFillEntry.recycled = tIdleEntry.recycled;
        //
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        OverLapped.Offset = lIdleOffset;
        if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
            return INVA_INDE_VALU;
    } else lIdleIndex = INVA_INDE_VALU;
    //
    return lIdleIndex;
}

// children rename
static VOID ChildrenRename(TCHAR *szExistsPath, DWORD dwExistsLength, const TCHAR *szFolderPath)
{
    TCHAR szPathName[MAX_PATH];
    _tcscpy_s(szPathName, MAX_PATH, szExistsPath+dwExistsLength);
    _tcscpy_s(szExistsPath, MAX_PATH, szFolderPath);
    _tcscat_s(szExistsPath, MAX_PATH, szPathName);
}

struct FolderShadow *CRiverFolder::RebuildFolderEntryI(struct FolderEntry *pFolderEntry, ULONG lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath)
{
    struct FolderEntry tFolderEntry;
    struct FolderShadow *fshado;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, lFolderIndex))
        return NULL;
    ChildrenRename(tFolderEntry.szFolderName, dwExistsLength, szFolderPath);
    if(INVA_INDE_VALU == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, lFolderIndex))
        return NULL;
    memcpy(pFolderEntry, &tFolderEntry, sizeof(struct FolderEntry));
    //
    tFolderEntry.parent = (DWORD)parent;
    tFolderEntry.sibling = (DWORD)parent->child;
    fshado = parent->child = NFolderVec::AddNewNode(&m_vRiverFolder, &tFolderEntry, lFolderIndex);
    //
    objFilesRiver.EntryRestore(fshado->leaf_inde);
    FolderIndex::BuildingIndex(&m_mRiverFolder, &m_mIsonymFolder, &m_mFolderIndex, fshado);
    //
    return fshado;
}

struct FolderShadow *CRiverFolder::EntryRebuildI(ULONG lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath)
{
    if(INVA_INDE_VALU == lFolderIndex) return NULL;
    //
    struct FolderEntry tFolderEntry;
    FolderShadow *fshado = RebuildFolderEntryI(&tFolderEntry, lFolderIndex, parent, dwExistsLength, szFolderPath);
    //
    fshado->child = EntryRebuildI(tFolderEntry.child, fshado, dwExistsLength, szFolderPath);
    EntryRebuildI(tFolderEntry.sibling, fshado, dwExistsLength, szFolderPath);
    //
    return fshado;
}

DWORD CRiverFolder::EntryRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime, FolderShadow *pshado)
{
    struct FolderEntry tFolderEntry;
    ULONG lIdleIndex;
    //
    lIdleIndex = FindLastEntry(m_hRiverFolder, &tFolderEntry, szFolderPath, ftCreationTime);
    if(INVA_INDE_VALU == lIdleIndex) return ((DWORD)-1);
    tFolderEntry.parent = pshado->inde;
    tFolderEntry.sibling = FolderUtility::ModifyNode(m_hRiverFolder, pshado->inde, FODE_INDEX_CHILD, lIdleIndex);
    if(INVA_INDE_VALU == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, lIdleIndex))
        return ((DWORD)-1);
    //
    FolderShadow *fshado;
    DWORD dwExistsLength = _tcslen(tFolderEntry.szFolderName);
    fshado = RebuildFolderEntryI(&tFolderEntry, lIdleIndex, pshado, dwExistsLength, szFolderPath);
    //
    fshado->child = EntryRebuildI(tFolderEntry.child, fshado, dwExistsLength, szFolderPath);
    //
    return 0;
}

VOID CRiverFolder::RebuildFolderEntryII(struct FolderShadow *fshado, DWORD dwExistsLength, const TCHAR *szFolderPath)
{
    struct FolderEntry tFolderEntry;
    TCHAR *szExistsName;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, fshado->inde))
        return ;
    //
    szExistsName = fshado->szFolderName;
    ChildrenRename(tFolderEntry.szFolderName, dwExistsLength, szFolderPath);
    fshado->szFolderName = _tcsdup(tFolderEntry.szFolderName);
    //
    if(INVA_INDE_VALU == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, fshado->inde))
        return ;
    //
    FolderIndex::RebuildIndex(&m_mRiverFolder, szExistsName, fshado);
    free(szExistsName);
}

VOID CRiverFolder::EntryRebuildII(struct FolderShadow *fshado, DWORD dwExistsLength, const TCHAR *szFolderPath)
{
    if(!fshado) return;
    //
    EntryRebuildII(fshado->child, dwExistsLength, szFolderPath);
    EntryRebuildII(fshado->sibling, dwExistsLength, szFolderPath);
    //
    RebuildFolderEntryII(fshado, dwExistsLength, szFolderPath);
}

DWORD CRiverFolder::MoveEntry(const TCHAR *szExistsPath, const TCHAR *szMovePath, FolderShadow *pshado)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    struct FolderEntry tFolderEntry;
    FolderShadow *shadow, *prev_shado;
    //
    river_iter = m_mRiverFolder.find(szExistsPath);
    if(river_iter != m_mRiverFolder.end()) {
        //
        shadow = river_iter->second;
        FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, shadow->inde);
        if(shadow == shadow->parent->child) {
            FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FODE_INDEX_CHILD, tFolderEntry.sibling);
            shadow->parent->child = shadow->sibling;
        } else {
            prev_shado = shadow->parent->child;
            while(prev_shado->sibling != shadow) prev_shado = prev_shado->sibling;
            //
            FolderUtility::ModifyNode(m_hRiverFolder, prev_shado->inde, FODE_INDEX_SIBLING, shadow->sibling->inde);
            prev_shado->sibling = shadow->sibling;
        }
        //
        tFolderEntry.sibling = FolderUtility::ModifyNode(m_hRiverFolder, pshado->inde, FODE_INDEX_CHILD, shadow->inde);
        tFolderEntry.parent = pshado->inde;
        FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, shadow->inde);

        shadow->parent = pshado;
        shadow->sibling = FolderShadowPtr(tFolderEntry.sibling);
        //
        DWORD dwExistsLength = _tcslen(szExistsPath);
        EntryRebuildII(shadow->child, dwExistsLength, szMovePath);
        RebuildFolderEntryII(shadow, dwExistsLength, szMovePath);
        //
    }
    //
    return 0;
}

ULONG CRiverFolder::FileSiblingIndex(ULONG *lParentIndex, const TCHAR *szFolderPath)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    ULONG lSiblingIndex = INVA_INDE_VALU;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(river_iter != m_mRiverFolder.end()) {
        if(lParentIndex) *lParentIndex = river_iter->second->inde;
        lSiblingIndex = river_iter->second->leaf_inde;
    }
    //
    return lSiblingIndex;
}

ULONG CRiverFolder::FileSiblingIndex(ULONG lFolderIndex)
{
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    ULONG lSiblingIndex = INVA_INDE_VALU;
    //
    inde_iter = m_mFolderIndex.find(lFolderIndex);
    if(inde_iter != m_mFolderIndex.end()) {
        lSiblingIndex = inde_iter->second->leaf_inde;
    }
    //
    return lSiblingIndex;
}

DWORD CRiverFolder::SetChildIndex(ULONG lFolderIndex, ULONG lLeafIndex)
{
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    //
    inde_iter = m_mFolderIndex.find(lFolderIndex);
    if(inde_iter != m_mFolderIndex.end()) {
        inde_iter->second->leaf_inde = lLeafIndex;
        FolderUtility::ModifyNode(m_hRiverFolder, lFolderIndex, FODE_INDEX_LEAF, lLeafIndex);
    }
    //
    return 0;
}

DWORD CRiverFolder::FolderNameIndex(TCHAR *szFolderPath, ULONG lFolderIndex)
{
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    //
    inde_iter = m_mFolderIndex.find(lFolderIndex);
    if(inde_iter != m_mFolderIndex.end())
        _tcscpy_s(szFolderPath, MAX_PATH, inde_iter->second->szFolderName);
    //
    return 0;
}

struct FolderShadow *CRiverFolder::FolderShadowPtr(ULONG lFolderIndex)
{
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    FolderShadow *shadow = NULL;
    //
    inde_iter = m_mFolderIndex.find(lFolderIndex);
    if(inde_iter != m_mFolderIndex.end())
        shadow = inde_iter->second;
    //
    return shadow;
}

struct FolderShadow *CRiverFolder::FolderShadowName(const TCHAR *szFolderPath)
{
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    struct FolderShadow *shadow = NULL;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(river_iter != m_mRiverFolder.end()) {
        shadow = river_iter->second;
    }
    //
    return shadow;
}

DWORD CRiverFolder::FindIsonym(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime)
{
    struct FolderShadow *shadow;
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    DWORD bFound = FALSE;
    //
    river_iter = m_mIsonymFolder.find(szFolderName);
    if(river_iter != m_mIsonymFolder.end()) {
        shadow = river_iter->second;
        while (shadow) {
            if(!memcmp(&shadow->ftCreationTime, ftCreationTime, sizeof(FILETIME))) {
                _tcscpy_s(szExistsPath, MAX_PATH-ROOT_LENGTH, shadow->szFolderName);
                bFound = TRUE;
                break;
            }
            shadow = shadow->isonym_node;
        }
    }
    //
    return bFound;
}

DWORD CRiverFolder::FindRecycled(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime)
{
    struct FolderEntry tFillEntry;
    DWORD rlen;
    BOOL bFound = FALSE;
    //
    ULONG lIdleOffset = SetFilePointer(m_hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return 0;
    if(!ReadFile(m_hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return 0;
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        if(INVA_INDE_VALU != FolderUtility::ReadNode(m_hRiverFolder, &tFillEntry, tFillEntry.recycled)) {
            wchar_t *fileName = _tcsrchr(tFillEntry.szFolderName, _T('\\'));
            if(!memcmp(&tFillEntry.ftCreationTime, ftCreationTime, sizeof(FILETIME)) && !_tcscmp(szFolderName, ++fileName)) {
                _tcscpy_s(szExistsPath, MAX_PATH-ROOT_LENGTH, tFillEntry.szFolderName);
                bFound = 1;
            }
        }
    }
    return bFound;
}

DWORD CRiverFolder::VoidRecycled()
{
    struct FolderEntry tFillEntry;
    DWORD rlen;
    //
    ULONG lIdleOffset = SetFilePointer(m_hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!ReadFile(m_hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return ((DWORD)-1);
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled)
        FolderUtility::VoidNode(m_hRiverFolder, tFillEntry.recycled);
    //
    return 0;
}