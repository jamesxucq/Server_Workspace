#include "StdAfx.h"

#include "BuildTasksBzl.h"
#include "AnchorBzl.h"
#include "VerifyTasksBzl.h"
#include "HandleTasksBzl.h"
#include "ExceptAction.h"
#include "FinishUtility.h"
#include "LockAnchor.h"
#include "LocalBzl.h"
#include "ShellLinkBzl.h"
#include "NodeUtility.h"

#include "ExecuteUtility.h"

//
static void FillDireAttrib(FilesVec *pListVec, const TCHAR *szDriveLetter) {
    FilesVec::iterator iter;
    TCHAR file_name[MAX_PATH];
	//
	DRIVE_LETTE(file_name, szDriveLetter)
    for(iter = pListVec->begin(); pListVec->end() != iter; ++iter) {
        _tcscpy_s(NO_LETT(file_name), MAX_PATH-LETT_LENGTH, (*iter)->file_name);
		if(NDireUtility::IsDirectory(file_name)) {
			(*iter)->action_type |= DIRECTORY;
		}
    }
}

#define STSKATT_DEFAULT   _T("stskatt.tmp")
DWORD NExecuteUtility::TasksListProducerNOR(struct LocalConfig *pLocalConfig) {
	struct LIST_ARGUMENT tListArgu;
	DWORD slast_anchor;
	HANDLE hsatt = INVALID_HANDLE_VALUE;
	DWORD build_value = LIST_VALUE_FAILED;
	//
	CONVERT_SESSION_ARGU(&(tListArgu.tSeionArgu), &pLocalConfig->tClientConfig);
	CONVERT_SOCKETS_ARGU(&(tListArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
	//
	DWORD dwExceptionRetry = 0x00;
	while((RETRY_PRODUCER_TIMES>dwExceptionRetry++) && (LIST_VALUE_FINISH!=build_value)) {
		NBuildTasksBzl::InitTListEnviro();
		//
		DWORD list_type;
		DWORD llast_anchor = NAnchorBzl::GetLastAnchor();
		//
		SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), ONE_SOCKETS_THREAD); // get head
		build_value = NBuildTasksBzl::BuildListEnviro(&list_type, &slast_anchor, &tListArgu, llast_anchor);
		if(LIST_VALUE_FINISH == build_value) {
			SetServAnchor(slast_anchor);
			switch(list_type) {
			case GLIST_RECU_FILES:
				hsatt = NFileUtility::BuildCacheFile(STSKATT_DEFAULT);
				SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), FIVE_SOCKETS_THREAD); // get list
				if(!NBuildTasksBzl::BulidSlowListFile(hsatt, &tListArgu, pLocalConfig->tClientConfig.szLocation))
					build_value = LIST_VALUE_FINISH;
				SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), ONE_SOCKETS_THREAD); // keep alive
				NBuildTasksBzl::CreateSlowTasksList(hsatt, &tListArgu);
				CloseHandle( hsatt );
				break;
			case GLIST_ANCH_FILES:
				FilesVec sattent;
				SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), FIVE_SOCKETS_THREAD); // get list
// _LOG_DEBUG(_T("1 GetTickCount:%u"), GetTickCount()/1000);
				if(!NBuildTasksBzl::BulidFastListVector(&sattent, &tListArgu, pLocalConfig->tClientConfig.szLocation))
					build_value = LIST_VALUE_FINISH;
				FileColiVec *pColiVec = NFileColi::GetFileColiPoint();
				if(LIST_VALUE_FINISH == NBuildTasksBzl::BulidColiListVector(pColiVec, &tListArgu))
					build_value = LIST_VALUE_FINISH;
// _LOG_DEBUG(_T("2 GetTickCount:%u"), GetTickCount()/1000);
				// FillDireAttrib(&sattent, pLocalConfig->tClientConfig.szDriveLetter); /x // disable james 20150608 
				NBuildTasksBzl::CreateFastTasksList(&sattent, llast_anchor, &tListArgu);
				NFilesVec::DeleFilesVec(&sattent);
