#include "StdAfx.h"

#include "../third party/RiverFS/RiverFS.h"
#include "DebugPrinter.h"
#include "VerifyUtility.h"
#include "ExecuteTransmitBzl.h"
#include "ShellLinkBzl.h"
#include "KeepAlive.h"
#include "LocalBzl.h"

#include "HandleTasksBzl.h"

//
#define TNODE_PREPA_FAULT		-1
#define FINISH_TRANSMIT			0
#define TNODE_PREPA_OKAY		1
//
static int prepa_node;
// 读取工作节点
CCriticalSection gcsPrepaNode;
int NTLUtility::NodePrepare(struct TaskNode *task_node) {
    DWORD node_inde;
    int read_value;
    int prepa_value = TNODE_PREPA_FAULT;
    //
    gcsPrepaNode.Lock();
    for(; ; ) {
        node_inde = objTaskList.GetHandNodeAdd();
        read_value = objTaskList.ReadNodeEx(task_node, node_inde);
        if(!read_value) {
            prepa_value = FINISH_TRANSMIT;
            break;
        } else if(0 > read_value) {
            prepa_value = TNODE_PREPA_FAULT;
            break;
        }
// _LOG_DEBUG(_T("prepa_node:%d node_inde:%d array_id:%d"), ++prepa_node, node_inde, task_node->array_id);
        //
		if(ONLY_TASK_OKAY(task_node->success_flags) || (((STYPE_PUT|STYPE_GSYNC)&task_node->control_code) && !ONLY_VERIFY_FAIL(task_node->success_flags))) {
// _LOG_DEBUG(_T("continue success_flags:%x control_code:%x"), task_node->success_flags, task_node->control_code);
            continue;
		}
ndp::TaskNode(task_node); // disable by james 20130415
        prepa_value = TNODE_PREPA_OKAY;
        break;
    }
    //
    gcsPrepaNode.Unlock();
    //
    return prepa_value;
}

//
// 等待线程全部启动
VOID NTLUtility::WaitingThread(volatile LONG *array_locked) {
    DWORD thread_index;
    volatile LONG lArrayValue, lPrepaValue;
    DWORD array_value;
    //
    for(array_value = 0x00; ; array_value = 0x00) {
        InterlockedExchange(&lPrepaValue, array_locked[ARRAY_VALUE]);
        if(!lPrepaValue) break;
        for(thread_index = 0x00; thread_index < FIVE_TRANS_THREAD; ++thread_index) {
            InterlockedExchange(&lArrayValue, array_locked[thread_index]);
            array_value |= lArrayValue;
        }
        if(!array_value) {
            InterlockedExchange(&array_locked[ARRAY_VALUE], 0x00);
            break;
        }
        Sleep(128);
    }
}

// 检查同组节点状态
// 确定同时是一组节点,节点号递增
#define VALID_ARRAY_DELAY	192
CCriticalSection gcsArrayLocked;
VOID NTLUtility::ValidArrayLocked(volatile LONG *array_locked, volatile LONG *worker_locked, struct TaskNode *task_node) {
    DWORD thread_index;
    volatile LONG lArrayValue, lRunValue;
    //
    InterlockedExchange(&lRunValue, array_locked[ARRAY_VALUE]);
    if(lRunValue != task_node->array_id) {
// _LOG_DEBUG(_T("lRunValue:%d task_node->array_id:%d"), lRunValue, task_node->array_id);
        InterlockedExchange(worker_locked, task_node->array_id);
        //
        for(; ; ) {
            BOOL bNextArray = TRUE;
            unsigned long lMinValue = task_node->array_id;
            //
            gcsArrayLocked.Lock(); // LOCK
            InterlockedExchange(&lRunValue, array_locked[ARRAY_VALUE]);
// _LOG_DEBUG(_T("lRunValue:%d task_node->array_id:%d"), lRunValue, task_node->array_id);
            if(lRunValue != task_node->array_id) {
                for(thread_index = 0x00; thread_index < FIVE_TRANS_THREAD; ++thread_index) {
                    InterlockedExchange(&lArrayValue, array_locked[thread_index]);
// _LOG_DEBUG(_T("thread_index:%d, lArrayValue:%d, task_node->array_id:%d"), thread_index, lArrayValue, task_node->array_id); // disable by james 20140410
                    if(lRunValue == lArrayValue) {
                        bNextArray = FALSE;
                        break;
                    }
                    if(lMinValue > (unsigned long)lArrayValue) lMinValue = lArrayValue;
                }
                if(bNextArray) InterlockedExchange(&array_locked[ARRAY_VALUE], lMinValue);
            }
            gcsArrayLocked.Unlock(); // UNLOCK
            //
            if(bNextArray && (task_node->array_id == lMinValue)) break;
            else Sleep(VALID_ARRAY_DELAY);
// _LOG_DEBUG(_T("thread_index:%d, task_node->array_id:%d, array locked! waiting"), thread_index, task_node->array_id); // disable by james 20140410
        }
    }
_LOG_DEBUG(_T("array not locked!task_node->array_id:%d task_node->node_inde:%d"), task_node->array_id, task_node->node_inde); // disable by james 20130507
}

