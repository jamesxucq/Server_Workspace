#include "StdAfx.h"

#include "../third party/RiverFS/RiverFS.h"
#include "ApplicaBzl.h"

CApplicaInit::CApplicaInit(void) {
	INIT_APPLICA_FLAGS(m_dwApplicaInitFlags);
}

CApplicaInit::~CApplicaInit(void) {
}

class CApplicaInit objApplicaInit;


DWORD CApplicaInit::ApplicaInitialize(CWnd* pNotifyWnd) {
	if(!pNotifyWnd) return ((DWORD)-1);

	// init parse config
	if(!PCONFIG_CHECK_INIT(m_dwApplicaInitFlags)) { // not begin thread
		if(!NConfigBzl::Initialize(CLIENT_CONFIGURATION))
			PCONFIG_INIT_OKAY(m_dwApplicaInitFlags);
	}
	struct LocalConfig *pLocalConfig = NConfigBzl::GetLocalConfig();
	struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
	struct LogConfig *pLogConfig = NConfigBzl::GetLogConfig();

	// init log module
	TCHAR szLogfile[MAX_PATH];
	if(!SYSLOG_CHECK_INIT(m_dwApplicaInitFlags)) { // not begin thread
		log_init(struti::get_name(szLogfile, pClientConfig->szLocation, pLogConfig->szLogfile), pLogConfig->dwDebugLevel);
		SYSLOG_INIT_OKAY(m_dwApplicaInitFlags);
	}

	// init client gui module
	if(!CLIENT_CHECK_INIT(m_dwApplicaInitFlags)) { // not begin thread
		if(!NLocalBzl::Initialize(pNotifyWnd, pClientConfig))
			CLIENT_INIT_OKAY(m_dwApplicaInitFlags);
	}
	FileFobdVec *pFobdVec = NFileFobd::GetFileFobdPoint();
	FileColiVec *pColiVec = NFileColi::GetFileColiPoint();

	// init anchor module
	if(!ANCH_CHECK_INIT(m_dwApplicaInitFlags)) { // THREAD_PRIORITY_NORMAL
		if(!NAnchorBzl::Initialize(pClientConfig->szAnchorInde, pClientConfig->szAnchorData, pClientConfig->szLocation))
			ANCH_INIT_OKAY(m_dwApplicaInitFlags);
	}

	// start the river file system
	if(!RIVER_CHECK_INIT(m_dwApplicaInitFlags)) { // THREAD_PRIORITY_HIGHEST
		if(!RiverFS::Initialize(pClientConfig->szLocation, pClientConfig->szDriveLetter))
			RIVER_INIT_OKAY(m_dwApplicaInitFlags);
	}

	// start the dir watcher thread
	if(!DWATCH_CHECK_INIT(m_dwApplicaInitFlags)) { // THREAD_PRIORITY_HIGHEST
		if(!NWatcherBzl::Initialize(pClientConfig->szLocation, pClientConfig->szDriveLetter))
			DWATCH_INIT_OKAY(m_dwApplicaInitFlags);
	}

	// start the worker thread
	if(!WORKER_CHECK_INIT(m_dwApplicaInitFlags)) { // THREAD_PRIORITY_BELOW_NORMAL
		NBuildTasksBzl::Initialize(pClientConfig->szLocation);
		if(!NExecTransBzl::Initialize(pLocalConfig))
			WORKER_INIT_OKAY(m_dwApplicaInitFlags);
	}

	// init display cache module
	FilesVec *pActioVec = NAnchorBzl::GetLastActioVec();
	if(!LINKER_CHECK_INIT(m_dwApplicaInitFlags)) { // THREAD_PRIORITY_NORMAL
		if(!NShellLinkBzl::Initialize(pFobdVec, pColiVec, pActioVec, pClientConfig->szDriveLetter))
			LINKER_INIT_OKAY(m_dwApplicaInitFlags);
	}

	// just for test
	// NLocalBzl::DisplayClientShell();

	return 0x00;
}