// _LOG_DEBUG(_T("3 GetTickCount:%u"), GetTickCount()/1000);
				break;
			}
			// _LOG_DEBUG(_T("set server anchor:%d"), NExecTransBzl::GetServAnchor());
_LOG_DEBUG(_T("-------- tasks list producer exception retry:%d"), dwExceptionRetry);
		} // 生成同步任务失败
	}
	//
	if(LIST_VALUE_FAILED == build_value) {
		SET_SOCKETS_THREADS(&(tListArgu.tSockeArgu), ONE_SOCKETS_THREAD); // get head
		NBuildTasksBzl::EarseListEnviro(&tListArgu);
_LOG_DEBUG(_T("tasks list producer normal, build_value:%08x"), build_value);
		NBuildTasksBzl::TListEnviroFinal();
	} else NLocalBzl::PerformTransmitProcess(NBuildTasksBzl::GetNodeTotal(), NULL);
_LOG_DEBUG(_T("t build_value:%08x"), build_value);
	return build_value;
}

DWORD NExecuteUtility::TasksListVerifier(struct LocalConfig *pLocalConfig) {
	DWORD veri_value = VERIFY_VALUE_FINISH;
	// 设置排除监视
	EXCEPT_ACTION_INITIAL
	//
	DWORD dwExceptionRetry = 0x00;
	while(RETRY_EXCEPTION_TIMES>dwExceptionRetry++ && NodeUtility::CheckVerifyFinished()) {
_LOG_DEBUG(_T("exception retry:%d"), dwExceptionRetry);
		veri_value = VerifyTasksBzl::BeginVerifyThread(pLocalConfig);
		//
		if((VERIFY_VALUE_FORCESTOP|VERIFY_VALUE_PAUSED|VERIFY_VALUE_KILLED
			|VERIFY_VALUE_FAILED|VERIFY_VALUE_NETWORK) & veri_value) { // 用户强制退出
_LOG_DEBUG(_T("veri_value:%d"), veri_value);
_LOG_INFO(_T("user force stop paused killed, exit exception loop in verifier!"));
			break;
		}
	}
	//
	// 清理排除监视
	FINAL_EXCEPT_ACTION // add by james 20130418 need to test
	return veri_value;
}

