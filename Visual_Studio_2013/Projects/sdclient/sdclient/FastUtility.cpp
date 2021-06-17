#include "StdAfx.h"

#include "FastUtility.h"

//
void static DeleANodeLink(struct FastANode *pFastANode) {
	struct FastANode *pPrevANode;
	while(pFastANode) {
		pPrevANode = pFastANode;
		pFastANode = pFastANode->pNext;
		NFastUtili::DeleANode(pPrevANode);
	}
}

void NFastUtili::DeleFastAHmap(FastAHmap *pFastAHmap) {
    unordered_map <wstring, struct FastANode*>::iterator aite;
    // if(pFastAHmap->empty()) return;
    for(aite = pFastAHmap->begin(); pFastAHmap->end() != aite; ++aite)
        DeleANodeLink(aite->second);
    pFastAHmap->clear();
}

//
struct FastANode *NFastUtili::AddNewANode(FastAHmap *pFastAHmap, ULONG lActioInde, struct Action *pActioNode) {
    struct FastANode *ast_new;
    //
    ast_new = (struct FastANode *)malloc(sizeof(struct FastANode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastANode));
	ast_new->dwActioType = pActioNode->dwActioType;
	ast_new->lActioInde = lActioInde;
	//
	pFastAHmap->insert(FastAHmap::value_type(pActioNode->szFileName, ast_new));
    return ast_new;
}

struct FastANode *NFastUtili::AppNewANode(struct FastANode *pFastANode, ULONG lActioInde, DWORD dwActioType) {
    struct FastANode *ast_new;
    //
    ast_new = (struct FastANode *)malloc(sizeof(struct FastANode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastANode));
	ast_new->dwActioType = dwActioType;
	ast_new->lActioInde = lActioInde;
	//
	while(pFastANode->pNext) pFastANode = pFastANode->pNext;
	pFastANode->pNext = ast_new;
	return ast_new;
}

//
void NFastUtili::BuildFastAHmap(FastAHmap *pFastAHmap, class CLockActio *pLkAiVec) {
	struct Action tExiActi;
	FastAHmap::iterator iter;
	//
	for(ULONG lInde = 0x00; INVA_LANDE_VALU != pLkAiVec->ReadNode(&tExiActi, lInde); lInde++) {
		if(!(LATT_FINISH_ATT & tExiActi.dwActioType)) {
			iter = pFastAHmap->find(tExiActi.szFileName);
            if(pFastAHmap->end() == iter) {
				AddNewANode(pFastAHmap, lInde, &tExiActi);
			} else {
				AppNewANode(iter->second, lInde, tExiActi.dwActioType);
			}
		}
	}
	//
}

	//////////////////////////////////////////////////////////////////////
VOID static DeleKNodeLink(struct FastKNode *pFastKNode) {
	struct FastKNode *pPrevKNode;
	while(pFastKNode) {
		pPrevKNode = pFastKNode;
		pFastKNode = pFastKNode->pNext;
		NFastUtili::DeleKNode(pPrevKNode);
	}
}

VOID NFastUtili::DeleFastKHmap(FastKHmap *pFastKHmap) {
    unordered_map <wstring, struct FastKNode*>::iterator aite;
    // if(pFastKHmap->empty()) return;
    for(aite = pFastKHmap->begin(); pFastKHmap->end() != aite; ++aite)
        DeleKNodeLink(aite->second);
    pFastKHmap->clear();
}

//
struct FastKNode *NFastUtili::AddNewKNode(FastKHmap *pFastKHmap, struct attent *atte) {
    struct FastKNode *ast_new;
    //
// _LOG_DEBUG(_T("add new node! %08x %s"), atte->action_type, atte->file_name);
    ast_new = (struct FastKNode *)malloc(sizeof(struct FastKNode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastKNode));
	ast_new->dwActioType = atte->action_type;
	ast_new->atte = atte;
	//
	pFastKHmap->insert(FastKHmap::value_type(atte->file_name, ast_new));
    return ast_new;
}