//
// 执行同步任务
int NTLUtility::HandleFileNode(CTRANSWorker *pTransWorker, struct TaskNode *task_node, DWORD dwThreadInde) {
    if(!pTransWorker || !task_node) return PROTO_FAULT;
    DWORD control_code = task_node->control_code;
    //
    int handle_value = PROTO_FAULT;
    switch(TNODE_STYPE_BYTE(control_code)) {
    case STYPE_GET:
// _LOG_DEBUG(_T("get fileName1:%s"), task_node->szFileName1);
        handle_value = pTransWorker->GetFile(task_node);
// _LOG_DEBUG(_T("______ handle_value:%d"), handle_value);
        break;
    case STYPE_PUT:
// _LOG_DEBUG(_T("put fileName1:%s"), task_node->szFileName1);
        handle_value = pTransWorker->PutFile(task_node);
// _LOG_DEBUG( _T(":b ++++++++++++++++++ handle_value:%08X"), handle_value);
        break;
    case STYPE_PSYNC:
// _LOG_DEBUG(_T("psync fileName1:%s"), task_node->szFileName1);
        handle_value = pTransWorker->PutSynchron(task_node);
        break;
    case STYPE_GSYNC:
// _LOG_DEBUG(_T("gsync fileName1:%s"), task_node->szFileName1);
        handle_value = pTransWorker->GetSynchron(task_node);
        break;
    case STYPE_MOVE:
// _LOG_DEBUG(_T("move fileName1:%s fileName2:%s"), task_node->szFileName1, task_node->szFileName2); // disable by james 20140410
		if(LOCATION_CLIENT & control_code) {
			handle_value = pTransWorker->MoveLocal(task_node);
			if(objKeepAlive.Continue(dwThreadInde)) pTransWorker->KeepAlive(); // keep alive
		} else if(LOCATION_SERVER & control_code) {
            do {
                handle_value = pTransWorker->MoveServer(task_node);
            } while(PROTO_PROCESSING == handle_value);
        }
        break;
    case STYPE_COPY:
// _LOG_DEBUG(_T("copy fileName1:%s fileName2:%s"), task_node->szFileName1, task_node->szFileName2); // disable by james 20140410
		if(LOCATION_CLIENT & control_code) {
			do {
				handle_value = pTransWorker->CopyLocal(task_node);
				if(objKeepAlive.Continue(dwThreadInde)) pTransWorker->KeepAlive(); // keep alive
			} while(PROTO_PROCESSING == handle_value);
		} else if(LOCATION_SERVER & control_code) {
            do {
                handle_value = pTransWorker->CopyServer(task_node);
            } while(PROTO_PROCESSING == handle_value);
        }
        break;
    case STYPE_DELETE:
		if(LOCATION_CLIENT & control_code) {
// _LOG_DEBUG(_T("dele c fileName1:%s"), task_node->szFileName1);
			handle_value = pTransWorker->DeleLocaFile(task_node);
			if(objKeepAlive.Continue(dwThreadInde)) pTransWorker->KeepAlive(); // keep alive
		} else if(LOCATION_SERVER & control_code) {
            do {
// _LOG_DEBUG(_T("dele s fileName1:%s"), task_node->szFileName1);
                handle_value = pTransWorker->DeleServFile(task_node);
            } while(PROTO_PROCESSING == handle_value);
        }
        break;
    default:
// _LOG_DEBUG(_T("default direName1:%s"), task_node->szFileName1);
        handle_value = PROTO_FAULT;
    }
_LOG_DEBUG(_T("handle file value:%08x"), handle_value);
    return handle_value;
}