DWORD NExecuteUtility::TasksListHandler(struct LocalConfig *pLocalConfig) {
	DWORD handle_value = WORKER_VALUE_FINISH;
	// _LOG_DEBUG(_T("检查工作记录,检查是否有数据需要同步.")); // disable by james 20140410
	struct ClientConfig	*pClientConfig = &pLocalConfig->tClientConfig;
	// _LOG_DEBUG(_T("have transmit tasks to do ! set except and refresh icon")); // disable by james 20140410
	// 设置排除监视
	EXCEPT_ACTION_INITIAL
	NExecuteUtility::BeginRefreshExcept();
	//
	DWORD dwExceptionRetry = 0x00;
	DWORD dwTasksFinished = NodeUtility::ValidTasksFinished();
	while((RETRY_EXCEPTION_TIMES>dwExceptionRetry++) && dwTasksFinished) {
_LOG_DEBUG(_T("==================== sync hand ==================== "));
_LOG_DEBUG(_T("start list worker, exception retry times:%d"), dwExceptionRetry); // disable by james 20140410
		// Sleep(2000); DWORD ulTimestamp = GetTickCount(); // add by james 20111113 for test download and upload rate
		handle_value = NHandTasksBzl::BeginWorkerThread(pLocalConfig);
_LOG_DEBUG(_T("worker handle value: %08X"), handle_value); // disable by james 20140410
		// _LOG_DEBUG(_T("ulTimestamp:%d"), GetTickCount() - ulTimestamp);
		// 用户强制退出
		if((WORKER_VALUE_FORCESTOP|WORKER_VALUE_PAUSED|WORKER_VALUE_KILLED
			|WORKER_VALUE_FAILED|WORKER_VALUE_NETWORK) & handle_value) { // add by james 20130218
_LOG_INFO(_T("user force stop paused killed, exit exception loop in worker thread!"));
			break;
		}
		// 效验上传数据块有效性  // modify by james 20130320 for test
// _LOG_DEBUG(_T("validate upload data ! exception retry times:%d"), dwExceptionRetry); // disable by james 20140410
		if((RETRY_EXCEPTION_FIRST==dwExceptionRetry) && NodeUtility::CheckUploadVerify()) {
_LOG_DEBUG(_T("first validate upload data !")); // disable by james 20140410
			handle_value = VerifyTasksBzl::ValidTransmitFirst(pLocalConfig);
			if((VERIFY_VALUE_FORCESTOP|VERIFY_VALUE_PAUSED|VERIFY_VALUE_KILLED
				|VERIFY_VALUE_FAILED|VERIFY_VALUE_NETWORK) & handle_value) {
_LOG_INFO(_T("user force stop paused killed, exit exception loop in first valid transmit!"));
				break;
			}
		} else if((RETRY_EXCEPTION_SECOND<=dwExceptionRetry) && NodeUtility::CheckUploadVerify()) {
_LOG_DEBUG(_T("second validate upload data !")); // disable by james 20140410
			handle_value = VerifyTasksBzl::ValidTransmitSecond(pLocalConfig);
			if((VERIFY_VALUE_FORCESTOP|VERIFY_VALUE_PAUSED|VERIFY_VALUE_KILLED
				|VERIFY_VALUE_FAILED|VERIFY_VALUE_NETWORK) & handle_value) {
_LOG_INFO(_T("user force stop paused killed, exit exception loop in second valid transmit!"));
				break;
			}
		}
		//
		dwTasksFinished = NodeUtility::ValidTasksFinished();
		if(dwTasksFinished) { // 处理同步异常
			NodeUtility::DelayExceptionWarning(pLocalConfig);
			handle_value = WORKER_VALUE_EXCEPTION;
_LOG_WARN(_T("handle syncing tasks exception!"));
		} else handle_value = WORKER_VALUE_FINISH;
_LOG_DEBUG(_T("-------- tasks list handler exception retry:%d dwTasksFinished:%d"), dwExceptionRetry, dwTasksFinished);	
	}
	// 清理任务图标缓存
	NExecuteUtility::FailedRefreshExcept(); // disable by james 20130218
	FINAL_EXCEPT_ACTION // add by james 20130418 need to test
	//
	return handle_value;
}

//
DWORD NExecuteUtility::TasksListProducerEXP() {
	struct TasksCache tExcepCache;
	DWORD build_value = LIST_VALUE_FAILED;
	//
	if(!NBuildTasksBzl::BulidExcepCache(&tExcepCache)) 
		build_value = LIST_VALUE_FINISH;
// _LOG_DEBUG(_T("2 GetTickCount:%u"), GetTickCount()/1000);
	NBuildTasksBzl::InitTListEnviro();
	//
	NBuildTasksBzl::CreateExceptTasksList(&tExcepCache);
	NTaskCache::DeleTasksCache(&tExcepCache);
// FillDireAttrib(&stskatt, pLocalConfig->tClientConfig.szDriveLetter);
	//
	if(LIST_VALUE_FAILED == build_value) {
_LOG_DEBUG(_T("tasks list producer except, build_value:%08x"), build_value);
		NBuildTasksBzl::TListEnviroFinal();
	} else NLocalBzl::PerformTransmitProcess(NBuildTasksBzl::GetNodeTotal(), NULL);
_LOG_DEBUG(_T("t build_value:%08x"), build_value);
	return build_value;
}

