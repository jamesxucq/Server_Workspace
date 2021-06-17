#pragma once

#include "clientcom/clientcom.h"
#include "LockedAction.h"
//

struct FastANode {
	ULONG lActioInde;
    DWORD dwActioType;
	struct FastANode *pNext;
};
typedef unordered_map <wstring, struct FastANode*> FastAHmap;

//
struct FastKNode {
	struct attent *atte;
    DWORD dwActioType;
	struct FastKNode *pNext;
};
typedef unordered_map <wstring, struct FastKNode*> FastKHmap;

//
struct FastZNode {
	struct LockActio *lock_acti;
    DWORD dwActioType;
	struct FastZNode *pNext;
};
typedef unordered_map <wstring, struct FastZNode*> FastZHmap;
typedef unordered_map <DWORD, struct FastZNode*> FastNHmap;
//
namespace NFastUtili {
	inline VOID DeleANode(struct FastANode *pNode)
	{ if(pNode) free(pNode); }
	VOID DeleFastAHmap(FastAHmap *pFastAHmap);
	//
	struct FastANode *AddNewANode(FastAHmap *pFastAHmap, ULONG lInde, struct Action *pActioNode);
	struct FastANode *AppNewANode(struct FastANode *pFastANode, ULONG lInde, DWORD dwActioType);
	//
	void BuildFastAHmap(FastAHmap *pFastAHmap, class CLockActio *pLkAiVec);
	//////////////////////////////////////////////////////////////////////
	inline VOID DeleKNode(struct FastKNode *pNode)
	{ if(pNode) free(pNode); }
	VOID DeleFastKHmap(FastKHmap *pFastKHmap);
	//
	struct FastKNode *AddNewKNode(FastKHmap *pFastKHmap, struct attent *atte);
	struct FastKNode *AppNewKNode(struct FastKNode *pFastKNode, struct attent *atte);
	//
	struct FastKNode *InseToHmap(FastKHmap *pFastKHmap, struct attent *atte);
	VOID DeleFromHmap(FastKHmap *pFastKHmap, struct attent *atte);
	VOID DeleListHmap(FastKHmap *pFastKHmap, struct FastKNode *pListHead);
	//
	struct FastKNode *FindFromHmap(FastKHmap *pFindKHmap, struct attent *atte);
	struct attent *FindNextValue(struct FastKNode *pNode);
	void BuildFastKHmap(FastKHmap *pFastKHmap, FilesVec *pFindVec);
	//////////////////////////////////////////////////////////////////////
	inline VOID DeleZNode(struct FastZNode *pNode)
	{ if(pNode) free(pNode); }
	VOID DeleFastZHmap(FastZHmap *pFastZHmap);
	//
	struct FastZNode *AddNewZNode(FastZHmap *pFastZHmap, TCHAR *szFileName, struct LockActio *lock_acti);
	struct FastZNode *AddNewZNode(FastZHmap *pFastZHmap, struct LockActio *lock_acti);
	struct FastZNode *AppNewZNode(struct FastZNode *pFastZNode, struct LockActio *lock_acti);
	//
	void BuildFastZHmap(FastZHmap *pFastZHmap, LkAiVec *pLkAiVec);
	void BuildFastZHmapEx(FastZHmap *pFastZHmap, LkAiVec *pLkAiVec);
	struct LockActio *FindAddiZHmap(FastZHmap *pFastZHmap, TCHAR *szFileName);
	struct LockActio *FindDeleZHmap(FastZHmap *pFastZHmap, TCHAR *szFileName);
	//////////////////////////////////////////////////////////////////////
	VOID DeleFastNHmap(FastNHmap *pFastNHmap);
	struct FastZNode *AddNewZNode(FastNHmap *pFastNHmap, struct LockActio *lock_acti);
	void BuildFastNHmap(FastNHmap *pFastNHmap, LkAiVec *pLkAiVec);
};