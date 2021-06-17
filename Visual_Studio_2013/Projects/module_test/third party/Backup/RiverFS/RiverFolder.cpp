#include "StdAfx.h"
#include "CommonUtility.h"

#include "FilesRiver.h"
#include "RiverFolder.h"
#include "../../clientcom/utility/ulog.h"

CRiverFolder::CRiverFolder(void)
:m_hRiverFolder(INVALID_HANDLE_VALUE)
{
	memset(m_szRiverFolder, '\0', MAX_PATH);
}

CRiverFolder::~CRiverFolder(void) {
}

CRiverFolder objRiverFolder;

DWORD CRiverFolder::Initialize(const TCHAR *szLocation) {
	CommonUtility::get_name(m_szRiverFolder, szLocation, RIVER_DIRECTORY_DEFAULT);
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
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;

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
	tFolderEntry.parent = (DWORD)FolderSoulPtr(lParentIndex);
	tFolderEntry.child = NULL;
	tFolderEntry.sibling = (DWORD)FolderSoulPtr(lChildIndex);
	struct FolderSoul *folderSoul = NFolderVec::AddNewNode(&m_vRiverFolder, &tFolderEntry, lEmptyIndex);
	if(!folderSoul) return -1;
	folderSoul->parent->child = folderSoul;
	FolderIndex::BuildingIndex(&m_hmRiverFolder, &m_hmNsakeFolder, &m_hmFolderIndex, folderSoul);
	/////////////////////////////////////////////////////
	return 0;	
}

void CRiverFolder::DelFolderEntry(struct FolderSoul *folderSoul) {
	FolderIndex::IndexDelete(&m_hmRiverFolder, &m_hmNsakeFolder, &m_hmFolderIndex, folderSoul);
	NFolderVec::DeleteFromVec(&m_vRiverFolder, folderSoul);
	////////////////////////////////////////////////////
	objFilesRiver.DelFolderEntry(folderSoul->leaf_index);
	NFolderVec::DeleteNode(folderSoul);
}

void CRiverFolder::EntryDelete(struct FolderSoul *folderSoul) {
	if(!folderSoul) return;
	/////////////////////////////////////
	EntryDelete(folderSoul->child);
	EntryDelete(folderSoul->sibling);
	/////////////////////////////////////
	DelFolderEntry(folderSoul);
}

DWORD CRiverFolder::EntryEraseRecu(const TCHAR *szFolderPath) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	struct FolderEntry tFolderEntry;
	struct FolderSoul *folder_soul, *prev_soul;

	river_iter = m_hmRiverFolder.find(szFolderPath);
	if(river_iter != m_hmRiverFolder.end()) {
		folder_soul = river_iter->second;
		FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, folder_soul->index);
		if(folder_soul == folder_soul->parent->child) {
			FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FILE_INDEX_CHILD, tFolderEntry.sibling);
			folder_soul->parent->child = folder_soul->sibling;
		} else {
			prev_soul = folder_soul->parent->child;
			while(prev_soul->sibling != folder_soul) prev_soul = prev_soul->sibling;
			///////////////////////////
			if(folder_soul->sibling)
				FolderUtility::ModifyNode(m_hRiverFolder, prev_soul->index, FILE_INDEX_SIBLING, folder_soul->sibling->index);
			else FolderUtility::ModifyNode(m_hRiverFolder, prev_soul->index, FILE_INDEX_SIBLING, INVALID_INDEX_VALUE);
			prev_soul->sibling = folder_soul->sibling;
		}
		/////////////////////////////////////////////////////
		if(INVALID_INDEX_VALUE == FolderUtility::AddEmptyEntry(m_hRiverFolder, river_iter->second->index))
			return -1;
		/////////////////////////////////////////////////////
		EntryDelete(folder_soul->child);
		DelFolderEntry(folder_soul);
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
static void ChildrenRename(TCHAR *szExistsPath, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	TCHAR szPathName[MAX_PATH];
	_tcscpy_s(szPathName, MAX_PATH, szExistsPath+dwExistsLength);
	_tcscpy_s(szExistsPath, MAX_PATH, szFolderPath);
	_tcscat_s(szExistsPath, MAX_PATH, szPathName);
}