//
int NTLUtility::HandleDireNode(CTRANSWorker *pTransWorker, struct TaskNode *task_node, DWORD dwThreadInde) {
    if(!pTransWorker || !task_node) return PROTO_FAULT;
    DWORD control_code = task_node->control_code;
    //
    int handle_value = PROTO_FAULT;
    switch(TNODE_STYPE_BYTE(control_code)) {
    case STYPE_GET:
// _LOG_DEBUG(_T("get direName1:%s"), task_node->szFileName1);
        handle_value = pTransWorker->GetDire(task_node);
// if(handle_value) _LOG_DEBUG(_T("______ handle_value:%d"), handle_value);
        break;
    case STYPE_PUT:
// _LOG_DEBUG(_T("put direName1:%s"), task_node->szFileName1);
        handle_value = pTransWorker->PutDire(task_node);
        break;
    case STYPE_DELETE:
		if(LOCATION_CLIENT & control_code) {
// _LOG_DEBUG(_T("dele c direName1:%s"), task_node->szFileName1);
			handle_value = pTransWorker->DeleLocaDire(task_node);
			if(objKeepAlive.Continue(dwThreadInde)) pTransWorker->KeepAlive(); // keep alive
		} else if(LOCATION_SERVER & control_code) {
            do {
// _LOG_DEBUG(_T("dele s direName1:%s"), task_node->szFileName1);
                handle_value = pTransWorker->DeleServDire(task_node);
            } while(PROTO_PROCESSING == handle_value);
        }
        break;
    default:
// _LOG_DEBUG(_T("default direName1:%s"), task_node->szFileName1);
        handle_value = PROTO_FAULT;
    }
_LOG_DEBUG(_T("handle dire value:%08x"), handle_value);
    return handle_value;
}

// static int node_transmit;
// 执行传输工作
DWORD NTLUtility::NodeTransmit(CTRANSWorker *pTransWorker, struct TaskNode *task_node, DWORD dwThreadInde) {
// _LOG_DEBUG(_T("node_transmit:%d"), ++node_transmit);
    DWORD handle_value = WORKER_VALUE_FINISH;
_LOG_DEBUG(_T("--- node transmit. node_inde:%d isdire:%d szFileName1:%s"), task_node->node_inde, task_node->isdire, task_node->szFileName1);
	//
	if(task_node->isdire) {
		switch(NTLUtility::HandleDireNode(pTransWorker, task_node, dwThreadInde)) {
		case PROTO_FAULT:
			SET_TRANSMIT_EXCEPTION(task_node->success_flags);
			SET_INTERNAL_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_EXCEPTION;
_LOG_WARN(_T("handle syncing tasks exception!"));
			break;
		case PROTO_OKAY:
			SET_TRANSMIT_DONE(task_node->success_flags);
			handle_value = WORKER_VALUE_FINISH;
// _LOG_DEBUG(_T("handle syncing tasks success! task_node->node_inde:%d"), task_node->node_inde);  // disable by james 20140410
			break;
		case PROTO_NETWORK:
			SET_TRANSMIT_EXCEPTION(task_node->success_flags);
			SET_NETWORK_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_NETWORK;
			break;
		case PROTO_LOCKED:
			SET_TRANSMIT_EXCEPTION(task_node->success_flags);
			SET_LOCKED_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_EXCEPTION;
_LOG_WARN(_T("handle syncing tasks exception!"));
			break;
		case PROTO_NOT_FOUND:
			SET_TASK_WAIVE(task_node->success_flags);
			SET_CREATE_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_EXCEPTION;
_LOG_WARN(_T("handle syncing tasks exception!"));
			break;
		}
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
	} else {
		switch(NTLUtility::HandleFileNode(pTransWorker, task_node, dwThreadInde)) {
		case PROTO_FAULT:
			SET_TRANSMIT_EXCEPTION(task_node->success_flags);
			SET_INTERNAL_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_EXCEPTION;
_LOG_WARN(_T("handle syncing tasks exception!"));
			break;
		case PROTO_OKAY:
			SET_TRANSMIT_DONE(task_node->success_flags);
			handle_value = WORKER_VALUE_FINISH;
// _LOG_DEBUG(_T("handle syncing tasks success! task_node->node_inde:%d"), task_node->node_inde);  // disable by james 20140410
			break;
		case PROTO_NETWORK:
			SET_TRANSMIT_EXCEPTION(task_node->success_flags);
			SET_NETWORK_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_NETWORK;
			break;
		case PROTO_LOCKED:
			SET_TRANSMIT_EXCEPTION(task_node->success_flags);
			SET_LOCKED_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_EXCEPTION;
_LOG_WARN(_T("handle syncing tasks exception!"));
			break;
		case PROTO_CREATE:
		case PROTO_NOT_FOUND:
			SET_TASK_WAIVE(task_node->success_flags);
			SET_CREATE_EXP(task_node->excepti);
			handle_value = WORKER_VALUE_EXCEPTION;
_LOG_WARN(_T("handle syncing tasks exception! success_flags:%08x"), task_node->success_flags);
			break;
		}
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
	}
_LOG_DEBUG(_T("node transmit. handle_value:%08x"), handle_value);
    return handle_value;
}

