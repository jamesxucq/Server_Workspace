#include "StdAfx.h"
#include "CommonUtility.h"

#include "RiverUtility.h"
#include "FolderUtility.h"
#include "../../clientcom/utility/ulog.h"


void NFolderVec::DeleteFolderVec(FolderVec *pFolderVec) {
	vector <struct FolderSoul *>::iterator iter;

	// if(pFolderVec->empty()) return;
	for(iter=pFolderVec->begin(); iter!=pFolderVec->end(); iter++) 
		DeleteNode(*iter);

	pFolderVec->clear();
}

void NFolderVec::DeleteFolderHmap(FolderHmap *pFolderHmap) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;

	// if(pFilesHmap->empty()) return;
	for(river_iter=pFolderHmap->begin(); river_iter!=pFolderHmap->end(); ++river_iter) 
		DeleteNode(river_iter->second);

	pFolderHmap->clear();
}

struct FolderSoul *NFolderVec::AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	struct FolderSoul *folderSoul;
	//SYSTEMTIME ftLastWrite;

	folderSoul = (struct FolderSoul *)malloc(sizeof(struct FolderSoul));
	if(!folderSoul) return NULL;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	INIT_FOLDER_SOUL(folderSoul)
	folderSoul->szFolderName = _tcsdup(pFolderEntry->szFolderName);
	folderSoul->ftCreationTime = pFolderEntry->ftCreationTime;
	folderSoul->parent = (struct FolderSoul *)pFolderEntry->parent;
	folderSoul->child = (struct FolderSoul *)pFolderEntry->child;
	folderSoul->sibling = (struct FolderSoul *)pFolderEntry->sibling;
	folderSoul->index = lFolderIndex;
	folderSoul->leaf_index = pFolderEntry->leaf_index;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// pFolderHmap->insert(std::pair<wstring, struct FolderSoul *>(szDirectory, folderSoul));
	pFolderVec->push_back(folderSoul);

	return folderSoul;
}

struct FolderSoul *NFolderVec::AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
	struct FolderSoul *folderSoul;
	TCHAR szDirectory[MAX_PATH];
	//SYSTEMTIME ftLastWrite;

	folderSoul = (struct FolderSoul *)malloc(sizeof(struct FolderSoul));
	if(!folderSoul) return NULL;
	INIT_FOLDER_SOUL(folderSoul)
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	CommonUtility::full_name(szDirectory, szPathString, pFindData->cFileName);
	folderSoul->szFolderName = _tcsdup(szDirectory);
	folderSoul->ftCreationTime = pFindData->ftCreationTime;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	pFolderVec->push_back(folderSoul);

	return folderSoul;
}

struct FolderSoul *NFolderVec::DeleteFromVec(FolderVec *pFolderVec, struct FolderSoul *folderSoul) {
	vector <struct FolderSoul *>::iterator iter;
	struct FolderSoul *folder_soul = NULL;

	for(iter=pFolderVec->begin(); iter!=pFolderVec->end(); iter++) {
		if(folderSoul == *iter) {
			pFolderVec->erase(iter);
			folder_soul = folderSoul;
			break;
		}
	}

	return folder_soul;
}

///////////////////////////////////////////////////////////////////
DWORD FolderIndex::InitalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec , HANDLE hRiverFolder) {
	if(hRiverFolder == INVALID_HANDLE_VALUE) return -1;

	if(!LoadRiverFolder(pFolderVec, hRiverFolder, ROOT_INDEX_VALUE)) 
		return -1;
	if(FolderIndex::BuildingIndex(pRiverHmap, pNsakeHmap, pIndexHmap, pFolderVec)) 
		return -1;

	return 0;
}

void FolderIndex::FinalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec) {
	pRiverHmap->clear();
	pNsakeHmap->clear();
	pIndexHmap->clear();
	NFolderVec::DeleteFolderVec(pFolderVec);
}

static inline void FillChildrenParent(struct FolderSoul *child, struct FolderSoul *parent) {
	while(child) {
		child->parent = parent;
		child = child->sibling;
	}
}

struct FolderSoul *FolderIndex::LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, DWORD lFolderIndex) {
	struct FolderSoul *travel, *sibling, *child;
	struct FolderEntry tFolderEntry;

