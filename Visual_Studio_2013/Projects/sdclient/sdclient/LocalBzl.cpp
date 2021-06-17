#include "StdAfx.h"
#include "Resource.h"

#include "PoolResizeBzl.h"
#include "ApplicaBzl.h"
#include "LocalBzl.h"

//
DWORD NLocalBzl::Initialize(CWnd* pNotifyWnd, ClientConfig *pClientConfig) {
    if(!pNotifyWnd || !pClientConfig) return ((DWORD)-1);
    if(objTrayIcon.Create(pNotifyWnd)) return ((DWORD)-1);
    if(!_tcscmp(_T("true"), pClientConfig->szShowBalloonTip)) objTrayIcon.ShowBalloonTip(TRUE);
	//
    // Load file attribut extend
    TCHAR szFileAttrib[MAX_PATH];
    struti::get_name(szFileAttrib, pClientConfig->szLocation, pClientConfig->szFileForbid);
    if(NFileFobd::Initialize(szFileAttrib))
        return ((DWORD)-1);
    struti::get_name(szFileAttrib, pClientConfig->szLocation, pClientConfig->szFileConfli);
    if(NFileColi::Initialize(szFileAttrib))
        return ((DWORD)-1);
    // Init client param
    objLocalObj.Create(pNotifyWnd->m_hWnd);
    // clear cache directory
    NDireUtility::CCacheDirectory();
    // Mount the file disk
    if(!NDireUtility::IsDirectory(pClientConfig->szDriveLetter)) { // 已有挂载数据
        BalloonTip(BALLOON_TIP_INFORM, _T("正在挂载用户数据..."));
        if(NVolumeUtility::MountVolume(pClientConfig->szLocation, pClientConfig->szUserPool,
			pClientConfig->dwPoolLength, pClientConfig->szDriveLetter)) {
			BalloonTip(BALLOON_TIP_WARNING, _T("挂载用户数据失败！"));
            return ((DWORD)-1);		
		}
    }
	//
	ShellUtility::InitiExploWindo();
    return 0x00;
}

DWORD NLocalBzl::Finalize() {
    struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
    // destroy the cretate volume object
    BalloonTip(BALLOON_TIP_INFORM, _T("正在卸载用户数据..."));
    // int dwDismntValue = NVolumeUtility::DismntVolume(pClientConfig->szDriveLetter);
    DWORD dwDismntValue = NVolumeUtility::DismntVolume(pClientConfig->szDriveLetter);
    if(dwDismntValue) return dwDismntValue;
    // clear cache directory
    NDireUtility::CCacheDirectory();
    // Init client param
    objLocalObj.Destroy();
    // destroy the file attribute object
    NFileColi::Finalize();
    NFileFobd::Finalize();
    // destroy tray icon object
    objTrayIcon.Destroy();
    //
    return 0x00;
}

DWORD NLocalBzl::AddIconToTaskbar(CWnd* pNotifyWnd) {
	if(objTrayIcon.Destroy()) return ((DWORD)-1);
    if(objTrayIcon.Create(pNotifyWnd)) return ((DWORD)-1);
    return 0x00;
}

DWORD NLocalBzl::ShutdownApplica() {
    if(!NExecTransBzl::GetTransmitProcessStatus()) NLocalBzl::KillTransmitProcess();
    NApplicaBzl::ApplicaFinalize();
    //
    LOCAL_OBJECT(m_bShutdown) = TRUE;
    SendMessage(LOCAL_OBJECT(m_hNotifyWnd), WM_CLOSE, NULL, NULL);
    //
    return 0x00;
}

DWORD NLocalBzl::SwitchToLoginWizard() {
    TCHAR szLoginWizard[MAX_PATH];
    //
    MKZEO(szLoginWizard);
    if(!NFileUtility::ModuleBaseNameEx(szLoginWizard)) return ((DWORD)-1);
    _tcscat_s(szLoginWizard, LWIZARD_EXECUTE_NAME);
    if(SystemCall::ExecuteApplica(szLoginWizard)) return ((DWORD)-1);
    //
    ShutdownApplica();
    //
    return 0x00;
}

