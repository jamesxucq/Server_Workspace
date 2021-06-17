#pragma once

#include "kchashdb.h"
// #include "dbz_t.h"

using namespace std;
using namespace kyotocabinet;

#include "clientcom/clientcom.h"
#include "TRANSWorker.h"
#include "BuildTasksBzl.h"

//
namespace ListUtility {
	DWORD OptimizeSlowVec(struct TasksCache *slowvec, HANDLE hsatt, TreeDB *lattdb);
	DWORD OptimizeColiVec(FilesVec *sattent, FilesVec *lattent, FileColiVec *pColiVec);
	DWORD OptimizeFastVec(struct TasksCache *fastvec, FilesVec *sattent, FilesVec *lattent);
	//
	CTRANSWorker *CreateWorker(DWORD *cvalue, struct LIST_ARGUMENT *pListArgu);
	void DropWorker(CTRANSWorker *pListWorker);
	void DestroyWorker(CTRANSWorker *pListWorker, DWORD dwFinalType);
	//
	int FindExceptTNode(TNodeVec *pTNodeV);
};

//
namespace ProduceUtility {
	static VOID AddFileMovecNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddFileMovesNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddFileCopycNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddFileCopysNode(struct taskatt *exist, struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddFileDelecNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddFileDelesNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddFileGetNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddFilePutNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID FilePutSynchNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID FileGetSynchNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	//
	static VOID AddDireDelecNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddDireDelesNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddDireGetNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	static VOID AddDirePutNode(struct taskatt *katt, DWORD dwTaskCode, DWORD dwArrayID);
	//
	static VOID BuildServerTlist(TasksVec *stskatt, DWORD *dwArrayID);
	static VOID BuildClientTlist(TasksVec *ltskatt, DWORD *dwArrayID);
	DWORD CreateTasksList(struct TasksCache *pTasksCache);
};

//