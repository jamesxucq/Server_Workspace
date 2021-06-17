#include "StdAfx.h"
#include "CommonUtility.h"

#include "FilesRiver.h"
#include "RiverFolder.h"
#include "../../clientcom/utility/ulog.h"

CRiverFolder::CRiverFolder(void)
:m_hRiverFolder(INVALID_HANDLE_VALUE)
{
	MKZERO(m_szRiverFolder);
}

CRiverFolder::~CRiverFolder(void) {
}

CRiverFolder objRiverFolder;

DWORD CRiverFolder::Initialize(const TCHAR *szLocation) {
	CommonUtility::get_name(m_szRiverFolder, szLocation, RIVER_FOLDER_DEFAULT);
	m_hRiverFolder = CreateFile( m_szRiverFolder, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hRiverFolder == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}
	//////////////////////////////
	if(FolderIndex::InitalFolderIndex(&m_hmRiverFolder, &m_hmNsakeFolder, 
		&m_hmFolderIndex, &m_vRiverFolder, m_hRiverFolder)) 
		return -1;

	return 0;
}

DWORD CRiverFolder::Finalize() {
	FolderIndex::FinalFolderIndex(&m_hmRiverFolder, &m_hmNsakeFolder, 
		&m_hmFolderIndex, &m_vRiverFolder);

	if(m_hRiverFolder != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hRiverFolder );
		m_hRiverFolder = INVALID_HANDLE_VALUE;
	}

	return 0;
}

DWORD CRiverFolder::IsDirectory(const TCHAR *szFolderPath) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;

	river_iter = m_hmRiverFolder.find(szFolderPath);
	if(river_iter != m_hmRiverFolder.end()) return 1;

	return 0;	
}

DWORD CRiverFolder::InsEntry(const TCHAR *szFolderPath, FILETIME *ftCreationTime, DWORD lParentIndex) {
	struct FolderEntry tFolderEntry;

	DWORD lEmptyIndex = FolderUtility::FindEmptyEntry(m_hRiverFolder);
	/////////////////////////////////////////////////////
	DWORD lChildIndex = FolderUtility::ModifyNode(m_hRiverFolder, lParentIndex, FILE_INDEX_CHILD, lEmptyIndex);
	///////////////////////////////
	INIT_FOLDER_ENTRY(tFolderEntry)
	_tcscpy_s(tFolderEntry.szFolderName, szFolderPath);
	tFolderEntry.ftCreationTime = *ftCreationTime;
	tFolderEntry.parent = lParentIndex;
	tFolderEntry.child = INVALID_INDEX_VALUE;
	tFolderEntry.sibling = lChildIndex;
	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, lEmptyIndex))
		return -1;
	/////////////////////////////////////////////////////
	tFolderEntry.parent = (DWORD)FolderShadowPtr(lParentIndex);
	tFolderEntry.child = NULL;
	tFolderEntry.sibling = (DWORD)FolderShadowPtr(lChildIndex);
	struct FolderShadow *shadow = NFolderVec::AddNewNode(&m_vRiverFolder, &tFolderEntry, lEmptyIndex);
	if(!shadow) return -1;
	shadow->parent->child = shadow;
	FolderIndex::BuildingIndex(&m_hmRiverFolder, &m_hmNsakeFolder, &m_hmFolderIndex, shadow);
	/////////////////////////////////////////////////////
	return 0;	
}

VOID CRiverFolder::DelFolderEntry(struct FolderShadow *shadow) {
	FolderIndex::IndexDelete(&m_hmRiverFolder, &m_hmNsakeFolder, &m_hmFolderIndex, shadow);
	NFolderVec::DeleteFromVec(&m_vRiverFolder, shadow);
	////////////////////////////////////////////////////
	objFilesRiver.DelFolderEntry(shadow->leaf_index);
	NFolderVec::DeleteNode(shadow);
}

VOID CRiverFolder::EntryDelete(struct FolderShadow *shadow) {
	if(!shadow) return;
	/////////////////////////////////////
	EntryDelete(shadow->child);
	EntryDelete(shadow->sibling);
	/////////////////////////////////////
	DelFolderEntry(shadow);
}