DWORD NLocalBzl::CheckOptionInput() {
    return 0x00;
}

static DWORD NLocalBzl::ConfigChangedAction() {
    ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
    if(!pClientConfig) return ((DWORD)-1);
    //
    if(!_tcscmp(_T("true"), pClientConfig->szShowBalloonTip))
        objTrayIcon.ShowBalloonTip(TRUE);
    else objTrayIcon.ShowBalloonTip(FALSE);
    //
    return 0x00;
}

DWORD NLocalBzl::SaveOptionConfig() {
    if(CheckOptionInput()) return ((DWORD)-1);
    if(objParseConfig.SaveConfiguration()) return ((DWORD)-1);
    if(ConfigChangedAction()) return ((DWORD)-1);
    //
    return 0x00;
}

DWORD NLocalBzl::AccountUnlink() {
    struct LocalConfig *pLocalConfig = objParseConfig.GetLocalConfig();
    if(!pLocalConfig) return ((DWORD)-1);
    //
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
    // Link to server
    DWORD dwUnlinkStatus = NValidBzl::Unlink(pValSeion);
    // uninit session and validate
    NValSeionBzl::Finalize();
    NValidBzl::FinishValid();
    //
    return dwUnlinkStatus;
}

VOID NLocalBzl::OpenRootDirectorise(HWND hParentHWnd) {
    TCHAR szRootDirectorise[MAX_PATH];
    //
    ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
    if(!pClientConfig) return;
    //
    _stprintf_s(szRootDirectorise, _T("%s%s"), pClientConfig->szDriveLetter, _T("\\"));
    DialogCode::OpenDirectorise(hParentHWnd, szRootDirectorise);
}

VOID NLocalBzl::TrayiconTransmit(DWORD dwRunStep) {
    static DWORD dwLastTickCount;
    switch(dwRunStep) {
    case RUN_INVALID_CHANGE_STEP:
        objTrayIcon.TrayiconSuccess(TRUE);
        break;
    case RUN_WAITING_CHANGE_STEP:
        objTrayIcon.TrayiconSyncing(_T("等待数据同步."));
        break;
    case RUN_OPERATE_DELAY_STEP:
        objTrayIcon.TrayiconSyncing(_T("正在锁定数据."));
        break;
    case RUN_FILE_LOCKED_STEP:
        objTrayIcon.TrayiconSyncing(_T("文件被打开,不能同步."));
        break;
    case RUN_TIMEOUT_CHECK_STEP:
        objTrayIcon.TrayiconWaiting(_T("检查同步数据."));
        break;
    case RUN_OPERATI_CHECK_STEP:
        objTrayIcon.TrayiconSyncing(_T("检查同步数据."));
        break;
    case RUN_PERFROM_SYNC_STEP:
        objTrayIcon.TrayiconSyncing(_T("正在进行同步."));
        break;
    }
}

//
#define CON_BALLOON_TIP		GetTickCount()-dwLastTickCount > 300000
VOID NLocalBzl::TrayiconConnecting(DWORD dwConnectStep) {
    static DWORD dwLastTickCount;
    switch(dwConnectStep) {
    case CONNECT_ACTIVE_STEP:
        objTrayIcon.TrayiconConnecting(_T("正在连接服务器..."), CON_BALLOON_TIP);
        break;
    case CONNECT_NOACCOUNT_STEP:
        objTrayIcon.TrayiconConnecting(_T("没有用户信息,请重新运行向导."), CON_BALLOON_TIP);
        break;
    case CONNECT_OFFLINK_STEP:
        objTrayIcon.TrayiconConnecting(_T("未连接网络,请检查!"), CON_BALLOON_TIP);
        break;
    case CONNECT_DISCONNECT_STEP:
        objTrayIcon.TrayiconConnecting(_T("不能连接到服务器,请检查!"), CON_BALLOON_TIP);
        break;
    case CONNECT_SERVERBUSY_STEP:
        objTrayIcon.TrayiconConnecting(_T("服务器忙,请稍候."), CON_BALLOON_TIP);
        break;
    case CONNECT_EXCEPTION_STEP:
        objTrayIcon.TrayiconConnecting(_T("配置不正确,发生意外!"), CON_BALLOON_TIP);
        break;
    case CONNECT_NOTAUTH_STEP:
        objTrayIcon.TrayiconConnecting(_T("用户认证错误,请检查!"), CON_BALLOON_TIP);
        break;
    default:
        objTrayIcon.TrayiconConnecting(_T("服务器忙,请稍候."), CON_BALLOON_TIP);
        break;
    }
    dwLastTickCount = GetTickCount();
}

