#include "StdAfx.h"

// #include "client_macro.h"
#include "DebugPrinter.h"
#include "ShellLinkBzl.h"

#include "./RiverFS/RiverFS.h"
#include "VerifyUtility.h"
#include "BuildTasksBzl.h"
#include "TRANSWorker.h"
#include "LockedAction.h"
#include "ExecuteUtility.h"
#include "LkAiUtili.h"
#include "PrepaCache.h"
#include "LockAnchor.h"
#include "FinishUtility.h"
#include "ListUtility.h"
#include "ProduceObj.h"
#include "WatcherBzl.h"
#include "LocalBzl.h"

#include "NodeUtility.h"
#include "ExecuteTransmitBzl.h"

//
#define ACTION_CACEHE_EXIST		0x0001
#define LOCKED_ACTION_EXIST		0x0002
int NExecTransBzl::LockedActionInitial(const TCHAR *szDriveLetter, const TCHAR *szLocation, DWORD dwActioExist) {  // >0:finish 0:nothing <0:exception
	HANDLE hLockActio = objLockActio.LockActioIniti();
	if(INVALID_HANDLE_VALUE == hLockActio) return -1;
_LOG_DEBUG(_T("in prepa locked action! initial lock action file ok!")); // disable by james 20140410
	//
	int lock_value = 0;
	if(ACTION_CACEHE_EXIST & dwActioExist) {
		lock_value = NPrepaCache::PrepaActionCache(hLockActio, szDriveLetter, szLocation); // >0:finish 0:nothing <0:exception
		// ����û��޸��ļ�������
		NWatcherBzl::ClearActioCache();
		_LOG_DEBUG(_T("prepa locked action! lock value:%d"), lock_value); // disable by james 20140410
	}
	//
_LOG_DEBUG(_T("prepa lock action ok! lock value:%d"), lock_value); // disable by james 20140410
	if((0<lock_value) || (LOCKED_ACTION_EXIST&dwActioExist)) {
		// �õ������ݲ��뱾��êϵͳ
		if(NLkAiUtili::InitActionConve(szDriveLetter, szLocation)) {
			// �Ż�����êϵͳ����
			lock_value = NAnchorBzl::OptimizeCached(szDriveLetter); // >0:finish 0:nothing
_LOG_DEBUG(_T("convert lock action ok! lock value:%d"), lock_value); // disable by james 20140410
		} else lock_value = -1;
	}
	//
	// ���action��������
	objLockActio.FinalLockActio(BOOL(0<=lock_value));
_LOG_DEBUG(_T("lock_value:%d"), lock_value); // disable by james 20140410
_LOG_DEBUG(_T("finish lock action ok!")); // disable by james 20140410
	//
	return lock_value;
}

DWORD NExecTransBzl::Initialize(struct LocalConfig *pLocalConfig) {
	if (!pLocalConfig) return ((DWORD)-1);
	objExcepActio.Initialize(pLocalConfig->tClientConfig.szLocation);
	objLockActio.Initialize(pLocalConfig->tClientConfig.szLocation);
	//
	DWORD dwActioExist = 0x00;
	if(NWatcherBzl::IsActioCacheExist()) dwActioExist |= ACTION_CACEHE_EXIST;
	if(objLockActio.IsLockActioExist()) dwActioExist |= LOCKED_ACTION_EXIST;
	if(dwActioExist) LockedActionInitial(pLocalConfig->tClientConfig.szDriveLetter, 
		pLocalConfig->tClientConfig.szLocation, dwActioExist);
	// Initialise objTRANSObject
	objTRANSObject.Initialize();
	// Start the sync thread
	objTRANSThread.Initialize(pLocalConfig);
	if(!objTRANSThread.BeginTRANSThread()) return ((DWORD)-1);	// test 20110121
	//
	return 0x00;
}

DWORD NExecTransBzl::Finalize() {
	objTRANSThread.Finalize();
	//
	objTRANSObject.Finalize();
	//
	objLockActio.Finalize();
	//
	objExcepActio.Finalize();
	//
	return 0x00;
}

CWinThread *NExecTransBzl::GetTRANSThread() {
	return TRANS_OBJECT(m_pTRANSThread);
}

