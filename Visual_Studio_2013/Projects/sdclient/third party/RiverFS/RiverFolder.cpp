#include "StdAfx.h"
#include "CommonUtility.h"

// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "INodeUtility.h"
#include "FilesRiver.h"
#include "RiverFolder.h"

CRiverFolder::CRiverFolder(void)
    :m_hRiverFolder(INVALID_HANDLE_VALUE)
{
    MKZEO(m_szRiverFolder);
}

CRiverFolder::~CRiverFolder(void) {
}

CRiverFolder objRiverFolder;

DWORD CRiverFolder::Initialize(const TCHAR *szLocation) {
    comutil::get_name(m_szRiverFolder, szLocation, RIVER_FOLDER_DEFAULT);
    m_hRiverFolder = CreateFile( m_szRiverFolder,
                                 GENERIC_WRITE | GENERIC_READ,
                                 /*NULL,*/ FILE_SHARE_READ,	// share test
                                 NULL,
                                 OPEN_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                 NULL);
    if( INVALID_HANDLE_VALUE == m_hRiverFolder ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    if(FolderIndex::InitalFolderInde(&m_mRiverFolder, &m_mFolderInde, 
									&m_vRiverFolder, m_hRiverFolder))
        return ((DWORD)-1);
    //
    return 0x00;
}

DWORD CRiverFolder::Finalize() {
    FolderIndex::FinalFolderInde(&m_mRiverFolder, &m_mFolderInde, &m_vRiverFolder);
    //
    if(INVALID_HANDLE_VALUE != m_hRiverFolder) {
        CloseHandle( m_hRiverFolder );
        m_hRiverFolder = INVALID_HANDLE_VALUE;
    }

    return 0x00;
}

DWORD CRiverFolder::IsDirectory(const TCHAR *szFolderPath) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(m_mRiverFolder.end() != river_iter) return ((DWORD)-1);
    //
    return 0x00;
}

DWORD CRiverFolder::InsEntry(const TCHAR *szFolderPath, FILETIME *ftCreatTime, FolderShadow *pshado) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(m_mRiverFolder.end() != river_iter) {
// _LOG_DEBUG( _T("inse m_mRiverFolder find:%s"), szFolderPath);
		FolderUtility::CTimeUpdate(m_hRiverFolder, river_iter->second->inde, ftCreatTime);
		return 0x00;
	}
//
    struct FolderEntry tFolderEntry;
// _LOG_DEBUG( _T("inse szFolderPath:%s"), szFolderPath);
    ULONG lIdleInde = FolderUtility::FindIdleEntry(m_hRiverFolder);
    ULONG lChildInde = FolderUtility::ModifyNode(m_hRiverFolder, pshado->inde, FODE_INDE_CHILD, lIdleInde);
    //
    INIT_FOLDER_ENTRY(tFolderEntry)
    _tcscpy_s(tFolderEntry.szFolderName, szFolderPath);
	memcpy(&tFolderEntry.ftCreatTime, ftCreatTime, sizeof(FILETIME));
    tFolderEntry.parent = pshado->inde;
    tFolderEntry.child = INVA_INDE_VALU;
    tFolderEntry.sibling = lChildInde;
    if(INVA_INDE_VALU == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, lIdleInde))
        return ((DWORD)-1);
    //
    tFolderEntry.parent = (DWORD)pshado;
    tFolderEntry.child = NULL;
    tFolderEntry.sibling = (DWORD)pshado->child;
    struct FolderShadow *shadow = NFolderVec::AddNewNode(&m_vRiverFolder, &tFolderEntry, lIdleInde);
    if(!shadow) return ((DWORD)-1);
    pshado->child = shadow;
    FolderIndex::BuildingIndex(&m_mRiverFolder, &m_mFolderInde, shadow);
_LOG_DEBUG( _T("new shadow: %08X pshado: %08X sibling: %08X"), shadow, pshado, shadow->sibling );
    //
    return 0x00;
}

VOID CRiverFolder::DeliFolderEntry(struct FolderShadow *shadow) {
    FolderIndex::IndexDele(&m_mRiverFolder, &m_mFolderInde, shadow);
    NFolderVec::DeleFromVec(&m_vRiverFolder, shadow);
    //
    objFilesRiver.DeliFolderEntry(shadow->leaf_inde);
    NFolderVec::DeleNode(shadow);
}

VOID CRiverFolder::EntryDelete(struct FolderShadow *shadow) {
    if(!shadow) return;
    //
    EntryDelete(shadow->child);
    EntryDelete(shadow->sibling);
    //
    DeliFolderEntry(shadow);
}