VOID NLocalBzl::TrayiconConnected()
{ objTrayIcon.TrayiconConnected(); }

DWORD NLocalBzl::ExclusOptionsEnable() {
	DWORD dwPassExclus = objOperaExclus.PassValid();
	if(dwPassExclus) objTrayIcon.MenuOptionsEnable(FALSE);
	return dwPassExclus;
}
	
VOID NLocalBzl::ExclusOptionsDisable(DWORD dwPassExclus) {
	if(dwPassExclus) objTrayIcon.MenuOptionsEnable(TRUE);
	objOperaExclus.ReleaseExclusive(dwPassExclus);
}

VOID NLocalBzl::BalloonTip(DWORD dwDisplayLevel, LPCTSTR szMessage) {
    switch(dwDisplayLevel) {
    case BALLOON_TIP_TRACE:
        objTrayIcon.BalloonTip(szMessage, _T("同步网盘:"));
        break;
    case BALLOON_TIP_INFORM:
        objTrayIcon.BalloonTipEnable(szMessage, _T("同步网盘:"), NIIF_INFO);
        break;
    case BALLOON_TIP_EXCEPTION:
        objTrayIcon.BalloonTipEnable(szMessage, _T("发生意外:"), NIIF_INFO);
        break;
    case BALLOON_TIP_WARNING:
        objTrayIcon.BalloonTipEnable(szMessage, _T("警告:"), NIIF_WARNING);
        break;
    }
}

// force kill the transmit proocess wait time 64*512 ms
VOID NLocalBzl::KillTransmitProcess() {
    DWORD dwCheckStatusTimes = 0;
    //
    _LOG_INFO(_T("user force exit program!"));
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_KILLED);
    if(!NExecTransBzl::GetTransmitProcessStatus()) {
        for(dwCheckStatusTimes = 0; (PROCESS_STATUS_KILLED!=NExecTransBzl::GetTransmitProcessStatus()) && (64>dwCheckStatusTimes); dwCheckStatusTimes++) {
 _LOG_DEBUG(_T("wait process killed stutus 512!"));
            Sleep(512);
        }
        if(PROCESS_STATUS_KILLED != NExecTransBzl::GetTransmitProcessStatus()) {
            NExecTransBzl::KillTRANSSession(); // timeout force kill process
            for(dwCheckStatusTimes = 0; (PROCESS_STATUS_KILLED!=NExecTransBzl::GetTransmitProcessStatus()) && (16>dwCheckStatusTimes); dwCheckStatusTimes++) {
 _LOG_DEBUG(_T("wait process forece killed 512!"));
                Sleep(512);
            }
            NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_KILLED); // disable by james 20130506
        }
    }
    // display normal trayicon
    objTrayIcon.TrayiconNoraml();
}

VOID NLocalBzl::KilledTransmitProcess() {
    _LOG_INFO(_T("user killed syncing process!"));
    NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_KILLED);
    // display normal trayicon
    objTrayIcon.TrayiconNoraml();
}

VOID NLocalBzl::StopTransmitProcess() {
    _LOG_INFO(_T("user stoped syncing process!"));
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_FORCESTOP);
    // display normal trayicon
    objTrayIcon.TrayiconNoraml();
}

VOID NLocalBzl::StopedTransmitProcess() {
    NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_FORCESTOP);
}

VOID NLocalBzl::PauseTransmitProcess() {
    objTrayIcon.TrayiconPaused();
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_PAUSED);
    if(!NExecTransBzl::GetTransmitProcessStatus()) {
        while(PROCESS_STATUS_PAUSED != NExecTransBzl::GetTransmitProcessStatus())
            Sleep(500);
    }
}

