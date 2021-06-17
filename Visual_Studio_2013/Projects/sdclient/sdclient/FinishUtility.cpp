#include "StdAfx.h"
//
#include "FinishUtility.h"
//
CTRANSWorker *FinishUtility::CreateWorker(DWORD *cvalue, struct FINISH_ARGUMENT *pFinishArgu) {
	CTRANSWorker *pFinishWorker = NTRANSWorker::Factory(&pFinishArgu->tSeionArgu, &pFinishArgu->tSockeArgu);
	if (!pFinishWorker) {
		NTRANSWorker::DestroyObject(pFinishWorker);
		*cvalue = FINISH_VALUE_NETWORK;
		return NULL;
	}
	// 生成工作记录,初始会话
	int proto_value = PROTO_FAULT;
	if(proto_value = pFinishWorker->Initial()) {
		NTRANSWorker::DestroyObject(pFinishWorker);
		*cvalue = PROTO_NETWORK == proto_value? FINISH_VALUE_NETWORK: FINISH_VALUE_FAILED;
		pFinishWorker = NULL;
	}
	return pFinishWorker;
}
//
void FinishUtility::DropWorker(CTRANSWorker *pFinishWorker){
	if(pFinishWorker)
		NTRANSWorker::DestroyObject(pFinishWorker);
}
//
void FinishUtility::DestroyWorker(CTRANSWorker *pFinishWorker){
	if(pFinishWorker) {
		// pFinishWorker->Final(FINAL_STATUS_END);
		NTRANSWorker::DestroyObject(pFinishWorker);
	}
}