DWORD CRiverFolder::EntryEraseRecurs(const TCHAR *szFolderPath) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    struct FolderEntry tFolderEntry;
    struct FolderShadow *fshado, *prev_shado;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(m_mRiverFolder.end() != river_iter) {
// _LOG_DEBUG( _T("erase szFolderPath:%s"), szFolderPath);
        fshado = river_iter->second;
        FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, fshado->inde);
        if(fshado == fshado->parent->child) {
            FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FODE_INDE_CHILD, tFolderEntry.sibling);
            fshado->parent->child = fshado->sibling;
        } else {
            prev_shado = fshado->parent->child;
            while(prev_shado->sibling != fshado) prev_shado = prev_shado->sibling;
            //
            if(fshado->sibling)
                FolderUtility::ModifyNode(m_hRiverFolder, prev_shado->inde, FODE_INDE_SIBLING, fshado->sibling->inde);
            else FolderUtility::ModifyNode(m_hRiverFolder, prev_shado->inde, FODE_INDE_SIBLING, INVA_INDE_VALU);
            prev_shado->sibling = fshado->sibling;
        }
        //
        if(INVA_INDE_VALU == FolderUtility::AddIdleEntry(m_hRiverFolder, fshado->inde))
            return ((DWORD)-1);
        //
        EntryDelete(fshado->child);
        DeliFolderEntry(fshado);
    }
    //
    return 0x00;
}

// children rename
static VOID ChildrenRename(TCHAR *szExistPath, DWORD dwExistLength, const TCHAR *szFolderPath) {
    TCHAR szPathName[MAX_PATH];
    _tcscpy_s(szPathName, MAX_PATH, szExistPath+dwExistLength);
    _tcscpy_s(szExistPath, MAX_PATH, szFolderPath);
    _tcscat_s(szExistPath, MAX_PATH, szPathName);
}

VOID CRiverFolder::RebuildFolderEntry(struct FolderShadow *fshado, DWORD dwExistLength, const TCHAR *szFolderPath) {
    struct FolderEntry tFolderEntry;
    TCHAR *szExistName;
    //
    if(INVA_INDE_VALU == FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, fshado->inde))
        return ;
    //
    szExistName = fshado->szFolderName;
    ChildrenRename(tFolderEntry.szFolderName, dwExistLength, szFolderPath);
    fshado->szFolderName = _tcsdup(tFolderEntry.szFolderName);
    //
    if(INVA_INDE_VALU == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, fshado->inde))
        return ;
    //
    FolderIndex::RebuildIndex(&m_mRiverFolder, szExistName, fshado);
    free(szExistName);
}

VOID CRiverFolder::EntryRebuild(struct FolderShadow *fshado, DWORD dwExistLength, const TCHAR *szFolderPath) {
    if(!fshado) return;
    //
    EntryRebuild(fshado->child, dwExistLength, szFolderPath);
    EntryRebuild(fshado->sibling, dwExistLength, szFolderPath);
    //
    RebuildFolderEntry(fshado, dwExistLength, szFolderPath);
}

DWORD CRiverFolder::MoveEntry(const TCHAR *szExistPath, const TCHAR *szNewFile, FolderShadow *pshado) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    struct FolderEntry tFolderEntry;
    FolderShadow *shadow, *prev_shado;
    //
    river_iter = m_mRiverFolder.find(szExistPath);
    if(m_mRiverFolder.end() != river_iter) {
        shadow = river_iter->second;
        FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, shadow->inde);
        if(shadow == shadow->parent->child) {
            FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FODE_INDE_CHILD, tFolderEntry.sibling);
            shadow->parent->child = shadow->sibling;
        } else {
            prev_shado = shadow->parent->child;
            while(prev_shado->sibling != shadow) prev_shado = prev_shado->sibling;
            //
            FolderUtility::ModifyNode(m_hRiverFolder, prev_shado->inde, FODE_INDE_SIBLING, shadow->sibling->inde);
            prev_shado->sibling = shadow->sibling;
        }
        //
        tFolderEntry.sibling = FolderUtility::ModifyNode(m_hRiverFolder, pshado->inde, FODE_INDE_CHILD, shadow->inde);
		pshado->child = shadow;
        tFolderEntry.parent = pshado->inde;
        FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, shadow->inde);
		//
        shadow->parent = pshado;
        shadow->sibling = FolderShadowPtr(tFolderEntry.sibling);
        //
        DWORD dwExistLength = _tcslen(szExistPath);
        EntryRebuild(shadow->child, dwExistLength, szNewFile);
        RebuildFolderEntry(shadow, dwExistLength, szNewFile);
    }
    //
    return 0x00;
}

ULONG CRiverFolder::FileSiblingIndex(ULONG *lFolderInde, const TCHAR *szFolderPath) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    ULONG lSibliInde = INVA_INDE_VALU;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(m_mRiverFolder.end() != river_iter) {
        *lFolderInde = river_iter->second->inde;
        lSibliInde = river_iter->second->leaf_inde;
    } else *lFolderInde = INVA_INDE_VALU;
    //
    return lSibliInde;
}

ULONG CRiverFolder::FileSiblingIndex(ULONG lFolderInde) {
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    ULONG lSibliInde = INVA_INDE_VALU;
    //
    inde_iter = m_mFolderInde.find(lFolderInde);
    if(m_mFolderInde.end() != inde_iter) {
        lSibliInde = inde_iter->second->leaf_inde;
    }
    //
    return lSibliInde;
}