DWORD NExecTransBzl::SetTRANSThread(CWinThread *pTRANSThread) {
	TRANS_OBJECT(m_pTRANSThread) = pTRANSThread;
	return 0x00;
}

VOID NExecTransBzl::RegisterRetry(struct LocalConfig *pLocalConfig) {
	// reinit session and validate
	struct ReplyArgu tReplyArgu;
	CONVERT_VALSOCKS_ARGU(&tReplyArgu, pLocalConfig);
	if(NValidBzl::InitValid(&tReplyArgu)) {
		NValidBzl::FinishValid();
		return;
	}
	//
	struct ValidSeion *pValSeion = NValSeionBzl::Initialize(&pLocalConfig->tClientConfig);
	if(!pValSeion) {
		NValidBzl::FinishValid();
		return;
	}
	//
	NValidBzl::Register(pValSeion); // Register���ӵ�������
	// uninit session and validate
	NValSeionBzl::Finalize();
	NValidBzl::FinishValid();
}

// ����ͬ���̣߳��������״̬
DWORD NExecTransBzl::ValidConnectStatus(struct LocalConfig *pLocalConfig, TCHAR *szAccountLinked) {
	DWORD connect_result = CONNECT_STATUS_OKAY;
	// �ͻ����û�û�����ӵ�������
	if(!_tcscmp(szAccountLinked, _T("false"))) {
		NExecTransBzl::SetConnectedStatus(FALSE);
		return CONNECT_STATUS_NOACCOUNT;
		//
	} else if(!_tcscmp(szAccountLinked, _T("true"))) {
		//û��������������
		if(NTRANSSockets::SWD_IsNetworkAlive()) {
			NExecTransBzl::SetConnectedStatus(FALSE);
			return CONNECT_STATUS_OFFLINK;
		}
		// �ͻ����û����ӵ�������
		if (NExecTransBzl::GetConnectedStatus()) 
			return CONNECT_STATUS_OKAY; // �Ѿ�����,ִ��ͬ��
		//
		NLocalBzl::TrayiconConnecting(CONNECT_ACTIVE_STEP);
		// reinit session and validate
		struct ReplyArgu tReplyArgu;
		CONVERT_VALSOCKS_ARGU(&tReplyArgu, pLocalConfig);
		if(NValidBzl::InitValid(&tReplyArgu)) {
			NValidBzl::FinishValid();
			return CONNECT_STATUS_FAULT;
		}
		//
		struct ValidSeion *pValSeion= NValSeionBzl::Initialize(&pLocalConfig->tClientConfig);
		if(!pValSeion) {
			NValidBzl::FinishValid();
			return CONNECT_STATUS_FAULT;
		}
		// regist to server
		if (!NValidBzl::Register(pValSeion)) {  // Register���ӵ�������
			NExecTransBzl::SetConnectedStatus(TRUE);
			connect_result = CONNECT_STATUS_OKAY;
		} else connect_result = CONNECT_STATUS_DISCONN; // Register���Ӳ���������
		// uninit session and validate
		NValSeionBzl::Finalize();
		NValidBzl::FinishValid();
	}
	//
	return connect_result;
}

