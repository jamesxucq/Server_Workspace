#include "StdAfx.h"

#include "DebugPrinter.h"
#include "AnchorBzl.h"
#include "ExecuteTransmitBzl.h"
#include "ListUtility.h"
#include "ProduceObj.h"

#include "BuildTasksBzl.h"


DWORD NBuildTasksBzl::Initialize(TCHAR *szLocation) {
    objTaskList.Initialize(TLIST_DATA_DEFAULT, szLocation);
    return 0x00;
}

DWORD NBuildTasksBzl::Finalize() {
    objTaskList.Finalize();
    return 0x00;
}

DWORD NBuildTasksBzl::BuildListEnviro(DWORD *plist_type, DWORD *pslast_anchor, LIST_ARGUMENT *pListArgu, DWORD llast_anchor) {
    unsigned __int64 cleng;
    TCHAR list_chks[MD5_TEXT_LENGTH];
    DWORD build_value = LIST_VALUE_FAILED;
    //
    CTRANSWorker *pListWorker = ListUtility::CreateWorker(&build_value, pListArgu);
    if(pListWorker) {
        if(ListDownd::GetSlastAnchor(pslast_anchor, pListWorker)) {
_LOG_DEBUG(_T("--- get slast anchor error."));
            ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
            build_value = LIST_VALUE_FAILED;
        } else {
            int list_value = ListDownd::GetListHead(&cleng, list_chks, pListWorker, llast_anchor, *pslast_anchor);
_LOG_DEBUG(_T("b list_value:%08x"), list_value);
            if(!list_value) {
_LOG_DEBUG(_T("fast sync."));
                ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
                objListHead.CreatListVec(list_chks, cleng, LIST_ANCH_FILES, llast_anchor, NULL);
                *plist_type = GLIST_ANCH_FILES;
                build_value = LIST_VALUE_FINISH;
            } else if(0 < list_value) {
                ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
_LOG_DEBUG(_T("force slowly sync."));
				// clear local anchor file;
                NAnchorBzl::ClearAnchorEnviro(TRUE); // 写Anchor标志,慢同步
                objListHead.CreatListVec(list_chks, cleng, LIST_RECU_FILES, NULL, _T("/"));
                *plist_type = GLIST_RECU_FILES;
                build_value = LIST_VALUE_FINISH;
            } else if(-1 == list_value) {
_LOG_DEBUG(_T("--- list failed error."));
                ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
				build_value = LIST_VALUE_FAILED;
            } else if(-2 == list_value) {
_LOG_DEBUG(_T("--- list network error."));
                ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
				build_value = LIST_VALUE_NETWORK;
			}
        }
        //
    }
_LOG_DEBUG(_T("b build_value:%08x"), build_value);
    return build_value;
}

void NBuildTasksBzl::EarseListEnviro(LIST_ARGUMENT *pListArgu) {
	DWORD erase_value = LIST_VALUE_FAILED;
    CTRANSWorker *pListWorker = ListUtility::CreateWorker(&erase_value, pListArgu);
    if(pListWorker) {
_LOG_DEBUG(_T("erase list enviro."));
		ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_FINISH);
	}
}

#define RETRY_LIST_TIMES	0x0003
int NBuildTasksBzl::BulidSlowListFile(HANDLE hListFile, LIST_ARGUMENT *pListArgu, TCHAR *szLocation) {
    int build_value = -1;
    objListVec.Initialize(PRODU_LIST_DEFAULT, szLocation);
    //
    pListArgu->pListHead = &objListHead;
    pListArgu->pListVec = &objListVec;
    DWORD dwExceptionRetry = 0x00;
    do {
        if(LIST_VALUE_FINISH == ListDownd::BuildListXml(pListArgu)) 
			build_value = 0x00;
    } while(RETRY_LIST_TIMES>dwExceptionRetry++ && ListWorkBzl::CheckListFile(&objListHead, &objListVec));
    //
    objListVec.ParseListXml((VOID *)hListFile, GLIST_RECU_FILES);
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ GLIST_RECU_FILES:%08x"), GLIST_RECU_FILES);
    objListHead.DeleListVec();
    objListVec.Finalize(); // disable for test
    //
    return build_value;
}

