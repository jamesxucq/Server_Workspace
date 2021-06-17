#include "StdAfx.h"
#include "CommonUtility.h"

#include "RiverUtility.h"
#include "FolderUtility.h"
#include "../../clientcom/utility/ulog.h"


VOID NFolderVec::DeleteFolderVec(FolderVec *pFolderVec) {
	vector <struct FolderShadow *>::iterator iter;

	// if(pFolderVec->empty()) return;
	for(iter=pFolderVec->begin(); iter!=pFolderVec->end(); iter++) 
		DeleteNode(*iter);

	pFolderVec->clear();
}

VOID NFolderVec::DeleteFolderHmap(FolderHmap *pFolderHmap) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;

	// if(pFilesHmap->empty()) return;
	for(river_iter=pFolderHmap->begin(); river_iter!=pFolderHmap->end(); ++river_iter) 
		DeleteNode(river_iter->second);

	pFolderHmap->clear();
}

struct FolderShadow *NFolderVec::AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	struct FolderShadow *shadow;
	//SYSTEMTIME ftLastWrite;

	shadow = (struct FolderShadow *)malloc(sizeof(struct FolderShadow));
	if(!shadow) return NULL;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	INIT_FOLDER_SHADOW(shadow)
	shadow->szFolderName = _tcsdup(pFolderEntry->szFolderName);
	shadow->ftCreationTime = pFolderEntry->ftCreationTime;
	shadow->parent = (struct FolderShadow *)pFolderEntry->parent;
	shadow->child = (struct FolderShadow *)pFolderEntry->child;
	shadow->sibling = (struct FolderShadow *)pFolderEntry->sibling;
	shadow->index = lFolderIndex;
	shadow->leaf_index = pFolderEntry->leaf_index;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// pFolderHmap->insert(std::pair<wstring, struct FolderShadow *>(szDirectory, shadow));
	pFolderVec->push_back(shadow);

	return shadow;
}

struct FolderShadow *NFolderVec::AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
	struct FolderShadow *shadow;
	TCHAR szDirectory[MAX_PATH];
	//SYSTEMTIME ftLastWrite;

	shadow = (struct FolderShadow *)malloc(sizeof(struct FolderShadow));
	if(!shadow) return NULL;
	INIT_FOLDER_SHADOW(shadow)
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	CommonUtility::full_name(szDirectory, szPathString, pFindData->cFileName);
	shadow->szFolderName = _tcsdup(szDirectory);
	shadow->ftCreationTime = pFindData->ftCreationTime;
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	pFolderVec->push_back(shadow);

	return shadow;
}

struct FolderShadow *NFolderVec::DeleteFromVec(FolderVec *pFolderVec, struct FolderShadow *shadow) {
	vector <struct FolderShadow *>::iterator iter;
	struct FolderShadow *fshadow = NULL;

	for(iter=pFolderVec->begin(); iter!=pFolderVec->end(); iter++) {
		if(shadow == *iter) {
			pFolderVec->erase(iter);
			fshadow = shadow;
			break;
		}
	}

	return fshadow;
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

VOID FolderIndex::FinalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec) {
	pRiverHmap->clear();
	pNsakeHmap->clear();
	pIndexHmap->clear();
	NFolderVec::DeleteFolderVec(pFolderVec);
}

static inline VOID FillChildrenParent(struct FolderShadow *child, struct FolderShadow *parent) {
	while(child) {
		child->parent = parent;
		child = child->sibling;
	}
}

struct FolderShadow *FolderIndex::LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, DWORD lFolderIndex) {
	struct FolderShadow *travel, *sibling, *child;
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

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	////////////////////////////////////////////////////////////////
	pRiverHmap->insert(std::pair<wstring, struct FolderShadow *>(shadow->szFolderName, shadow));
	pIndexHmap->insert(std::pair<DWORD, struct FolderShadow *>(shadow->index, shadow));
	////////////////////////////////////////////////////////////////
	wchar_t *folder = _tcsrchr(shadow->szFolderName, _T('\\'));
	if(folder) {
		river_iter = pNsakeHmap->find(++folder);
		if(river_iter != pNsakeHmap->end()) {
			struct FolderShadow *fshadow = river_iter->second;
			while(fshadow->nsake_bin) fshadow = fshadow->nsake_bin;
			fshadow->nsake_bin = shadow;
		} else pNsakeHmap->insert(std::pair<wstring, struct FolderShadow *>(folder, shadow));
	}
	////////////////////////////////////////////////////////////////
	return 0;
}

DWORD FolderIndex::BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec) {
	vector <struct FolderShadow *>::iterator iter;
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;

	for(iter = pFolderVec->begin(); iter != pFolderVec->end(); iter++) {
		////////////////////////////////////////////////////////////////
		pRiverHmap->insert(std::pair<wstring, struct FolderShadow *>((*iter)->szFolderName, *iter));
		pIndexHmap->insert(std::pair<DWORD, struct FolderShadow *>((*iter)->index, *iter));
		////////////////////////////////////////////////////////////////
		wchar_t *folder = _tcsrchr((*iter)->szFolderName, _T('\\'));
		if(folder && _T('')!=*(++folder)) {
			river_iter = pNsakeHmap->find(folder);
			if(river_iter != pNsakeHmap->end()) {
				struct FolderShadow *shadow = river_iter->second;
				while(shadow->nsake_bin) shadow = shadow->nsake_bin;
				shadow->nsake_bin = *iter;
			} else pNsakeHmap->insert(std::pair<wstring, struct FolderShadow *>(folder, *iter));
		}
		////////////////////////////////////////////////////////////////
	}

	return 0;
}