DWORD CApplicaInit::ApplicaFinalize() {
	// uninit display cache module
	if(LINKER_CHECK_INIT(m_dwApplicaInitFlags)) {
		if(!NShellLinkBzl::Finalize())
			LINKER_FINAL_OKAY(m_dwApplicaInitFlags);
	}

	// stop the worker thread
	if(WORKER_CHECK_INIT(m_dwApplicaInitFlags)) {
		if(!NExecTransBzl::Finalize())
			WORKER_FINAL_OKAY(m_dwApplicaInitFlags);
		NBuildTasksBzl::Finalize();
	}

// _LOG_DEBUG(_T("application finalize watcher_bzl")); // disable by james 20140410
	// stop the dir watcher thread
	if(DWATCH_CHECK_INIT(m_dwApplicaInitFlags)) {
		if(!NWatcherBzl::Finalize())
			DWATCH_FINAL_OKAY(m_dwApplicaInitFlags);
	}

	// stop the river file system
	if(RIVER_CHECK_INIT(m_dwApplicaInitFlags)) {
		if(!RiverFS::Finalize())
			RIVER_FINAL_OKAY(m_dwApplicaInitFlags);
	}

// _LOG_DEBUG(_T("application finalize local_bzl")); // disable by james 20140410
	// uninit client gui module
	DWORD dwLocalValue;
	if(CLIENT_CHECK_INIT(m_dwApplicaInitFlags)) {
		if(!(dwLocalValue = NLocalBzl::Finalize()))
			CLIENT_FINAL_OKAY(m_dwApplicaInitFlags);
		if(dwLocalValue) return dwLocalValue;
	}

// _LOG_DEBUG(_T("application finalize anchor_bzl")); // disable by james 20140410
	// uninit anchor module
	if(ANCH_CHECK_INIT(m_dwApplicaInitFlags)) {
		if(!NAnchorBzl::Finalize())
			ANCH_FINAL_OKAY(m_dwApplicaInitFlags);
	}

	// uninit log module
	if(SYSLOG_CHECK_INIT(m_dwApplicaInitFlags)) {
		log_fini();
		SYSLOG_FINAL_OKAY(m_dwApplicaInitFlags);
	}

	// uninit parse config
	if(PCONFIG_CHECK_INIT(m_dwApplicaInitFlags)) {
		if(!NConfigBzl::Finalize())
			PCONFIG_FINAL_OKAY(m_dwApplicaInitFlags);
	}

	return 0x00;
}

//*********************************************************************
static DWORD NApplicaBzl::SingleInstance(LPCTSTR szAppName) {
	HANDLE	hMutex = CreateMutex(NULL, TRUE, szAppName);
	if(ERROR_ALREADY_EXISTS==GetLastError() || NULL==hMutex)
		return ((DWORD)-1);
	return 0x00;
}

#define RIVER_FOLDER_DEFAULT			_T("~\\river_folder.sdo")
#define FILES_RIVER_DEFAULT				_T("~\\files_river.sdo")
#define FILES_CHKS_DEFAULT				_T("~\\files_chks.sdo")
#define FILES_ISONYM_DEFAULT			_T("~\\files_isonym.kct")
#define EXCEPT_ACTION_DEFAULT			_T("~\\except_action.acd")
#define ACTION_CACHE_DEFAULT			_T("~\\action_cache.acd")

static VOID NApplicaBzl::EraseUserspace(TCHAR *szLocation, TCHAR *szAnchorInde, TCHAR *szAnchorData, TCHAR *szFileConfli) {
	TCHAR szEraseFile[MAX_PATH];
	static TCHAR *szFileTable[] = {
		RIVER_FOLDER_DEFAULT,
		FILES_RIVER_DEFAULT,
		FILES_CHKS_DEFAULT,
		FILES_ISONYM_DEFAULT,
		EXCEPT_ACTION_DEFAULT,
		ACTION_CACHE_DEFAULT,
		NULL
	};
	//
	DWORD inde;
	for(inde = 0; NULL != szFileTable[inde]; ++inde) {
		struti::get_name(szEraseFile, szLocation, szFileTable[inde]);
		DeleteFile(szEraseFile);
	}
	struti::get_name(szEraseFile, szLocation, szAnchorData);
	DeleteFile(szEraseFile);
	struti::get_name(szEraseFile, szLocation, szAnchorInde);
	DeleteFile(szEraseFile);
	struti::get_name(szEraseFile, szLocation, szFileConfli);
	DeleteFile(szEraseFile);
	//
}

static DWORD NApplicaBzl::ValidateUserspaceIntegrity(TCHAR *szLocation, TCHAR *szUserPool, DWORD dwPoolSize, TCHAR *szFileForbid) {
	TCHAR szValidateEnviron[MAX_PATH];
	//
	struti::get_name(szValidateEnviron, szLocation, szUserPool);
	if(!NFileUtility::FileExist(szValidateEnviron)) return ((DWORD)-1);
	//
	unsigned __int64 qwLorgeSzie = NFileUtility::FileSize(szValidateEnviron);
	qwLorgeSzie >>= 30;
	if(dwPoolSize != qwLorgeSzie) return ((DWORD)-1);
	//
	struti::get_name(szValidateEnviron, szLocation, szFileForbid);
	if(!NFileUtility::FileExist(szValidateEnviron)) return ((DWORD)-1);
	//
	return 0x00;
}