DWORD CRiverFolder::EntryEraseRecu(const TCHAR *szFolderPath) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	struct FolderEntry tFolderEntry;
	struct FolderShadow *fshadow, *prev_shadow;

	river_iter = m_hmRiverFolder.find(szFolderPath);
	if(river_iter != m_hmRiverFolder.end()) {
		fshadow = river_iter->second;
		FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, fshadow->index);
		if(fshadow == fshadow->parent->child) {
			FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FILE_INDEX_CHILD, tFolderEntry.sibling);
			fshadow->parent->child = fshadow->sibling;
		} else {
			prev_shadow = fshadow->parent->child;
			while(prev_shadow->sibling != fshadow) prev_shadow = prev_shadow->sibling;
			///////////////////////////
			if(fshadow->sibling)
				FolderUtility::ModifyNode(m_hRiverFolder, prev_shadow->index, FILE_INDEX_SIBLING, fshadow->sibling->index);
			else FolderUtility::ModifyNode(m_hRiverFolder, prev_shadow->index, FILE_INDEX_SIBLING, INVALID_INDEX_VALUE);
			prev_shadow->sibling = fshadow->sibling;
		}
		/////////////////////////////////////////////////////
		if(INVALID_INDEX_VALUE == FolderUtility::AddEmptyEntry(m_hRiverFolder, river_iter->second->index))
			return -1;
		/////////////////////////////////////////////////////
		EntryDelete(fshadow->child);
		DelFolderEntry(fshadow);
		/////////////////////////////////////////////////////
	}

	return 0;
}

static DWORD FindLastEntry(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, const TCHAR *szFolderPath, FILETIME *ftCreationTime) {
	struct FolderEntry tFillEntry;
	DWORD lEmptyIndex;
	DWORD rlen, wlen;
	////////////////////////////////////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	lEmptyIndex = tFillEntry.sibling;
	//////////////////////////////////////////
	struct FolderEntry tEmptyEntry;
	FolderUtility::ReadNode(hRiverFolder, &tEmptyEntry, lEmptyIndex);
	if(pFolderEntry) memcpy(pFolderEntry, &tEmptyEntry, sizeof(struct FolderEntry));
	//////////////////////////////////////////
	wchar_t *fileName1 = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
	wchar_t *fileName2 = _tcsrchr((TCHAR *)tEmptyEntry.szFolderName, _T('\\'));
	if(!memcmp(&tEmptyEntry.ftCreationTime, ftCreationTime, sizeof(FILETIME)) && !_tcscmp(fileName1, fileName2)) {
		tFillEntry.sibling = tEmptyEntry.sibling;
		//////////////////////////////////////////	
		OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		OverLapped.Offset = lEmptyOffset;
		if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
			return INVALID_INDEX_VALUE;
	} else lEmptyIndex = INVALID_INDEX_VALUE;
	////////////////////////////////////////////////////////////////////////////

	return lEmptyIndex;
}

// children rename
static VOID ChildrenRename(TCHAR *szExistsPath, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	TCHAR szPathName[MAX_PATH];
	_tcscpy_s(szPathName, MAX_PATH, szExistsPath+dwExistsLength);
	_tcscpy_s(szExistsPath, MAX_PATH, szFolderPath);
	_tcscat_s(szExistsPath, MAX_PATH, szPathName);
}

struct FolderShadow *CRiverFolder::RebuildFolderEntryI(struct FolderEntry *pFolderEntry, DWORD lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	struct FolderEntry tFolderEntry;
	struct FolderShadow *fshadow;
	/////////////////////////////////////	
	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, lFolderIndex)) 
		return NULL;
	ChildrenRename(tFolderEntry.szFolderName, dwExistsLength, szFolderPath);
	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, lFolderIndex)) 
		return NULL;
	memcpy(pFolderEntry, &tFolderEntry, sizeof(struct FolderEntry));
	/////////////////////////////////////
	tFolderEntry.parent = (DWORD)parent;
	tFolderEntry.sibling = (DWORD)parent->child;
	fshadow = parent->child = NFolderVec::AddNewNode(&m_vRiverFolder, &tFolderEntry, lFolderIndex);
	/////////////////////////////////////
	objFilesRiver.EntryRestore(fshadow->leaf_index);
	FolderIndex::BuildingIndex(&m_hmRiverFolder, &m_hmNsakeFolder, &m_hmFolderIndex, fshadow);
	/////////////////////////////////////
	return fshadow;
}

struct FolderShadow *CRiverFolder::EntryRebuildI(DWORD lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	if(INVALID_INDEX_VALUE == lFolderIndex) return NULL;
	/////////////////////////////////////
	struct FolderEntry tFolderEntry;
	FolderShadow *fshadow = RebuildFolderEntryI(&tFolderEntry, lFolderIndex, parent, dwExistsLength, szFolderPath);
	/////////////////////////////////////
	fshadow->child = EntryRebuildI(tFolderEntry.child, fshadow, dwExistsLength, szFolderPath);
	EntryRebuildI(tFolderEntry.sibling, fshadow, dwExistsLength, szFolderPath);

	return fshadow;
}

