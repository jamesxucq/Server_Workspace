#pragma once

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <boost/tr1/unordered_map.hpp>
using std::tr1::unordered_map;
#include <vector>
using  std::vector;

//
struct FolderEntry {
	union {
		ULONG sibling;
		ULONG recycled;
	};
	ULONG parent;
	ULONG child, leaf_inde;
	//
	TCHAR szFolderName[MAX_PATH];
	FILETIME ftCreationTime;
};
//
struct FolderShadow {
	struct FolderShadow *sibling, *parent, *isonym_node, *child;
	ULONG inde, leaf_inde;
	//
	TCHAR *szFolderName;
	FILETIME ftCreationTime;
};
//
#define INIT_FOLDER_ENTRY(entry) \
	memset(&entry, '\0', sizeof(struct FolderEntry)); \
	entry.sibling = INVA_INDE_VALU;\
	entry.parent = INVA_INDE_VALU;\
	entry.child = INVA_INDE_VALU;\
	entry.leaf_inde = INVA_INDE_VALU;

#define INIT_FOLDER_SHADOW(shadow) \
	memset(shadow, '\0', sizeof(struct FolderShadow)); \
	shadow->inde = INVA_INDE_VALU;\
	shadow->leaf_inde = INVA_INDE_VALU;

typedef vector <struct FolderShadow *> FolderVec;
typedef unordered_map <wstring, struct FolderShadow *> FolderHmap;
typedef unordered_map <ULONG, struct FolderShadow *> IndexHmap;
//
namespace NFolderVec
{
	inline VOID DeleteNode(struct FolderShadow *shadow)
	{
		if(!shadow) return;
		free(shadow->szFolderName);
		free(shadow);
	}
	VOID DeleteFolderVec(FolderVec *pFolderVec);
	VOID DeleteFolderHmap(FolderHmap *pFolderHmap);
	//
	struct FolderShadow *AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, ULONG lFolderIndex);
	struct FolderShadow *AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData);
	struct FolderShadow *DeleteFromVec(FolderVec *pFolderVec, struct FolderShadow *shadow);
}
//
namespace FolderUtility
{
	DWORD FillIdleEntry(HANDLE hRiverFolder);
	ULONG FindIdleEntry(HANDLE hRiverFolder);
	ULONG AddIdleEntry(HANDLE hRiverFolder, ULONG lIdleIndex);
	ULONG ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderIndex);
	ULONG WriteNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderIndex);
	ULONG ModifyNode(HANDLE hRiverFolder, ULONG lFolderIndex, DWORD dwIndexType, ULONG lNextIndex);
	ULONG VoidNode(HANDLE hRiverFolder, ULONG lFolderIndex);
}
//
namespace FolderIndex
{
	struct FolderShadow *LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, ULONG lFolderIndex);
	DWORD InitalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec, HANDLE hRiverFolder);
	VOID FinalFolderIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec);
	//
	DWORD BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, FolderVec *pFolderVec);
	DWORD BuildingIndex(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow);
	DWORD IndexDelete(FolderHmap *pRiverHmap, FolderHmap *pIsonymHmap, IndexHmap *pIndexHmap, struct FolderShadow *shadow);
	DWORD RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistsPath,struct FolderShadow *shadow);
}