#define MOUNT_LETTER_EXISTS		0x0001
#define MOUNT_LETTER_OCCPUY		0x0002
static DWORD NApplicaBzl::ValidateMounted(DWORD dwPoolSize, TCHAR *szDriveLetter) {
	DWORD dwTotalSpace;
	DWORD dwMountValue = 0x00;
	if(!szDriveLetter || '\0'==szDriveLetter[0]) return MOUNT_LETTER_OCCPUY;
	//
	if(NDireUtility::IsDirectory(szDriveLetter)) {
		dwMountValue = MOUNT_LETTER_EXISTS;
		if(!DiskUtility::GetTotalImageSpace(&dwTotalSpace, szDriveLetter)) {
			if(dwPoolSize != dwTotalSpace) 
				dwMountValue = MOUNT_LETTER_OCCPUY;
		} 
	}
	//
	return dwMountValue;
}

static DWORD NApplicaBzl::ValidUserSettings(struct LocalConfig *pLocalConfig) {
	// reinit session and validate
	struct ReplyArgu tReplyArgu;
	CONVERT_VALSOCKS_ARGU(&tReplyArgu, pLocalConfig);
	if(NValidBzl::InitValid(&tReplyArgu)) {
		NValidBzl::FinishValid();
		return ((DWORD)-1);
	}
//
	struct ValidSeion *pValSeion = NValSeionBzl::Initialize(&pLocalConfig->tClientConfig);
	if(!pValSeion) {
		NValidBzl::FinishValid();
		return ((DWORD)-1);
	}
	// 检查用户设置是否有变化
	DWORD dwSettingValue = ((DWORD)-1);
	TCHAR client_version[VERSION_LENGTH];
	int pool_size, data_bomb;
	if(!NValidBzl::Settings(client_version, &pool_size, &data_bomb, pValSeion)) {
		dwSettingValue = 0;
		// logger(_T("c:\\setting.log"), _T("validate settings error!\r\n")); // ValidateSettings出错
		// logger(_T("c:\\setting.log"), _T("client settings error!\r\n")); // ClientInfo连接不到服务器
	}
	// uninit session and validate
	NValSeionBzl::Finalize();
	NValidBzl::FinishValid();
	// 检查用户设置是否有变化
	DWORD dwValidValue = 0;
	if(!dwSettingValue) {
		// logger(_T("c:\\setting.log"), _T("-----------------------------------------------\r\n"));
		// logger(_T("c:\\setting.log"), _T("local pool size:%d\r\n"), pool_size);
		dwValidValue = NLocalBzl::ValidateSettings(&pLocalConfig->tClientConfig, client_version, pool_size, data_bomb);
	}
	//
	return dwValidValue;
}