DWORD CRiverFolder::SetChildIndex(ULONG lFolderInde, ULONG lLeafInde) {
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    //
    inde_iter = m_mFolderInde.find(lFolderInde);
    if(m_mFolderInde.end() != inde_iter) {
        inde_iter->second->leaf_inde = lLeafInde;
        FolderUtility::ModifyNode(m_hRiverFolder, lFolderInde, FODE_INDE_LEAF, lLeafInde);
    }
    //
    return 0x00;
}

DWORD CRiverFolder::FolderNameIndex(TCHAR *szFolderPath, ULONG lFolderInde) { // 0x00:found !0:not found
	DWORD dwFound = ((DWORD)-1);
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    //
    inde_iter = m_mFolderInde.find(lFolderInde);
	if(m_mFolderInde.end() != inde_iter) {
        _tcscpy_s(szFolderPath, MAX_PATH, inde_iter->second->szFolderName);	
		dwFound = 0x00;
	}
    //
    return dwFound;
}

struct FolderShadow *CRiverFolder::FolderShadowPtr(ULONG lFolderInde) {
    unordered_map <ULONG, struct FolderShadow *>::iterator inde_iter;
    FolderShadow *shadow = NULL;
    //
    inde_iter = m_mFolderInde.find(lFolderInde);
    if(m_mFolderInde.end() != inde_iter)
        shadow = inde_iter->second;
    //
    return shadow;
}

struct FolderShadow *CRiverFolder::FolderShadowName(const TCHAR *szFolderPath) {
    unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
    struct FolderShadow *shadow = NULL;
    //
    river_iter = m_mRiverFolder.find(szFolderPath);
    if(m_mRiverFolder.end() != river_iter) {
        shadow = river_iter->second;
    }
    //
    return shadow;
}

DWORD CRiverFolder::FindSibliAppA(HANDLE hINodeA, struct FolderShadow *sibling) {
	FolderShadow *pshado;
	//
	pshado = sibling;
	while(pshado) {
		INodeUtili::INodeAApp(hINodeA, pshado->szFolderName, INTYPE_DIRECTORY);
_LOG_DEBUG( _T("---- folder add:%s"), pshado->szFolderName);
		pshado = pshado->sibling;
	}
	//
	return 0x00;
}

DWORD CRiverFolder::FindSibliAppV(HANDLE hINodeV, struct FolderShadow *sibling) {
	FolderShadow *pshado;
	//
	pshado = sibling;
	while(pshado) {
		INodeUtili::FolderAdd(hINodeV, pshado->szFolderName);
// _LOG_DEBUG( _T("---- folder add:%s"), pshado->szFolderName);
		pshado = pshado->sibling;
	}
	//
	return 0x00;
}

DWORD CRiverFolder::FindFolderName(TCHAR *szFolderPath, ULONG lFolderInde, ULONG lFileInde) { // 0x00:found !0x00:not found
	if(INVA_INDE_VALU == lFolderInde) return ((DWORD)-1);
	if(!objRiverFolder.FolderNameIndex(szFolderPath, lFolderInde))
		return 0x00;
	//
// _LOG_DEBUG(_T("xxx 1"));
	if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1);
	DWORD dwFound = ((DWORD)-1);
	struct FolderEntry tFillEntry;
    if(INVA_INDE_VALU != FolderUtility::ReadRecyEntry(m_hRiverFolder, &tFillEntry, lFolderInde)) {
		if(!objFilesRiver.IsRecycledEntry(tFillEntry.leaf_inde, lFileInde)) {
			_tcscpy_s(szFolderPath, MAX_PATH, tFillEntry.szFolderName);	
			dwFound = 0x00;
		}
    }
// _LOG_DEBUG(_T("xxx x"));
	//
	return dwFound;
}

VOID CRiverFolder::CTimeUpdate(const TCHAR *szFolderPath, const TCHAR *szDriveLetter) {
	FILETIME ftCreatTime = {0};
	TCHAR szFolderName[MAX_PATH];
	FolderShadow *pshado;
	//
	pshado = objRiverFolder.FolderShadowName(szFolderPath);
_LOG_DEBUG(_T("in ctime update.pshado->ftCreatTime:%s ftCreatTime:%s"), comutil::ftim_unis(&pshado->ftCreatTime), comutil::ftim_unis(&ftCreatTime));
	if(!memcmp(&ftCreatTime, &pshado->ftCreatTime, sizeof(FILETIME))) {
		ABSOLU_PATH(szFolderName, szDriveLetter, szFolderPath)
		comutil::CreationTime(&ftCreatTime, szFolderName);
		memcpy(&pshado->ftCreatTime, &ftCreatTime, sizeof(FILETIME));
		FolderUtility::CTimeUpdate(m_hRiverFolder, pshado->inde, &ftCreatTime);
_LOG_DEBUG(_T("update ctime:%s"), comutil::ftim_unis(&ftCreatTime));
	}
}