DWORD CRiverFolder::EntryRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime, DWORD lParentIndex) {
	struct FolderEntry tFolderEntry;
	DWORD lEmptyIndex;
	/////////////////////////////////////
	lEmptyIndex = FindLastEntry(m_hRiverFolder, &tFolderEntry, szFolderPath, ftCreationTime);
	if(INVALID_INDEX_VALUE == lEmptyIndex) return -1;
	tFolderEntry.parent = lParentIndex;
	tFolderEntry.sibling = FolderUtility::ModifyNode(m_hRiverFolder, lParentIndex, FILE_INDEX_CHILD, lEmptyIndex);
	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, lEmptyIndex)) 
		return -1;
	/////////////////////////////////////////////////////
	FolderShadow *fshadow;
	DWORD dwExistsLength = _tcslen(tFolderEntry.szFolderName);
	fshadow = RebuildFolderEntryI(&tFolderEntry, lEmptyIndex, FolderShadowPtr(lParentIndex), dwExistsLength, szFolderPath);
	/////////////////////////////////////
	fshadow->child = EntryRebuildI(tFolderEntry.child, fshadow, dwExistsLength, szFolderPath);
	/////////////////////////////////////
	return 0;
}

VOID CRiverFolder::RebuildFolderEntryII(struct FolderShadow *fshadow, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	struct FolderEntry tFolderEntry;
	TCHAR *szExistsName;
	/////////////////////////////////////	
	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, fshadow->index)) 
		return ;
	/////////////////////////////////////	
	szExistsName = fshadow->szFolderName;
	ChildrenRename(tFolderEntry.szFolderName, dwExistsLength, szFolderPath);
	fshadow->szFolderName = _tcsdup(tFolderEntry.szFolderName);
	/////////////////////////////////////	
	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, fshadow->index)) 
		return ;
	/////////////////////////////////////
	FolderIndex::RebuildIndex(&m_hmRiverFolder, szExistsName, fshadow);
	free(szExistsName);
	/////////////////////////////////////
}

VOID CRiverFolder::EntryRebuildII(struct FolderShadow *fshadow, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	if(!fshadow) return;
	/////////////////////////////////////
	EntryRebuildII(fshadow->child, dwExistsLength, szFolderPath);
	EntryRebuildII(fshadow->sibling, dwExistsLength, szFolderPath);
	/////////////////////////////////////
	RebuildFolderEntryII(fshadow, dwExistsLength, szFolderPath);
}

DWORD CRiverFolder::MoveEntry(const TCHAR *szExistsPath, const TCHAR *szMovePath, DWORD lParentIndex) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	struct FolderEntry tFolderEntry;
	FolderShadow *shadow, *prev_shadow;

	river_iter = m_hmRiverFolder.find(szExistsPath);
	if(river_iter != m_hmRiverFolder.end()) {
		///////////////////////////
		shadow = river_iter->second;
		FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, shadow->index);
		if(shadow == shadow->parent->child) {
			FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FILE_INDEX_CHILD, tFolderEntry.sibling);
			shadow->parent->child = shadow->sibling;
		} else {
			prev_shadow = shadow->parent->child;
			while(prev_shadow->sibling != shadow) prev_shadow = prev_shadow->sibling;
			///////////////////////////
			FolderUtility::ModifyNode(m_hRiverFolder, prev_shadow->index, FILE_INDEX_SIBLING, shadow->sibling->index);
			prev_shadow->sibling = shadow->sibling;
		}
		///////////////////////////
		tFolderEntry.sibling = FolderUtility::ModifyNode(m_hRiverFolder, lParentIndex, FILE_INDEX_CHILD, shadow->index);
		tFolderEntry.parent = lParentIndex;
		FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, shadow->index);

		shadow->parent = FolderShadowPtr(lParentIndex);
		shadow->sibling = FolderShadowPtr(tFolderEntry.sibling);
		///////////////////////////
		DWORD dwExistsLength = _tcslen(szExistsPath);
		EntryRebuildII(shadow->child, dwExistsLength, szMovePath);
		RebuildFolderEntryII(shadow, dwExistsLength, szMovePath);
		////////////////////////////////////////////////////////////////
	}

	return 0;
}

