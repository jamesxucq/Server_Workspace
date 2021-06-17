#include "StdAfx.h"

#include <stdlib.h>
#include "LoginWizdDlg.h"
#include "CVolThread.h"
#include "./utility/ulog.h"

#include "LoginWizdBzl.h"


DWORD LoginWizdBzl::Initialize() {
    // Load location config
    if(NConfigBzl::Initialize(CLIENT_CONFIGURATION)) return ((DWORD)-1);
    struct LocalConfig *pLocalConfig = NConfigBzl::GetLocalConfig();
    // init log module
    TCHAR szLogFile[MAX_PATH];
    struct LogConfig *pLogConfig = &pLocalConfig->tLogConfig;
    log_init(NFileUtility::ApplicaDataPath(szLogFile, _T("logiwiza.log")), pLogConfig->dwDebugLevel);
	//
    return 0x00;
}

DWORD LoginWizdBzl::Finalize() {
    // uninit log module
    log_fini();
    // destroy tray icon object
    NConfigBzl::Finalize();
    //
    return 0x00;
}

static DWORD LoginWizdBzl::SingleInstance(LPCTSTR pszAppName) {
    HANDLE	hMutex = CreateMutex(NULL, TRUE, pszAppName);
    if(ERROR_ALREADY_EXISTS==GetLastError() || NULL==hMutex)
        return ((DWORD)-1);
    return 0x00;
}

static DWORD LoginWizdBzl::MakeApplicaData(TCHAR *szCacheName, TCHAR *szUpdateName, TCHAR *szConfigName) {
	TCHAR szAppdaName[MAX_PATH];
//
	NFileUtility::ApplicaDataPath(szAppdaName, szCacheName);
	CreateDirectory(szAppdaName, NULL);
	NFileUtility::ApplicaDataPath(szAppdaName, szUpdateName);
	CreateDirectory(szAppdaName, NULL);
//
    TCHAR szBaseName[MAX_PATH];
    TCHAR szExistName[MAX_PATH];
    //
	NFileUtility::ApplicaDataPath(szAppdaName, szConfigName);
	if(!NFileUtility::FileExist(szAppdaName)) {
		NFileUtility::ModuleBaseName(szBaseName);
		struti::get_name(szExistName, szBaseName, szConfigName);
		CopyFile(szExistName, szAppdaName, FALSE);	
	}
    //
    return 0x00;
}

DWORD LoginWizdBzl::ApplicaExecuteType() {
	if (SingleInstance(LWIZARD_APPLICATION)) return EXECUTE_ONE_INSTANCE;
	MakeApplicaData(BUILD_CACHE, UPDATE_DIRECTORY, CLIENT_CONFIGURATION);
	if(NConfigBzl::Initialize(CLIENT_CONFIGURATION))  return EXECUTE_FAULT;
//
	return EXECUTE_SUCCESS;
}

DWORD LoginWizdBzl::LoginExecuteType() {  // -1
    struct ClientConfig *pClientConfig = objParseConfig.GetClientConfig();
    //
    if(!_tcscmp(pClientConfig->szUserName, _T("")) 
		|| !_tcscmp(pClientConfig->szPassword, _T("")) 
		|| !_tcscmp(pClientConfig->szLocalIdenti, _T("")))
        return PROGRAM_FIRSTRUN;
    //
    return PROGRAM_NORMALRUN;
}

static DWORD LoginWizdBzl::CheckOpdInput() {
    return 0x00;
}

static DWORD LoginWizdBzl::SaveWizardConfiguration() {
    // CheckInput
    if (!CheckOpdInput()) 
		objParseConfig.SaveConfiguration();
    return 0x00;
}

static DWORD LoginWizdBzl::CopyFileForbid(TCHAR *szDirePath, TCHAR *szFileForbid) {
    TCHAR szBaseName[MAX_PATH];
    TCHAR szExistName[MAX_PATH];
    TCHAR szNewName[MAX_PATH];
    //
    NFileUtility::ModuleBaseName(szBaseName);
    struti::get_name(szExistName, szBaseName, szFileForbid);
    struti::get_name(szNewName, szDirePath, szFileForbid);
    CopyFile(szExistName, szNewName, FALSE);
    //
    return 0x00;
}

