#include "StdAfx.h"

#include "../third party/RiverFS/RiverFS.h"
#include "VerifyUtility.h"
#include "ExecuteTransmitBzl.h"
#include "ShellLinkBzl.h"
#include "ProduceObj.h"
#include "LocalBzl.h"
#include "DebugPrinter.h"

#include "VerifyTasksBzl.h"

//
#define VERIFY_TYPE_INVALID		0x00
#define VERIFY_TYPE_WHOLE		0x0001
#define VERIFY_TYPE_PORTION		0x0002

// 读取工作节点
// MFC临界区类对象
CCriticalSection gcsVerifyPrepa;
int DowndVerify::VerifyPrepa(DWORD *dwVerifyType, DWORD *sta_inde, DWORD *end_inde) {
	DWORD node_inde;
	int read_value;
	struct TaskNode task_node;
	int prepa_value = TNODE_PREPA_FAULT;
	//
	gcsVerifyPrepa.Lock();
	for(; ; ) {
		node_inde = objTaskList.GetHandNodeAdd();
		read_value = objTaskList.ReadNodeEx(&task_node, node_inde);
		if(!read_value) {
			prepa_value = FINISH_VERIFY;
			break;
		} else if(0 > read_value) {
			prepa_value = TNODE_PREPA_FAULT;
			break;
		}
// _LOG_DEBUG(_T("node_inde:%d, read_value:%d array_id:%d"), node_inde, read_value, task_node.array_id);
// _LOG_DEBUG(_T("szFileName1:%s"), task_node.szFileName1);
		//
		if(((STYPE_GET|STYPE_PUT|STYPE_PSYNC|STYPE_GSYNC|STYPE_MOVE|STYPE_COPY)&task_node.control_code) && ONLY_VERIFY_UNDONE(task_node.success_flags)) {
			if(FILE_TASK_BEGIN & task_node.control_code) {
				prepa_value = TNODE_PREPA_OKAY;
				*sta_inde = node_inde;
			}
			if(FILE_TASK_END & task_node.control_code) {
				*end_inde = node_inde;
				if((STYPE_GET|STYPE_PUT|STYPE_PSYNC|STYPE_GSYNC) & task_node.control_code)
					*dwVerifyType = VERIFY_TYPE_PORTION;
				else if((STYPE_MOVE|STYPE_COPY) & task_node.control_code)
					*dwVerifyType = VERIFY_TYPE_WHOLE;
				break;
			}
// ndp::TaskNode(&task_node); // disable by james 20130415
		}
	}
// _LOG_DEBUG(_T("prepa_value:%d, node_inde:%d, control_code:%x success_flags:%x"), prepa_value, node_inde, task_node.control_code, task_node.success_flags);
	gcsVerifyPrepa.Unlock();
	//
	return prepa_value;
}

// && ((STYPE_MOVE|STYPE_COPY)&task_node.control_code)
DWORD DowndVerify::Sha1Verifier(HANDLE hFileValid, DWORD node_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	struct TaskNode task_node;
	DWORD veri_value = VERIFY_VALUE_FINISH;
	//
	NFileUtility::TruncateHandle(hFileValid, 0x00);
	if(0 < objTaskList.ReadNodeEx(&task_node, node_inde)) {
		veri_value = NVerifyUtility::FileSha1Request(hFileValid, pVerifyWorker, &task_node, RIVER_CACHED);
		if(VERIFY_VALUE_NETWORK & veri_value) 
			return VERIFY_VALUE_FAILED;
	}
_LOG_DEBUG(_T("veri_value:%08x"), veri_value);
	DWORD dwResult = SetFilePointer(hFileValid, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		return VERIFY_VALUE_FAILED;
	}
	//
	unsigned char szSha1Chks[SHA1_DIGEST_LEN];
	_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName2);
	if(NVerifyUtility::FileSha1River(szSha1Chks, pVerifyWorker, szFilePath))
		memset(szSha1Chks, 0, SHA1_DIGEST_LEN);
	DWORD dwFileFinish = NVerifyUtility::VerifyTasksNode(&task_node, szSha1Chks, hFileValid, 
		((VERIFY_VALUE_FINISH|VERIFY_VALUE_NCONTE)&veri_value));
	objTaskList.WriteNodeEx(&task_node, task_node.node_inde);
	//
	if(!dwFileFinish) {
_LOG_DEBUG(_T("downd sha1 success :%s :%08x"), szFilePath, task_node.success_flags);
		RiverFS::FileLawiUpdate(task_node.szFileName2, &task_node.last_write);
		LINKER_EXCEPT_START(&task_node)
		NFileUtility::SetFileSizeLawi(szFilePath, task_node.file_size, &task_node.last_write);
		LINKER_EXCEPT_SUCCESS(&task_node)
		LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
		NLocalBzl::PerformTransmitProcess(NULL, 0x01);
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
		LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire); 
TCHAR timestr[64];
_LOG_DEBUG( _T("set name:%s lawi:%s"), szFilePath, timcon::ftim_unis(timestr, &task_node.last_write) );
_LOG_DEBUG(_T("upv success :%s :%08x"), szFilePath, task_node.success_flags);
	}
	//
	return VERIFY_VALUE_FINISH;
}