DWORD NApplicaBzl::CheckEnviron(TCHAR *szDriveLetter) {
	if (SingleInstance(APPLICATION_NAME)) return EXECUTE_ONE_INSTANCE;
	//
	struct LocalConfig *pLocalConfig;
	if(NConfigBzl::Initialize(CLIENT_CONFIGURATION))  return EXECUTE_CONFIGURATION;
	if(!(pLocalConfig = NConfigBzl::GetLocalConfig()))
		return EXECUTE_FAULT;
	struct ClientConfig *pClientConfig = &pLocalConfig->tClientConfig;
	//
	if(NULL != szDriveLetter) {
		_tcscpy_s(szDriveLetter, 32, pClientConfig->szDriveLetter);
	}
	// this pc is linked
	if(!_tcscmp(pClientConfig->szAccountLinked, _T("false"))) return EXECUTE_NOACCOUNT;
	//
	// handle force control flag
	if(!_tcscmp(pClientConfig->szForceCtrl, _T("slowly"))) {
		EraseUserspace(pClientConfig->szLocation, pClientConfig->szAnchorInde,
			pClientConfig->szAnchorData, pClientConfig->szFileConfli);
		_tcscpy_s(pClientConfig->szForceCtrl, _T("false"));
		NConfigBzl::SaveConfiguration();
	}
	//
	DWORD dwExecuteType = EXECUTE_SUCCESS;
	if(!_tcscmp(pClientConfig->szProtected, _T("true")))
		dwExecuteType |= EXECUTE_PASSWD_PROTECT;
	if(ValidateUserspaceIntegrity(pClientConfig->szLocation, pClientConfig->szUserPool, 
		pClientConfig->dwPoolLength, pClientConfig->szFileForbid))
		dwExecuteType |= EXECUTE_USER_SPACE;
	//
	DWORD dwMountedType = ValidateMounted(pClientConfig->dwPoolLength, pClientConfig->szDriveLetter);
	if(dwMountedType) {
		if(MOUNT_LETTER_EXISTS & dwMountedType) dwExecuteType |= EXECUTE_DRIVE_MOUNTED;
		else if(MOUNT_LETTER_OCCPUY & dwMountedType) dwExecuteType |= EXECUTE_LETTER_OCCPUY;
	}
	// 检查用户设置是否有变化
	if(!_tcscmp(pClientConfig->szAccountLinked, _T("true"))) {
		DWORD dwValidValue = ValidUserSettings(pLocalConfig);
		if((VALID_POOL_DESTROY|VALID_POOL_ADJUST) & dwValidValue) {
			if(VALID_POOL_DESTROY & dwValidValue) {
				dwExecuteType |= EXECUTE_NOACCOUNT;
				AfxMessageBox(_T("用户本地空间已销毁."), MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
			}
			NConfigBzl::SaveConfiguration();
		}
		if(VALID_ADJUST_FAULT & dwValidValue) dwExecuteType |= EXECUTE_USER_SPACE;
		// 暂时没有自动更新功能
		if((VALID_LIVE_UPDATE|VALID_ACTIVE_UPDATE) & dwValidValue) {
			if(!_tcscmp(pClientConfig->szAutoUpdate, _T("true"))) {
				if(VALID_LIVE_UPDATE & dwValidValue) dwExecuteType |= EXECUTE_LIVE_UPDATE;
				else if(VALID_ACTIVE_UPDATE & dwValidValue) dwExecuteType |= EXECUTE_ACTIVE_UPDATE;
			} else {
				if(VALID_LIVE_UPDATE & dwValidValue) {
					if(IDYES == AfxMessageBox(_T("软件有新版本,是否更新?"), MB_YESNO|MB_ICONWARNING||MB_DEFBUTTON1|MB_SYSTEMMODAL))
						dwExecuteType |= EXECUTE_LIVE_UPDATE;
				} else if(VALID_ACTIVE_UPDATE & dwValidValue) dwExecuteType |= EXECUTE_ACTIVE_UPDATE;
			}
		}
	}
	//
	return dwExecuteType;
}

DWORD NApplicaBzl::RunLiveUpdate() {
	TCHAR szLiveUpdate[MAX_PATH];
	//
	MKZEO(szLiveUpdate);
	if(!NFileUtility::ModuleBaseNameEx(szLiveUpdate)) return ((DWORD)-1);
	_tcscat_s(szLiveUpdate, UPDATE_EXECUTE_NAME);
	if(SystemCall::ExecuteApplica(szLiveUpdate)) return ((DWORD)-1);
	//
	return 0x00;
}

DWORD NApplicaBzl::RunLoginWizard() {
	TCHAR szLoginWizard[MAX_PATH];
	//
	MKZEO(szLoginWizard);
	if(!NFileUtility::ModuleBaseNameEx(szLoginWizard)) return ((DWORD)-1);
	_tcscat_s(szLoginWizard, LWIZARD_EXECUTE_NAME);
	if(SystemCall::ExecuteApplica(szLoginWizard)) return ((DWORD)-1);
	//
	return 0x00;
}

DWORD NApplicaBzl::ShiftDriveLetter() {
	struct LocalConfig *pLocalConfig;
	if(!(pLocalConfig = NConfigBzl::GetLocalConfig()))
		return ((DWORD)-1);
	//
	TCHAR szDriveLetter[8] = _T(" :");
	DWORD dwLogicDrives = GetLogicalDrives();
	//
	for (int inde=0; inde<26; inde++) {
		szDriveLetter[0] = 'A' + inde;
		if(!(0x00000001 & (dwLogicDrives>>inde))) {
			if (*szDriveLetter!=_T('A') && *szDriveLetter!=_T('B'))
				break;
		}
	}
	//
	_tcscpy_s(pLocalConfig->tClientConfig.szDriveLetter, MIN_TEXT_LEN, szDriveLetter);
	NConfigBzl::SaveConfiguration();
	//
	return 0x00;
}