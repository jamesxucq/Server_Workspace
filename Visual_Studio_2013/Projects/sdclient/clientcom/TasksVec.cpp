#include "StdAfx.h"

//
#include "TasksVec.h"

//
VOID NTNodeVec::DeleTNodeV(TNodeVec *pTNodeV) {
	TNodeVec::iterator iter;
// if(pTasksVec->empty()) return;
	for(iter = pTNodeV->begin(); pTNodeV->end() != iter; ++iter)
		DeleNode(*iter);
	pTNodeV->clear();
}

struct TaskNode *NTNodeVec::AppNewNode(TNodeVec *pTNodeV, struct TaskNode *pTNode) {
    struct TaskNode *tnod_new;
    // SYSTEMTIME ftLastWrite;
    tnod_new = (struct TaskNode *)malloc(sizeof(struct TaskNode));
    if(!tnod_new) return NULL;
	memcpy(tnod_new, pTNode, sizeof(struct TaskNode));
	//
    pTNodeV->push_back(tnod_new);
    return tnod_new;
}

//
//
VOID NTasksVec::DeleTasksVec(TasksVec *pTasksVec) {
	TasksVec::iterator iter;
// if(pTasksVec->empty()) return;
	for(iter = pTasksVec->begin(); pTasksVec->end() != iter; ++iter)
		DeleNode(*iter);
	pTasksVec->clear();
}

//
struct taskatt *NTasksVec::AppNewNode(TasksVec *pTasksVec, struct attent *atte) {
    struct taskatt *katt_new;
    // SYSTEMTIME ftLastWrite;
    katt_new = (struct taskatt *)malloc(sizeof(struct taskatt));
    if(!katt_new) return NULL;
	// memcpy(katt_new, atte, sizeof(struct attent));
	katt_new->action_type = atte->action_type;
	_tcscpy_s(katt_new->file_name, atte->file_name);
	katt_new->file_size = atte->file_size;
	memcpy(&katt_new->last_write, &atte->last_write, sizeof(FILETIME));
	katt_new->reserved = atte->reserved;
	//
    pTasksVec->push_back(katt_new);
    return katt_new;
}

struct taskatt *NTasksVec::AppNewNode(TasksVec *pTasksVec, struct attent *atte, DWORD action_type) {
    struct taskatt *katt_new;
    // SYSTEMTIME ftLastWrite;
    katt_new = (struct taskatt *)malloc(sizeof(struct taskatt));
    if(!katt_new) return NULL;
	// memcpy(katt_new, atte, sizeof(struct attent));
	katt_new->action_type = action_type;
	_tcscpy_s(katt_new->file_name, atte->file_name);
	katt_new->file_size = atte->file_size;
	memcpy(&katt_new->last_write, &atte->last_write, sizeof(FILETIME));
	katt_new->reserved = atte->reserved;
	//
    pTasksVec->push_back(katt_new);
    return katt_new;
}

struct taskatt *NTasksVec::AppNewNode(TasksVec *pTasksVec, struct TaskNode *pTNode, DWORD dwActioType, DWORD dwNameType) {
    struct taskatt *katt_new;
    // SYSTEMTIME ftLastWrite;
    katt_new = (struct taskatt *)malloc(sizeof(struct taskatt));
    if(!katt_new) return NULL;
	// memcpy(katt_new, atte, sizeof(struct attent));
	katt_new->action_type = dwActioType;
	if(dwNameType) _tcscpy_s(katt_new->file_name, pTNode->szFileName1);
	else _tcscpy_s(katt_new->file_name, pTNode->szFileName2);
	katt_new->file_size = ((unsigned __int64)-1);
	memcpy(&katt_new->last_write, &pTNode->last_write, sizeof(FILETIME));
	katt_new->reserved = pTNode->reserved;
	//
    pTasksVec->push_back(katt_new);
    return katt_new;
}

//
//
VOID NTaskCache::DeleTasksCache(struct TasksCache *pTasksCache) {
    if(pTasksCache) {
        NTasksVec::DeleTasksVec(&pTasksCache->ltskatt);
        NTasksVec::DeleTasksVec(&pTasksCache->stskatt);
    }
}

DWORD NTaskCache::MoveCache(struct TasksCache *pTo, struct TasksCache *pFrom) {
	TasksVec::iterator iter;
	//
	if(!pTo || !pFrom) return ((DWORD)-1);
	//
	TasksVec *pToVec = &pTo->stskatt;
	TasksVec *pFromVec = &pFrom->stskatt;
	for(iter = pFromVec->begin(); pFromVec->end() != iter; ) {
		pToVec->push_back(*iter);
		iter = pFromVec->erase(iter);
    }
	pFromVec->clear();
	//
	pToVec = &pTo->ltskatt;
	pFromVec = &pFrom->ltskatt;
	for(iter = pFromVec->begin(); pFromVec->end() != iter; ) {
		pToVec->push_back(*iter);
		iter = pFromVec->erase(iter);
    }
	pFromVec->clear();
	//
	return 0x00;
}

VOID NTaskCache::CompleTasksCache(struct TasksCache *pCompCache, struct TasksCache *pTaskCache) {
	TasksVec::iterator iter;
	TasksVec::iterator ater;
	//
	TasksVec *pComVec = &pCompCache->stskatt;
	TasksVec *pTskVec = &pTaskCache->stskatt;
	for(iter = pComVec->begin(); pComVec->end() != iter; ++iter) {
		for(ater = pTskVec->begin(); pTskVec->end() != ater; ++ater) {
			if(!_tcscmp((*iter)->file_name, (*ater)->file_name)) {
				(*iter)->file_size = (*ater)->file_size;
				break;
			}
		}
    }
	//
	pComVec = &pCompCache->ltskatt;
	pTskVec = &pTaskCache->ltskatt;
	for(iter = pComVec->begin(); pComVec->end() != iter; ++iter) {
		for(ater = pTskVec->begin(); pTskVec->end() != ater; ++ater) {
			if(!_tcscmp((*iter)->file_name, (*ater)->file_name)) {
				(*iter)->file_size = (*ater)->file_size;
				break;
			}
		}
    }
}

//
