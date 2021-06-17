#include "StdAfx.h"

#include "clientcom/clientcom.h"
#include "third_party.h"

#include "LocalBzl.h"
#include "DebugPrinter.h"
// #include "TaskListObj.h"
#include "NodeUtility.h"

//
DWORD NodeUtility::CheckChanValue() {
	struct TaskNode tReadNode;
	DWORD dwChanValue = 0x00;
//
	objTaskList.SetNodePoint(0);
	while(0 < objTaskList.ReadNode(&tReadNode)) {
		if(!ONLY_BREAK_WAIVE_OKAY(tReadNode.success_flags)) {
			dwChanValue |= HAVE_TASK_VALU;
			if(LOCATION_SERVER & TNODE_LOCATION_BYTE(tReadNode.control_code)) {
ndp::TaskNode(&tReadNode);
_LOG_DEBUG(_T("tReadNode node_inde:%d control_code:%08x tReadNode.success_flags:%08x"), tReadNode.node_inde, tReadNode.control_code, tReadNode.success_flags); // disable by james 20140410
				dwChanValue |= SERV_CHAN_VALU;
				break;
			}
		}
	}
	return dwChanValue;
}

//
DWORD NodeUtility::CheckVerifyFinished() {
	struct TaskNode tReadNode;
	DWORD dwVerifyFinished = 0x00;
	objTaskList.SetNodePoint(0);
	while(0 < objTaskList.ReadNode(&tReadNode)) {
// _LOG_DEBUG(_T("r tReadNode node_inde:%d success_flags:%08x"), tReadNode.node_inde, tReadNode.success_flags);
		if((STYPE_GET|STYPE_PUT|STYPE_PSYNC|STYPE_GSYNC|STYPE_MOVE|STYPE_COPY)&tReadNode.control_code
			&& !ONLY_BREAK_WAIVE_OKAY(tReadNode.success_flags) && ONLY_VERIFY_UNDONE(tReadNode.success_flags)) {
// _LOG_DEBUG(_T("b tReadNode node_inde:%d success_flags:%08x"), tReadNode.node_inde, tReadNode.success_flags); // disable by james 20140410
			dwVerifyFinished = 0x01;
			break;
		}
	}
	return dwVerifyFinished;
}

//
DWORD NodeUtility::CheckUploadVerify() { // 0:finished >0:have verify
	struct TaskNode tReadNode;
	DWORD dwVerifyFound = 0x00;
	objTaskList.SetNodePoint(0);
	while(0 < objTaskList.ReadNode(&tReadNode)) {
// _LOG_DEBUG(_T("tReadNode node_inde:%d success_flags:%08x"), tReadNode.node_inde, tReadNode.success_flags);
		if(LOCATION_SERVER&TNODE_LOCATION_BYTE(tReadNode.control_code) &&
			((STYPE_PUT|STYPE_GSYNC|STYPE_MOVE|STYPE_COPY)&tReadNode.control_code)) {
			if(!ONLY_BREAK_WAIVE_OKAY(tReadNode.success_flags) && !ONLY_TRANSMIT_OKAY(tReadNode.success_flags) 
				&& ONLY_VERIFY_OKAY(tReadNode.success_flags)) {
				dwVerifyFound = 0x01;
				break;
			}
// ndp::TaskNode(task_node); // disable by james 20130415
		}
	}
	return dwVerifyFound;
}

//
DWORD NodeUtility::ValidTasksFinished() {
	struct TaskNode tReadNode;
	DWORD dwTasksFinished = 0x00;
	objTaskList.SetNodePoint(0);
	while(0 < objTaskList.ReadNode(&tReadNode)) {
		if(ONLY_TASK_UNDONE(tReadNode.success_flags)) {
_LOG_DEBUG(_T("check tasks finished.tReadNode node_inde:%d success_flags:%08x"), tReadNode.node_inde, tReadNode.success_flags); // disable by james 20140410
_LOG_DEBUG(_T("tReadNode szFileName1:%s control_code:%08x excepti:%08x"), tReadNode.szFileName1, tReadNode.control_code, tReadNode.excepti);
			dwTasksFinished = 0x01;
			break;
		}
	}
	return dwTasksFinished;
}

