#pragma once

#include "common_macro.h"

#include "FilesVec.h"

#include <vector>
using std::vector;
//
struct TaskNode {
    DWORD node_inde;
    DWORD array_id;
    DWORD control_code;		/* add mod del list recu */
    DWORD success_flags;	// 0x00:success 0x01:undone 0x02:exception
	DWORD excepti;	// excep code
	DWORD isdire; // node is dire
    TCHAR builder_cache[MIN_TEXT_LEN];
    //
    TCHAR szFileName1[MAX_PATH];
    TCHAR szFileName2[MAX_PATH];
    unsigned __int64 offset;
    DWORD build_length;
    unsigned __int64 file_size;
    FILETIME last_write;		/* When the item was last modified */
    unsigned char correct_chks[MAX_CHKS_LEN]; // Todo:
    DWORD reserved;  /* reserved */
};

typedef vector <struct TaskNode *>  TNodeVec;

namespace NTNodeVec {
	inline VOID DeleNode(struct TaskNode *pNode)
	{ if(pNode) free(pNode); }
	VOID DeleTNodeV(TNodeVec *pTNodeV);
	//
	struct TaskNode *AppNewNode(TNodeVec *pTNodeV, struct TaskNode *pTNode);
};

// ”≈œ»º∂ get put copy mvoe putsync getsync delete
struct taskatt {
	DWORD action_type;		/* add mod del list recu */
	TCHAR file_name[MAX_PATH];
	unsigned __int64 file_size;
	FILETIME last_write;		/* When the item was last modified */
	DWORD reserved;  /* reserved */
};

typedef vector <struct taskatt *> TasksVec;

//
namespace NTasksVec {
	inline VOID DeleNode(struct taskatt *pNode)
	{ if(pNode) free(pNode); }
	VOID DeleTasksVec(TasksVec *pTasksVec);
	//
	struct taskatt *AppNewNode(TasksVec *pTasksVec, struct attent *atte);
	struct taskatt *AppNewNode(TasksVec *pTasksVec, struct attent *atte, DWORD action_type);
#define MVCPY_TYPE	0x0000
#define GPSYN_TYPE	0x0001
	struct taskatt *AppNewNode(TasksVec *pTasksVec, struct TaskNode *pTNode, DWORD dwActioType, DWORD dwNameType);
};

//
#define NEWN_DELN(pTasksVec, pNode) { \
	NTasksVec::AppNewNode(pTasksVec, pNode); \
	NFilesVec::DeleNode(pNode); \
}

#define NEWT_DELN(pTasksVec, pNode, AType) { \
	NTasksVec::AppNewNode(pTasksVec, pNode, AType); \
	NFilesVec::DeleNode(pNode); \
}

//
struct TasksCache {
    // server modify
    vector <struct taskatt *> stskatt;
    // client modify
    vector <struct taskatt *> ltskatt;
};

namespace NTaskCache {
	VOID DeleTasksCache(struct TasksCache *pTasksCache);
	DWORD MoveCache(struct TasksCache *pTo, struct TasksCache *pFrom);
	VOID CompleTasksCache(struct TasksCache *pCompCache, struct TasksCache *pTaskCache);
};

//