int NBuildTasksBzl::BulidFastListVector(FilesVec *pListVec, LIST_ARGUMENT *pListArgu, TCHAR *szLocation) {
    int build_value = -1;
    objListVec.Initialize(PRODU_LIST_DEFAULT, szLocation);
    //
    pListArgu->pListHead = &objListHead;
    pListArgu->pListVec = &objListVec;
    DWORD dwExceptionRetry = 0x00;
    do {
        if(LIST_VALUE_FINISH == ListDownd::BuildListXml(pListArgu)) 
			build_value = 0;
    } while(RETRY_LIST_TIMES>dwExceptionRetry++ && ListWorkBzl::CheckListFile(&objListHead, &objListVec));
    //
    objListVec.ParseListXml((VOID *)pListVec, GLIST_ANCH_FILES);
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ GLIST_ANCH_FILES:%08x"), GLIST_ANCH_FILES);
// ndp::FilesVector(pListVec);
    objListHead.DeleListVec();
    objListVec.Finalize(); // disable for test
    //
    return build_value;
}

//
int NBuildTasksBzl::BulidColiListVector(FileColiVec *pColiVec, LIST_ARGUMENT *pListArgu) {
    unsigned __int64 cleng;
    FILETIME last_write;
    DWORD build_value = LIST_VALUE_FINISH;
	struct FileColi *pFileColi;
	FileColiVec::iterator iter;
    //
    CTRANSWorker *pListWorker = ListUtility::CreateWorker(&build_value, pListArgu);
    if(pListWorker) {
		for(iter=pColiVec->begin(); pColiVec->end()!=iter; ++iter) {
			pFileColi = (*iter);
            int list_value = ListDownd::GetFileHead(&cleng, &last_write, pListWorker, pFileColi->szFileName);
_LOG_DEBUG(_T("b list_value:%08x"), list_value);
            if(0 <= list_value) {
				pFileColi->qwFileSize = cleng;
				memcpy(&pFileColi->ftLastWrite, &last_write, sizeof(FILETIME));
				SECF_ATTR_SUCCE(pFileColi->dwExcepValue);
			} else if(-1 == list_value) {
_LOG_DEBUG(_T("--- list failed error."));
				SECF_ATTR_EXCEP(pFileColi->dwExcepValue);
			} else if(-2 == list_value) {
_LOG_DEBUG(_T("--- list network error."));
				NTLUtility::DropWorker(pListWorker);
				if(pListWorker = ListUtility::CreateWorker(&build_value, pListArgu)) --iter;
				else {
					build_value = LIST_VALUE_FAILED;
					break;				
				} 
			}		
		}
        //
		ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
    }
_LOG_DEBUG(_T("b build_value:%08x"), build_value);
    return build_value;
}

int NBuildTasksBzl::BulidExcepCache(struct TasksCache *pExcepCache) { // 0:finish -1:error
	TNodeVec etnodv;
    int build_value = -1;
	//
	if(0x00 <= ListUtility::FindExceptTNode(&etnodv))
		build_value = 0;
	ListDownd::TasksExceptBuilder(pExcepCache, &etnodv);
	//
	struct TasksCache *pTasksCache = objTasksCache.GetTasksCache();
	NTaskCache::CompleTasksCache(pExcepCache, pTasksCache);
    //
	NTNodeVec::DeleTNodeV(&etnodv);
_LOG_DEBUG(_T("build_value:%d"), build_value);
    return build_value;
}