VOID NLocalBzl::PausedTransmitProcess()
{ NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_PAUSED); }

VOID NLocalBzl::StartTransmitProcess() {
    TrayiconTransmit(RUN_PERFROM_SYNC_STEP);
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_RUNNING);
    NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_RUNNING);
}

VOID NLocalBzl::PerformTransmitProcess(DWORD dwNodeTotal, DWORD dwIncreValue) {
	static volatile LONG lIndeTotal, lEntryCounter;
	TCHAR szTip[128];
	//
	if(dwNodeTotal) {
		InterlockedExchange(&lIndeTotal, (LONG)dwNodeTotal);
		InterlockedExchange(&lEntryCounter, 0x00);
	} else {
		LONG lCountValue = InterlockedExchangeAdd(&lEntryCounter, (LONG)dwIncreValue);
		if(!POW2N_MOD(lCountValue++, 4) && (lIndeTotal>=lCountValue)) {
			_stprintf_s(szTip, _T("已完成(%u/%u)同步数据."), lCountValue, lIndeTotal);
			objTrayIcon.TrayiconPerformance(szTip);
		}
	}
}

VOID NLocalBzl::TransmitLockedProcess() {
    TrayiconTransmit(RUN_FILE_LOCKED_STEP);
    if(PROCESS_STATUS_KILLED == NExecTransBzl::GetTransmitProcessStatus()) {
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_KILLED);
    } else {
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_WAITING);
        NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_WAITING);
    }
}

VOID NLocalBzl::FailedTransmitProcess() {
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_WAITING);
    NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_WAITING);
}

VOID NLocalBzl::ErrorTransmitProcess() {
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_FAULT);
    NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_FAULT);
}

VOID NLocalBzl::TransmitSuccessfully(DWORD dwSuccessType) {
    static DWORD dwFirstCounter;
    //
    switch(dwSuccessType) {
    case NOCHANGE_SUCCESS:
        if(dwFirstCounter) objTrayIcon.TrayiconSuccess(FALSE);
        else objTrayIcon.TrayiconSuccess(TRUE);
        break;
    case CHANGED_SUCCESS:
        objTrayIcon.TrayiconSuccess(TRUE);
        break;
    }
    //
    dwFirstCounter++;
    NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_WAITING);
    NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_WAITING);
}

VOID NLocalBzl::PausedResumeTransmitProcess() {
    DWORD dwProcessStatus = NExecTransBzl::GetTransmitProcessStatus();
//
    if(PROCESS_STATUS_RUNNING==dwProcessStatus || PROCESS_STATUS_WAITING==dwProcessStatus) {
        objTrayIcon.TrayiconPaused(); // 更新托盘为暂停
        // 停止同步进程
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_PAUSED);
        if(!NExecTransBzl::GetTransmitProcessStatus()) {
            while(PROCESS_STATUS_PAUSED != NExecTransBzl::GetTransmitProcessStatus())
                Sleep(500);
        }
        NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_PAUSED);
    } else if(PROCESS_STATUS_PAUSED == dwProcessStatus) {
        // objTrayIcon.TrayiconSyncing(); // 更新托盘为同步
        TrayiconTransmit(RUN_WAITING_CHANGE_STEP);
        // 设置同步进程为等待
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_WAITING);
        NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_WAITING);
        // 改变暂停,触发同步事件
_LOG_DEBUG(_T("paused resume transmit process! set event"));
        NExecTransBzl::SetWaitingEvent();
    }
}

//
DWORD NLocalBzl::ResumeTransmitProcess(DWORD dwProcessStatus) {
    switch(dwProcessStatus) {
    case PROCESS_STATUS_RUNNING:
        // objTrayIcon.TrayiconSyncing();
        TrayiconTransmit(RUN_WAITING_CHANGE_STEP);
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_WAITING);
        NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_WAITING);
        break;
    case PROCESS_STATUS_FORCESTOP:
        objTrayIcon.TrayiconNoraml();
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_FORCESTOP);
        NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_FORCESTOP);
        break;
    case PROCESS_STATUS_PAUSED:
        objTrayIcon.TrayiconPaused();
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_PAUSED);
        NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_PAUSED);
        break;
    case PROCESS_STATUS_WAITING:
        objTrayIcon.TrayiconSuccess(FALSE);
        NExecTransBzl::SetTransmitNextStatus(PROCESS_STATUS_WAITING);
        NExecTransBzl::SetTransmitProcessStatus(PROCESS_STATUS_WAITING);
        break;
    }
    return 0x00;
}