struct FolderSoul *CRiverFolder::RebuildFolderEntryI(struct FolderEntry *pFolderEntry, DWORD lFolderIndex, FolderSoul *parent, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	struct FolderEntry tFolderEntry;
	struct FolderSoul *folder_soul;
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
	folder_soul = parent->child = NFolderVec::AddNewNode(&m_vRiverFolder, &tFolderEntry, lFolderIndex);
	/////////////////////////////////////
	objFilesRiver.EntryRestore(folder_soul->leaf_index);
	FolderIndex::BuildingIndex(&m_hmRiverFolder, &m_hmNsakeFolder, &m_hmFolderIndex, folder_soul);
	/////////////////////////////////////
	return folder_soul;
}

struct FolderSoul *CRiverFolder::EntryRebuildI(DWORD lFolderIndex, FolderSoul *parent, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	if(INVALID_INDEX_VALUE == lFolderIndex) return NULL;
	/////////////////////////////////////
	struct FolderEntry tFolderEntry;
	FolderSoul *folder_soul = RebuildFolderEntryI(&tFolderEntry, lFolderIndex, parent, dwExistsLength, szFolderPath);
	/////////////////////////////////////
	folder_soul->child = EntryRebuildI(tFolderEntry.child, folder_soul, dwExistsLength, szFolderPath);
	EntryRebuildI(tFolderEntry.sibling, folder_soul, dwExistsLength, szFolderPath);

	return folder_soul;
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
	FolderSoul *folder_soul;
	DWORD dwExistsLength = _tcslen(tFolderEntry.szFolderName);
	folder_soul = RebuildFolderEntryI(&tFolderEntry, lEmptyIndex, FolderSoulPtr(lParentIndex), dwExistsLength, szFolderPath);
	/////////////////////////////////////
	folder_soul->child = EntryRebuildI(tFolderEntry.child, folder_soul, dwExistsLength, szFolderPath);
	/////////////////////////////////////
	return 0;
}

void CRiverFolder::RebuildFolderEntryII(struct FolderSoul *folder_soul, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	struct FolderEntry tFolderEntry;
	TCHAR *szExistsName;
	/////////////////////////////////////	
	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, folder_soul->index)) 
		return ;
	/////////////////////////////////////	
	szExistsName = folder_soul->szFolderName;
	ChildrenRename(tFolderEntry.szFolderName, dwExistsLength, szFolderPath);
	folder_soul->szFolderName = _tcsdup(tFolderEntry.szFolderName);
	/////////////////////////////////////	
	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, folder_soul->index)) 
		return ;
	/////////////////////////////////////
	FolderIndex::RebuildIndex(&m_hmRiverFolder, szExistsName, folder_soul);
	free(szExistsName);
	/////////////////////////////////////
}

void CRiverFolder::EntryRebuildII(struct FolderSoul *folder_soul, DWORD dwExistsLength, const TCHAR *szFolderPath) {
	if(!folder_soul) return;
	/////////////////////////////////////
	EntryRebuildII(folder_soul->child, dwExistsLength, szFolderPath);
	EntryRebuildII(folder_soul->sibling, dwExistsLength, szFolderPath);
	/////////////////////////////////////
	RebuildFolderEntryII(folder_soul, dwExistsLength, szFolderPath);
}

DWORD CRiverFolder::MoveEntry(const TCHAR *szExistsPath, const TCHAR *szMovePath, DWORD lParentIndex) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	struct FolderEntry tFolderEntry;
	FolderSoul *folderSoul, *prev_soul;

	river_iter = m_hmRiverFolder.find(szExistsPath);
	if(river_iter != m_hmRiverFolder.end()) {
		///////////////////////////
		folderSoul = river_iter->second;
		FolderUtility::ReadNode(m_hRiverFolder, &tFolderEntry, folderSoul->index);
		if(folderSoul == folderSoul->parent->child) {
			FolderUtility::ModifyNode(m_hRiverFolder, tFolderEntry.parent, FILE_INDEX_CHILD, tFolderEntry.sibling);
			folderSoul->parent->child = folderSoul->sibling;
		} else {
			prev_soul = folderSoul->parent->child;
			while(prev_soul->sibling != folderSoul) prev_soul = prev_soul->sibling;
			///////////////////////////
			FolderUtility::ModifyNode(m_hRiverFolder, prev_soul->index, FILE_INDEX_SIBLING, folderSoul->sibling->index);
			prev_soul->sibling = folderSoul->sibling;
		}
		///////////////////////////
		tFolderEntry.sibling = FolderUtility::ModifyNode(m_hRiverFolder, lParentIndex, FILE_INDEX_CHILD, folderSoul->index);
		tFolderEntry.parent = lParentIndex;
		FolderUtility::WriteNode(m_hRiverFolder, &tFolderEntry, folderSoul->index);

		folderSoul->parent = FolderSoulPtr(lParentIndex);
		folderSoul->sibling = FolderSoulPtr(tFolderEntry.sibling);
		///////////////////////////
		DWORD dwExistsLength = _tcslen(szExistsPath);
		EntryRebuildII(folderSoul->child, dwExistsLength, szMovePath);
		RebuildFolderEntryII(folderSoul, dwExistsLength, szMovePath);
		////////////////////////////////////////////////////////////////
	}

	return 0;
}

