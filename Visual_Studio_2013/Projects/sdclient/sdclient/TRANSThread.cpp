#include "StdAfx.h"

#include "sdclient.h"
#include "LocalBzl.h"
#include "AnchorBzl.h"
#include "clientcom/clientcom.h"
#include "ExecuteUtility.h"
#include "LkAiUtili.h"
#include "ExecuteTransmitBzl.h"

#include "TRANSThread.h"

CTRANSThread objTRANSThread;

CTRANSThread::CTRANSThread(void) {
    memset(&m_tTransArgu, 0, sizeof(struct TRANSMIT_ARGUMENT));
}

CTRANSThread::~CTRANSThread(void) {
}

UINT TRANSThreadProcess(LPVOID lpParam);
DWORD CTRANSThread::BeginTRANSThread() {
    InterlockedExchange((LONG*)&m_tTransArgu.bProcessStarted, TRUE);
    // THREAD_PRIORITY_BELOW_NORMAL
    CWinThread *pWinThread = AfxBeginThread(TRANSThreadProcess, (LPVOID)&m_tTransArgu, THREAD_PRIORITY_BELOW_NORMAL);
    NExecTransBzl::SetTRANSThread(pWinThread);
//
    return 0x01;
}

DWORD CTRANSThread::Initialize(struct LocalConfig *pLocalConfig) {
    if(!pLocalConfig) return ((DWORD)-1);
//
    m_tTransArgu.pLocalConfig = pLocalConfig;
    m_tTransArgu.pClientConfig = &pLocalConfig->tClientConfig;
    // m_tTransArgu.pTRANSSeion = pTRANSSeion;
//
    return 0x00;
}

DWORD CTRANSThread::Finalize() {
    InterlockedExchange((LONG*)&m_tTransArgu.bProcessStarted, FALSE);
    // while(WAIT_OBJECT_0 != WaitForSingleObject(NExecTransBzl::GetTRANSThread()->m_hThread, 3000)) // wait five seconds
    LONG lProcessExit;
    InterlockedExchange(&lProcessExit, m_tTransArgu.bProcessStarted);
_LOG_DEBUG(_T("finalize! set event"));
    NExecTransBzl::SetWaitingEvent(); // 触发使同步线程退出 add by james 20130402
    while(!lProcessExit && (WAIT_OBJECT_0 != WaitForSingleObject(NExecTransBzl::GetTRANSThread()->m_hThread, 3000))) {
        _LOG_INFO(_T("wait trans thread exit!"));
        Sleep(3000);
        InterlockedExchange(&lProcessExit, m_tTransArgu.bProcessStarted);
    }
//
    return 0x00;
}

//
UINT TRANSThreadProcess(LPVOID lpParam) {
    struct TRANSMIT_ARGUMENT *lpObject = (struct TRANSMIT_ARGUMENT *)lpParam;
    if (NULL == lpObject) ((DWORD)-1); //输入参数非法
//
    struct LocalConfig *pLocalConfig = lpObject->pLocalConfig;
    struct ClientConfig *pClientConfig = lpObject->pClientConfig;
//
	// do not create sdclent attrib folder
    // NFileUtility::CreatAttribFolder(pClientConfig->szDriveLetter);
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_WAITING);
    NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_WAITING);
    // NLocalBzl::TrayiconConnected(); // for test;
    //
	DWORD dwPrevioOperati = 0x00;
    // Enter a loop reading data
    int process_waiting_time = PROCESS_INITIAL_WAITING;
    LONG lProcessContinue;
    InterlockedExchange(&lProcessContinue, lpObject->bProcessStarted);
    while(lProcessContinue) { // 线程成功启动
        // 循环等待事件发生或超时.
        // WaitForSingleObject(hEvent, INFINITE);
        DWORD wait_object_status = WaitForSingleObject(NExecTransBzl::GetWaitingEvent(), process_waiting_time);
_LOG_DEBUG(_T("+++++++++++++++++++++++++ wait_object_status:%08x"), wait_object_status); // disable by james 20140410
        switch(wait_object_status) {
        case WAIT_TIMEOUT:
            // The process terminated.
            break;
        case WAIT_OBJECT_0:
            // the process did not terminate within 5000 milliseconds.
            break;
        case WAIT_FAILED:
            // Bad call to function (invalid handle?)
            return ((DWORD)-1);
            break;
        }
        //
        // 检查界面的互斥操作,检查同步进程状态
_LOG_DEBUG(_T("check exclusive!")); //
		DWORD dwPassExclusive = NLocalBzl::ExclusOptionsEnable();
		if(NLkAiUtili::ValidateProcessFinished() || !dwPassExclusive) {
			if(!dwPassExclusive && (WAIT_OBJECT_0==wait_object_status)) {
				dwPrevioOperati = 0x01;
				NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, _T("有互斥操作,暂停同步."));
			}
_LOG_DEBUG(_T("validate process finished! dwPassExclusive:%08X dwPrevioOperati:%08X"), dwPassExclusive, dwPrevioOperati);
			process_waiting_time = PROCESS_FAILED_WAITING;
			goto FINALIZE;
		}
		//
        // 本地有修改,刷新图标延时等待操作完成
_LOG_DEBUG(_T("prepare locked action! dwPrevioOperati:%08x"), dwPrevioOperati); //
		if((WAIT_OBJECT_0==wait_object_status) || dwPrevioOperati) {
			NLocalBzl::TrayiconTransmit(RUN_OPERATE_DELAY_STEP); // add by james 20110304
			// >0:finish 0:nothing <0:exception
			int lock_value = NExecTransBzl::PrepareLockedAction(pClientConfig->szDriveLetter, pClientConfig->szLocation);
_LOG_DEBUG(_T("lock_value:%d"), lock_value); // disable by james 20140410
			if(!lock_value) {
				NLocalBzl::TrayiconTransmit(RUN_INVALID_CHANGE_STEP); // add by james 20110304
				dwPrevioOperati = 0x00;
				goto FINALIZE; // Todo
			} else if(0 > lock_value) {
				dwPrevioOperati = 0x01;
				goto FINALIZE; // Todo	
			} else dwPrevioOperati = 0x00;
		}
        //
        // 检查连网状态