//
DWORD NBuildTasksBzl::CreateSlowTasksList(HANDLE hListFile, LIST_ARGUMENT *pListArgu) {
    TCHAR szSpaceLocation[MAX_PATH];
    DWORD build_value = LIST_VALUE_FAILED;
    //
    struct TasksCache *pTasksCache = objTasksCache.GetTasksCache();
    NTaskCache::DeleTasksCache(pTasksCache);
_LOG_DEBUG(_T("--------------------- 1 clear tasks vecotr cache ---------------------")); // disable by james 20130410
    //
    DWORD list_value = LIST_VALUE_FINISH;
    CTRANSWorker *pListWorker = ListUtility::CreateWorker(&list_value, pListArgu);
    if(pListWorker) {
        _tcscpy_s(szSpaceLocation, MAX_PATH, pListArgu->tSeionArgu.szDriveLetter);
        build_value = ListDownd::TasksSlowBuilder(pTasksCache, hListFile, szSpaceLocation, pListWorker);
    }
    ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
    if(build_value || (LIST_VALUE_FINISH != list_value))
		return LIST_VALUE_FAILED;
_LOG_DEBUG(_T("--------------------- 2 build tasks vector ---------------------")); // disable by james 20130410
ndp::TasksCache(pTasksCache);
    if(ProduceUtility::CreateTasksList(pTasksCache)) 
		return LIST_VALUE_FAILED;
_LOG_DEBUG(_T("--------------------- 3 create tasks list ---------------------")); // disable by james 20130410
ndp::TasksCache(pTasksCache);
_LOG_DEBUG(_T("build_value:%08x"), build_value);
    return LIST_VALUE_FINISH;
}

DWORD NBuildTasksBzl::CreateFastTasksList(FilesVec *pListVec, DWORD llast_anchor, LIST_ARGUMENT *pListArgu) {
	struct TasksCache *pTasksCache = objTasksCache.GetTasksCache();
    NTaskCache::DeleTasksCache(pTasksCache);
_LOG_DEBUG(_T("--------------------- 1 clear tasks vecotr cache ---------------------")); // disable by james 20130410
    //
// _LOG_DEBUG(_T("4 GetTickCount:%u"), GetTickCount()/1000);
    TCHAR szSpaceLocation[MAX_PATH];
	_tcscpy_s(szSpaceLocation, pListArgu->tSeionArgu.szDriveLetter);
    if(ListDownd::TasksFastBuilder(pTasksCache, pListVec, llast_anchor, szSpaceLocation)) 
		return LIST_VALUE_FAILED;
_LOG_DEBUG(_T("--------------------- 2 build tasks vector ---------------------")); // disable by james 20130410
ndp::TasksCache(pTasksCache);
// _LOG_DEBUG(_T("5 GetTickCount:%u"), GetTickCount()/1000);
    if(ProduceUtility::CreateTasksList(pTasksCache)) 
		return LIST_VALUE_FAILED;
_LOG_DEBUG(_T("--------------------- 3 create tasks list ---------------------")); // disable by james 20130410
ndp::TasksCache(pTasksCache);
// _LOG_DEBUG(_T("6 GetTickCount:%u"), GetTickCount()/1000);
// _LOG_DEBUG(_T("build_value:%08x"), build_value);
    return LIST_VALUE_FINISH;
}

DWORD NBuildTasksBzl::CreateExceptTasksList(struct TasksCache *pExcepCache) {
	struct TasksCache *pTasksCache = objTasksCache.GetTasksCache();
    NTaskCache::DeleTasksCache(pTasksCache);
_LOG_DEBUG(_T("--------------------- 1 clear tasks vecotr cache ---------------------")); // disable by james 20130410
	NTaskCache::MoveCache(pTasksCache, pExcepCache);
_LOG_DEBUG(_T("--------------------- 2 build tasks vector ---------------------")); // disable by james 20130410
// ndp::TasksCache(pTasksCache);
// _LOG_DEBUG(_T("5 GetTickCount:%u"), GetTickCount()/1000);
    if(ProduceUtility::CreateTasksList(pTasksCache)) 
		return LIST_VALUE_FAILED;
_LOG_DEBUG(_T("--------------------- 3 create tasks list ---------------------")); // disable by james 20130410
ndp::TasksCache(pTasksCache);
// _LOG_DEBUG(_T("6 GetTickCount:%u"), GetTickCount()/1000);
// _LOG_DEBUG(_T("build_value:%08x"), build_value);
    return LIST_VALUE_FINISH;
}