//
// 数据同步完成,释放锚点数据
#define LIST_FINISH_SUCCESS(FINISH_WORKER, LOCK_ANCHOR) { \
	FINISH_WORKER->Final(FINAL_STATUS_SUCCESS); \
	SetServAnchor(FINISH_WORKER->GetNewAnchor()); \
	*LOCK_ANCHOR = ANCHOR_FINISH_SUCCESS; \
}

// 数据同步失败,释放锚点数据.
#define LIST_FINISH_FAILED(FINISH_WORKER, LOCK_ANCHOR) { \
	FINISH_WORKER->Final(FINAL_STATUS_FINISH); \
	*LOCK_ANCHOR = ANCHOR_FINISH_FAILED; \
}

// 数据同步完成,释放锚点数据
#define LIST_FINISH_UPDATE(FINISH_WORKER, LOCK_ANCHOR) { \
	FINISH_WORKER->Final(FINAL_STATUS_FINISH); \
	*LOCK_ANCHOR = ANCHOR_FINISH_UPDATE; \
}

// 没有数据同步,释放锚点数据
#define LIST_FINISH_LEAVE(FINISH_WORKER, LOCK_ANCHOR) { \
	FINISH_WORKER->Final(FINAL_STATUS_FINISH); \
	*LOCK_ANCHOR = ANCHOR_FINISH_LEAVE; \
}