DWORD NExecTransBzl::CheckUserPoolChanged(struct LocalConfig *pLocalConfig) {  // ִ��ͬ�� ����Ƿ�Ҫͬ��
	DWORD change_value = CHANGE_STATUS_CHANGED;
	// reinit session and validate
	struct ComandSeion *pCmdSeion = NCmdSeionBzl::Initialize(&pLocalConfig->tClientConfig);
	if(!pCmdSeion) {
		NComandBzl::FinishComand();
		return CONNECT_STATUS_FAULT;
	}
	//
	struct ReplyArgu tReplyArgu;
	CONVERT_AUTHSOCKS_ARGU(&tReplyArgu, pCmdSeion->ldata, &pLocalConfig->tNetworkConfig);
	if(NComandBzl::InitComand(&tReplyArgu)) {
		NComandBzl::FinishComand();
		return CONNECT_STATUS_FAULT;
	}
	// check the server and client file change
	// _LOG_DEBUG(_T("----------------")); // disable by james 20140410
	if(NExecTransBzl::GetConnectedStatus()) {
		DWORD cache_locked;
		UINT llast_anchor = 0, cached_anchor = 0;
		for(int retry_times=0; REGIST_RETRY_TIMES>retry_times; retry_times++) {
			cache_locked = NComandBzl::QueryCached(&cached_anchor, pCmdSeion);
			if(WORKER_STATUS_NOTAUTH != cache_locked)
				break;
			Sleep(REGIST_RETRY_DELAY);
			NExecTransBzl::RegisterRetry(pLocalConfig);
		}
_LOG_DEBUG(_T("cache_locked:%d"), cache_locked); // disable by james 20140410
		switch(cache_locked) {
		case WORKER_STATUS_OKAY:
			llast_anchor = NAnchorBzl::GetLastAnchor();
_LOG_DEBUG(_T("llast_anchor:%d cached_anchor:%d"), llast_anchor, cached_anchor); // disable by james 20140410
			if (!cached_anchor && !llast_anchor) {
				change_value = CHANGE_STATUS_CHANGED;
			} else if ((cached_anchor==llast_anchor) && !NAnchorBzl::CheckActiveAction()) {
				change_value = CHANGE_STATUS_NOCHANGE;
			} else change_value = CHANGE_STATUS_CHANGED;
			break;
		case WORKER_STATUS_NOTAUTH:
			NExecTransBzl::SetConnectedStatus(FALSE);
			change_value = CHANGE_STATUS_NOTAUTH;
			break;
		case WORKER_STATUS_LOCKED:
			change_value = CHANGE_STATUS_LOCKED;
			break;
		case WORKER_STATUS_NOTCONN:
			NExecTransBzl::SetConnectedStatus(FALSE);
			change_value = CHANGE_STATUS_DISCONNECT;
			break;
		case CONNECT_STATUS_FAULT:
		default:
			change_value = CHANGE_STATUS_FAULT;
			break;
		}
	} else change_value = CHANGE_STATUS_DISCONNECT;
	// uninit session and validate
	NComandBzl::FinishComand();
	NCmdSeionBzl::Finalize();
	//
	return change_value;
}

#define RECURSIVE_FILE_ENTRY   _T("recursive_file.tmp")
DWORD NExecTransBzl::GetSlowFilesDbBzl(TreeDB *lattdb, TCHAR *szDriveLetter) { // 0:ok 0x01:error 0x02:processing
	TCHAR szEntryFile[MAX_PATH];
	HANDLE hEntryFile = NFileUtility::BuildCacheFile(szEntryFile, RECURSIVE_FILE_ENTRY);
	//
	DWORD recurs_value = RecursiveUtility::RecursiveFile(hEntryFile, szDriveLetter);
	if(recurs_value) {
		// _LOG_DEBUG(_T("recurs_value:%d"), recurs_value);
		CloseHandle( hEntryFile );
		return recurs_value;
	}
	//
	DWORD dwResult = SetFilePointer(hEntryFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle( hEntryFile );
		return 0x01;
	}
	struct attent atte;
	while(!NFilesVec::ReadNode(&atte, hEntryFile)) {
		if(!NLocalBzl::ForbidTransmitValid(atte.file_name)) {
			atte.action_type |= RECURSIVE;
_LOG_DEBUG(_T("atte.file_name:%s atte.action_type:%08X"), atte.file_name, atte.action_type);
			// Perform the insertion
			DWORD key_len = _tcslen(atte.file_name) << 1;
			if(!lattdb->set((const char *)atte.file_name, key_len, (char *)&atte, sizeof(struct attent))) {
				// fprintf(stderr, "key already exist.\n");
				_LOG_WARN(_T("key already exist."));
			}
		}
	}
	//
	if(INVALID_HANDLE_VALUE != hEntryFile) CloseHandle( hEntryFile );
	DeleteFile(szEntryFile);
	// _LOG_DEBUG(_T("lattdb->count():%d"), lattdb->count());
	return 0x00;
}