//
DWORD NBuildTasksBzl::InitTListEnviro() {
    struct TasksCache *pTasksCache = objTasksCache.GetTasksCache();
    if(pTasksCache)
        NTaskCache::DeleTasksCache(pTasksCache);
	//
    objTaskList.SetHandNumbe(0);
    if(objTaskList.OpenTasksFile()) {
        _LOG_WARN(_T("can not open the syncing task list file !"));
        return ((DWORD)-1);
    }
	//
	return 0x00;
}

void NBuildTasksBzl::TListEnviroFinal() {
    struct TasksCache *pTasksCache = objTasksCache.GetTasksCache();
    if(pTasksCache)
        NTaskCache::DeleTasksCache(pTasksCache);
	//
    objTaskList.CloseTasksFile();
	objTaskList.SetHandNumbe(0);
}

//

int ListDownd::RecursiveListHead(unsigned __int64 *pleng, TCHAR *list_chks, CTRANSWorker *pListWorker) {
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ build syncing tasks +++++++++++++++++++++++++")); // disable by james 20130410
    int list_value = PROTO_FAULT;
    do {
        list_value = pListWorker->HeadList(pleng, list_chks, HLIST_RECU_DIRES, NULL, _T("/"));
    } while(PROTO_PROCESSING == list_value);
_LOG_DEBUG(_T("----------------------- final tasks vector: -----------------------")); // disable by james 20130410
_LOG_DEBUG(_T("list_value:%08x"), list_value);
    return list_value? 1: 0;
}

int NBuildTasksBzl::RecursiveListEnviro(LIST_ARGUMENT *pListArgu) {
    unsigned __int64 cleng;
    TCHAR list_chks[MD5_TEXT_LENGTH];
    DWORD build_value = LIST_VALUE_FAILED;
    //
    CTRANSWorker *pListWorker = ListUtility::CreateWorker(&build_value, pListArgu);
    if(pListWorker) {
        int list_value = ListDownd::RecursiveListHead(&cleng, list_chks, pListWorker);
        if(list_value) {
            ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_FINISH);
            build_value = LIST_VALUE_FAILED;
        } else {
            ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
            objListHead.CreatListVec(list_chks, cleng, LIST_RECU_DIRES, NULL, _T("/")); // todo: fix the list_value
            build_value = LIST_VALUE_FINISH;
        }
    }
    //
_LOG_DEBUG(_T("build_value:%08x"), build_value);
    return build_value;
}

int ListDownd::GetSlastAnchor(DWORD *pslast_anchor, CTRANSWorker *pListWorker) {
    DWORD slast_anchor;
    int igval = 0;
//
    if(!pListWorker->GetAnchor(&slast_anchor)) { // 会话初始成功,保存服务器最后一个锚点
        *pslast_anchor = slast_anchor;
    } else igval = -1;
//
    return igval;
}

//
#define CONVERT_HEAD_VALUE(LIST_VALU, HEAD_VALU) { \
	switch(HEAD_VALU) { \
	case PROTO_FAULT: \
	case PROTO_NOT_FOUND: \
		LIST_VALU = -1; \
		break; \
	case PROTO_OKAY: \
		LIST_VALU = 0x01; \
		break; \
	case PROTO_NETWORK: \
		LIST_VALU = -2; \
		break; \
	} \
}