struct FastKNode *NFastUtili::AppNewKNode(struct FastKNode *pFastKNode, struct attent *atte) {
    struct FastKNode *ast_new;
    //
// _LOG_DEBUG(_T("app new node! %08x %s"), atte->action_type, atte->file_name);
    ast_new = (struct FastKNode *)malloc(sizeof(struct FastKNode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastKNode));
	ast_new->dwActioType = atte->action_type;
	ast_new->atte = atte;
	//
	while(pFastKNode->pNext) pFastKNode = pFastKNode->pNext;
	pFastKNode->pNext = ast_new;
	return ast_new;
}

//
struct FastKNode *NFastUtili::InseToHmap(FastKHmap *pFastKHmap, struct attent *atte) {
	struct FastKNode *pFastKNode = NULL;
	FastKHmap::iterator iter;
	//
	iter = pFastKHmap->find(atte->file_name);
    if(pFastKHmap->end() == iter) {
		pFastKNode = AddNewKNode(pFastKHmap, atte);
	} else {
		pFastKNode = AppNewKNode(iter->second, atte);
	}
	//
	return pFastKNode;
}
	
VOID NFastUtili::DeleFromHmap(FastKHmap *pFastKHmap, struct attent *atte) {
	struct FastKNode *pPrevKNode, *pFastKNode;
	FastKHmap::iterator iter;
	//
	iter = pFastKHmap->find(atte->file_name);
    if(pFastKHmap->end() != iter) {
		pPrevKNode = NULL;
		pFastKNode = iter->second;
		while(pFastKNode) {
			if(atte == pFastKNode->atte) {
				if(pPrevKNode) {
					pPrevKNode->pNext = pFastKNode->pNext;
				} else {
					pFastKHmap->erase(iter);
					if(pFastKNode->pNext)
						pFastKHmap->insert(FastKHmap::value_type(atte->file_name, pFastKNode->pNext));
				}
				DeleKNode(pFastKNode);
				break;
			}
			pPrevKNode = pFastKNode;
			pFastKNode = pFastKNode->pNext;
		}
	}
}

VOID NFastUtili::DeleListHmap(FastKHmap *pFastKHmap, struct FastKNode *pListHead) {
	FastKHmap::iterator iter;
	//
	iter = pFastKHmap->find(pListHead->atte->file_name);
	if(pFastKHmap->end() != iter) {
		DeleKNodeLink(iter->second);
		pFastKHmap->erase(iter);
	}
}

//
struct FastKNode *NFastUtili::FindFromHmap(FastKHmap *pFindKHmap, struct attent *atte) {
	struct FastKNode *pFoundKNode = NULL;
	FastKHmap::iterator iter;
	//
// _LOG_DEBUG(_T("atte->file_name:%08x %x %s"), atte, atte->action_type, atte->file_name);
	iter = pFindKHmap->find(atte->file_name);
	if(pFindKHmap->end() != iter) {
		pFoundKNode = iter->second;
// _LOG_DEBUG(_T("pFoundKNode:%08x %x %s"), pFoundKNode->atte, pFoundKNode->atte->action_type, pFoundKNode->atte->file_name);
	}
	//
	return pFoundKNode;
}

// 有删除节点,要破坏链
struct attent *NFastUtili::FindNextValue(struct FastKNode *pNode) {
    static FilesVec lattent;
    static FilesVec::iterator toke;
//
    struct attent *find_value = NULL;
	if(pNode) {
		for(; pNode; pNode = pNode->pNext) {
			lattent.push_back(pNode->atte);
		}
        toke = lattent.begin();
        if(lattent.end() != toke) find_value = *toke;
	} else {
        toke++;
        if(lattent.end() != toke) find_value = *toke;
	}	
    if(!find_value) lattent.clear();
    return find_value;
}