DWORD CRiverFolder::FileSiblingIndex(DWORD *lParentIndex, const TCHAR *szFolderPath) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	DWORD lSiblingIndex = INVALID_INDEX_VALUE;

	river_iter = m_hmRiverFolder.find(szFolderPath);
	if(river_iter != m_hmRiverFolder.end()) {
		if(lParentIndex) *lParentIndex = river_iter->second->index;
		lSiblingIndex = river_iter->second->leaf_index;
	}

	return lSiblingIndex;
}

DWORD CRiverFolder::FileSiblingIndex(DWORD lFolderIndex) {
	unordered_map <DWORD, struct FolderSoul *>::iterator index_iter;
	DWORD lSiblingIndex = INVALID_INDEX_VALUE;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end()) {
		lSiblingIndex = index_iter->second->leaf_index;
	}

	return lSiblingIndex;
}

DWORD CRiverFolder::SetChildIndex(DWORD lFolderIndex, DWORD lLeafIndex) {
	unordered_map <DWORD, struct FolderSoul *>::iterator index_iter;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end()) {
		index_iter->second->leaf_index = lLeafIndex;
		FolderUtility::ModifyNode(m_hRiverFolder, lFolderIndex, FILE_INDEX_LEAF, lLeafIndex);
	}

	return 0;
}

DWORD CRiverFolder::FolderNameIndex(TCHAR *szFolderPath, DWORD lFolderIndex) {
	unordered_map <DWORD, struct FolderSoul *>::iterator index_iter;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end())
		_tcscpy_s(szFolderPath, MAX_PATH, index_iter->second->szFolderName);

	return 0;
}

struct FolderSoul *CRiverFolder::FolderSoulPtr(DWORD lFolderIndex) {
	unordered_map <DWORD, struct FolderSoul *>::iterator index_iter;
	FolderSoul *folderSoul = NULL;

	index_iter = m_hmFolderIndex.find(lFolderIndex);
	if(index_iter != m_hmFolderIndex.end())
		folderSoul = index_iter->second;

	return folderSoul;
}

DWORD CRiverFolder::FolderIndexName(const TCHAR *szFolderPath) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	DWORD lFolderIndex = INVALID_INDEX_VALUE;

	river_iter = m_hmRiverFolder.find(szFolderPath);
	if(river_iter != m_hmRiverFolder.end()) {
		lFolderIndex = river_iter->second->index;
	}

	return lFolderIndex;
}

DWORD CRiverFolder::FindNsake(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime) {
	struct FolderSoul *folderSoul;
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	DWORD dwFound = 0;

	river_iter = m_hmNsakeFolder.find(szFolderName);
	if(river_iter != m_hmNsakeFolder.end()) {
		folderSoul = river_iter->second;
		while (folderSoul) {
			if(!memcmp(&folderSoul->ftCreationTime, ftCreationTime, sizeof(FILETIME))) {
				_tcscpy_s(szExistsPath, MAX_PATH-ROOT_LENGTH, folderSoul->szFolderName);
				dwFound = 1;
				break;
			}
			folderSoul = folderSoul->nsake_bin;
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
	DWORD lFolderIndex = tFillEntry.sibling;
	if(INVALID_INDEX_VALUE != FolderUtility::ReadNode(m_hRiverFolder, &tFillEntry, lFolderIndex)) {
		wchar_t *fileName = _tcsrchr((TCHAR *)tFillEntry.szFolderName, _T('\\'));
		if(!memcmp(&tFillEntry.ftCreationTime, ftCreationTime, sizeof(FILETIME)) && !_tcscmp(szFolderName, ++fileName)) {	
			_tcscpy_s(szExistsPath, MAX_PATH-ROOT_LENGTH, tFillEntry.szFolderName);
			dwFound = 1;
		}
	}

	return dwFound;
}