DWORD NExecuteUtility::FinishTasksList(DWORD *dwLockAnchor, struct LocalConfig *pLocalConfig, DWORD handle_value, DWORD dwChanValue) {
	struct FINISH_ARGUMENT tFinishArgu;
	DWORD finish_value = FINISH_VALUE_FINISH;
	//
	CONVERT_SESSION_ARGU(&(tFinishArgu.tSeionArgu), &pLocalConfig->tClientConfig);
	CONVERT_SOCKETS_ARGU(&(tFinishArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
	SET_SOCKETS_THREADS(&(tFinishArgu.tSockeArgu), ONE_SOCKETS_THREAD); // finish
	//
	CTRANSWorker *pFinishWorker = FinishUtility::CreateWorker(&finish_value, &tFinishArgu);
	if(pFinishWorker) { //
		if(HAVE_TASK_VALU & dwChanValue) {
_LOG_DEBUG(_T("有数据同步"));
			if(WORKER_VALUE_FINISH == handle_value) { // 数据同步完成，关闭连接，修改本地数据。
_LOG_DEBUG(_T("数据同步完成,服务器有变动,更新本地及服务器锚点.")); // disable by james 20140410
				// 服务器有变动,更新本地及服务器锚点
				if(SERV_CHAN_VALU & dwChanValue) LIST_FINISH_SUCCESS(pFinishWorker, dwLockAnchor)  // 数据同步完成,释放锚点数据
				else {
_LOG_DEBUG(_T("数据同步完成,服务器没有变动,更新本地锚点数据.")); // disable by james 20140410
_LOG_DEBUG(_T("GetLastAnchor:%d GetServAnchor:%d"), NAnchorBzl::GetLastAnchor(), NExecTransBzl::GetServAnchor()); // disable by james 20140410
					// 服务器没有变动,更新本地锚点
					if(NAnchorBzl::GetLastAnchor() != GetServAnchor()) LIST_FINISH_UPDATE(pFinishWorker, dwLockAnchor)
						// 特殊情况,有数据需要同步,服务器没有变动,初始锚点相同.(本地及服务器锚点都为NULL)
					else LIST_FINISH_SUCCESS(pFinishWorker, dwLockAnchor)
				}
			} else LIST_FINISH_FAILED(pFinishWorker, dwLockAnchor)// _LOG_DEBUG(_T("数据同步失败,释放锚点数据.")); // disable by james 20140410
				//
		} else { // 会话初始成功,没有数据需要同步
_LOG_DEBUG(_T("没有数据同步,锚点不相等,更新本地锚点.")); // disable by james 20140410
			// 锚点不相等,更新本地锚点
			if(NAnchorBzl::GetLastAnchor() != GetServAnchor()) LIST_FINISH_UPDATE(pFinishWorker, dwLockAnchor)
				// 没有数据需要同步,锚点相等
			else {
_LOG_DEBUG(_T("锚点相等,不更新锚点."));
				// 锚点相等,不更新锚点
				if(NAnchorBzl::GetLastAnchor()) LIST_FINISH_LEAVE(pFinishWorker, dwLockAnchor)
					// 特殊情况,没有数据需要同步,服务器没有变动,初始锚点相同.(本地及服务器锚点都为NULL)
				else LIST_FINISH_SUCCESS(pFinishWorker, dwLockAnchor)
			}
		}
		//
	} else { // 会话初始失败,返回失败.会话初始失败,返回错误。
// _LOG_DEBUG(_T("初始失败.")); // disable by james 20140410
		*dwLockAnchor = ANCHOR_FINISH_FAILED;
	}
	// 销毁服务器连接
	FinishUtility::DestroyWorker(pFinishWorker);
_LOG_DEBUG(_T("Finish Tasks List"));
	NBuildTasksBzl::TListEnviroFinal(); // 清理任务缓存
	//
	return finish_value;
}

VOID NExecuteUtility::BeginRefreshIcon(TCHAR *szDriveLetter) {
	struct TaskNode task_node;
	DWORD dwFileFinish = 0x00;
	TCHAR szFileName[MAX_PATH];
	//
_LOG_DEBUG(_T("begin refresh icon!")); // disable by james 20140410
	DRIVE_LETTE(szFileName, szDriveLetter)
	objTaskList.SetHandNumbe(0);
	DWORD node_inde = objTaskList.GetHandNodeAdd();
	while(0 < objTaskList.ReadNodeEx(&task_node, node_inde)) {
// ndp::TaskNode(&task_node); // disable by james 20130408
		if(FILE_TASK_BEGIN & task_node.control_code)  dwFileFinish = 0x00;
		if(!dwFileFinish && ONLY_TASK_UNDONE(task_node.success_flags)) dwFileFinish = 0x01;
		//
		if((FILE_TASK_END&task_node.control_code) && dwFileFinish) {
// ndp::TaskNode(&task_node); // disable by james 20130408
			_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
			LINKER_DISPLAY_START(szFileName, task_node.isdire)
_LOG_DEBUG(_T("linker start:%s :%08x"), szFileName, task_node.success_flags);
			if((STYPE_MOVE|STYPE_COPY) & task_node.control_code) {
				_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, task_node.szFileName2);
				LINKER_DISPLAY_START(szFileName, task_node.isdire)
_LOG_DEBUG(_T("linker start:%s :%08x"), szFileName, task_node.success_flags);
			}
		}
		//
		node_inde = objTaskList.GetHandNodeAdd();
	}
	REFRESH_EXPLORER_WINDOWS(szDriveLetter)
}

VOID NExecuteUtility::FailedRefreshIcon(TCHAR *szDriveLetter) {
	struct TaskNode task_node;
	DWORD dwFileFinish = 0x00;
	TCHAR szFileName[MAX_PATH];
	//
_LOG_DEBUG(_T("failed refresh icon!")); // disable by james 20140410
	DRIVE_LETTE(szFileName, szDriveLetter)
	FileColiVec *pColiVec = NFileColi::GetFileColiPoint();
	FileColiVec::iterator iter;
	for(iter=pColiVec->begin(); pColiVec->end()!=iter; ++iter) {
		if(COFT_HAND_DONE((*iter)->dwExcepValue)) {
			_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->szFileName);
			LINKER_REVOKE_CONFLI(szFileName)		
		}
	}
//
	objTaskList.SetHandNumbe(0);
	DWORD node_inde = objTaskList.GetHandNodeAdd();
	while(0 < objTaskList.ReadNodeEx(&task_node, node_inde)) {
		if(FILE_TASK_BEGIN & task_node.control_code)  dwFileFinish = 0x00;
		if(!dwFileFinish && ONLY_TASK_UNDONE(task_node.success_flags)) dwFileFinish = 0x01;
		//
		if((FILE_TASK_END&task_node.control_code) && dwFileFinish) {
// ndp::TaskNode(&task_node); // disable by james 20130408
			_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
			LINKER_DISPLAY_FAILED(szFileName, task_node.isdire)
			if(((TRANSMIT_EXCEPTION|VERIFY_EXCEPTION)&task_node.success_flags) && IS_INTERNAL_EXP(task_node.excepti)) {
				NFileColi::AddNewColi(pColiVec, task_node.szFileName1);
				LINKER_REVOKE_CONFLI(szFileName);
			}
			if((STYPE_MOVE|STYPE_COPY) & task_node.control_code) {
				_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, task_node.szFileName2);
				LINKER_DISPLAY_FAILED(szFileName, task_node.isdire)
			}
		}
		//
		node_inde = objTaskList.GetHandNodeAdd();
	}
	//
	NFileColi::FileColiSerial();
	REFRESH_EXPLORER_WINDOWS(szDriveLetter)
}