//
//
CCriticalSection gcsRiverUpdate;
static VOID HandleRiverDire(struct TaskNode *task_node, const TCHAR *szDriveLetter) {
	if(STYPE_GET & task_node->control_code) {
		gcsRiverUpdate.Lock(); // LOCK
_LOG_DEBUG(_T("appe folder:%s"), task_node->szFileName1);
		RiverFS::FolderAppendVs(task_node->szFileName1, szDriveLetter);
		gcsRiverUpdate.Unlock(); // UNLOCK
		//
		SET_OWRTMRVE_DONE(task_node->success_flags);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
	} else if (STYPE_DELETE & task_node->control_code) {
		gcsRiverUpdate.Lock(); // LOCK
_LOG_DEBUG(_T("dele folder:%s"), task_node->szFileName1);
		RiverFS::EraseFolderVs(task_node->szFileName1);
		gcsRiverUpdate.Unlock(); // UNLOCK
		//
		SET_OWRTMRVE_DONE(task_node->success_flags);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
	}
}

// 执行覆盖写数据工作
static VOID FileMateUpdate(struct TaskNode *task_node, const TCHAR *szDriveLetter) {
	DWORD handle_value = NTRANSHandler::HandleOwriteModified(task_node, szDriveLetter);
	switch(handle_value) {
	case OWRITE_STATUS_FAULT:
		SET_OWRTMRVE_EXCEPTION(task_node->success_flags);
		SET_CREATE_EXP(task_node->excepti);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
		break;
	case OWRITE_STATUS_SUCCESS:
		// 插入正确的效验植 // 20131119
		gcsRiverUpdate.Lock(); // LOCK
		RiverFS::ChunkUpdate(task_node->szFileName1, task_node->offset >> 22, task_node->build_length,
			task_node->correct_chks, &task_node->last_write, szDriveLetter);
_LOG_DEBUG(_T("finish transmit chunk update."));
		gcsRiverUpdate.Unlock(); // UNLOCK
		//
		SET_OWRTMRVE_DONE(task_node->success_flags);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
		break;
	case OWRITE_STATUS_FAILED:
		SET_OWRTMRVE_EXCEPTION(task_node->success_flags);
		SET_LOCKED_EXP(task_node->excepti);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
		break;
	}
}