DWORD NExecTransBzl::GetFastFilesVecBzl(FilesVec *lattent, UINT dwLocalAnchor, const TCHAR *szDriveLetter) { // 0:ok ((DWORD)-1):error
_LOG_DEBUG(_T("get atte files vector bzl")); // disable by james 20140410
	// 0:ok 0x01:error 0x02:processing
	DWORD acti_value = NAnchorBzl::GetActionByAnchor(lattent, dwLocalAnchor);
	if(acti_value) return ((DWORD)-1);
//
_LOG_DEBUG(_T("hand forbid valid.")); // disable by james 20140410
	FilesVec::iterator iter;
	for(iter=lattent->begin(); lattent->end()!=iter; ) { // ++iter
_LOG_DEBUG(_T("file_name:%s action_type:%08x"), (*iter)->file_name, (*iter)->action_type); // disable by james 20140410
		if(NLocalBzl::ForbidTransmitValid((*iter)->file_name)) {
			NFilesVec::DeleNode(*iter);
			iter = lattent->erase(iter);
		} else ++iter;
	}
	NFilesVec::CompleteFilesVec(lattent, szDriveLetter);
_LOG_DEBUG(_T("get fast files OK.")); // disable by james 20140410
	return 0x00;
}

VOID NExecTransBzl::StopTRANSSession() {
	DWORD dwThreadsCount;
	HANDLE *hProcessThreads = NULL;
	DWORD dwThreadIndex;
	// �ͷ��߳���Դ
	NVerifyUtility::CloseVerifyHandle();
	NBuildUtility::CloseBuilderHandle();
	NTRANSWorker::CloseWorkerResource();
	//
	hProcessThreads = PRODUCE_OBJECT(GetListThreads(&dwThreadsCount));
	// Todo:ɱ��ͬ���߳�		// ɱ�������߳�
	for(dwThreadIndex = 0; dwThreadIndex<dwThreadsCount; dwThreadIndex++)
		TerminateThread(hProcessThreads[dwThreadIndex], WORKER_VALUE_KILLED);
	//
	hProcessThreads = VERIFY_OBJECT(GetVerifyThreads(&dwThreadsCount));
	// Todo:ɱ��ͬ���߳�		// ɱ�������߳�
	for(dwThreadIndex = 0; dwThreadIndex<dwThreadsCount; dwThreadIndex++)
		TerminateThread(hProcessThreads[dwThreadIndex], WORKER_VALUE_KILLED);
	//
	hProcessThreads = TRANS_OBJECT(GetWorkerThreads(&dwThreadsCount));
	// Todo:ɱ��ͬ���߳�		// ɱ�������߳�
	for(dwThreadIndex = 0; dwThreadIndex<dwThreadsCount; dwThreadIndex++)
		TerminateThread(hProcessThreads[dwThreadIndex], WORKER_VALUE_KILLED);
	//
	objTaskList.SetNodePoint(0);
}

VOID NExecTransBzl::KillTRANSSession() {
	DWORD dwThreadsCount;
	HANDLE *hProcessThreads = NULL;
	DWORD dwThreadIndex;
	// �ͷ��߳���Դ
	NVerifyUtility::CloseVerifyHandle();
	NBuildUtility::CloseBuilderHandle();
	NTRANSWorker::CloseWorkerResource();
	//
	hProcessThreads = PRODUCE_OBJECT(GetListThreads(&dwThreadsCount));
	// Todo:ɱ��ͬ���߳�		// ɱ�������߳�
	for(dwThreadIndex = 0; dwThreadIndex<dwThreadsCount; dwThreadIndex++)
		TerminateThread(hProcessThreads[dwThreadIndex], WORKER_VALUE_KILLED);
	//
	hProcessThreads = VERIFY_OBJECT(GetVerifyThreads(&dwThreadsCount));
	// Todo:ɱ��ͬ���߳�		// ɱ�������߳�
	for(dwThreadIndex = 0; dwThreadIndex<dwThreadsCount; dwThreadIndex++)
		TerminateThread(hProcessThreads[dwThreadIndex], WORKER_VALUE_KILLED);
	//
	hProcessThreads = TRANS_OBJECT(GetWorkerThreads(&dwThreadsCount));
	// Todo:ɱ��ͬ���߳�		// ɱ�������߳�
	for(dwThreadIndex = 0; dwThreadIndex<dwThreadsCount; dwThreadIndex++)
		TerminateThread(hProcessThreads[dwThreadIndex], WORKER_VALUE_KILLED);
	//
	objTaskList.SetNodePoint(0);
}