VOID NExecuteUtility::BeginRefreshExcept() {
	struct TaskNode task_node;
	DWORD dwFileFinish = 0x00;
	//
_LOG_DEBUG(_T("begin except refresh!")); // disable by james 20140410
	objTaskList.SetHandNumbe(0);
	DWORD node_inde = objTaskList.GetHandNodeAdd();
	while(0 < objTaskList.ReadNodeEx(&task_node, node_inde)) {
// ndp::TaskNode(&task_node); // disable by james 20130408
		if(FILE_TASK_BEGIN & task_node.control_code)  dwFileFinish = 0x00;
		if(!dwFileFinish && ONLY_TASK_UNDONE(task_node.success_flags)) dwFileFinish = 0x01;
		//
		if((FILE_TASK_END&task_node.control_code) && dwFileFinish) {
// ndp::TaskNode(&task_node); // disable by james 20130408
			if(LOCATION_CLIENT & TNODE_LOCATION_BYTE(task_node.control_code))
				LINKER_EXCEPT_START(&task_node)
		}
		node_inde = objTaskList.GetHandNodeAdd();
	}
}

VOID NExecuteUtility::FailedRefreshExcept() {
	struct TaskNode task_node;
	DWORD dwFileFinish = 0x00;
	//
_LOG_DEBUG(_T("failed except refresh!")); // disable by james 20140410
	objTaskList.SetHandNumbe(0);
	DWORD node_inde = objTaskList.GetHandNodeAdd();
	while(0 < objTaskList.ReadNodeEx(&task_node, node_inde)) {
		if(FILE_TASK_BEGIN & task_node.control_code)  dwFileFinish = 0x00;
		if(!dwFileFinish && ONLY_TASK_UNDONE(task_node.success_flags)) dwFileFinish = 0x01;
		//
		if((FILE_TASK_END&task_node.control_code) && dwFileFinish) {
// ndp::TaskNode(&task_node); // disable by james 20130408
			if(LOCATION_CLIENT & TNODE_LOCATION_BYTE(task_node.control_code))
				LINKER_EXCEPT_FAILED(&task_node)
		}
		//
		node_inde = objTaskList.GetHandNodeAdd();
	}
}

/* Limits and constants. */
#define TYPE_FILE 0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIRE 0x02
#define TYPE_DOT 0x04

static BOOL TraverseDirectory (FilesVec *, TCHAR *, TCHAR *);
static DWORD FileType (LPWIN32_FIND_DATA);
static BOOL ProcessItem (FilesVec *, TCHAR *, LPWIN32_FIND_DATA );