static DWORD LoginWizdBzl::ValidateUserspaceExist() {
    TCHAR szValidateEnviron[MAX_PATH];
    struct ClientConfig *pClientConfig = objParseConfig.GetClientConfig();
    //
    struti::get_name(szValidateEnviron, pClientConfig->szLocation, pClientConfig->szUserPool);
    if(!NFileUtility::FileExist(szValidateEnviron)) return ((DWORD)-1);
    //
    unsigned __int64 qwLorgeSzie = NFileUtility::FileSize(szValidateEnviron);
    qwLorgeSzie >>= 30;
    if(pClientConfig->dwPoolLength != qwLorgeSzie) return ((DWORD)-1);
    //
    struti::get_name(szValidateEnviron, pClientConfig->szLocation, pClientConfig->szFileForbid);
    if(!NFileUtility::FileExist(szValidateEnviron)) return ((DWORD)-1);
    //
    return 0x00;
}

DWORD LoginWizdBzl::ValidateUserspaceExistEx(TCHAR *szNewLocation) {
    TCHAR szValidateEnviron[MAX_PATH];
    struct ClientConfig *pClientConfig = objParseConfig.GetClientConfig();
    //
    struti::get_name(szValidateEnviron, szNewLocation, pClientConfig->szUserPool);
    if(!NFileUtility::FileExist(szValidateEnviron)) return ((DWORD)-1);
    //
    unsigned __int64 qwLorgeSzie = NFileUtility::FileSize(szValidateEnviron);
    qwLorgeSzie >>= 30;
    if(pClientConfig->dwPoolLength != qwLorgeSzie) return ((DWORD)-1);
    //
    struti::get_name(szValidateEnviron, szNewLocation, pClientConfig->szFileForbid);
    if(!NFileUtility::FileExist(szValidateEnviron)) return ((DWORD)-1);
    //
    return 0x00;
}

static DWORD LoginWizdBzl::Begain() {
//
    return ERROR_OKAY;
}

static DWORD LinkAccount(struct LocalConfig *pLocalConfig, struct ClientConfig *pClientConfig) {
    // reinit session and validate
    struct ReplyArgu tReplyArgu;
    CONVERT_VALSOCKS_ARGU(&tReplyArgu, pLocalConfig);
	if(NValidBzl::InitValid(&tReplyArgu)) {
		NValidBzl::FinishValid();
		return ERROR_ERR;
	}
    //
    struct ValidSeion *pValSeion = NValSeionBzl::Initialize(pClientConfig);
    if(!pValSeion) {
        NValidBzl::FinishValid();
        return ERROR_ERR;
    }
    //
    // Link to server
    DWORD dwLinkValue = ERROR_OKAY;
    if(NValidBzl::Link(pValSeion)) {
        _tcscpy_s(pClientConfig->szAccountLinked, BOOL_TEXT_LEN, _T("false"));
        dwLinkValue = ERROR_LOGIN;
    } else _tcscpy_s(pClientConfig->szAccountLinked, BOOL_TEXT_LEN, _T("true"));
    // uninit session and validate
    NValSeionBzl::Finalize();
    NValidBzl::FinishValid();
    //
    return dwLinkValue;
}


static DWORD PoolSetting(struct LocalConfig *pLocalConfig, struct ClientConfig *pClientConfig) {
    // reinit session and validate
    struct ReplyArgu tReplyArgu;
    CONVERT_VALSOCKS_ARGU(&tReplyArgu, pLocalConfig);
	if(NValidBzl::InitValid(&tReplyArgu)) {
		NValidBzl::FinishValid();
		return ERROR_ERR;
	}
    //
    struct ValidSeion *pValSeion = NValSeionBzl::Initialize(pClientConfig);
    if(!pValSeion) {
        NValidBzl::FinishValid();
        return ERROR_ERR;
    }
	// 得到用户设置
	DWORD dwSettingValue = ERROR_OKAY;
	TCHAR client_version[VERSION_LENGTH];
	int pool_size, data_bomb;
	if(!NValidBzl::Settings(client_version, &pool_size, &data_bomb, pValSeion)) {
		if(data_bomb) dwSettingValue = ERROR_LOGIN;
		else pClientConfig->dwPoolLength = pool_size;
	} else dwSettingValue = ERROR_LOGIN;
    // uninit session and validate
    NValSeionBzl::Finalize();
    NValidBzl::FinishValid();
    //
    return dwSettingValue;
}