// ����lock action
#define ALOCK_RETRY_TIMES			60 // 60*20/60 20m
#define INVAL_SLEEP_TYPE(LOCK_RETRY) (0xFFFFFFFC & LOCK_RETRY)
int NExecTransBzl::PrepareLockedAction(const TCHAR *szDriveLetter, const TCHAR *szLocation) {  // >0:finish 0:nothing <0:exception
	// ��ʼ��action��������
	HANDLE hLockActio = objLockActio.LockActioIniti();
	if(INVALID_HANDLE_VALUE == hLockActio) return -1;
_LOG_DEBUG(_T("in prepa locked action! initial lock action file ok!")); // disable by james 20140410
	//
	static DWORD INVAL_ACTION_DELAY[4] = {20000, 8000, 15000, 20000};
	DWORD dwLockRetry = 0x00, dwPrepareValue = 0x00;
	// �õ��û��޸��ļ�������
	int lockValue = CACHE_ACTION_COPY(hLockActio) // >0:finish 0:nothing <0:exception
	NShellLinkBzl::ExploSleep(0x00, szDriveLetter);
_LOG_DEBUG(_T("lockValue:%d"), lockValue);
	if(lockValue) {
		while(ALOCK_RETRY_TIMES > dwLockRetry++) {
// _LOG_DEBUG(_T("materi exist:%d"), NFileUtility::MateriExist(hLockActio)); // disable by james 20140410
			if(0 < lockValue) { // Ԥ����õ�������
				dwPrepareValue = NLkAiUtili::ActioPrepaProcess(szDriveLetter); // 0:pass >0:undone
				if(!dwPrepareValue) break;
			}
			if(INVAL_SLEEP_TYPE(dwLockRetry))
				NShellLinkBzl::ExploSleep(INVAL_ACTION_DELAY[0], NULL);
			else NShellLinkBzl::ExploSleep(INVAL_ACTION_DELAY[dwLockRetry], NULL);
			// �õ��û��޸��ļ�������
			lockValue = CACHE_ACTION_COPY(hLockActio) // >0:finish 0:nothing <0:exception
_LOG_DEBUG(_T("dwLockRetry:%d dwPrepareValue:%d lockValue:%d"), dwLockRetry, dwPrepareValue, lockValue);
		}
	}
_LOG_DEBUG(_T("dwPrepareValue:%08X"), dwPrepareValue);
	// delete temp icon status
	if(!dwPrepareValue) NLkAiUtili::ClearActioDisplay(szDriveLetter);
	if((0<lockValue) && !dwPrepareValue) {
// �õ������ݲ��뱾��êϵͳ
		if(NLkAiUtili::LockActionConve(szDriveLetter, szLocation)) { // �Ż�����êϵͳ����
			lockValue = NAnchorBzl::OptimizeCached(szDriveLetter); // >0:finish 0:nothing
_LOG_DEBUG(_T("convert lock action ok! lock value:%d"), lockValue); // disable by james 20140410
		} else lockValue = -1;
	}
	// ���action��������
	objLockActio.FinalLockActio(BOOL(!dwPrepareValue));
_LOG_DEBUG(_T("dwPrepareValue:%d lockValue:%d"), dwPrepareValue, lockValue); // disable by james 20140410
_LOG_DEBUG(_T("finish lock action ok!")); // disable by james 20140410
	//
	return dwPrepareValue? -1: lockValue;
}