DWORD CRiverFolder::FileSiblingIndex(DWORD *lParentIndex, const TCHAR *szFolderPath) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	DWORD lSiblingIndex = INVALID_INDEX_VALUE;

	river_iter = m_hmRiverFolder.find(szFolderPath);
	if(river_iter != m_hmRiverFolder.end()) {
		if(lParentIndex) *lParentIndex = river_iter->second->index;
		lSiblingIndex = river_iter->second->leaf_index;
	}

	return lSiblingIndex;
}

DWORD CRiverFolder::FileSiblingIndex(DWORD lFolderIndex) {
	unordered_map <DWORD, struct FolderShadow *>::iterator index_iter;
	DWORD lSiblingIndex = INVALID_INDEX_VALUE;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end()) {
		lSiblingIndex = index_iter->second->leaf_index;
	}

	return lSiblingIndex;
}

DWORD CRiverFolder::SetChildIndex(DWORD lFolderIndex, DWORD lLeafIndex) {
	unordered_map <DWORD, struct FolderShadow *>::iterator index_iter;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end()) {
		index_iter->second->leaf_index = lLeafIndex;
		FolderUtility::ModifyNode(m_hRiverFolder, lFolderIndex, FILE_INDEX_LEAF, lLeafIndex);
	}

	return 0;
}

DWORD CRiverFolder::FolderNameIndex(TCHAR *szFolderPath, DWORD lFolderIndex) {
	unordered_map <DWORD, struct FolderShadow *>::iterator index_iter;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end())
		_tcscpy_s(szFolderPath, MAX_PATH, index_iter->second->szFolderName);

	return 0;
}

struct FolderShadow *CRiverFolder::FolderShadowPtr(DWORD lFolderIndex) {
	unordered_map <DWORD, struct FolderShadow *>::iterator index_iter;
	FolderShadow *shadow = NULL;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end())
		shadow = index_iter->second;

	return shadow;
}

DWORD CRiverFolder::FolderIndexName(const TCHAR *szFolderPath) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	DWORD lFolderIndex = INVALID_INDEX_VALUE;

	river_iter = m_hmRiverFolder.find(szFolderPath);
	if(river_iter != m_hmRiverFolder.end()) {
		lFolderIndex = river_iter->second->index;
	}

	return lFolderIndex;
}

DWORD CRiverFolder::FindNsake(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime) {
	struct FolderShadow *shadow;
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	DWORD dwFound = 0;

	river_iter = m_hmNsakeFolder.find(szFolderName);
	if(river_iter != m_hmNsakeFolder.end()) {
		shadow = river_iter->second;
		while (shadow) {
			if(!memcmp(&shadow->ftCreationTime, ftCreationTime, sizeof(FILETIME))) {
				_tcscpy_s(szExistsPath, MAX_PATH-ROOT_LENGTH, shadow->szFolderName);
				dwFound = 1;
				break;
			}
			shadow = shadow->nsake_bin;
		}
	}

	return dwFound;
}

DWORD CRiverFolder::FindRecycled(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime) {
	struct FolderEntry tFillEntry;
	DWORD rlen;
	DWORD dwFound = 0;

	DWORD lEmptyOffset = SetFilePointer(m_hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return 0;
	if(!ReadFile(m_hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return 0;
	}
	//////////////////////////////////////////////////////////////////////////////
	if(INVALID_INDEX_VALUE != tFillEntry.sibling) {
		if(INVALID_INDEX_VALUE != FolderUtility::ReadNode(m_hRiverFolder, &tFillEntry, tFillEntry.sibling)) {
			wchar_t *fileName = _tcsrchr((TCHAR *)tFillEntry.szFolderName, _T('\\'));
			if(!memcmp(&tFillEntry.ftCreationTime, ftCreationTime, sizeof(FILETIME)) && !_tcscmp(szFolderName, ++fileName)) {	
				_tcscpy_s(szExistsPath, MAX_PATH-ROOT_LENGTH, tFillEntry.szFolderName);
				dwFound = 1;
			}
		}
	}
	return dwFound;
}

DWORD CRiverFolder::VoidRecycled() {
	struct FolderEntry tFillEntry;
	DWORD rlen;
	//////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(m_hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return -1;
	if(!ReadFile(m_hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}
	//////////////////////////////////////////////
	if(INVALID_INDEX_VALUE != tFillEntry.sibling) 
		FolderUtility::VoidNode(m_hRiverFolder, tFillEntry.sibling);

	return 0;
}