static DWORD LoginWizdBzl::Login() {
    struct LocalConfig *pLocalConfig = objParseConfig.GetLocalConfig();
    if(!pLocalConfig) return ERROR_ERR;
    struct ClientConfig *pClientConfig = objParseConfig.GetClientConfig();
    if(!pClientConfig) return ERROR_ERR;
	//
	DWORD dwLoginStatus = ERROR_OKAY;
	dwLoginStatus = LinkAccount(pLocalConfig, pClientConfig);
	if(!dwLoginStatus)
		dwLoginStatus = PoolSetting(pLocalConfig, pClientConfig);
	//
	return dwLoginStatus;
}

static DWORD LoginWizdBzl::DiskSettings() {
    // delete by james 20110311
    /*
    TCHAR szFileName[MAX_PATH];
    NFileUtility::ModuleBaseNameEx(szFileName);//添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
    _tcscat_s(szFileName, SERVICE_CACHE_NAME);
    InstallService::InstallServiceEx(szFileName, CLIENT_SERVICE_NAME);
    */
    return ERROR_OKAY;
}

static DWORD LoginWizdBzl::ValidateDiskSettings() {
    DWORD dwResult = 0x00;
    struct ClientConfig *pClientConfig = objParseConfig.GetClientConfig();
	//
    if(ValidateUserspaceExist()) dwResult |= SPACE_NOT_EXIST;
	else return dwResult;
    //
    DWORD dwFreeSize = 0x00;
	if(DiskUtility::GetFreeDiskSpaceSWD(&dwFreeSize, pClientConfig->szLocation)) {
		dwResult |= DRIVE_NOT_EXIST;
		return dwResult;
	}
	if(dwFreeSize < (pClientConfig->dwPoolLength+1)) {
		dwResult |= ERROR_FREE_SPACE;
		return dwResult;
	}
	//
    TCHAR szFileSystem[MAX_PATH], szDriveLetter[MAX_PATH];
	MKZEO(szFileSystem);
    DiskUtility::GetVolumeFileSystem(szFileSystem, struti::get_drive_root(szDriveLetter, pClientConfig->szLocation));
	if(_tcscmp(szFileSystem, _T("NTFS"))) {
		dwResult |= ERROR_NOT_NTFS;
		return dwResult;
	}
    //
    return dwResult;
}

const UINT uiFormatTimerID = 0x0001;
static DWORD LoginWizdBzl::FormatBegin(HWND hFormatDlgWnd) {
    if(!hFormatDlgWnd) return ERROR_FORMAT;
    //
    // run view progress thread
    SetTimer(hFormatDlgWnd, uiFormatTimerID, 500, ProgressTimerFunction);
    //
    // run create volume thread
    CWinThread *pCVolThread = AfxBeginThread(CVolThread, (LPVOID)NULL);
    //
    return ERROR_OKAY;
}

DWORD LoginWizdBzl::FormatFinishAction() {
    LWIZARD_OBJECT(m_dwCurrentNumbe)++;
    PerformNextRefresh();
    //
    return ERROR_OKAY;
}

static DWORD LoginWizdBzl::FormatFinish(HWND hFormatDlgWnd) {
    KillTimer(hFormatDlgWnd, uiFormatTimerID);
    return ERROR_OKAY;
}