_LOG_DEBUG(_T("valid connect status!")); //
        DWORD connect_status = NExecTransBzl::ValidConnectStatus(pLocalConfig, pClientConfig->szAccountLinked);
_LOG_DEBUG(_T("connect_status:%08X"), connect_status); // disable by james 20140410
        switch(connect_status) {
        case CONNECT_STATUS_OKAY:
            NLocalBzl::TrayiconConnected();
            process_waiting_time = PROCESS_NORMAL_WAITING;
            break;
        case CONNECT_STATUS_NOACCOUNT:
            NLocalBzl::TrayiconConnecting(CONNECT_NOACCOUNT_STEP);
            process_waiting_time = CONNECT_ERROR_WAITING;
            break;
        case CONNECT_STATUS_OFFLINK:
            NLocalBzl::TrayiconConnecting(CONNECT_OFFLINK_STEP);
            process_waiting_time = CONNECT_ERROR_WAITING;
            break;
        case CONNECT_STATUS_FAULT:
        case CONNECT_STATUS_DISCONN:
            NLocalBzl::TrayiconConnecting(CONNECT_DISCONNECT_STEP);
            process_waiting_time = CONNECT_ERROR_WAITING;
            break;
		default:
			process_waiting_time = PROCESS_NORMAL_WAITING;
			break;
        }
        if(CONNECT_STATUS_OKAY != connect_status) goto FINALIZE;
        //
        // 检查是否需要同步
_LOG_DEBUG(_T("check pool changed!")); //
        DWORD changed_status = NExecTransBzl::CheckUserPoolChanged(pLocalConfig);
_LOG_DEBUG(_T("changed_status:%08X"), changed_status); // disable by james 20140410
        switch(changed_status) {
        case CHANGE_STATUS_LOCKED: // add by james 20110304
			if(NAnchorBzl::CheckActiveAction())
				NLocalBzl::TrayiconTransmit(RUN_OPERATI_CHECK_STEP);
			else NLocalBzl::TrayiconTransmit(RUN_TIMEOUT_CHECK_STEP);
            break;
        case CHANGE_STATUS_FAULT:
        case CHANGE_STATUS_DISCONNECT:
            NLocalBzl::TrayiconConnecting(CONNECT_DISCONNECT_STEP);
            break;
        case CHANGE_STATUS_CHANGED:
            break;
        case CHANGE_STATUS_NOCHANGE:
            NLocalBzl::TransmitSuccessfully(NOCHANGE_SUCCESS);
            break;
        }
        process_waiting_time = PROCESS_NORMAL_WAITING;
        if(CHANGE_STATUS_CHANGED != changed_status) goto FINALIZE;
        //
        // begain to syncing
// DWORD before = GetTickCount();
_LOG_DEBUG(_T("perfrom synchron transport!")); //
        DWORD syncing_status = ((DWORD)-1);
        if (NExecTransBzl::GetConnectedStatus()) {
            NLocalBzl::StartTransmitProcess(); // 设置同步线程状态
            // 执行同步
            syncing_status = NExecTransBzl::PerfromSynchronTransport(pLocalConfig); // result 0 succ, 1 wait, -1 error
_LOG_DEBUG(_T("syncing_status:%08X"), syncing_status); // disable by james 20140410
// _LOG_DEBUG(_T("run time:%d"), GetTickCount() - before); //
            switch(syncing_status) {
            case WORKER_VALUE_FINISH:
                NLocalBzl::TransmitSuccessfully(CHANGED_SUCCESS);
                break;
            case WORKER_VALUE_PAUSED:
                NLocalBzl::PauseTransmitProcess();
                break;
            case WORKER_VALUE_FORCESTOP:
                NLocalBzl::StopedTransmitProcess();
                break;
            case WORKER_VALUE_KILLED:
                NLocalBzl::KilledTransmitProcess();
                break;
            case WORKER_VALUE_LOCKED: // add by james 201121010
                VOID NLocalBzl::TransmitLockedProcess();
                break;
            case WORKER_VALUE_FAULT:
				NAnchorBzl::InserAnchorIcon(pLocalConfig->tClientConfig.szDriveLetter);
                NLocalBzl::TrayiconConnecting(CONNECT_SERVERBUSY_STEP);
                NLocalBzl::ErrorTransmitProcess();
                process_waiting_time = CONNECT_ERROR_WAITING;
                break;
			default: // WORKER_VALUE_FAILED WORKER_VALUE_NETWORK WORKER_VALUE_EXCEPTION
				NAnchorBzl::InserAnchorIcon(pLocalConfig->tClientConfig.szDriveLetter);
                NLocalBzl::FailedTransmitProcess();
                process_waiting_time = PROCESS_FAILED_WAITING;
				break;
            }
            // 转换同步线程状态
        }
        //
FINALIZE:
        InterlockedExchange(&lProcessContinue, lpObject->bProcessStarted);
		NLocalBzl::ExclusOptionsDisable(dwPassExclusive);
    }
	// add by james 20151030
	NExecTransBzl::PrepareLockedAction(pClientConfig->szDriveLetter, pClientConfig->szLocation);
    InterlockedExchange(&lpObject->bProcessStarted, TRUE);
    //
_LOG_DEBUG(_T("trans thread process exit!"));
    return 0;
}