DWORD DowndVerify::FileVerifier(HANDLE hFileValid, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	struct TaskNode task_node;
	DWORD veri_value = VERIFY_VALUE_FINISH;
	//
	HANDLE hFileLocal = NFileUtility::BuildVerifyFile(NULL);
	if(INVALID_HANDLE_VALUE == hFileLocal) 
		return VERIFY_VALUE_FAILED;
	objVerifyVector.AppendResource(hFileLocal);
	//
_LOG_DEBUG(_T("+++++++++++ File Verifier start:%d end:%d"), sta_inde, end_inde);
	DWORD dwFileFinish;
	DWORD node_inde;
	for(node_inde = sta_inde; (node_inde<=end_inde) && (0<objTaskList.ReadNodeEx(&task_node, node_inde)); node_inde++) {
_LOG_DEBUG(_T("file verifier node_inde:%d control_code:%08x"), node_inde, task_node.control_code);
		if(FILE_TASK_BEGIN & task_node.control_code) {
			_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
			dwFileFinish = ERR_SUCCESS;
			//
			NFileUtility::TruncateHandle(hFileValid, 0x00);
			veri_value = NVerifyUtility::FileChksRequest(hFileValid, pVerifyWorker, &task_node, RIVER_CACHED);
			if(VERIFY_VALUE_NETWORK & veri_value) break;
_LOG_DEBUG(_T("veri_value:%08x"), veri_value);
			NFileUtility::TruncateHandle(hFileLocal, 0x00);
			NVerifyUtility::FileChksRiver(hFileLocal, pVerifyWorker, szFilePath);
		}
		//
		dwFileFinish |= NVerifyUtility::VerifyTasksNode(&task_node, hFileLocal, hFileValid, 
			(VERIFY_VALUE_FINISH|VERIFY_VALUE_NCONTE)&veri_value);
_LOG_DEBUG(_T("File Verifier dwFileFinish:%X"), dwFileFinish);
		objTaskList.WriteNodeEx(&task_node, task_node.node_inde);
		//
		if((FILE_TASK_END&task_node.control_code) && !dwFileFinish) {
			// update really file lastwrite
			if((STYPE_GET|STYPE_PSYNC) & task_node.control_code) {
_LOG_DEBUG(_T("down verify,update file lastwrite."));
				RiverFS::FileLawiUpdate(task_node.szFileName1, &task_node.last_write);
				LINKER_EXCEPT_START(&task_node)
				NFileUtility::SetFileSizeLawi(szFilePath, task_node.file_size, &task_node.last_write);
				LINKER_EXCEPT_SUCCESS(&task_node)
TCHAR timestr[64];
_LOG_DEBUG( _T("set name:%s lawi:%s"), szFilePath, timcon::ftim_unis(timestr, &task_node.last_write) );
			}
_LOG_DEBUG(_T("file verifier success."));
			LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
			NLocalBzl::PerformTransmitProcess(NULL, (end_inde-sta_inde+0x01));
_LOG_DEBUG(_T("fv success :%s :%08x"), szFilePath, task_node.success_flags);
		}
		//
	}
	//
_LOG_DEBUG(_T("file verifier end."));
	if(INVALID_HANDLE_VALUE != hFileLocal) {
		objVerifyVector.EraseResource(hFileLocal);
		CloseHandle( hFileLocal );
		hFileLocal = INVALID_HANDLE_VALUE;
	}
_LOG_DEBUG(_T("file verifier veri_value:%x"), veri_value);
	//
	return veri_value;
}

//
DWORD DowndVerify::ListVerifier(DWORD dwVerifyType, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	//
	HANDLE hFileValid = NFileUtility::BuildVerifyFile(NULL);
	if(INVALID_HANDLE_VALUE == hFileValid)
		return VERIFY_VALUE_FAILED;
	objVerifyVector.AppendResource(hFileValid);
_LOG_DEBUG(_T("list verifier start! dwVerifyType:%x"), dwVerifyType);
	//
	DWORD dwVeriValue = VERIFY_VALUE_FAILED;
	switch(dwVerifyType) {
	case VERIFY_TYPE_INVALID:
		break;
	case VERIFY_TYPE_WHOLE:
		dwVeriValue = Sha1Verifier(hFileValid, sta_inde, pVerifyWorker, szFilePath);
		break;
	case VERIFY_TYPE_PORTION:
		dwVeriValue = FileVerifier(hFileValid, sta_inde, end_inde, pVerifyWorker, szFilePath);
		break;
	}
	//
_LOG_DEBUG(_T("list verifier end!"));
	if(INVALID_HANDLE_VALUE != hFileValid) {
		objVerifyVector.EraseResource(hFileValid);
		CloseHandle( hFileValid );
		hFileValid = INVALID_HANDLE_VALUE;
	}
	//
	return dwVeriValue;
}