// ִ��ͬ��
DWORD NExecTransBzl::PerfromSynchronTransport(struct LocalConfig *pLocalConfig) { // ���������û��ռ�
_LOG_DEBUG(_T("==================== sync trans ==================== "));
	TCHAR *szDriveLetter = pLocalConfig->tClientConfig.szDriveLetter;
	DWORD dwLockAnchor = ANCHOR_FINISH_FAILED;
	if(NLockAnchor::LockAnchorObject(szDriveLetter)) { // ��ʼ�ɹ�,����ê������
		NLockAnchor::UnlockAnchorObject(ANCHOR_FINISH_FAILED); // �������
		return WORKER_VALUE_LOCKED;
	}
	NAnchorBzl::EraseAnchorIcon(szDriveLetter); // ����last action cache
	NTRANSSockets::InitialWinsock(); // Initialize Winsock
	// ���ɹ�����¼,��ʼ�Ự
_LOG_DEBUG(_T("tasks list producer_no!")); // disable by james 20140410
	DWORD producer_value = NExecuteUtility::TasksListProducerNOR(pLocalConfig);
	if(LIST_VALUE_FINISH != producer_value) {
_LOG_DEBUG(_T("tasks list producer error! producer_value:%X"), producer_value);
		NTRANSSockets::FinishWinsock();
		NLockAnchor::UnlockAnchorObject(ANCHOR_FINISH_FAILED); // �������
		if(LIST_VALUE_NETWORK == producer_value) 
			NExecTransBzl::SetConnectedStatus(FALSE);
		return producer_value;
	}
_LOG_DEBUG(_T("tasks list producer! producer_value:%X"), producer_value); // disable by james 20140410
	// ����ˢ��ͼ��
	NExecuteUtility::BeginRefreshIcon(szDriveLetter);
// ndp::LastTaskNode();
	// ��鹤����¼����Ч��
_LOG_DEBUG(_T("verify tasks list!")); // disable by james 20140410
	DWORD chanValue = 0x00;
	DWORD handle_value = VERIFY_VALUE_FAILED;
	handle_value = NExecuteUtility::TasksListVerifier(pLocalConfig);
_LOG_DEBUG(_T("verify tasks list! verifier_value:%X"), handle_value); // disable by james 20140410
	if(VERIFY_VALUE_FINISH == handle_value) {
 // ��鹤����¼,����Ƿ���������Ҫͬ�� // �Ự��ʼ�ɹ�,��������Ҫͬ�� // ���������߳�,��ɹ���
_LOG_DEBUG(_T("tasks list worker!")); // disable by james 20140410
		handle_value = NExecuteUtility::TasksListHandler(pLocalConfig);
		chanValue |= NodeUtility::CheckChanValue(); // disable for test
_LOG_DEBUG(_T("tasks list worker! handle_value:%X chanValue:%d"), handle_value, chanValue);
	}
	// ���������¼(move copy exception) 
	if(WORKER_VALUE_EXCEPTION & handle_value) {
_LOG_DEBUG(_T("==================== sync excep ==================== "));
_LOG_DEBUG(_T("tasks list producer_ep!")); // disable by james 20140410
		producer_value = NExecuteUtility::TasksListProducerEXP();
		if(LIST_VALUE_FINISH != producer_value) {
_LOG_DEBUG(_T("tasks list producer error! producer_value:%X"), producer_value);
			NTRANSSockets::FinishWinsock();
			NLockAnchor::UnlockAnchorObject(ANCHOR_FINISH_FAILED); // �������
			return producer_value;
		}
_LOG_DEBUG(_T("tasks list producer! producer_value:%X"), producer_value); // disable by james 20140410
		// ��������¼����Ч��
_LOG_DEBUG(_T("verify tasks list_ep!")); // disable by james 20140410
		handle_value = VERIFY_VALUE_FAILED;
		handle_value = NExecuteUtility::TasksListVerifier(pLocalConfig);
_LOG_DEBUG(_T("verify tasks list! verifier_value:%X"), handle_value); // disable by james 20140410
		if(VERIFY_VALUE_FINISH == handle_value) {
		// ��������¼,����Ƿ���������Ҫͬ��
_LOG_DEBUG(_T("tasks list worker_ep!")); // disable by james 20140410
			handle_value = NExecuteUtility::TasksListHandler(pLocalConfig);
			chanValue |= NodeUtility::CheckChanValue();
_LOG_DEBUG(_T("tasks list worker! handle_value:%X chanValue:%d"), handle_value, chanValue);
		}
	}
	// ��������,��ʼ�Ự
_LOG_DEBUG(_T("end tasks list!")); // disable by james 20140410
	DWORD finish_value = NExecuteUtility::FinishTasksList(&dwLockAnchor, pLocalConfig, handle_value, chanValue);
_LOG_DEBUG(_T("end tasks list! finish_value:%X"), finish_value); // disable by james 20140410
	// ��������ͼ�껺��
	NExecuteUtility::FailedRefreshIcon(szDriveLetter);
	NTRANSSockets::FinishWinsock(); // Finish Winsock
	//
	if(WORKER_VALUE_NETWORK == handle_value || FINISH_VALUE_NETWORK == finish_value)
		NExecTransBzl::SetConnectedStatus(FALSE);
	// ������������û��ռ�
	NLockAnchor::UnlockAnchorObject(dwLockAnchor);
	//
	return handle_value;
}