DWORD FolderIndex::RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistsPath, struct FolderShadow *shadow) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	////////////////////////////////////////////////////////////////
	river_iter = pRiverHmap->find(szExistsPath);
	if (river_iter != pRiverHmap->end()) pRiverHmap->erase(river_iter);
	////////////////////////////////////////////////////////////////
	pRiverHmap->insert(std::pair<wstring, struct FolderShadow *>(shadow->szFolderName, shadow));
	////////////////////////////////////////////////////////////////
	return 0;
}

DWORD FolderIndex::IndexDelete(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow) {
	unordered_map <wstring, struct FolderShadow *>::iterator river_iter;
	unordered_map <DWORD, struct FolderShadow *>::iterator index_iter;
	struct FolderShadow *prev, *fshadow;
	//////////////////////////////////////////////////////////////
	river_iter = pRiverHmap->find(shadow->szFolderName);
	if (river_iter != pRiverHmap->end()) pRiverHmap->erase(river_iter);

	index_iter = pIndexHmap->find(shadow->index);
	if (index_iter != pIndexHmap->end()) pIndexHmap->erase(index_iter);
	//////////////////////////////////////////////////////////////
	wchar_t *folder = _tcsrchr(shadow->szFolderName, _T('\\'));
	river_iter = pNsakeHmap->find(++folder);
	if (river_iter != pNsakeHmap->end()) {
		fshadow = river_iter->second;
		if(shadow == fshadow) {
			pNsakeHmap->erase(river_iter);
			if(shadow->nsake_bin)
				pNsakeHmap->insert(std::pair<wstring, struct FolderShadow *>(folder, shadow->nsake_bin));
		} else {
			prev = fshadow;
			while(fshadow = fshadow->nsake_bin) {
				if(shadow == fshadow) {
					prev->nsake_bin = fshadow->nsake_bin;
					break;
				}
				prev = fshadow;
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
	////////////////////////////////////////////////////////////
	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lNextIndex))
		return INVALID_INDEX_VALUE;
	if(INVALID_INDEX_VALUE == tFolderEntry.child && INVALID_INDEX_VALUE == tFolderEntry.sibling) 
		return lNextIndex;
	////////////////////////////////////////////////////////////
	lLeafIndex = FindNextEntry(hRiverFolder, tFolderEntry.child);
	if(INVALID_INDEX_VALUE != lLeafIndex) {
		if(lLeafIndex == tFolderEntry.child) {
			tFolderEntry.child = INVALID_INDEX_VALUE;
			FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lNextIndex);
		}
		return lLeafIndex;
	}
	////////////////////////////////////////////////////////////
	lLeafIndex = FindNextEntry(hRiverFolder, tFolderEntry.sibling);
	if(INVALID_INDEX_VALUE != lLeafIndex) {
		if(lLeafIndex == tFolderEntry.sibling) {
			tFolderEntry.sibling = INVALID_INDEX_VALUE;
			FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lNextIndex);
		}
		return lLeafIndex;
	}
	////////////////////////////////////////////////////////////
	return INVALID_INDEX_VALUE;
}

DWORD FolderUtility::FindEmptyEntry(HANDLE hRiverFolder) {
	struct FolderEntry tFillEntry;
	LONG lEmptyOffset, lEmptyIndex = 0;
	DWORD rlen, wlen;

	lEmptyOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}

	if(INVALID_INDEX_VALUE != tFillEntry.sibling) {
		lEmptyIndex = FindNextEntry(hRiverFolder, tFillEntry.sibling);

		if(tFillEntry.sibling == lEmptyIndex) {
			tFillEntry.sibling = INVALID_INDEX_VALUE;
			OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
			OverLapped.Offset = lEmptyOffset;
			if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
				return INVALID_INDEX_VALUE;	
		}
	} else {
		if(FillEmptyEntry(hRiverFolder)) return INVALID_INDEX_VALUE;
		lEmptyIndex = lEmptyOffset / sizeof(struct FolderEntry);
	}

	return lEmptyIndex;
}

DWORD FolderUtility::AddEmptyEntry(HANDLE hRiverFolder, DWORD lEmptyIndex) {
	struct FolderEntry tFillEntry;
	DWORD rlen, wlen;

	DWORD lEmptyOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;
	if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	////////////////////////////////////////////////////////////////
	ModifyNode(hRiverFolder, lEmptyIndex, FILE_INDEX_SIBLING, tFillEntry.sibling);
	////////////////////////////////////////////////////////////////
	tFillEntry.sibling = lEmptyIndex;
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lEmptyOffset;
	if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lEmptyIndex;
}

DWORD FolderUtility::DelEmptyEntry(HANDLE hRiverFolder) {
	struct FolderEntry tFillEntry;
	DWORD lEmptyIndex;
	DWORD rlen, wlen;

	DWORD lEmptyOffset = SetFilePointer(hRiverFolder, -(LONG)sizeof(struct FolderEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	lEmptyIndex = tFillEntry.sibling;
	//////////////////////////////////////////////////////////////////////
	struct FolderEntry tEmptyEntry;
	ReadNode(hRiverFolder, &tEmptyEntry, lEmptyIndex);
	tFillEntry.sibling = tEmptyEntry.sibling;
	//////////////////////////////////////////////////////////////////////
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lEmptyOffset;
	if(!WriteFile(hRiverFolder, &tFillEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
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

DWORD FolderUtility::VoidNode(HANDLE hRiverFolder, DWORD lFolderIndex) {
	DWORD rlen, wlen;
	struct FolderEntry tFolderEntry;
	DWORD lExistsIndex = INVALID_INDEX_VALUE;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFolderIndex * sizeof(struct FolderEntry);
	if(!ReadFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &rlen, &OverLapped)) {
		// if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVALID_INDEX_VALUE;
	}

	memset(&tFolderEntry.ftCreationTime, '\0', sizeof(FILETIME));
	if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lExistsIndex;
}