static VOID HandleRiverFile(struct TaskNode *task_node, const TCHAR *szDriveLetter) {
	if((STYPE_GET|STYPE_PSYNC) & task_node->control_code) {
		// 执行覆盖写数据工作
		FileMateUpdate(task_node, szDriveLetter);
	} else if (STYPE_DELETE & task_node->control_code) {
		gcsRiverUpdate.Lock(); // LOCK
		RiverFS::DeleFileV(task_node->szFileName1);
		gcsRiverUpdate.Unlock(); // UNLOCK
		//
		SET_OWRTMRVE_DONE(task_node->success_flags);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
	} else if (STYPE_MOVE & task_node->control_code) { 
		gcsRiverUpdate.Lock(); // LOCK
		RiverFS::FileMoveV(task_node->szFileName1, task_node->szFileName2, szDriveLetter);
		gcsRiverUpdate.Unlock(); // UNLOCK
		//
		SET_OWRTMRVE_DONE(task_node->success_flags);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
	} else if (STYPE_COPY & task_node->control_code) { 
		gcsRiverUpdate.Lock(); // LOCK
		RiverFS::FileCopyV(task_node->szFileName1, task_node->szFileName2, szDriveLetter);
		gcsRiverUpdate.Unlock(); // UNLOCK
		//
		SET_OWRTMRVE_DONE(task_node->success_flags);
		objTaskList.WriteNodeEx(task_node, task_node->node_inde);
	}
}

// static int finish_count;
// static int entry_count;
DWORD NTLUtility::FinishTransmit(struct TaskNode *task_node, const TCHAR *szDriveLetter) {
	// _LOG_DEBUG(_T("entry_count:%d"), ++entry_count);
	// 初始化此组未完成的节点数 add by james 20131212
	if((STYPE_GET|STYPE_PSYNC|STYPE_MOVE|STYPE_COPY|STYPE_DELETE) & task_node->control_code) {
		objTaskList.InitArray(task_node->array_id, task_node->node_inde);
	}
	//// _LOG_DEBUG(_T("finish transmit"));
	if(LOCATION_CLIENT & TNODE_LOCATION_BYTE(task_node->control_code)) {
		if(!ONLY_TRANSMIT_VERIFY_OKAY(task_node->success_flags) && ONLY_OWRTMRVE_OKAY(task_node->success_flags)) {
			if(task_node->isdire) HandleRiverDire(task_node, szDriveLetter);
			else HandleRiverFile(task_node, szDriveLetter);
		}	
	}
	// _LOG_DEBUG(_T("finish value:%X finish_count:%d"), handle_value, ++finish_count);
	return WORKER_VALUE_FINISH;
}

CCriticalSection gcsUpdateAttrib;
VOID NTLUtility::HandleUpdateExceptAttrib(struct TaskNode *task_node, const TCHAR *szDriveLetter) {
	static DWORD dwIndeTatol;
	TCHAR szFileName[MAX_PATH];
	// 刷新非上传数据图标缓存
_LOG_DEBUG(_T("handle except refresh icon!")); // disable by james 20140410
	if((STYPE_GET|STYPE_PSYNC|STYPE_MOVE|STYPE_COPY|STYPE_DELETE) & task_node->control_code) {
		gcsUpdateAttrib.Lock(); // LOCK // success
_LOG_DEBUG(_T("handle except szFileName:%s control_code:%08X, success_flags:%08X dwIndeTatol:%d"), task_node->szFileName1, task_node->control_code, task_node->success_flags, dwIndeTatol); // disable by james 20140410
		if(ONLY_TASK_OKAY(task_node->success_flags)) {
			if(++dwIndeTatol == objTaskList.ArrayCapacity()) {
_LOG_DEBUG(_T("dwIndeTatol:%u"), dwIndeTatol); // disable by james 20140410
				DRIVE_LETTE(szFileName, szDriveLetter)
				_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, task_node->szFileName1);
				// update really file lastwrite
				if(!task_node->isdire && ((STYPE_GET|STYPE_PSYNC)&task_node->control_code)) {
_LOG_DEBUG(_T("handle file update."));
					RiverFS::FileUpdate(task_node->szFileName1, &task_node->last_write, szDriveLetter);
// TCHAR timestr[64];
// _LOG_DEBUG(_T("szFileName:%s last_write:%s"), szFileName, timcon::ftim_unis(timestr, &task_node->last_write));
					NFileUtility::SetFileSizeLawi(szFileName, task_node->file_size, &task_node->last_write);
				}
				LINKER_DISPLAY_SUCCESS(szFileName, task_node->isdire)
_LOG_DEBUG(_T("first success:%s success_flags:%08x dwIndeTatol:%d"), szFileName, task_node->success_flags, dwIndeTatol);
				if((STYPE_MOVE|STYPE_COPY)&task_node->control_code) {
					_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, task_node->szFileName2);
					if(LOCATION_CLIENT & TNODE_LOCATION_BYTE(task_node->control_code)) {
						NFileUtility::SetFileSizeLawi(szFileName, task_node->file_size, &task_node->last_write);
// TCHAR timestr[64];
// _LOG_DEBUG( _T("set name:%s lawi:%s"), szFileName, timcon::ftim_unis(timestr, &task_node->last_write) );
					}
					LINKER_DISPLAY_SUCCESS(szFileName, task_node->isdire)
_LOG_DEBUG(_T("second success :%s :%08x"), szFileName, task_node->success_flags);
				}
// ndp::TaskNode(task_node); // disable by james 20130408
				if(LOCATION_CLIENT & TNODE_LOCATION_BYTE(task_node->control_code))
					LINKER_EXCEPT_SUCCESS(task_node)
// _LOG_DEBUG(_T("d refresh icon file name:%s success_flags:%08x"), szFileName, task_node->success_flags); // disable by james 20140410
				dwIndeTatol = 0x00;
			}
			NLocalBzl::PerformTransmitProcess(NULL, 0x01);
		}
		gcsUpdateAttrib.Unlock();
	} else if((STYPE_PUT&task_node->control_code) && (!task_node->build_length)) { // put file and file size is 0
		DRIVE_LETTE(szFileName, szDriveLetter)
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, task_node->szFileName1);
		LINKER_DISPLAY_SUCCESS(szFileName, task_node->isdire)
		NLocalBzl::PerformTransmitProcess(NULL, 0x01);