//
void NFastUtili::BuildFastKHmap(FastKHmap *pFastKHmap, FilesVec *pFindVec) {
	FilesVec::iterator iter;
	FastKHmap::iterator site;
	//
    for(iter = pFindVec->begin(); pFindVec->end() != iter; ++iter) {
		site = pFastKHmap->find((*iter)->file_name);
        if(pFastKHmap->end() == site) {
			AddNewKNode(pFastKHmap, (*iter));
		} else {
			AppNewKNode((site)->second, (*iter));
		}
	}
	//
}

//////////////////////////////////////////////////////////////////////
static void DeleZNodeLink(struct FastZNode *pFastZNode) {
	struct FastZNode *pPrevZNode;
	while(pFastZNode) {
		pPrevZNode = pFastZNode;
		pFastZNode = pFastZNode->pNext;
		NFastUtili::DeleZNode(pPrevZNode);
	}
}

void NFastUtili::DeleFastZHmap(FastZHmap *pFastZHmap) {
    unordered_map <wstring, struct FastZNode*>::iterator aite;
    // if(pFastZHmap->empty()) return;
    for(aite = pFastZHmap->begin(); pFastZHmap->end() != aite; ++aite)
        DeleZNodeLink(aite->second);
    pFastZHmap->clear();
}

//
struct FastZNode *NFastUtili::AddNewZNode(FastZHmap *pFastZHmap, TCHAR *szFileName, struct LockActio *lock_acti) {
// _LOG_DEBUG(_T("add new node! %08x %s"), atte->action_type, atte->file_name);
    struct FastZNode *ast_new = (struct FastZNode *)malloc(sizeof(struct FastZNode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastZNode));
	ast_new->dwActioType = lock_acti->dwActioType;
	ast_new->lock_acti = lock_acti;
	//
	pFastZHmap->insert(FastZHmap::value_type(szFileName, ast_new));
    return ast_new;
}

struct FastZNode *NFastUtili::AddNewZNode(FastZHmap *pFastZHmap, struct LockActio *lock_acti) {
// _LOG_DEBUG(_T("add new node! %08x %s"), atte->action_type, atte->file_name);
    struct FastZNode *ast_new = (struct FastZNode *)malloc(sizeof(struct FastZNode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastZNode));
	ast_new->dwActioType = lock_acti->dwActioType;
	ast_new->lock_acti = lock_acti;
	//
	pFastZHmap->insert(FastZHmap::value_type(lock_acti->szFileName, ast_new));
    return ast_new;
}

struct FastZNode *NFastUtili::AppNewZNode(struct FastZNode *pFastZNode, struct LockActio *lock_acti) {
// _LOG_DEBUG(_T("app new node! %08x %s"), atte->action_type, atte->file_name);
    struct FastZNode *ast_new = (struct FastZNode *)malloc(sizeof(struct FastZNode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastZNode));
	ast_new->dwActioType = lock_acti->dwActioType;
	ast_new->lock_acti = lock_acti;
	//
	while(pFastZNode->pNext) pFastZNode = pFastZNode->pNext;
	pFastZNode->pNext = ast_new;
	return ast_new;
}

//
void NFastUtili::BuildFastZHmap(FastZHmap *pFastZHmap, LkAiVec *pLkAiVec) {
    LkAiVec::iterator iter;
	FastZHmap::iterator site;
	//
	TCHAR *fileName;
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
		if(!(*iter)->coupleID) {
			fileName = _tcsrchr((*iter)->szFileName, _T('\\'));
			site = pFastZHmap->find(++fileName);
			if(pFastZHmap->end() == site) {
				AddNewZNode(pFastZHmap, fileName, (*iter));
			} else {
				AppNewZNode((site)->second, (*iter));
			}		
		}
    }
	//
}

