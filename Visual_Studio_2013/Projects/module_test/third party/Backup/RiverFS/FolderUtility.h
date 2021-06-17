#pragma once

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::tr1::unordered_map;
#include <vector>
using  std::vector;

///////////////////////////////////////////////////////////////////////
struct FolderEntry {
	DWORD parent, child, sibling, leaf_index;
	////////////////////////////////////////
    FILETIME ftCreationTime;
	TCHAR szFolderName[MAX_PATH];
};

struct FolderSoul {
	struct FolderSoul *nsake_bin, *parent, *child, *sibling;
	DWORD index, leaf_index;
	////////////////////////////////////////
    FILETIME ftCreationTime;
	TCHAR *szFolderName;
};

#define INIT_FOLDER_ENTRY(ENTRY) \
	memset(&ENTRY, '\0', sizeof(struct FolderEntry)); \
	ENTRY.parent = INVALID_INDEX_VALUE;\
	ENTRY.child = INVALID_INDEX_VALUE;\
	ENTRY.sibling = INVALID_INDEX_VALUE;\
	ENTRY.leaf_index = INVALID_INDEX_VALUE;

#define INIT_FOLDER_SOUL(SOUL) \
	memset(SOUL, '\0', sizeof(struct FolderSoul)); \
	SOUL->index = INVALID_INDEX_VALUE;\
	SOUL->leaf_index = INVALID_INDEX_VALUE;

typedef vector <struct FolderSoul *> FolderVec;
typedef unordered_map <wstring, struct FolderSoul *> FolderHmap;
typedef unordered_map <DWORD, struct FolderSoul *> IndexHmap;

namespace NFolderVec {
	inline void DeleteNode(struct FolderSoul *folderSoul)
	{if(!folderSoul) return; free(folderSoul->szFolderName); free(folderSoul);}
	void DeleteFolderVec(FolderVec *pFolderVec);
	void DeleteFolderHmap(FolderHmap *pFolderHmap);
	/////////////////////////////////////////////////////
	struct FolderSoul *AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
	struct FolderSoul *AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData);
	struct FolderSoul *DeleteFromVec(FolderVec *pFolderVec, struct FolderSoul *folderSoul);
}

namespace FolderUtility {
	DWORD FillEmptyEntry(HANDLE hRiverFolder);
	DWORD FindEmptyEntry(HANDLE hRiverFolder);
	DWORD AddEmptyEntry(HANDLE hRiverFolder, DWORD lEmptyIndex);
	DWORD DelEmptyEntry(HANDLE hRiverFolder);
	DWORD ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
	DWORD WriteNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
	DWORD ModifyNode(HANDLE hRiverFolder, DWORD lFolderIndex, DWORD dwIndexType, DWORD lNextIndex);

}

namespace FolderIndex {
	struct FolderSoul *LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, DWORD lFolderIndex);
	DWORD InitalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec, HANDLE hRiverFolder);
	void FinalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec);
	////////////////////////////////////////
	DWORD BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec);
	DWORD BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderSoul *folderSoul);
	DWORD IndexDelete(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderSoul *folderSoul);
	DWORD RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistsPath,struct FolderSoul *folderSoul);
}
