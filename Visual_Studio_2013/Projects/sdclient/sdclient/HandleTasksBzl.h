#pragma once
#include "clientcom/clientcom.h"
#include "TRANSWorker.h"

//
#define	WORKER_VALUE_FAULT	SYNP_VALUE_FAULT		// -1
#define	WORKER_VALUE_RUNNING	SYNP_VALUE_RUNNING		// 0x00000000
#define	WORKER_VALUE_WAITING	SYNP_VALUE_WAITING		// 0x00000001
#define	WORKER_VALUE_PAUSED	SYNP_VALUE_PAUSED		// 0x00000002
#define	WORKER_VALUE_FORCESTOP	SYNP_VALUE_FORCESTOP	// 0x00000004
#define	WORKER_VALUE_FINISH	SYNP_VALUE_FINISH		// 0x00000008
#define	WORKER_VALUE_FAILED	SYNP_VALUE_FAILED		// 0x00000010
#define	WORKER_VALUE_KILLED	SYNP_VALUE_KILLED		// 0x00000020
#define	WORKER_VALUE_NETWORK	SYNP_VALUE_NETWORK		// 0x00000040
#define	WORKER_VALUE_LOCKED	SYNP_VALUE_LOCKED		// 0x00000080
#define	WORKER_VALUE_EXCEPTION	SYNP_VALUE_EXCEPTION	// 0x00010000

//
#define CONVERT_WORKER_STATUS(RET_VALUE, TRANSMIT_STATUS) { \
	switch(TRANSMIT_STATUS) { \
	case PROCESS_STATUS_FORCESTOP: \
		RET_VALUE = WORKER_VALUE_FORCESTOP; \
		break; \
	case PROCESS_STATUS_PAUSED: \
		RET_VALUE = WORKER_VALUE_PAUSED; \
		break; \
	case PROCESS_STATUS_KILLED: \
		RET_VALUE = WORKER_VALUE_KILLED; \
		break; \
	} \
}

namespace NHandTasksBzl {
	DWORD BeginWorkerThread(struct LocalConfig *pLocalConfig);
};

//
#define CTRL_ARRAY_INIT			6
#define ARRAY_VALUE				5
#define FIVE_TRANS_THREAD		5
#define INVALID_ARRAY_VALUE		((LONG)-1)

struct WORKER_ARGUMENT {
	volatile LONG array_locked[CTRL_ARRAY_INIT]; // 0 unlocked, not 0 locked
	struct SessionArgu	tSeionArgu;
	struct SocketsArgu	tSockeArgu;
	struct ClientConfig	*pClientConfig;
	//
};

namespace NTLUtility {
	int NodePrepare(struct TaskNode *task_node);
	// 等待线程全部启动
	VOID WaitingThread(volatile LONG *array_locked);
	// 检查同组节点状态
	VOID ValidArrayLocked(volatile LONG *array_locked, volatile LONG *worker_locked, struct TaskNode *task_node);
	int HandleFileNode(CTRANSWorker *pTransWorker, struct TaskNode *task_node, DWORD dwThreadInde);
	int HandleDireNode(CTRANSWorker *pTransWorker, struct TaskNode *task_node, DWORD dwThreadInde);
	DWORD NodeTransmit(CTRANSWorker *pTransWorker, struct TaskNode *task_node, DWORD dwThreadInde);
	DWORD FinishTransmit(struct TaskNode *task_node, const TCHAR *szDriveLetter);
	VOID HandleUpdateExceptAttrib(struct TaskNode *task_node, const TCHAR *szDriveLetter);
	//
	CTRANSWorker *CreateWorker(DWORD *cvalue, struct WORKER_ARGUMENT *pWrokerArgu);
	void DropWorker(CTRANSWorker *pTransWorker);
	void DestroyWorker(CTRANSWorker *pTransWorker);
};