//
void NFastUtili::BuildFastZHmapEx(FastZHmap *pFastZHmap, LkAiVec *pLkAiVec) {
    LkAiVec::iterator iter;
	FastZHmap::iterator site;
	//
	struct LockActio *lock_acti;
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
		lock_acti = (*iter);
		site = pFastZHmap->find(lock_acti->szFileName);
        if(pFastZHmap->end() == site) {
			AddNewZNode(pFastZHmap, lock_acti);
		} else {
			AppNewZNode((site)->second, lock_acti);
		}
    }
	//
}

struct LockActio *NFastUtili::FindAddiZHmap(FastZHmap *pFastZHmap, TCHAR *szFileName) {
	unordered_map <wstring, struct FastZNode*>::iterator site;
	struct FastZNode *pFastZNode;
	struct LockActio *lock_acti = NULL;
//
	site = pFastZHmap->find(szFileName);
	if(pFastZHmap->end() != site) { 
		pFastZNode = (site)->second;
		while(pFastZNode) {
			if(ADDI & pFastZNode->lock_acti->dwActioType) {
				lock_acti = pFastZNode->lock_acti;
				break;
			}
			pFastZNode = pFastZNode->pNext;
		}	
	}
	//
	return lock_acti;
}

struct LockActio *NFastUtili::FindDeleZHmap(FastZHmap *pFastZHmap, TCHAR *szFileName) {
	unordered_map <wstring, struct FastZNode*>::iterator site;
	struct FastZNode *pFastZNode;
	struct LockActio *lock_acti = NULL;
//
	site = pFastZHmap->find(szFileName);
	if(pFastZHmap->end() != site) { 
		pFastZNode = (site)->second;
		while(pFastZNode) {
			if(DELE & pFastZNode->lock_acti->dwActioType) {
				lock_acti = pFastZNode->lock_acti;
				break;
			}
			pFastZNode = pFastZNode->pNext;
		}	
	}
	//
	return lock_acti;
}

//
//
void NFastUtili::DeleFastNHmap(FastNHmap *pFastNHmap) {
    unordered_map <DWORD, struct FastZNode*>::iterator aite;
    // if(pFastZHmap->empty()) return;
    for(aite = pFastNHmap->begin(); pFastNHmap->end() != aite; ++aite)
        DeleZNodeLink(aite->second);
    pFastNHmap->clear();
}

struct FastZNode *NFastUtili::AddNewZNode(FastNHmap *pFastNHmap, struct LockActio *lock_acti) {
// _LOG_DEBUG(_T("add new node! %08x %s"), atte->action_type, atte->file_name);
    struct FastZNode *ast_new = (struct FastZNode *)malloc(sizeof(struct FastZNode));
	if(!ast_new) return NULL;
    memset(ast_new, '\0', sizeof(struct FastZNode));
	ast_new->dwActioType = lock_acti->dwActioType;
	ast_new->lock_acti = lock_acti;
	//
	pFastNHmap->insert(FastNHmap::value_type(lock_acti->coupleID, ast_new));
    return ast_new;
}

//
void NFastUtili::BuildFastNHmap(FastNHmap *pFastNHmap, LkAiVec *pLkAiVec) {
    LkAiVec::iterator iter;
	FastNHmap::iterator site;
	//
	struct LockActio *lock_acti;
    for(iter=pLkAiVec->begin(); pLkAiVec->end()!=iter; ++iter) {
		lock_acti = (*iter);
		site = pFastNHmap->find(lock_acti->coupleID);
        if(pFastNHmap->end() == site) {
// _LOG_DEBUG(_T("++++++++ coupleID:%08X at:%08X add:%s"), lock_acti->coupleID, lock_acti->dwActioType, lock_acti->szFileName);
			AddNewZNode(pFastNHmap, lock_acti);
		} else {
// _LOG_DEBUG(_T("++++++++ coupleID:%08X at:%08X app:%s"), lock_acti->coupleID, lock_acti->dwActioType, lock_acti->szFileName);
			AppNewZNode((site)->second, lock_acti);
		}
    }
	//
}