//
#define SHOW_LOCKED_NUM	 30 // 8K/MAX_PATH
static DWORD CheckExceptDisplay(TCHAR *szDispText, TCHAR *szDriveLetter) { // 0x00: finish ((DWORD)-1):exception
	struct TaskNode tReadNode;
	TCHAR szFileName[MAX_PATH];
	//
	DWORD dwFileFinish = 0x00;
	DWORD dwExcepType = 0x00;
	DWORD dwExceptValue = 0x00;
	//
	MKZEO(szDispText);
	DRIVE_LETTE(szFileName, szDriveLetter)
	objTaskList.SetNodePoint(0);
	while((SHOW_LOCKED_NUM>dwExceptValue) && (0<objTaskList.ReadNode(&tReadNode))) {
		if(FILE_TASK_BEGIN & tReadNode.control_code)  {
			dwExcepType = 0x00;
			dwFileFinish = 0x00;
		}
		//
		if((TRANSMIT_EXCEPTION|OWRTMRVE_EXCEPTION) & tReadNode.success_flags) {
			dwExcepType |= tReadNode.excepti;
			dwFileFinish = 0x01;
		}
		//
		if((FILE_TASK_END&tReadNode.control_code) && dwFileFinish){
			if(FILE_LOCKED_EXP & dwExcepType) {
				_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, tReadNode.szFileName1);
				if(NFileUtility::IsFileLocked(szFileName)) {
					dwExceptValue++;
					_tcscat_s(szDispText, MAX_PATH, szFileName);
					_tcscat_s(szDispText, MAX_PATH, _T("\r\n"));
				}
			}
_LOG_DEBUG(_T("except display tReadNode node_inde:%d success_flags:%08x dwExcepType:%08x"),
		   tReadNode.node_inde,
		   tReadNode.success_flags, 
		   dwExcepType);
		}
	}
	//
	return dwExceptValue;
}

CTRANSWorker *NodeUtility::CreateWorker(struct NODE_ARGUMENT *pNodeArgu) {
    CTRANSWorker *pNodeWorker = NTRANSWorker::Factory(&pNodeArgu->tSeionArgu, &pNodeArgu->tSockeArgu);
    if (!pNodeWorker) {
        NTRANSWorker::DestroyObject(pNodeWorker);
        return NULL;
    }
    // 生成工作记录,初始会话
    if(pNodeWorker->Initial()) {
        NTRANSWorker::DestroyObject(pNodeWorker);
        pNodeWorker = NULL;
    }
    return pNodeWorker;
}

void NodeUtility::DestroyWorker(CTRANSWorker *pNodeWorker, DWORD dwFinalType) {
    if(pNodeWorker) {
        pNodeWorker->Final(dwFinalType);
        NTRANSWorker::DestroyObject(pNodeWorker);
    }
}

//
static VOID FileLockedTip(TCHAR *szFileName) {
_LOG_WARN(_T("sync exception, file locked:%s"), szFileName);
	TCHAR szMessage[DEFAULT_BUFFER];
	_stprintf_s(szMessage, _T("以下文件同步失败,可能被锁定,请关闭!\r\n%s"), szFileName);
	NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, szMessage);
}

#define KEEP_ALIVE_TIMES	12 // 12
#define DELAY_EXCEPTION_TIME	16000 // 16s
VOID NodeUtility::DelayExceptionWarning(struct LocalConfig *pLocalConfig) {
	struct NODE_ARGUMENT tNodeArgu;
	CONVERT_SESSION_ARGU(&(tNodeArgu.tSeionArgu), &pLocalConfig->tClientConfig);
	CONVERT_SOCKETS_ARGU(&(tNodeArgu.tSockeArgu), &pLocalConfig->tNetworkConfig);
	TCHAR szDispText[DEFAULT_BUFFER];
	//
	DWORD dwKeepTime = 0x00;
	CTRANSWorker *pNodeWorker = NodeUtility::CreateWorker(&tNodeArgu);
	if(pNodeWorker) {
		while(KEEP_ALIVE_TIMES > dwKeepTime++) {
			if(POW2N_MOD(dwKeepTime, 2)) {
				if(!CheckExceptDisplay(szDispText, pLocalConfig->tClientConfig.szDriveLetter))
					break;
				else FileLockedTip(szDispText);
			}
			//
			Sleep(DELAY_EXCEPTION_TIME);
			pNodeWorker->KeepAlive();
		}
	}
	NodeUtility::DestroyWorker(pNodeWorker, FINAL_STATUS_END);
	//
}