_LOG_DEBUG(_T("sta_inde:%d"), szFileName, task_node->node_inde);
_LOG_DEBUG(_T("put success :%s :%08x"), szFileName, task_node->success_flags);
// _LOG_DEBUG(_T("u refresh icon file name:%s success_flags:%08x"), szFileName, task_node->success_flags); // disable by james 20140410
	} else NLocalBzl::PerformTransmitProcess(NULL, 0x01);
_LOG_DEBUG(_T("end except refresh icon!")); // disable by james 20140410
}

UINT WorkerThreadProcess(LPVOID lpParam) {
    struct WORKER_ARGUMENT *pWorkerArgu = (struct WORKER_ARGUMENT *)lpParam;
    if (NULL == pWorkerArgu) FAULT_RETURN; // 输入参数非法
    //
    struct ClientConfig *pClientConfig = pWorkerArgu->pClientConfig;
    volatile LONG *array_locked = pWorkerArgu->array_locked;
    volatile LONG *worker_locked;
    DWORD thread_index;
    // 分配一个array locked空间给此线程,解锁线程标志
    for(thread_index = 0x00; thread_index < FIVE_TRANS_THREAD; ++thread_index) {
        if(InterlockedCompareExchange(&array_locked[thread_index], 0x0000, INVALID_ARRAY_VALUE)) {
// _LOG_DEBUG(_T("thread_index:%d array_locked:%d"), thread_index, array_locked[thread_index]);
            worker_locked = array_locked + thread_index;
            break;
        }
    }
    // 等待线程全部启动
    NTLUtility::WaitingThread(array_locked);
// _LOG_DEBUG(_T("in worker thread process! thread_index:%d"), thread_index); // disable by james 20140410
    DWORD handle_value = WORKER_VALUE_FINISH;
    CTRANSWorker *pTransWorker = NTLUtility::CreateWorker(&handle_value, pWorkerArgu);
    struct TaskNode task_node;
    for(; ; ) {
        // 检查运行状态
_LOG_DEBUG(_T("check transmit run status!")); // disable by james 20140410
        DWORD dwProcessStatus = NExecTransBzl::GetTransmitNextStatus();
        if(PROCESS_STATUS_RUNNING != dwProcessStatus) {
            CONVERT_WORKER_STATUS(handle_value, dwProcessStatus)
 _LOG_DEBUG(_T("process status is not running, so exit! handle_value:%X"), handle_value); // disable by james 20140410
            break;
        }
        // 读取工作节点
_LOG_DEBUG(_T("read task node! index:%d"), thread_index); // disable by james 20140410
        int prepa_value = NTLUtility::NodePrepare(&task_node);
        if(!prepa_value) {
            InterlockedExchange(worker_locked, INVALID_ARRAY_VALUE);
            handle_value = WORKER_VALUE_FINISH;
_LOG_DEBUG(_T("read node finish!")); // disable by james 20140410
            break;
        } else if(0 > prepa_value) {
            InterlockedExchange(worker_locked, INVALID_ARRAY_VALUE);
            handle_value = WORKER_VALUE_FAULT;
_LOG_DEBUG(_T("read node error!")); // disable by james 20140410
            break;
        }
        // 检查同组节点状态
_LOG_DEBUG(_T("valid array locked"));
        NTLUtility::ValidArrayLocked(array_locked, worker_locked, &task_node);
_LOG_DEBUG(_T("valid array unlock %08x"), *worker_locked);
        //
        // 创建服务器连接// _LOG_DEBUG(_T("read node error!")); // disable by james 20140410
        if(!pTransWorker) {
            if(!(pTransWorker = NTLUtility::CreateWorker(&handle_value, pWorkerArgu))) {
_LOG_DEBUG(_T("create worker error!"));
                break;
            }
        }
        // 执行传输工作
// _LOG_DEBUG(_T("transmit task node!")); // disable by james 20140410
_LOG_DEBUG(_T("node transmit %08x"), worker_locked);
        handle_value = NTLUtility::NodeTransmit(pTransWorker, &task_node, thread_index);
        if(WORKER_VALUE_NETWORK & handle_value) {
_LOG_DEBUG(_T("worker network error!"));
            NTLUtility::DropWorker(pTransWorker);
            pTransWorker = NULL;
			continue;
        }
		if(WORKER_VALUE_FINISH != handle_value) {
_LOG_DEBUG(_T("node transmit keep alive!"));
			if(objKeepAlive.Continue(thread_index)) pTransWorker->KeepAlive(); // keep alive
			continue;
		}
        // 效验下载数据块有效性
// _LOG_DEBUG(_T("validate download transmit!")); // disable by james 20140410
_LOG_DEBUG(_T("valid transmit downd %08x"), worker_locked);
		if(WORKER_VALUE_FINISH != NVerifyUtility::ValidTransmitDownd(&task_node)) {
_LOG_DEBUG(_T("valid downd continue!"));
			continue;
		}
        // 执行覆盖写数据工作 插入正确的效验植 // 20131119
// _LOG_DEBUG(_T("overwrite transmit data, refresh icon!")); // disable by james 20140410
// _LOG_DEBUG(_T("------------ array_id:%u node_inde:%u"), task_node.array_id, task_node.node_inde);
_LOG_DEBUG(_T("finish transmit %08x"), worker_locked);
        NTLUtility::FinishTransmit(&task_node, pClientConfig->szDriveLetter);
        // 刷新非上传数据图标缓存
// ndp::TaskNode(&task_node); // add by james 20130520 for test
        NTLUtility::HandleUpdateExceptAttrib(&task_node, pClientConfig->szDriveLetter);
    }
    // 销毁服务器连接
    NTLUtility::DestroyWorker(pTransWorker);
_LOG_DEBUG(_T("finish worker and exit thread! handle_value:%X thread_index:%d"), handle_value, thread_index);  // disable by james 20140410
    return handle_value;
}