UINT VerifyThreadProcess(LPVOID lpParam) {
	struct VERIFY_ARGUMENT *pVerifyArgu = (struct VERIFY_ARGUMENT *)lpParam;
	if (NULL == pVerifyArgu) FAULT_RETURN; //输入参数非法
	// must lock data file
	TCHAR szFilePath[MAX_PATH];
	DRIVE_LETTE(szFilePath, pVerifyArgu->pClientConfig->szDriveLetter)
_LOG_DEBUG(_T("initial verify worker."));
	DWORD veri_value = VERIFY_VALUE_FINISH;
	CTRANSWorker *pVerifyWorker = NVerifyUtility::CreateWorker(&veri_value, pVerifyArgu);
	DWORD dwVerifyType = VERIFY_TYPE_INVALID;
	DWORD sta_inde, end_inde;
	for(; ; ) { // 检查运行状态
// _LOG_DEBUG(_T("check transmit run status!"));
		DWORD dwProcessStatus = NExecTransBzl::GetTransmitNextStatus();
		if(PROCESS_STATUS_RUNNING != dwProcessStatus) {
			CONVERT_VERIFY_STATUS(veri_value, dwProcessStatus)
// _LOG_DEBUG(_T("process status is not running, so exit! veri_value:%X"), veri_value); // disable by james 20140410
			break;
		}
		// 读取工作节点
// _LOG_DEBUG(_T("read task node!"));
		int prepa_value = DowndVerify::VerifyPrepa(&dwVerifyType, &sta_inde, &end_inde);
		if(!prepa_value) {
			veri_value = VERIFY_VALUE_FINISH;
_LOG_DEBUG(_T("read node finish!")); // disable by james 20140410
			break;
		} else if(0 > prepa_value) {
			veri_value = VERIFY_VALUE_FAILED;
_LOG_DEBUG(_T("read node error!")); // disable by james 20140410
			break;
		}
_LOG_DEBUG(_T("read node start:%d end:%d"), sta_inde, end_inde);
		// 创建服务器连接
		if(!pVerifyWorker) {
			// _LOG_DEBUG(_T("build verify worker."));
			if(!(pVerifyWorker = NVerifyUtility::CreateWorker(&veri_value, pVerifyArgu))) {
				// _LOG_DEBUG(_T("build verify worker error."));
				break;
			}
		}
		// 执行效验工作 
_LOG_DEBUG(_T("verify task node!"));
		veri_value = DowndVerify::ListVerifier(dwVerifyType, sta_inde, end_inde, pVerifyWorker, szFilePath);
		if(VERIFY_VALUE_NETWORK & veri_value) {
_LOG_DEBUG(_T("drop verify worker."));
			NVerifyUtility::DropWorker(pVerifyWorker);
			pVerifyWorker = NULL;
		}
// _LOG_DEBUG(_T("verify end!"));
	}
	// 销毁服务器连接
_LOG_DEBUG(_T("final verify worker."));
	NVerifyUtility::DestroyWorker(pVerifyWorker);
_LOG_DEBUG(_T("finish worker and exit thread! veri_value:%X"), veri_value);
	//
	return veri_value;
}

DWORD VerifyTasksBzl::BeginVerifyThread(struct LocalConfig *pLocalConfig) {
	HANDLE hVerifyThreads[FIVE_VERIFY_THREAD];
	DWORD dwThreadExitValue[FIVE_VERIFY_THREAD];
	DWORD dwVerifyExitValue = VERIFY_VALUE_FINISH;
	struct VERIFY_ARGUMENT tVerifyArgu;
	DWORD thread_index;
	//
	CONVERT_SESSION_ARGU(&(tVerifyArgu.tSeionArgu), &pLocalConfig->tClientConfig);
	CONVERT_SOCKETS_ARGU(&(tVerifyArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
	SET_SOCKETS_THREADS(&(tVerifyArgu.tSockeArgu), FIVE_SOCKETS_THREAD); // verify
	tVerifyArgu.pClientConfig = &pLocalConfig->tClientConfig;
	VERIFY_OBJECT(SetVerifyThreads(hVerifyThreads, FIVE_VERIFY_THREAD));
	// 启动线程
_LOG_DEBUG(_T("效验 启动线程"));
	objTaskList.SetHandNumbe(0);
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
		CWinThread *pWinThread = AfxBeginThread(VerifyThreadProcess, (LPVOID)&tVerifyArgu, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		pWinThread->m_bAutoDelete = FALSE;
		hVerifyThreads[thread_index] = pWinThread->m_hThread;
		pWinThread->ResumeThread();
	}
	// 等待结束
_LOG_DEBUG(_T("效验 等待结束"));
	WaitForMultipleObjects(FIVE_VERIFY_THREAD, hVerifyThreads, TRUE, INFINITE);
	//
	// 得到退出码
_LOG_DEBUG(_T("效验 得到工作线程退出码"));
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
		if(!GetExitCodeThread(hVerifyThreads[thread_index], &dwThreadExitValue[thread_index])) {
			_LOG_WARN(_T("Get ThreadExitValue[%d] error! GetLastError:%08x"), thread_index, GetLastError());
		}
		CloseHandle( hVerifyThreads[thread_index] );
	}
	// 处理退出码
_LOG_DEBUG(_T("效验 处理工作线程退出码"));
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
		// _LOG_DEBUG(_T("------------------ thread:%d exit code:%X"), thread_index, ThreadExitValue[thread_index]); // disable by james 20140410
		if(VERIFY_VALUE_FINISH != dwThreadExitValue[thread_index]) {
			dwVerifyExitValue = dwThreadExitValue[thread_index];
		}
	}
	// delete malloc resouce
	VERIFY_OBJECT(SetVerifyThreads(NULL, 0));
_LOG_DEBUG(_T("+++++++++++++++++++++++++ verify exit value:%X"), dwVerifyExitValue); // disable by james 20140410
	return dwVerifyExitValue;
}

