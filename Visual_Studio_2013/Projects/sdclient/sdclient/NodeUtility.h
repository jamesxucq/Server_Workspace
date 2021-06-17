#pragma once

#include "TRANSWorker.h"
//
struct NODE_ARGUMENT {
	struct SessionArgu	tSeionArgu;
	struct SocketsArgu	tSockeArgu;
};

//
namespace NodeUtility {
#define HAVE_TASK_VALU 0x0001
#define SERV_CHAN_VALU 0x0100
	DWORD CheckChanValue();
	DWORD ValidTasksFinished();
	//
	DWORD CheckVerifyFinished();
	DWORD CheckUploadVerify();
	//
	static CTRANSWorker *CreateWorker(struct NODE_ARGUMENT *pNodeArgu);
	static void DestroyWorker(CTRANSWorker *pNodeWorker, DWORD dwFinalType);
	VOID DelayExceptionWarning(struct LocalConfig *pLocalConfig);
};