DWORD NHandTasksBzl::BeginWorkerThread(struct LocalConfig *pLocalConfig) {
    HANDLE hWorkerThreads[FIVE_TRANS_THREAD];
    DWORD dwThreadExitValue[FIVE_TRANS_THREAD];
    DWORD dwWorkerExitValue = WORKER_VALUE_FINISH;
    struct WORKER_ARGUMENT tWorkerArgu;
    DWORD thread_index;
    // initial the lock flag array
    for(thread_index = 0x00; thread_index < CTRL_ARRAY_INIT; ++thread_index)
        tWorkerArgu.array_locked[thread_index] = INVALID_ARRAY_VALUE;
    CONVERT_SESSION_ARGU(&(tWorkerArgu.tSeionArgu), &pLocalConfig->tClientConfig);
    CONVERT_SOCKETS_ARGU(&(tWorkerArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
    SET_SOCKETS_THREADS(&(tWorkerArgu.tSockeArgu), FIVE_SOCKETS_THREAD); // transmit
    //
    tWorkerArgu.pClientConfig = &pLocalConfig->tClientConfig;
    TRANS_OBJECT(SetWorkerThreads(hWorkerThreads, FIVE_TRANS_THREAD));
    // 启动线程
// _LOG_DEBUG(_T("启动线程")); // disable by james 20140410
    objTaskList.SetHandNumbe(0);
    for(thread_index = 0x00; thread_index < FIVE_TRANS_THREAD; ++thread_index) {
        CWinThread *pWinThread = AfxBeginThread(WorkerThreadProcess, (LPVOID)&tWorkerArgu, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        pWinThread->m_bAutoDelete = FALSE;
        hWorkerThreads[thread_index] = pWinThread->m_hThread;
        pWinThread->ResumeThread();
    }
    // 等待结束
// _LOG_DEBUG(_T("等待结束")); // disable by james 20140410
    WaitForMultipleObjects(FIVE_TRANS_THREAD, hWorkerThreads, TRUE, INFINITE);
    // 得到退出码
_LOG_DEBUG(_T("得到工作线程退出码")); // disable by james 20140410
    for(thread_index = 0x00; thread_index < FIVE_TRANS_THREAD; ++thread_index) {
        if(!GetExitCodeThread(hWorkerThreads[thread_index], &dwThreadExitValue[thread_index])) {
            _LOG_WARN(_T("Get ThreadExitValue[%d] error! GetLastError:%08x"), thread_index, GetLastError());
        }
        CloseHandle( hWorkerThreads[thread_index] );
    }
    // 处理退出码
// _LOG_DEBUG(_T("处理工作线程退出码")); // disable by james 20140410
    for(thread_index = 0x00; thread_index < FIVE_TRANS_THREAD; ++thread_index) {
// _LOG_DEBUG(_T("------------------ thread:%d exit code:%X"), thread_index, dwThreadExitValue[thread_index]); // disable by james 20140410
        if(WORKER_VALUE_FINISH != dwThreadExitValue[thread_index]) {
            dwWorkerExitValue = dwThreadExitValue[thread_index];
        }
    }
    // delete malloc resouce
    TRANS_OBJECT(SetWorkerThreads(NULL, 0));
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ dwWorkerExitValue:%X"), dwWorkerExitValue); // disable by james 20140410
    return dwWorkerExitValue;
}

CTRANSWorker *NTLUtility::CreateWorker(DWORD *cvalue, struct WORKER_ARGUMENT *pWrokerArgu) {
    CTRANSWorker *pTransWorker = NTRANSWorker::Factory(&pWrokerArgu->tSeionArgu, &pWrokerArgu->tSockeArgu);
    if (!pTransWorker) {
        NTRANSWorker::DestroyObject(pTransWorker);
        *cvalue = WORKER_VALUE_NETWORK;
        return NULL;
    }
    // 生成工作记录,初始会话
_LOG_DEBUG(_T("worker create ok! initial it."));
    int proto_value = PROTO_FAULT;
    if(proto_value = pTransWorker->Initial()) {
        NTRANSWorker::DestroyObject(pTransWorker);
        *cvalue = (PROTO_NETWORK == proto_value)? WORKER_VALUE_NETWORK: WORKER_VALUE_FAILED;
        pTransWorker = NULL;
    }
    return pTransWorker;
}

void NTLUtility::DropWorker(CTRANSWorker *pTransWorker) {
	if(pTransWorker) {
        NTRANSWorker::DestroyObject(pTransWorker);
_LOG_DEBUG(_T("drop worker!"));
	}
}

void NTLUtility::DestroyWorker(CTRANSWorker *pTransWorker) {
    if(pTransWorker) {
        pTransWorker->Final(FINAL_STATUS_END);
        NTRANSWorker::DestroyObject(pTransWorker);
_LOG_DEBUG(_T("destroy worker!"));
    }
}