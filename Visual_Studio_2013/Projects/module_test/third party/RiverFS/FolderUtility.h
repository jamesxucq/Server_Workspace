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

struct FolderShadow {
	struct FolderShadow *nsake_bin, *parent, *child, *sibling;
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

#define INIT_FOLDER_SHADOW(SHADOW) \
	memset(SHADOW, '\0', sizeof(struct FolderShadow)); \
	SHADOW->index = INVALID_INDEX_VALUE;\
	SHADOW->leaf_index = INVALID_INDEX_VALUE;

typedef vector <struct FolderShadow *> FolderVec;
typedef unordered_map <wstring, struct FolderShadow *> FolderHmap;
typedef unordered_map <DWORD, struct FolderShadow *> IndexHmap;

namespace NFolderVec {
	inline VOID DeleteNode(struct FolderShadow *shadow)
	{if(!shadow) return; free(shadow->szFolderName); free(shadow);}
	VOID DeleteFolderVec(FolderVec *pFolderVec);
	VOID DeleteFolderHmap(FolderHmap *pFolderHmap);
	/////////////////////////////////////////////////////
	struct FolderShadow *AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
	struct FolderShadow *AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData);
	struct FolderShadow *DeleteFromVec(FolderVec *pFolderVec, struct FolderShadow *shadow);
}

namespace FolderUtility {
	DWORD FillEmptyEntry(HANDLE hRiverFolder);
	DWORD FindEmptyEntry(HANDLE hRiverFolder);
	DWORD AddEmptyEntry(HANDLE hRiverFolder, DWORD lEmptyIndex);
	DWORD DelEmptyEntry(HANDLE hRiverFolder);
	DWORD ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
	DWORD WriteNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
	DWORD ModifyNode(HANDLE hRiverFolder, DWORD lFolderIndex, DWORD dwIndexType, DWORD lNextIndex);
	DWORD VoidNode(HANDLE hRiverFolder, DWORD lFolderIndex);
}

namespace FolderIndex {
	struct FolderShadow *LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, DWORD lFolderIndex);
	DWORD InitalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec, HANDLE hRiverFolder);
	VOID FinalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec);
	////////////////////////////////////////
	DWORD BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec);
	DWORD BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow);
	DWORD IndexDelete(FolderHmap *pRiverHmap, FolderHmap *pNsakeHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow);
	DWORD RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistsPath,struct FolderShadow *shadow);
}