//
int UploadVerify::VerifyPrepa(DWORD *dwVerifyType, DWORD *sta_inde, DWORD *end_inde) {
	DWORD node_inde;
	int read_value;
	struct TaskNode task_node;
	int iVerifyFound = 0;
	int prepa_value = TNODE_PREPA_FAULT;
	//
	gcsVerifyPrepa.Lock();
	for(; ; ) {
		node_inde = objTaskList.GetHandNodeAdd();
		read_value = objTaskList.ReadNodeEx(&task_node, node_inde);
		if(!read_value) {
			prepa_value = FINISH_VERIFY;
			break;
		} else if(0 > read_value) {
			prepa_value = TNODE_PREPA_FAULT;
			break;
		}
// ndp::TaskNode(task_node); // disable by james 20130415
		//
		if((STYPE_PUT|STYPE_GSYNC|STYPE_MOVE|STYPE_COPY) & task_node.control_code) {
			if(FILE_TASK_BEGIN & task_node.control_code) {
				prepa_value = TNODE_PREPA_OKAY;
				*sta_inde = node_inde;
			}
			//
			if(!ONLY_TRANSMIT_OKAY(task_node.success_flags) && ONLY_VERIFY_OKAY(task_node.success_flags))
				iVerifyFound = 1;
			//
			if(FILE_TASK_END & task_node.control_code) {
				*end_inde = node_inde;
				if((STYPE_PUT|STYPE_GSYNC) & task_node.control_code)
					*dwVerifyType = VERIFY_TYPE_PORTION;
				else if((STYPE_MOVE|STYPE_COPY) & task_node.control_code)
					*dwVerifyType = VERIFY_TYPE_WHOLE;
				//
				if(iVerifyFound) break;
				prepa_value = TNODE_PREPA_FAULT;
			}
//
		}
	}
	// 离开临界区
	gcsVerifyPrepa.Unlock();
	//
	return prepa_value;
}