int ListDownd::GetListHead(unsigned __int64 *pleng, TCHAR *list_chks, CTRANSWorker *pListWorker, DWORD llast_anchor, DWORD slast_anchor) { // -1 error 0 fast sync > 0 slow sync
	int list_value = -1;
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ build syncing tasks +++++++++++++++++++++++++")); // disable by james 20130410
_LOG_DEBUG(_T("----------------------- local last anchor:%d"), llast_anchor); // disable by james 20130410
	int head_value = PROTO_FAULT;
	if(!llast_anchor || slast_anchor < llast_anchor) { // local anchor is 0
_LOG_DEBUG(_T("init or excep env, begain slow sync.")); // disable by james 20130410
		do {
			head_value = pListWorker->HeadList(pleng, list_chks, HLIST_RECU_FILES, NULL, _T("/"));
		} while(PROTO_PROCESSING == head_value);
		// list_value = 1;
		CONVERT_HEAD_VALUE(list_value, head_value)
	} else { // local is normal
		do {
			head_value = pListWorker->HeadList(pleng, list_chks, HLIST_ANCH_FILES, llast_anchor, NULL);
		} while(PROTO_PROCESSING == head_value);
		CONVERT_HEAD_VALUE(list_value, head_value)
		//
		if(PROTO_OKAY==head_value || PROTO_NOT_FOUND==head_value) {
			if(llast_anchor && !head_value) { // base anchor, fast sync
				list_value = 0;
_LOG_DEBUG(_T("fast sync base on the anchor.")); // disable by james 20130410
			} else { // anchor not found, slowly sync
_LOG_DEBUG(_T("do not find the anchor, begain slow sync.")); // disable by james 20130410
				do {
					head_value = pListWorker->HeadList(pleng, list_chks, HLIST_RECU_FILES, NULL, _T("/"));
				} while(PROTO_PROCESSING == head_value);
				// list_value = 1;
				CONVERT_HEAD_VALUE(list_value, head_value)
			}
		}
	}
_LOG_DEBUG(_T("----------------------- final tasks vector: -----------------------"));
_LOG_DEBUG(_T("list_value:%08x"), list_value);
	return list_value;
}

int ListDownd::GetFileHead(unsigned __int64 *pleng, FILETIME *last_write, CTRANSWorker *pListWorker, TCHAR *szFileName) { // -1 error 0 fast sync > 0 slow sync
	int list_value = -1;
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ build syncing tasks +++++++++++++++++++++++++")); // disable by james 20130410
	int head_value = PROTO_FAULT;
	do {
		head_value = pListWorker->HeadFile(pleng, last_write, szFileName);
	} while(PROTO_PROCESSING == head_value);
	// list_value = 1;
	CONVERT_HEAD_VALUE(list_value, head_value)
_LOG_DEBUG(_T("----------------------- final tasks vector: -----------------------"));
_LOG_DEBUG(_T("list_value:%08x"), list_value);
	return list_value;
}

int ListDownd::ListPrepa(struct ListNode **pListNode, class CListHObj *pListHead) {
    return pListHead->GetNodeAddition(pListNode);
}

int ListDownd::RecursiveGetList(struct ListNode *list_node, CTRANSWorker *pListWorker) {
    if(PROTO_NETWORK == pListWorker->GetList(list_node))
        return LIST_VALUE_NETWORK;
    return LIST_VALUE_FINISH;
}

#define COPY_BUFFER	0x2000  // 8K
int ListDownd::FinishWrite(struct ListNode *list_node, class CListVObj *pListVec) {
    HANDLE	hBuilderCache = INVALID_HANDLE_VALUE;
    int finish_value = 0;
    // open source and destination file
    hBuilderCache = NTRANSCache::OpenBuilder(list_node->builder_cache);
    if( INVALID_HANDLE_VALUE == hBuilderCache ) {
        _LOG_WARN( _T("overwrite open file failed. file name is %s."), list_node->builder_cache);
        return HANDLE_FAULT;
    }
    // overwrite destination file
    DWORD dwWritenSize = 0x00;
    DWORD dwReadSize = 0x00;
    CHAR szBuffer[COPY_BUFFER];
    unsigned __int64 offset = list_node->offset;

    while(ReadFile(hBuilderCache, szBuffer, COPY_BUFFER, &dwReadSize, NULL) && 0<dwReadSize) {
        dwWritenSize = pListVec->WriteListFile(szBuffer, dwReadSize, offset);
        if(dwReadSize != dwWritenSize) {
            _LOG_WARN(_T("fatal write error: %d"), GetLastError());
            finish_value = -1;
            break;
        }
        offset += dwWritenSize;
    }
    // close source and destination file
    NTRANSCache::DestroyBuilder(hBuilderCache, list_node->builder_cache);
//
    return finish_value;
}

