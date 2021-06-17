#pragma once

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;
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
	FILETIME ftCreatTime;
};
//
struct FolderShadow {
	struct FolderShadow *sibling, *parent, *child;
	ULONG inde, leaf_inde;
	//
	TCHAR *szFolderName;
	FILETIME ftCreatTime;
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
namespace NFolderVec {
	inline VOID DeleNode(struct FolderShadow *pNode) {
		if(!pNode) return;
		free(pNode->szFolderName);
		free(pNode);
	}
	VOID DeleFolderVec(FolderVec *pFolderVec);
	VOID DeleFolderHmap(FolderHmap *pFolderHmap);
	//
	struct FolderShadow *AddNewNode(FolderVec *pFolderVec, struct FolderEntry *pFolderEntry, ULONG lFolderInde);
	struct FolderShadow *AddNewNode(FolderVec *pFolderVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData);
	struct FolderShadow *DeleFromVec(FolderVec *pFolderVec, struct FolderShadow *shadow);
}
//
namespace FolderUtility {
	DWORD FillIdleEntry(HANDLE hRiverFolder);
	ULONG FindIdleEntry(HANDLE hRiverFolder);
	ULONG AddIdleEntry(HANDLE hRiverFolder, ULONG lIdleInde);
	ULONG ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderInde);
	ULONG WriteNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderInde);
	ULONG ModifyNode(HANDLE hRiverFolder, ULONG lFolderInde, DWORD dwIndeType, ULONG lNextInde);
	DWORD CTimeUpdate(HANDLE hRiverFolder, ULONG lFolderInde, FILETIME *ftCreatTime);
	ULONG VoidNode(HANDLE hRiverFolder, ULONG lFolderInde);
	//
	ULONG ReadRecyEntry(HANDLE hRiverFolder, struct FolderEntry *pRecyEntry, ULONG lFolderInde);
}
//
namespace FolderIndex {
	struct FolderShadow *LoadRiverFolder(FolderVec *pFolderVec, HANDLE hRiverFolder, ULONG lFolderInde);
	DWORD InitalFolderInde(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, FolderVec *pFolderVec, HANDLE hRiverFolder);
	VOID FinalFolderInde(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, FolderVec *pFolderVec);
	//
	DWORD BuildingIndex(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, FolderVec *pFolderVec);
	DWORD BuildingIndex(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, struct FolderShadow *shadow);
	DWORD IndexDele(FolderHmap *pRiverHmap, IndexHmap *pIndeHmap, struct FolderShadow *shadow);
	DWORD RebuildIndex(FolderHmap *pRiverHmap, const TCHAR *szExistPath,struct FolderShadow *shadow);
}