DWORD UploadVerify::Sha1Verifier(HANDLE hFileValid, DWORD node_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	struct TaskNode task_node;
	DWORD veri_value = VERIFY_VALUE_FINISH;
	//
	NFileUtility::TruncateHandle(hFileValid, 0x00);
	if(0 < objTaskList.ReadNodeEx(&task_node, node_inde)) {
		veri_value = NVerifyUtility::FileSha1Request(hFileValid, pVerifyWorker, &task_node, VERIFY_CACHE);
		if(VERIFY_VALUE_NETWORK & veri_value) 
			return VERIFY_VALUE_FAILED;
_LOG_DEBUG(_T("file sha1 req:%08x"), veri_value);
	}
	DWORD dwResult = SetFilePointer(hFileValid, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		return VERIFY_VALUE_FAILED;
	}
	//
	DWORD dwFileFinish = ERR_SUCCESS;
	if(dwFileFinish = NVerifyUtility::Sha1ValidTransmit(&task_node, hFileValid, 
		(VERIFY_VALUE_FINISH|VERIFY_VALUE_NCONTE)&veri_value)) {
_LOG_DEBUG(_T("set transmit verify excep, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
		SET_TRANSMIT_VERIFY_EXCEPTION(task_node.success_flags);
		SET_DOWND_EXP(task_node.excepti);
		veri_value = VERIFY_VALUE_EXCEPTION;
	} else {
_LOG_DEBUG(_T("set transmit verify done, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
		SET_VERIFY_DONE(task_node.success_flags);
	}
	objTaskList.WriteNodeEx(&task_node, task_node.node_inde);
	//
	if(!dwFileFinish) {
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName2);
		LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
_LOG_DEBUG(_T("upv success :%s :%08x"), szFilePath, task_node.success_flags);
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
		LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
_LOG_DEBUG(_T("upv success :%s :%08x"), szFilePath, task_node.success_flags);
	}
	//
	return veri_value;
}

DWORD UploadVerify::FileVerifier(HANDLE hFileValid, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	struct TaskNode task_node;
	DWORD veri_value = VERIFY_VALUE_FINISH;
	//
// _LOG_DEBUG(_T("verify transmit tasks.")); // disable by james 20140410
	DWORD dwFileFinish = ERR_SUCCESS;
	DWORD node_inde;
	for(node_inde = sta_inde; (node_inde<=end_inde) && (0<objTaskList.ReadNodeEx(&task_node, node_inde)); node_inde++) {
// _LOG_DEBUG(_T("-------------------------------------------------")); // disable by james 20140410
// _LOG_DEBUG(_T("node_inde:%d task_node.success_flags:%08X :%s"), node_inde, task_node.success_flags, task_node.szFileName1); // disable by james 20140410
		//
		if(FILE_TASK_BEGIN & task_node.control_code) {
			_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
			dwFileFinish = ERR_SUCCESS;
			//
			NFileUtility::TruncateHandle(hFileValid, 0x00);
			veri_value = NVerifyUtility::FileChksRequest(hFileValid, pVerifyWorker, &task_node, VERIFY_CACHE);
			if(VERIFY_VALUE_NETWORK & veri_value) break;
		}
		//
		if(!ONLY_TRANSMIT_OKAY(task_node.success_flags) && ONLY_VERIFY_OKAY(task_node.success_flags)) {
			DWORD dwFileValid = NVerifyUtility::FileValidTransmit(&task_node, hFileValid, 
				(VERIFY_VALUE_FINISH|VERIFY_VALUE_NCONTE)&veri_value);
			if(dwFileValid) {
_LOG_DEBUG(_T("set transmit verify excep, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
				SET_TRANSMIT_VERIFY_EXCEPTION(task_node.success_flags);
				SET_DOWND_EXP(task_node.excepti);
// _LOG_DEBUG(_T("verify chunk, exception. task_node.success_flags:%X"), task_node.success_flags); // disable by james 20140410
// _LOG_DEBUG(_T("node_inde:%d task_node.success_flags:%X"), node_inde, task_node.success_flags);
				veri_value = VERIFY_VALUE_EXCEPTION;
			} else {
_LOG_DEBUG(_T("set transmit verify done, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
				SET_VERIFY_DONE(task_node.success_flags);
			}
// _LOG_DEBUG(_T("node_inde::%d success_flags:%X FileValid:%X"), node_inde, task_node.success_flags, dwFileValid);
			dwFileFinish |= dwFileValid;
			objTaskList.WriteNodeEx(&task_node, task_node.node_inde);
		}
		//
		if((FILE_TASK_END&task_node.control_code) && !dwFileFinish) {
			RiverFS::FileUpdate(szFilePath, &task_node.last_write);
			LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
// _LOG_DEBUG(_T("upfv success :%s :%08x"), szFilePath, task_node.success_flags);
		}
	}
	// _LOG_DEBUG(_T("verify over close handle. veri_value:%X"), veri_value); // disable by james 20140410
	// _LOG_DEBUG(_T("file finished:%X"), dwFileFinish);
	return veri_value;
}

DWORD UploadVerify::ListVerifier(DWORD dwVerifyType, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	//
	HANDLE hFileValid = NFileUtility::BuildVerifyFile(NULL);
	if(INVALID_HANDLE_VALUE == hFileValid)
		return VERIFY_VALUE_FAILED;
	objVerifyVector.AppendResource(hFileValid);
	//
	DWORD dwVeriValue = VERIFY_VALUE_FAILED;
	switch(dwVerifyType) {
	case VERIFY_TYPE_INVALID:
		break;
	case VERIFY_TYPE_WHOLE:
		dwVeriValue = Sha1Verifier(hFileValid, sta_inde, pVerifyWorker, szFilePath);
// _LOG_DEBUG(_T("sta_inde:%d"), sta_inde);
		break;
	case VERIFY_TYPE_PORTION:
		dwVeriValue = FileVerifier(hFileValid, sta_inde, end_inde, pVerifyWorker, szFilePath);
// _LOG_DEBUG(_T("sta_inde:%d end_inde:%d"), sta_inde, end_inde);
		break;
	}
	//
	if(INVALID_HANDLE_VALUE != hFileValid) {
		objVerifyVector.EraseResource(hFileValid);
		CloseHandle( hFileValid );
		hFileValid = INVALID_HANDLE_VALUE;
	}
	//
	return dwVeriValue;
}

UINT VerifyThreadFirst(LPVOID lpParam) {
	struct VERIFY_ARGUMENT *pVerifyArgu = (struct VERIFY_ARGUMENT *)lpParam;
	if (NULL == pVerifyArgu) FAULT_RETURN; //输入参数非法
	//
	// must lock data file
	TCHAR szFilePath[MAX_PATH];
	DRIVE_LETTE(szFilePath, pVerifyArgu->pClientConfig->szDriveLetter)
	// _LOG_DEBUG(_T("build first verify worker!"));
	//
	DWORD veri_value = VERIFY_VALUE_FINISH;
	CTRANSWorker *pVerifyWorker = NVerifyUtility::CreateWorker(&veri_value, pVerifyArgu);
	DWORD dwVerifyType = VERIFY_TYPE_INVALID;
	DWORD sta_inde, end_inde;
	for(; ; ) {
		// 检查运行状态
// _LOG_DEBUG(_T("check transmit run status!")); // disable by james 20140410
		DWORD dwProcessStatus = NExecTransBzl::GetTransmitNextStatus();
		if(PROCESS_STATUS_RUNNING != dwProcessStatus) {
			CONVERT_VERIFY_STATUS(veri_value, dwProcessStatus)
_LOG_DEBUG(_T("process status is not running, so exit! veri_value:%X"), veri_value); // disable by james 20140410
			break;
		}
		//
		// 读取工作节点
// _LOG_DEBUG(_T("read task node!")); // disable by james 20140410
		int prepa_value = UploadVerify::VerifyPrepa(&dwVerifyType, &sta_inde, &end_inde);
		if(!prepa_value) {
			veri_value = VERIFY_VALUE_FINISH;
// _LOG_DEBUG(_T("read node finish!")); // disable by james 20140410
			break;
		} else if(0 > prepa_value) {
			veri_value = VERIFY_VALUE_FAILED;
// _LOG_DEBUG(_T("read node error!")); // disable by james 20140410
			break;
		}
// _LOG_DEBUG(_T("read node start:%d end:%d"), sta_inde, end_inde); // disable by james 20140410
		//
		// 创建服务器连接// _LOG_DEBUG(_T("read node error!")); // disable by james 20140410
		if(!pVerifyWorker) {
			if(!(pVerifyWorker = NVerifyUtility::CreateWorker(&veri_value, pVerifyArgu))) {
				break;
			}
// _LOG_DEBUG(_T("build first verify worker!"));
		}
		//
		// 执行效验工作
// _LOG_DEBUG(_T("transmit task node!")); // disable by james 20140410
		veri_value = UploadVerify::ListVerifier(dwVerifyType, sta_inde, end_inde, pVerifyWorker, szFilePath);
		if(VERIFY_VALUE_NETWORK & veri_value) {
			NVerifyUtility::DropWorker(pVerifyWorker);
			pVerifyWorker = NULL;
// _LOG_DEBUG(_T("drop first verify worker!"));
		}
	}
	// 销毁服务器连接
	NVerifyUtility::DestroyWorker(pVerifyWorker);
// _LOG_DEBUG(_T("destroy first verify worker!"));
	//
// _LOG_DEBUG(_T("finish worker and exit thread! veri_value:%X"), veri_value);  // disable by james 20140410
	return veri_value;
}

DWORD VerifyTasksBzl::ValidTransmitFirst(struct LocalConfig *pLocalConfig) {
	HANDLE hVerifyThreads[FIVE_VERIFY_THREAD];
	DWORD dwThreadExitValue[FIVE_VERIFY_THREAD];
	DWORD dwVerifyExitValue = VERIFY_VALUE_FINISH;
	struct VERIFY_ARGUMENT tVerifyArgu;
	DWORD thread_index;
	//
	CONVERT_SESSION_ARGU(&(tVerifyArgu.tSeionArgu), &pLocalConfig->tClientConfig);
	CONVERT_SOCKETS_ARGU(&(tVerifyArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
	SET_SOCKETS_THREADS(&(tVerifyArgu.tSockeArgu), FIVE_SOCKETS_THREAD); // verify
	tVerifyArgu.pClientConfig = &pLocalConfig->tClientConfig;
	VERIFY_OBJECT(SetVerifyThreads(hVerifyThreads, FIVE_VERIFY_THREAD));
	//
	// 启动线程
	// _LOG_DEBUG(_T("启动线程")); // disable by james 20140410
	objTaskList.SetHandNumbe(0);
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
		CWinThread *pWinThread = AfxBeginThread(VerifyThreadFirst, (LPVOID)&tVerifyArgu, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		pWinThread->m_bAutoDelete = FALSE;
		hVerifyThreads[thread_index] = pWinThread->m_hThread;
		pWinThread->ResumeThread();
	}
	//
	// 等待结束
	// _LOG_DEBUG(_T("等待结束")); // disable by james 20140410
	WaitForMultipleObjects(FIVE_VERIFY_THREAD, hVerifyThreads, TRUE, INFINITE);
	//
	// 得到退出码
	// _LOG_DEBUG(_T("得到工作线程退出码")); // disable by james 20140410
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
		if(!GetExitCodeThread(hVerifyThreads[thread_index], &dwThreadExitValue[thread_index])) {
			_LOG_WARN(_T("Get ThreadExitValue[%d] error! GetLastError:%08x"), thread_index, GetLastError());
		}
		CloseHandle( hVerifyThreads[thread_index] );
	}
	//
	// 处理退出码
	// _LOG_DEBUG(_T("处理工作线程退出码")); // disable by james 20140410
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
// _LOG_DEBUG(_T("------------------ thread:%d exit code:%X"), thread_index, dwThreadExitValue[thread_index]); // disable by james 20140410
		if(VERIFY_VALUE_FINISH != dwThreadExitValue[thread_index]) {
			dwVerifyExitValue = dwThreadExitValue[thread_index];
		}
	}
	//
	// delete malloc resouce
	VERIFY_OBJECT(SetVerifyThreads(NULL, 0));
_LOG_DEBUG(_T("+++++++++++++++++++++++++ verify exit value:%X"), dwVerifyExitValue); // disable by james 20140410
	return dwVerifyExitValue;
}
//

int UploadSecond::VerifyPrepa(DWORD *dwVerifyType, DWORD *sta_inde, DWORD *end_inde) {
	DWORD node_inde;
	int read_value;
	struct TaskNode task_node;
	int iVerifyFound = 0;
	int prepa_value = TNODE_PREPA_FAULT;
	//
	gcsVerifyPrepa.Lock();
	for(; ; ) {
		node_inde = objTaskList.GetHandNodeAdd();
		read_value = objTaskList.ReadNodeEx(&task_node, node_inde);
		if(!read_value) {
			prepa_value = FINISH_VERIFY;
			break;
		} else if(0 > read_value) {
			prepa_value = TNODE_PREPA_FAULT;
			break;
		}
		//
		if(((STYPE_PUT|STYPE_GSYNC|STYPE_MOVE|STYPE_COPY) & task_node.control_code)) {
			if(FILE_TASK_BEGIN & task_node.control_code) {
				prepa_value = TNODE_PREPA_OKAY;
				*sta_inde = node_inde;
			}
			//
			if(!ONLY_TRANSMIT_OKAY(task_node.success_flags) && ONLY_VERIFY_OKAY(task_node.success_flags))
				iVerifyFound = 1;
			//
			if(FILE_TASK_END & task_node.control_code) {
				*end_inde = node_inde;
				if((STYPE_PUT|STYPE_GSYNC) & task_node.control_code)
					*dwVerifyType = VERIFY_TYPE_PORTION;
				else if((STYPE_MOVE|STYPE_COPY) & task_node.control_code)
					*dwVerifyType = VERIFY_TYPE_WHOLE;
				//
				if(iVerifyFound) break;
				prepa_value = TNODE_PREPA_FAULT;
			}
// ndp::TaskNode(task_node); // disable by james 20130415
		}
	}
	gcsVerifyPrepa.Unlock();
	//
	return prepa_value;
}

DWORD UploadSecond::Sha1Verifier(HANDLE hFileValid, DWORD node_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	struct TaskNode task_node;
	DWORD veri_value = VERIFY_VALUE_FINISH;
	//
	NFileUtility::TruncateHandle(hFileValid, 0x00);
	if(0 < objTaskList.ReadNodeEx(&task_node, node_inde)) {
		veri_value = NVerifyUtility::FileSha1Request(hFileValid, pVerifyWorker, &task_node, VERIFY_CACHE);
		if(VERIFY_VALUE_NETWORK & veri_value) 
			return VERIFY_VALUE_FAILED;
_LOG_DEBUG(_T("file sha1 req:%08x"), veri_value);
	}
	DWORD dwResult = SetFilePointer(hFileValid, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		return VERIFY_VALUE_FAILED;
	}
	//
	DWORD dwFileFinish = ERR_SUCCESS;
	if(dwFileFinish = NVerifyUtility::Sha1ValidTransmit(&task_node, hFileValid, 
		((VERIFY_VALUE_FINISH|VERIFY_VALUE_NCONTE)&veri_value))) {
_LOG_DEBUG(_T("set transmit verify excep, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
		SET_TRANSMIT_VERIFY_EXCEPTION(task_node.success_flags);
		SET_DOWND_EXP(task_node.excepti);
		veri_value = VERIFY_VALUE_EXCEPTION;
	} else {
_LOG_DEBUG(_T("set transmit verify done, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
		SET_VERIFY_DONE(task_node.success_flags);
	}
	objTaskList.WriteNodeEx(&task_node, task_node.node_inde);
	//
	if(!dwFileFinish) {
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName2);
		LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
_LOG_DEBUG(_T("upv success :%s :%08x"), szFilePath, task_node.success_flags);
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
		LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
_LOG_DEBUG(_T("sha1 success :%s :%08x"), szFilePath, task_node.success_flags);
	}
	//
	return veri_value;
}

DWORD UploadSecond::ChunkVerifier(HANDLE hFileValid, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	struct TaskNode task_node;
	DWORD veri_value = VERIFY_VALUE_FINISH;
	//
	// _LOG_DEBUG(_T("verify transmit tasks.")); // disable by james 20140410
	DWORD dwFileFinish = ERR_SUCCESS;
	DWORD node_inde;
	for(node_inde = sta_inde; (node_inde<=end_inde) && (0<objTaskList.ReadNodeEx(&task_node, node_inde)); node_inde++) {
		// _LOG_DEBUG(_T("-------------------------------------------------")); // disable by james 20140410
		// _LOG_DEBUG(_T("node_inde:%d task_node.success_flags:%X"), node_inde, task_node.success_flags); // disable by james 20140410
		if(!ONLY_TRANSMIT_OKAY(task_node.success_flags) && ONLY_VERIFY_OKAY(task_node.success_flags)) {
			NFileUtility::TruncateHandle(hFileValid, 0x00);
			veri_value = NVerifyUtility::ChunkChksRequest(hFileValid, pVerifyWorker, &task_node, VERIFY_CACHE);
			if(VERIFY_VALUE_NETWORK & veri_value) break;
			//
			DWORD dwFileValid = NVerifyUtility::ChunkValidTransmit(&task_node, hFileValid);
			if(dwFileValid) {
_LOG_DEBUG(_T("set transmit verify excep, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
				SET_TRANSMIT_VERIFY_EXCEPTION(task_node.success_flags);
				SET_DOWND_EXP(task_node.excepti);
// _LOG_DEBUG(_T("verify chunk, exception. task_node.success_flags:%X"), task_node.success_flags); // disable by james 20140410
// _LOG_DEBUG(_T("node_inde:%d task_node.success_flags:%X"), node_inde, task_node.success_flags);
				veri_value = VERIFY_VALUE_EXCEPTION;
			} else {
_LOG_DEBUG(_T("set transmit verify done, szFileName1:%s node_inde:%d"), task_node.szFileName1, task_node.node_inde);
				SET_VERIFY_DONE(task_node.success_flags);
			}
			dwFileFinish |= dwFileValid;
			objTaskList.WriteNodeEx(&task_node, task_node.node_inde);
		}
		//
		if((FILE_TASK_END&task_node.control_code) && !dwFileFinish) {
			_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, task_node.szFileName1);
			RiverFS::FileUpdate(szFilePath, &task_node.last_write);
			LINKER_DISPLAY_SUCCESS(szFilePath, task_node.isdire);
_LOG_DEBUG(_T("chunk success :%s :%08x"), szFilePath, task_node.success_flags);
		}
		//
	}
	// _LOG_DEBUG(_T("verify over close handle. veri_value:%X"), veri_value); // disable by james 20140410
	return veri_value;
}

DWORD UploadSecond::ListVerifier(DWORD dwVerifyType, DWORD sta_inde, DWORD end_inde, CTRANSWorker *pVerifyWorker, TCHAR *szFilePath) {
	//
	HANDLE hFileValid = NFileUtility::BuildVerifyFile(NULL);
	if(INVALID_HANDLE_VALUE == hFileValid)
		return VERIFY_VALUE_FAILED;
	objVerifyVector.AppendResource(hFileValid);
	//
	DWORD dwVeriValue = VERIFY_VALUE_FAILED;
	switch(dwVerifyType) {
	case VERIFY_TYPE_INVALID:
		break;
	case VERIFY_TYPE_WHOLE:
		dwVeriValue = Sha1Verifier(hFileValid, sta_inde, pVerifyWorker, szFilePath);
		break;
	case VERIFY_TYPE_PORTION:
		dwVeriValue = ChunkVerifier(hFileValid, sta_inde, end_inde, pVerifyWorker, szFilePath);
		break;
	}
	//
	if(INVALID_HANDLE_VALUE != hFileValid) {
		objVerifyVector.EraseResource(hFileValid);
		CloseHandle( hFileValid );
		hFileValid = INVALID_HANDLE_VALUE;
	}
	//
	return dwVeriValue;
}

UINT VerifyThreadSecond(LPVOID lpParam) {
	struct VERIFY_ARGUMENT *pVerifyArgu = (struct VERIFY_ARGUMENT *)lpParam;
	if (NULL == pVerifyArgu) FAULT_RETURN; //输入参数非法
	//
	TCHAR szFilePath[MAX_PATH];
	DRIVE_LETTE(szFilePath, pVerifyArgu->pClientConfig->szDriveLetter)
	//
	DWORD veri_value = VERIFY_VALUE_FINISH;
	CTRANSWorker *pVerifyWorker = NVerifyUtility::CreateWorker(&veri_value, pVerifyArgu);
	DWORD dwVerifyType = VERIFY_TYPE_INVALID;
	DWORD sta_inde, end_inde;
	for(; ; ) {
		// 检查运行状态
// _LOG_DEBUG(_T("check transmit run status!")); // disable by james 20140410
		DWORD dwProcessStatus = NExecTransBzl::GetTransmitNextStatus();
		if(PROCESS_STATUS_RUNNING != dwProcessStatus) {
			CONVERT_VERIFY_STATUS(veri_value, dwProcessStatus)
_LOG_DEBUG(_T("process status is not running, so exit! veri_value:%X"), veri_value); // disable by james 20140410
			break;
		}
		//
		// 读取工作节点
		// _LOG_DEBUG(_T("read task node! index:%d"), thread_index); // disable by james 20140410
		int prepa_value = UploadSecond::VerifyPrepa(&dwVerifyType, &sta_inde, &end_inde);
		if(!prepa_value) {
			veri_value = VERIFY_VALUE_FINISH;
			// _LOG_DEBUG(_T("read node finish!")); // disable by james 20140410
			break;
		} else if(0 > prepa_value) {
			veri_value = VERIFY_VALUE_FAILED;
			// _LOG_DEBUG(_T("read node error!")); // disable by james 20140410
			break;
		}
		// _LOG_DEBUG(_T("read node start:%d end:%d"), sta_inde, end_inde); // disable by james 20140410
		//
		// 创建服务器连接// _LOG_DEBUG(_T("read node error!")); // disable by james 20140410
		if(!pVerifyWorker) {
			if(!(pVerifyWorker = NVerifyUtility::CreateWorker(&veri_value, pVerifyArgu))) {
				break;
			}
		}
		//
		// 执行效验工作// _LOG_DEBUG(_T("transmit task node!")); // disable by james 20140410
		veri_value = UploadSecond::ListVerifier(dwVerifyType, sta_inde, end_inde, pVerifyWorker, szFilePath);
		if(VERIFY_VALUE_NETWORK & veri_value) {
			NVerifyUtility::DropWorker(pVerifyWorker);
			pVerifyWorker = NULL;
		}
	}
	// 销毁服务器连接
	NVerifyUtility::DestroyWorker(pVerifyWorker);
	// _LOG_DEBUG(_T("finish worker and exit thread! veri_value:%X"), veri_value);  // disable by james 20140410
	//
	return veri_value;
}

DWORD VerifyTasksBzl::ValidTransmitSecond(struct LocalConfig *pLocalConfig) {
	HANDLE hVerifyThreads[FIVE_VERIFY_THREAD];
	DWORD dwThreadExitValue[FIVE_VERIFY_THREAD];
	DWORD dwVerifyExitValue = VERIFY_VALUE_FINISH;
	struct VERIFY_ARGUMENT tVerifyArgu;
	DWORD thread_index;
	//
	CONVERT_SESSION_ARGU(&(tVerifyArgu.tSeionArgu), &pLocalConfig->tClientConfig);
	CONVERT_SOCKETS_ARGU(&(tVerifyArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
	SET_SOCKETS_THREADS(&(tVerifyArgu.tSockeArgu), FIVE_SOCKETS_THREAD); // verify
	tVerifyArgu.pClientConfig = &pLocalConfig->tClientConfig;
	VERIFY_OBJECT(SetVerifyThreads(hVerifyThreads, FIVE_VERIFY_THREAD));
	//
	// 启动线程
	// _LOG_DEBUG(_T("启动线程")); // disable by james 20140410
	objTaskList.SetHandNumbe(0);
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
		CWinThread *pWinThread = AfxBeginThread(VerifyThreadSecond, (LPVOID)&tVerifyArgu, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		pWinThread->m_bAutoDelete = FALSE;
		hVerifyThreads[thread_index] = pWinThread->m_hThread;
		pWinThread->ResumeThread();
	}
	//
	// 等待结束
	// _LOG_DEBUG(_T("等待结束")); // disable by james 20140410
	WaitForMultipleObjects(FIVE_VERIFY_THREAD, hVerifyThreads, TRUE, INFINITE);
	//
	// 得到退出码
	// _LOG_DEBUG(_T("得到工作线程退出码")); // disable by james 20140410
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
		if(!GetExitCodeThread(hVerifyThreads[thread_index], &dwThreadExitValue[thread_index])) {
			_LOG_WARN(_T("Get ThreadExitValue[%d] error! GetLastError:%08x"), thread_index, GetLastError());
		}
		CloseHandle( hVerifyThreads[thread_index] );
	}
	//
	// 处理退出码
	// _LOG_DEBUG(_T("处理工作线程退出码")); // disable by james 20140410
	for(thread_index = 0x00; thread_index < FIVE_VERIFY_THREAD; ++thread_index) {
// _LOG_DEBUG(_T("------------------ thread:%d exit code:%X"), thread_index, dwThreadExitValue[thread_index]); // disable by james 20140410
		if(VERIFY_VALUE_FINISH != dwThreadExitValue[thread_index]) {
			dwVerifyExitValue = dwThreadExitValue[thread_index];
		}
	}
	//
	// delete malloc resouce
	VERIFY_OBJECT(SetVerifyThreads(NULL, 0));
_LOG_DEBUG(_T("+++++++++++++++++++++++++ verify exit value:%X"), dwVerifyExitValue); // disable by james 20140410
	return dwVerifyExitValue;
}