	if(INVALID_INDEX_VALUE == lFolderIndex) return NULL;
	/////////////////////////////////
	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderIndex))
		return NULL;
	/////////////////////////////////
	child = LoadRiverFolder(pFolderVec, hRiverFolder, tFolderEntry.child);
	sibling = LoadRiverFolder(pFolderVec, hRiverFolder, tFolderEntry.sibling);
	/////////////////////////////////
	tFolderEntry.child = (DWORD)child;
	tFolderEntry.sibling = (DWORD)sibling;
	/////////////////////////////////
	travel = NFolderVec::AddNewNode(pFolderVec, &tFolderEntry, lFolderIndex);
	if(child) FillChildrenParent(child, travel);

	return travel;
}

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderSoul *folderSoul) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	////////////////////////////////////////////////////////////////
	pRiverHmap->insert(std::pair<wstring, struct FolderSoul *>(folderSoul->szFolderName, folderSoul));
	pIndexHmap->insert(std::pair<DWORD, struct FolderSoul *>(folderSoul->index, folderSoul));
	////////////////////////////////////////////////////////////////
	wchar_t *folder = _tcsrchr(folderSoul->szFolderName, _T('\\'));
	if(folder) {
		river_iter = pNsakeHmap->find(++folder);
		if(river_iter != pNsakeHmap->end()) {
			struct FolderSoul *folder_soul = river_iter->second;
			while(folder_soul->nsake_bin) folder_soul = folder_soul->nsake_bin;
			folder_soul->nsake_bin = folderSoul;
		} else pNsakeHmap->insert(std::pair<wstring, struct FolderSoul *>(folder, folderSoul));
	}
	////////////////////////////////////////////////////////////////
	return 0;
}

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec) {
	vector <struct FolderSoul *>::iterator iter;
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;

	for(iter = pFolderVec->begin(); iter != pFolderVec->end(); iter++) {
		////////////////////////////////////////////////////////////////
		pRiverHmap->insert(std::pair<wstring, struct FolderSoul *>((*iter)->szFolderName, *iter));
		pIndexHmap->insert(std::pair<DWORD, struct FolderSoul *>((*iter)->index, *iter));
		////////////////////////////////////////////////////////////////
		wchar_t *folder = _tcsrchr((*iter)->szFolderName, _T('\\'));
		if(folder && _T('')!=*(++folder)) {
			river_iter = pNsakeHmap->find(folder);
			if(river_iter != pNsakeHmap->end()) {
				struct FolderSoul *folderSoul = river_iter->second;
				while(folderSoul->nsake_bin) folderSoul = folderSoul->nsake_bin;
				folderSoul->nsake_bin = *iter;
			} else pNsakeHmap->insert(std::pair<wstring, struct FolderSoul *>(folder, *iter));
		}
		////////////////////////////////////////////////////////////////
	}

	return 0;
}

DWORD FolderIndex::RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistsPath, struct FolderSoul *folderSoul) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	////////////////////////////////////////////////////////////////
	river_iter = pRiverHmap->find(szExistsPath);
	if (river_iter != pRiverHmap->end()) pRiverHmap->erase(river_iter);
	////////////////////////////////////////////////////////////////
	pRiverHmap->insert(std::pair<wstring, struct FolderSoul *>(folderSoul->szFolderName, folderSoul));
	////////////////////////////////////////////////////////////////
	return 0;
}

DWORD FolderIndex::IndexDelete(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderSoul *folderSoul) {
	unordered_map <wstring, struct FolderSoul *>::iterator river_iter;
	unordered_map <DWORD, struct FolderSoul *>::iterator index_iter;
	struct FolderSoul *prev, *folder_soul;
	//////////////////////////////////////////////////////////////
	river_iter = pRiverHmap->find(folderSoul->szFolderName);
	if (river_iter != pRiverHmap->end()) pRiverHmap->erase(river_iter);

	index_iter = pIndexHmap->find(folderSoul->index);
	if (index_iter != pIndexHmap->end()) pIndexHmap->erase(index_iter);
	//////////////////////////////////////////////////////////////
	wchar_t *folder = _tcsrchr(folderSoul->szFolderName, _T('\\'));
	river_iter = pNsakeHmap->find(++folder);
	if (river_iter != pNsakeHmap->end()) {
		folder_soul = river_iter->second;
		if(folderSoul == folder_soul) {
			pNsakeHmap->erase(river_iter);
			if(folderSoul->nsake_bin)
				pNsakeHmap->insert(std::pair<wstring, struct FolderSoul *>(folder, folderSoul->nsake_bin));
		} else {
			prev = folder_soul;
			while(folder_soul = folder_soul->nsake_bin) {
				if(folderSoul == folder_soul) {
					prev->nsake_bin = folder_soul->nsake_bin;
					break;
				}
				prev = folder_soul;
			} 
		}
	}
	////////////////////////////////////////////////////////////////
	return 0;
}