UINT ListThreadProcess(LPVOID lpParam) {
    struct LIST_ARGUMENT *pListArgu = (struct LIST_ARGUMENT *)lpParam;
    if (NULL == pListArgu) FAULT_RETURN; //输入参数非法
// _LOG_DEBUG(_T("initial verify worker."));
    DWORD list_value = LIST_VALUE_FINISH;
    CTRANSWorker *pListWorker = ListUtility::CreateWorker(&list_value, pListArgu);
    struct ListNode *pListNode;
    for(; ; ) {
        // 检查运行状态
// _LOG_DEBUG(_T("check transmit run status!")); // disable by james 20140410
        DWORD dwProcessStatus = NExecTransBzl::GetTransmitNextStatus();
        if(PROCESS_STATUS_RUNNING != dwProcessStatus) {
            CONVERT_LIST_STATUS(list_value, dwProcessStatus);
_LOG_DEBUG(_T("process status:%X"), dwProcessStatus); // disable by james 20140410
_LOG_DEBUG(_T("process status is not running, so exit! list_value:%X"), list_value); // disable by james 20140410
            break;
        }
        // 读取工作节点
// _LOG_DEBUG(_T("read task node! inde:%d"), thread_index); // disable by james 20140410
        int prepa_value = ListDownd::ListPrepa(&pListNode, pListArgu->pListHead);
        if(!prepa_value) {
            list_value = LIST_VALUE_FINISH;
// _LOG_DEBUG(_T("read node finish!")); // disable by james 20140410
            break;
        } else if(0 > prepa_value) {
            list_value = LIST_VALUE_FAILED;
// _LOG_DEBUG(_T("read node error!")); // disable by james 20140410
            break;
        }
// _LOG_DEBUG(_T("read node start:%d end:%d"), sta_inde, end_inde); // disable by james 20140410
        // 创建服务器连接
        if(!pListWorker) {
// _LOG_DEBUG(_T("build verify worker."));
            if(!(pListWorker = ListUtility::CreateWorker(&list_value, pListArgu))) {
// _LOG_DEBUG(_T("build verify worker error."));
                break;
            }
        }
        // 执行传输工作// _LOG_DEBUG(_T("transmit task node!")); // disable by james 20140410
        list_value = ListDownd::RecursiveGetList(pListNode, pListWorker);
        if(LIST_VALUE_NETWORK & list_value) {
// _LOG_DEBUG(_T("drop verify worker."));
            ListUtility::DropWorker(pListWorker);
            pListWorker = NULL;
			continue;
        }
        // 执行覆盖写数据工作
// _LOG_DEBUG(_T("overwrite transmit data, refresh icon!")); // disable by james 20140410
		ListDownd::FinishWrite(pListNode, pListArgu->pListVec);
    }
    // 销毁服务器连接
// _LOG_DEBUG(_T("final verify worker."));
    ListUtility::DestroyWorker(pListWorker, FINAL_STATUS_END);
// _LOG_DEBUG(_T("finish worker and exit thread! list_value:%X thread_index:%d"), list_value, thread_index);  // disable by james 20140410
    return list_value;
}