static DWORD LoginWizdBzl::Finish() {
    // 拷贝文件属性xml
    struct ClientConfig *pClientConfig = objParseConfig.GetClientConfig();
    CopyFileForbid(pClientConfig->szLocation, pClientConfig->szFileForbid);
    // 注册SWDOverlays.DLL
    // System("dir");
    // 添加自动运行和启动图标服务
    SystemCall::AddiApplicaAuto(APPLICATION_NAME, CLIENT_EXECUTE_NAME);
    _tcscpy_s(pClientConfig->szAutoRun, _T("true"));
    /* delete by james 20110311	*/
    /*
    TCHAR szFileName[MAX_PATH];
    NFileUtility::ModuleBaseNameEx(szFileName);//添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
    _tcscat_s(szFileName, SERVICE_CACHE_NAME);
    InstallService::EnableService(szFileName, CLIENT_SERVICE_NAME);
    // kill the explorer
    SystemCall::EndApplica(EXPLORER_PROCESS_NAME);
    // start the explorer
    _tcscpy_s(szFileName, _tgetenv(_T("windir")));
    _tcscat_s(szFileName, _T("\\"));
    _tcscat_s(szFileName, SERVICE_CACHE_NAME);
    SystemCall::ExecuteApplicaEx(szFileName, EXPLORER_PROCESS_NAME, true);
    */
    return ERROR_OKAY;
}

void LoginWizdBzl::RefreshWindowNormal(DWORD dwCurrentNumbe) {
    LWIZARD_OBJECT(m_dwCurrentNumbe) = dwCurrentNumbe;
    LWIZARD_OBJECT(m_hCurrentPageHandle) = LWIZARD_OBJECT(m_pCVolWizardDlg)->DisplayWindowNormal(dwCurrentNumbe);
}

void LoginWizdBzl::PerformPrevRefresh() {
    switch(LWIZARD_OBJECT(m_dwCurrentNumbe)) {
    case COVER_PAGE:
        break;
    case LOGIN_PAGE:
		LWIZARD_OBJECT(m_dwCVolStatus) &= 0xFFFFFF00;
        break;
    case DISKSETTING_PAGE:
        LWIZARD_OBJECT(m_dwCVolStatus) &= 0xFFFF00FF;
		break;
    case START_FORMAT_PAGE:
        LWIZARD_OBJECT(m_dwCVolStatus) &= 0xFF00FFFF;
        break;
    case FINISH_FORMAT_PAGE:
        LWIZARD_OBJECT(m_dwCVolStatus) &= 0xFF00FFFF;
        LWIZARD_OBJECT(m_dwCurrentNumbe) --;
        break;
    case FINISH_PAGE:
        break;
    }
    if(LOGIN_PAGE != LWIZARD_OBJECT(m_dwCurrentNumbe)) {
        LWIZARD_OBJECT(m_dwCurrentNumbe) --;
        RefreshWindowNormal(LWIZARD_OBJECT(m_dwCurrentNumbe));
    }
}