DWORD FolderUtility::FillEmptyEntry(HANDLE hRiverFolder) {
	struct FolderEntry tFolderEntry;
	DWORD wlen;

	INIT_FOLDER_ENTRY(tFolderEntry)
	DWORD lEmptyOffset = SetFilePointer(hRiverFolder, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return -1;
	if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, NULL))
		return -1;

	return 0;
}

static DWORD FindNextEntry(HANDLE hRiverFolder, DWORD lNextIndex) {
	struct FolderEntry tFolderEntry;
	DWORD lLeafIndex;

	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lNextIndex))
		return INVALID_INDEX_VALUE;

	if(INVALID_INDEX_VALUE == tFolderEntry.child && INVALID_INDEX_VALUE == lNextIndex) 
		return lNextIndex;

	lLeafIndex = FindNextEntry(hRiverFolder, tFolderEntry.child);
	if(INVALID_INDEX_VALUE != lLeafIndex) return lLeafIndex;
	lLeafIndex = FindNextEntry(hRiverFolder, tFolderEntry.sibling);
	if(INVALID_INDEX_VALUE != lLeafIndex) return lLeafIndex;

	return INVALID_INDEX_VALUE;
}

DWORD FolderUtility::FindEmptyEntry(HANDLE hRiverFolder) {
	struct FolderEntry tFolderEntry;
	LONG lEmptyOffset, lEmptyIndex = 0;
	DWORD rlen, wlen;

	lEmptyOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}

	if(INVALID_INDEX_VALUE != tFolderEntry.sibling) {
		lEmptyIndex = FindNextEntry(hRiverFolder, lEmptyIndex);

		if(tFolderEntry.sibling == lEmptyIndex) {
			tFolderEntry.sibling = INVALID_INDEX_VALUE;
			OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
			OverLapped.Offset = lEmptyOffset;
			if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
				return INVALID_INDEX_VALUE;	
		}
	} else {
		if(FillEmptyEntry(hRiverFolder)) return INVALID_INDEX_VALUE;
		lEmptyIndex = lEmptyOffset / sizeof(struct FolderEntry);
	}

	return lEmptyIndex;
}

DWORD FolderUtility::AddEmptyEntry(HANDLE hRiverFolder, DWORD lEmptyIndex) {
	struct FolderEntry tFolderEntry;
	DWORD rlen, wlen;

	DWORD lEmptyOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;
	if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	////////////////////////////////////////////////////////////////
	ModifyNode(hRiverFolder, lEmptyIndex, FILE_INDEX_SIBLING, tFolderEntry.sibling);
	////////////////////////////////////////////////////////////////
	tFolderEntry.sibling = lEmptyIndex;
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lEmptyOffset;
	if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lEmptyIndex;
}

DWORD FolderUtility::DelEmptyEntry(HANDLE hRiverFolder) {
	struct FolderEntry tFolderEntry;
	DWORD lEmptyIndex;
	DWORD rlen, wlen;

	DWORD lEmptyOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	lEmptyIndex = tFolderEntry.sibling;
	//////////////////////////////////////////////////////////////////////
	struct FolderEntry tEmptyEntry;
	ReadNode(hRiverFolder, &tEmptyEntry, lEmptyIndex);
	tFolderEntry.sibling = tEmptyEntry.sibling;
	//////////////////////////////////////////////////////////////////////
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lEmptyOffset;
	if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;


	return lEmptyIndex;
}

DWORD FolderUtility::ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	DWORD rlen;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
	if(!ReadFile(hRiverFolder, pFolderEntry, sizeof(struct FolderEntry), &rlen, &OverLapped)) {
		// if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVALID_INDEX_VALUE;
	}

	return lFolderIndex;
}

DWORD FolderUtility::WriteNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	DWORD wlen;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
	if(!WriteFile(hRiverFolder, pFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lFolderIndex;
}

DWORD FolderUtility::ModifyNode(HANDLE hRiverFolder, DWORD lFolderIndex, DWORD dwIndexType, DWORD lNextIndex) {
	DWORD rlen, wlen;
	struct FolderEntry tFolderEntry;
	DWORD lExistsIndex = INVALID_INDEX_VALUE;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
	if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &rlen, &OverLapped)) {
		// if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVALID_INDEX_VALUE;
	}

	switch(dwIndexType) {
	case FILE_INDEX_CHILD:
		lExistsIndex = tFolderEntry.child;
		tFolderEntry.child = lNextIndex;
		break;
	case FILE_INDEX_SIBLING:
		lExistsIndex = tFolderEntry.sibling;
		tFolderEntry.sibling = lNextIndex;
		break;
	case FILE_INDEX_LEAF:
		lExistsIndex = tFolderEntry.leaf_index;
		tFolderEntry.leaf_index = lNextIndex;
		break;
	}

	if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lExistsIndex;
}
