#pragma once

#include "clientcom/clientcom.h"
#include "TRANSWorker.h"
#include "VerifyTasksBzl.h"

namespace NVerifyUtility {
	DWORD FileSha1Request(HANDLE hFileValid, CTRANSWorker *pVerifyWorker, struct TaskNode *task_node, DWORD dwCacheVerify);
	DWORD FileChksRequest(HANDLE hFileValid, CTRANSWorker *pVerifyWorker, struct TaskNode *task_node, DWORD dwCacheVerify);
	DWORD ChunkChksRequest(HANDLE hFileValid, CTRANSWorker *pVerifyWorker, struct TaskNode *task_node, DWORD dwCacheVerify);
	DWORD FileSha1River(unsigned char *szSha1Chks, CTRANSWorker *pVerifyWorker, TCHAR *szFileName);
	DWORD FileChksRiver(HANDLE hRiveChks, CTRANSWorker *pVerifyWorker, TCHAR *szFileName);
	DWORD VerifyTasksNode(struct TaskNode *task_node, unsigned char *szSha1Chks, HANDLE hFileValid, DWORD dwReqOk);
	DWORD VerifyTasksNode(struct TaskNode *task_node, HANDLE hFileLocal, HANDLE hFileValid, DWORD dwReqOk);
	//
	DWORD Sha1ValidTransmit(struct TaskNode *task_node, HANDLE hFileValid, DWORD dwReqOk);
	DWORD FileValidTransmit(struct TaskNode *task_node, HANDLE hFileValid, DWORD dwReqOk);
	DWORD ChunkValidTransmit(struct TaskNode *task_node, HANDLE hFileValid);
	DWORD ValidTransmitDownd(struct TaskNode *task_node);
	//
	CTRANSWorker *CreateWorker(DWORD *cvalue, struct VERIFY_ARGUMENT *pVerifyArgu);
	void DropWorker(CTRANSWorker *pVerifyWorker);
	void DestroyWorker(CTRANSWorker *pVerifyWorker);
	inline void CloseVerifyHandle() {
		objVerifyVector.CloseResource();
	}
};