// 用于syncing检查是否要同步
#define IS_EXCEPTED_FOUND			0x00000001
#define IS_EXCEPTED_NFOUND			0

DWORD NLocalBzl::ForbidTransmitValid(TCHAR *szFileName) {
    FileFobdVec *pFobdVec;
    FileFobdVec::iterator iter;
    //
    // 用户和系统设置不需要同步
    pFobdVec = NFileFobd::GetFileFobdPoint();
    for(iter=pFobdVec->begin(); pFobdVec->end()!=iter; ++iter) {
        if(!_tcsncmp(szFileName, (*iter)->szFilePath, _tcslen((*iter)->szFilePath)))
            return IS_EXCEPTED_FOUND;
    }
    // if(!bFound) _LOG_DEBUG(_T("%s"), szFileName);
    return IS_EXCEPTED_NFOUND;
}

static VOID EraseUserInformation(struct ClientConfig *pClientConfig) {
    _tcscpy_s(pClientConfig->szAccountLinked, _T("false"));
    MKZEO(pClientConfig->szUserName);
    MKZEO(pClientConfig->szPassword);
    //
    _tcscpy_s(pClientConfig->szProtected, _T("false"));
    MKZEO(pClientConfig->szLocalPassword);
    //
}

static BOOL ActiveUpdateExist() {
    TCHAR szActiveUpdate[MAX_PATH];
    MKZEO(szActiveUpdate);
	NFileUtility::ApplicaDataPath(szActiveUpdate, ACTIVE_PATH_NAME);
    //
    WIN32_FIND_DATA wfd;
    BOOL bFound = FALSE;
    //
    HANDLE hFind = FindFirstFile(szActiveUpdate, &wfd);
    if (INVALID_HANDLE_VALUE!=hFind) bFound = TRUE;
    //
    FindClose(hFind);
    return bFound;
}

// TCHAR szVersionTip[MID_TEXT_LEN];
// _stprintf_s(szVersionTip, _T("本地版本:%s\n最新版本:%s 请更新..."), pClientConfig->szVersion, client_version);
// AfxMessageBox(szVersionTip, MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
DWORD NLocalBzl::ValidateSettings(struct ClientConfig *pClientConfig, TCHAR *client_version, int pool_size, int data_bomb) {  // Todo: add the client version check 20111130
    DWORD dwValidValue = 0;
    //
    // add by james 20120927 todo handle the data bomb.
    // logger(_T("c:\\setting.log"), _T("data bomb value: %d\r\n"), data_bomb);
    // logger(_T("c:\\setting.log"), _T("client version value: %s\r\n"), client_version);
    if(data_bomb) {
		NDireUtility::DeleDirectory(pClientConfig->szLocation);
        EraseUserInformation(pClientConfig);
        dwValidValue |= VALID_POOL_DESTROY;
        //
    } else { // add by james 20120105
        if(pool_size != pClientConfig->dwPoolLength) {
            // NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, _T("用户配置发生改变,调整本地空间.")); // add by james 20120105
            if(!NPoolResizeBzl::AdjustPoolLength(pClientConfig, pClientConfig->dwPoolLength, pool_size)) {
                pClientConfig->dwPoolLength = pool_size;
                dwValidValue |= VALID_POOL_ADJUST;
            } else dwValidValue |= VALID_ADJUST_FAULT;
        }
        if(_tcscmp(client_version, pClientConfig->szVersion)) {
            // add by james 20120929
            if(ActiveUpdateExist()) dwValidValue |= VALID_ACTIVE_UPDATE;
            else dwValidValue |= VALID_LIVE_UPDATE ;
        }
    }
    //
    return dwValidValue;
}