void LoginWizdBzl::PerformNextRefresh() {
    int iBackupNumbe;
    DWORD dwActionValue;
    DWORD *pCurrentNumbe = LWIZARD_OBJECT(GetCurrentNumbePoint());
    CLoginWizdDlg *pCVolWizardDlg = LWIZARD_OBJECT(GetCVolWizardDlgPoint());
    //
    switch(*pCurrentNumbe) {
    case COVER_PAGE:
        pCVolWizardDlg->DisplayWindowAction(*pCurrentNumbe);
		LoginWizdBzl::Begain();
        pCVolWizardDlg->DisplayWindowFinish(*pCurrentNumbe);
        break;
    case LOGIN_PAGE:
        pCVolWizardDlg->DisplayWindowAction(*pCurrentNumbe);
        dwActionValue = LoginWizdBzl::Login();
        pCVolWizardDlg->DisplayWindowFinish(*pCurrentNumbe);
		LoginWizdBzl::SaveWizardConfiguration();
        if(dwActionValue) {
            if(IDYES == MessageBox(LWIZARD_OBJECT(m_pCVolWizardDlg)->m_hWnd,
                                   _T("认证用户出错!\n\n'是'重新认证,'否'跳过认证."),
                                   _T("Tip"),
                                   MB_YESNO|MB_ICONERROR|MB_DEFBUTTON1)) {
                LWIZARD_OBJECT(m_dwCVolStatus) &= 0xFFFFFF00;
                (*pCurrentNumbe)--;
            } else LWIZARD_OBJECT(m_dwCVolStatus) |= 0x000000FF;
        }
        break;
    case DISKSETTING_PAGE:
        pCVolWizardDlg->DisplayWindowAction(*pCurrentNumbe);
        //
        dwActionValue = LoginWizdBzl::DiskSettings();
        pCVolWizardDlg->DisplayWindowFinish(*pCurrentNumbe);
		LoginWizdBzl::SaveWizardConfiguration();
        // 检查是否存在用户数据
        dwActionValue = LoginWizdBzl::ValidateDiskSettings();
        if(SPACE_NOT_EXIST & dwActionValue) { // 不存在用户数据
            if(DRIVE_NOT_EXIST & dwActionValue) { // 所选驱动器不存在
                MessageBox(LWIZARD_OBJECT(m_pCVolWizardDlg)->m_hWnd, _T("目标磁盘不存在."), _T("Tip"), MB_OK|MB_ICONERROR);
                (*pCurrentNumbe) --;
                break;
            } else if(ERROR_NOT_NTFS & dwActionValue) { // 文件系统不是NTFS
                MessageBox(LWIZARD_OBJECT(m_pCVolWizardDlg)->m_hWnd, _T("目标磁盘文件系统不是NTFS."), _T("Tip"), MB_OK|MB_ICONERROR);
                (*pCurrentNumbe) --;
                break;
            } else if(ERROR_FREE_SPACE & dwActionValue) { // 剩余磁盘空间不足
                MessageBox(LWIZARD_OBJECT(m_pCVolWizardDlg)->m_hWnd, _T("目标磁盘没有足够的空间."), _T("Tip"), MB_OK|MB_ICONERROR);
                (*pCurrentNumbe) --;
                break;
            } else LWIZARD_OBJECT(m_dwCVolStatus) &= 0xFFFF00FF;
        } else { // 存在用户数据
            LWIZARD_OBJECT(m_dwCVolStatus) |= 0x0000FF00;
            (*pCurrentNumbe) += 2;
        }
        break;
    case START_FORMAT_PAGE:
        // 分配 挂载 格式化磁盘空间
        pCVolWizardDlg->DisplayWindowAction(*pCurrentNumbe);
        LoginWizdBzl::FormatBegin(LWIZARD_OBJECT(m_hCurrentPageHandle));
        pCVolWizardDlg->DisplayWindowFinish(*pCurrentNumbe);
        break;
    case FINISH_FORMAT_PAGE:
        pCVolWizardDlg->DisplayWindowAction(*pCurrentNumbe);
        iBackupNumbe = (*pCurrentNumbe);
        LoginWizdBzl::FormatFinish(LWIZARD_OBJECT(m_hCurrentPageHandle));
        if(LoginWizdBzl::GetFormatStatus()) { // msgbox
_LOG_WARN(_T("format page error, m_dwCVolStatus:%08X"), LWIZARD_OBJECT(m_dwCVolStatus));
            if(IDYES == MessageBox(LWIZARD_OBJECT(m_pCVolWizardDlg)->m_hWnd,
                                   _T("创建空间错误!\n\n'是'重新创建,'否'跳过创建."),
                                   _T("Tip"),
                                   MB_YESNO|MB_ICONERROR|MB_DEFBUTTON1)) {
                LWIZARD_OBJECT(m_dwCVolStatus) &= 0xFF00FFFF;
                (*pCurrentNumbe)-= 2;
            } else LWIZARD_OBJECT(m_dwCVolStatus) |= 0x00FF0000;
        }
        pCVolWizardDlg->DisplayWindowFinish(iBackupNumbe);
        break;
    case FINISH_PAGE:
        pCVolWizardDlg->DisplayWindowAction(*pCurrentNumbe);
        LoginWizdBzl::Finish();
        pCVolWizardDlg->DisplayWindowFinish(*pCurrentNumbe);
        LWIZARD_OBJECT(m_pCVolWizardDlg)->ExitApplica();
        break;
    }
    //
    if(FINISH_PAGE!=(*pCurrentNumbe) && START_FORMAT_PAGE!=(*pCurrentNumbe)) {
        (*pCurrentNumbe)++;
        RefreshWindowNormal(*pCurrentNumbe);
    }
}