DWORD ListDownd::BuildListXml(LIST_ARGUMENT *pListArgu) {
    HANDLE hListThreads[FIVE_LIST_THREAD];
    DWORD dwThreadExitValue[FIVE_LIST_THREAD];
    DWORD dwProduceExitValue = LIST_VALUE_FINISH;
    DWORD thread_index;
    //
    PRODUCE_OBJECT(SetListThreads(hListThreads, FIVE_LIST_THREAD));
    // 启动线程
// _LOG_DEBUG(_T("启动线程")); // disable by james 20140410
	pListArgu->pListHead->SetHandNumbe(0);
    for(thread_index = 0x00; thread_index < FIVE_LIST_THREAD; ++thread_index) {
        CWinThread *pWinThread = AfxBeginThread(ListThreadProcess, (LPVOID)pListArgu, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        pWinThread->m_bAutoDelete = FALSE;
        hListThreads[thread_index] = pWinThread->m_hThread;
        pWinThread->ResumeThread();
    }
    // 等待结束
// _LOG_DEBUG(_T("等待结束")); // disable by james 20140410
    WaitForMultipleObjects(FIVE_LIST_THREAD, hListThreads, TRUE, INFINITE);
    // 得到退出码
// _LOG_DEBUG(_T("得到工作线程退出码")); // disable by james 20140410
    for(thread_index = 0x00; thread_index < FIVE_LIST_THREAD; ++thread_index) {
        if(!GetExitCodeThread(hListThreads[thread_index], &dwThreadExitValue[thread_index])) {
            _LOG_WARN(_T("Get ThreadExitValue[%d] error! GetLastError:%08x"), thread_index, GetLastError());
        }
        CloseHandle( hListThreads[thread_index] );
    }
    // 处理退出码
// _LOG_DEBUG(_T("处理工作线程退出码")); // disable by james 20140410
    for(thread_index = 0x00; thread_index < FIVE_LIST_THREAD; ++thread_index) {
// _LOG_DEBUG(_T("------------------ thread:%d exit code:%X"), thread_index, ThreadExitValue[thread_index]); // disable by james 20140410
        if(LIST_VALUE_FINISH != dwThreadExitValue[thread_index]) {
            dwProduceExitValue = dwThreadExitValue[thread_index];
        }
    }
    // delete malloc resouce
    PRODUCE_OBJECT(SetListThreads(NULL, 0));
// _LOG_DEBUG(_T("+++++++++++++++++++++++++ dwProduceExitValue:%X"), dwProduceExitValue); // disable by james 20140410
    return dwProduceExitValue;
}

int ListDownd::TasksSlowBuilder(struct TasksCache *pTasksCache, HANDLE hsatt, TCHAR *szSpaceLocation, CTRANSWorker *pListWorker) {
    char fileName[MAX_PATH];
    NFileUtility::build_cache_name(fileName, "slow_latt.kct");
    // create the database object
    TreeDB lattdb;
    // Open our database;
    if (!lattdb.open(fileName, TreeDB::OWRITER | TreeDB::OCREATE)) {
        // cerr << "open error: " << lattdb.error().name() << endl;
        _LOG_WARN(_T("lattdb open error."));
    }
    // local anchor is 0 or anchor没有找到，慢同步  // 0:ok 0x01:error 0x02:processing
    DWORD list_value = 0x00;
    do {
        list_value = NExecTransBzl::GetSlowFilesDbBzl(&lattdb, szSpaceLocation);
// _LOG_DEBUG(_T("list_value:%d"), list_value);
        pListWorker->KeepAlive(); // keep alive
    } while(0x02 == list_value);
    if(!list_value) ListUtility::OptimizeSlowVec(pTasksCache, hsatt, &lattdb);
    // Auto-commit the transaction and close our handle
    if (!lattdb.close()) {
        // cerr << "close error: " << lattdb.error().name() << endl;
        _LOG_WARN(_T("lattdb close error."));
    }
    DeleteFileA(fileName);
//
    return list_value;
}

int ListDownd::TasksFastBuilder(struct TasksCache *pTasksCache, FilesVec *sattent, DWORD llast_anchor, TCHAR *szSpaceLocation) {
    FilesVec lattent;
    // 基于anchor的快同步  // 0:ok 0x01:error 0x02:processing
	// 计算优化后,不用keep alive
_LOG_DEBUG(_T("------------ server files vector: ------------")); // disable by james 20130410
ndp::FilesVector(sattent);
	DWORD list_value = NExecTransBzl::GetFastFilesVecBzl(&lattent, llast_anchor, szSpaceLocation);
_LOG_DEBUG(_T("------------ local files vector: ------------")); // disable by james 20130410
ndp::FilesVector(&lattent);
	if(!list_value) {
		FileColiVec *pColiVec = NFileColi::GetFileColiPoint();
		ListUtility::OptimizeColiVec(sattent, &lattent, pColiVec);
		ListUtility::OptimizeFastVec(pTasksCache, sattent, &lattent);
	} 
_LOG_DEBUG(_T("------------")); // disable by james 20130410
ndp::FilesVector(sattent);
_LOG_DEBUG(_T("------------"));
ndp::FilesVector(&lattent);
_LOG_DEBUG(_T("------------"));
    NFilesVec::DeleFilesVec(&lattent);
//
_LOG_DEBUG(_T("list_value:%d"), list_value);
    return list_value;
}

//
void ListDownd::TasksExceptBuilder(struct TasksCache *pTasksCache, TNodeVec *pTNodeV) { // 0:ok 0x01:error
	TNodeVec::iterator iter;
	TasksVec *pLocalVec, *pServVec;
	struct TaskNode *pTNode;
	//
	pLocalVec = &pTasksCache->ltskatt;
	pServVec = &pTasksCache->stskatt;
	for(iter = pTNodeV->begin(); pTNodeV->end() != iter; ++iter) {
		pTNode = *iter;
		//
		if(LOCATION_CLIENT & pTNode->control_code) {
			switch(TNODE_STYPE_BYTE(pTNode->control_code)) {
			case STYPE_GET:
				NTasksVec::AppNewNode(pLocalVec, pTNode, ADDI, GPSYN_TYPE);
				break;
			case STYPE_PSYNC:
				NTasksVec::AppNewNode(pLocalVec, pTNode, MODIFY, GPSYN_TYPE);
				break;
			case STYPE_MOVE:
 				NTasksVec::AppNewNode(pLocalVec, pTNode, ADDI, MVCPY_TYPE);
				 break;
			case STYPE_COPY:
				NTasksVec::AppNewNode(pLocalVec, pTNode, ADDI, GPSYN_TYPE);
				NTasksVec::AppNewNode(pLocalVec, pTNode, ADDI, MVCPY_TYPE);
				break;
			case STYPE_DELETE:
				NTasksVec::AppNewNode(pLocalVec, pTNode, DELE, GPSYN_TYPE);
				break;
			}
		} else if(LOCATION_SERVER & pTNode->control_code) {
			switch(TNODE_STYPE_BYTE(pTNode->control_code)) {
			case STYPE_PUT:
				NTasksVec::AppNewNode(pServVec, pTNode, ADDI, GPSYN_TYPE);
				break;
			case STYPE_GSYNC:
				NTasksVec::AppNewNode(pServVec, pTNode, MODIFY, GPSYN_TYPE);
				break;
			case STYPE_MOVE:
 				NTasksVec::AppNewNode(pServVec, pTNode, ADDI, MVCPY_TYPE);
				 break;
			case STYPE_COPY:
				NTasksVec::AppNewNode(pServVec, pTNode, ADDI, GPSYN_TYPE);
				NTasksVec::AppNewNode(pServVec, pTNode, ADDI, MVCPY_TYPE);
				break;
			case STYPE_DELETE:
				NTasksVec::AppNewNode(pServVec, pTNode, DELE, GPSYN_TYPE);
				break;
			}
		}
    }
	//
}

int NBuildTasksBzl::RecursiveListVector(DiresVec *pListVec, LIST_ARGUMENT *pListArgu, TCHAR *szLocation) {
    int build_value = -1;
    objListVec.Initialize(PRODU_LIST_DEFAULT, szLocation);
    //
    pListArgu->pListHead = &objListHead;
    pListArgu->pListVec = &objListVec;
    DWORD dwExceptionRetry = 0x00;
    do {
        if(LIST_VALUE_FINISH == ListDownd::BuildListXml(pListArgu)) 
			build_value = 0x00;
    } while(RETRY_LIST_TIMES>dwExceptionRetry++ && ListWorkBzl::CheckListFile(&objListHead, &objListVec));
    //
    objListVec.ParseListXml((VOID *)pListVec, GLIST_RECU_DIRES);
    //
    objListHead.DeleListVec(); // todo: fix the list_value
    objListVec.Finalize();
    //
    return build_value;
}