DWORD NExecuteUtility::GetFilesByDirectory(FilesVec *pFilesVec, const TCHAR *szPath) {
	BOOL ok = TRUE;
	TCHAR szSearchName [MAX_PATH], szCurrentPath [MAX_PATH];
	TCHAR szPathString [MAX_PATH] = _T("");
//
	if(!pFilesVec || !szPath) return ((DWORD)-1);
	if(!GetCurrentDirectory (MAX_PATH, szCurrentPath)) return ((DWORD)-1);
//
	_stprintf_s(szSearchName, _T("%s\\"), szPath);
	if(!SetCurrentDirectory (szSearchName)) return ((DWORD)-1);
//
	ok = TraverseDirectory (pFilesVec, _T ("*"), szPathString);
	if(!SetCurrentDirectory (szCurrentPath)) return ((DWORD)-1);
//
	return ok ? 0: -1;
}

/*	Traverse a directory - Carrying out an implementation-specific
"action" for every name encountered. The action in this version is "Delete". */
/*	PathName: Relative or absolute pathname to traverse. */
static BOOL TraverseDirectory (FilesVec *pFilesVec, TCHAR *szPathName, TCHAR *szPathString) {
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	DWORD dwFileType, dwPass;
//
	/*	Open up the directory search handle and get the
	first file name to satisfy the pathname.
	Make two passes. The first processes the files
	and the second processes the directories. */
//
	for (dwPass = 0; dwPass < 2; dwPass++) {
		SearchHandle = FindFirstFile (szPathName, &FindData);
		/* A deleted file will not be found on the second pass */
		if (INVALID_HANDLE_VALUE == SearchHandle) return TRUE;
//
		/* Scan the directory and its subdirectories
		for files satisfying the pattern. */
		do {
			/* For each file located, get the type. Delete files on pass 1.
			On pass 2, recursively process the subdirectory
			contents, if the recursive option is set. */

			dwFileType = FileType (&FindData); /* Delete a file. */
			if (!dwPass) ProcessItem (pFilesVec, szPathString, &FindData);

			/* Traverse the subdirectory on the second pass. */
			if (dwFileType&TYPE_DIRE && dwPass) {
				// fprintf(stderr, "----------------%s\n", szPathString);
				SetCurrentDirectory (FindData.cFileName);
				struti::appe_path(szPathString, FindData.cFileName);
				//
				TraverseDirectory (pFilesVec, _T ("*"), szPathString);
				//
				struti::dele_last_path(szPathString, FindData.cFileName);
				SetCurrentDirectory (_T (".."));
			}
			/* Get the next file or directory name. */
		} while (FindNextFile (SearchHandle, &FindData));
		FindClose (SearchHandle);
	}
	return TRUE;
}

/* Return file type from the find data structure.
Types supported:
TYPE_FILE:	If this is a file
TYPE_DIRE:	If this is a directory other than . or ..
TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData) {
	DWORD dwFileType = TYPE_FILE;
	if (FILE_ATTRIBUTE_DIRECTORY & pFileData->dwFileAttributes)
		if (!lstrcmp(pFileData->cFileName, _T (".")) || !lstrcmp(pFileData->cFileName, _T ("..")))
			dwFileType = TYPE_DOT;
		else dwFileType = TYPE_DIRE;
//
		return dwFileType;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static BOOL ProcessItem (FilesVec *pFilesVec, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
	DWORD dwFileType;
//
	dwFileType = FileType (pFileData);
	if (!(TYPE_FILE&dwFileType) && !(TYPE_DIRE&dwFileType)) return FALSE;
//
	if (TYPE_DIRE & dwFileType) {
		//_tprintf (_T ("send directory %s\n"), pFileData->szFileName);
	} else if (TYPE_FILE & dwFileType) {
		//_tprintf (_T ("send file %s\n"), pFileData->szFileName);
		NFilesVec::AddNewNode(pFilesVec, szPathString, pFileData);
	}
//
	return TRUE;
}
