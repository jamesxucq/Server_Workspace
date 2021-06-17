#include "StdAfx.h"
#include "clientcom/lupdatecom.h"
#include "UpdateXml.h"
#include "LiveUtility.h"
#include "InsLibrary.h"
#include "UpdateBzl.h"

//
DWORD NUpdateBzl::SingleInstance(LPCTSTR szAppName) {
	HANDLE	hMutex = CreateMutex(NULL, TRUE, szAppName);  
	if(ERROR_ALREADY_EXISTS==GetLastError() || NULL==hMutex)  
		return ((DWORD)-1);
	return 0x00;
}

static BOOL ActiveUpdateExist() {
	//
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
	//
}

DWORD NUpdateBzl::ApplicaExecuteType() {
	if (SingleInstance(UPDATE_APPLICATION)) return EXECUTE_ONE_INSTANCE;
	//
	struct LocalConfig *pLocalConfig;
	if(NConfigBzl::Initialize(CLIENT_CONFIGURATION))  return EXECUTE_FAULT;
	if(!(pLocalConfig = NConfigBzl::GetLocalConfig()))
		return EXECUTE_CONFIGURATION;
	//
	DWORD dwExecuteType = EXECUTE_SUCCESS;
	if(ActiveUpdateExist()) dwExecuteType |= EXECUTE_ACTIVE_UPDATE;
	else dwExecuteType |= EXECUTE_LIVE_UPDATE ;
	return dwExecuteType;
}

static DWORD NUpdateBzl::ExecuteApplica(TCHAR *szLocation, TCHAR *szExecuteName) {
	TCHAR szApplicaPath[MAX_PATH];
//
	MKZEO(szApplicaPath);
	struti::get_name(szApplicaPath, szLocation, szExecuteName);
	Sleep(1024);
	if(SystemCall::ExecuteApplica(szApplicaPath)) return ((DWORD)-1);	
	//
	return 0x00;
}

static DWORD ParseUpdateXml(TCHAR *szVersion, LiveVec *pLiveVec, char *szUpdateXml) {
	objUpdateXml.Initialize(szVersion, pLiveVec);
	if(objUpdateXml.ParseUpdateXml(szUpdateXml, strlen(szUpdateXml)))
		return ((DWORD)-1);
	objUpdateXml.Finalize();
//
	return 0x00;
}

static DWORD UpdateLocalData(LiveVec *pLiveVec) {
	vector <struct LiveData *>::iterator iter;
	TCHAR szFromPath[MAX_PATH], szToPath[MAX_PATH];
	//
	struct LiveData *ldata;
	DWORD max_pid = NLiveVec::MaxLivePid(pLiveVec);
	DWORD inde;
	for(inde = 0x00; inde <= max_pid; inde++) {
		ldata = NLiveVec::GetLivePid(pLiveVec, inde);
		if(!ldata) continue;
		//
		if(CONV_PATH_OPE(ldata->command)) {
			lutility::Location(szFromPath, ldata->szNoA);
			lutility::Location(szToPath, ldata->szNoB);
		}
		//
		switch(ldata->command) {
		case LIVE_INVALID_CMD:
			break;
		case LIVE_REPLACE_CMD: //
			NDireUtility::MakeFileFolder(szToPath);
			MoveFileEx(szFromPath, szToPath, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED|MOVEFILE_WRITE_THROUGH);
			break;
		case LIVE_COPY_CMD: //
			NDireUtility::MakeFileFolder(szToPath);
			CopyFile(szFromPath, szToPath, FALSE);
			break;
		case LIVE_DELETE_CMD:
			DeleteFile(szFromPath);
			break;
		case LIVE_EXECUTE_CMD:
			SystemCall::ExecuteApplica(szFromPath);
			break;
		case LIVE_UNILIB_CMD:
			NInsLibrary::UnregLibrary(szFromPath);
		case LIVE_INSLIB_CMD:
			NInsLibrary::RegistLibrary(szFromPath);
			break;
		case LIVE_COPYDIR_CMD: //
			NDireUtility::MakeFileFolder(szToPath);
			NDireUtility::CopyDirectory(szFromPath, szToPath);
			break;
		case LIVE_DELDIR_CMD:
			NDireUtility::DeleDirectory(szFromPath);
			break;
		case LIVE_ERADIR_CMD:
			NDireUtility::EmptyDirectory(szFromPath);
			break;
		case LIVE_CREKEY_CMD: //
			lutility::AddiSubkey(ldata->szReqURI, ldata->szNoA, ldata->szNoB);
			break;
		case LIVE_SETREG_CMD:
			lutility::SetRegistry(ldata->szReqURI, ldata->szNoA, ldata->szNoB);
			break;
		case LIVE_DELKEY_CMD:
			lutility::DeliSubkey(ldata->szReqURI, ldata->szNoA);
			break;
		case LIVE_DELREG_CMD:
			lutility::DeliRegistry(ldata->szReqURI, ldata->szNoA);
			break;
		case LIVE_ENDRIV_CMD:
			LoadDriver::EnableDeviceDriver(ldata->szNoA, ldata->szNoB);
			break;
		case LIVE_DSDRIV_CMD:
			LoadDriver::DisableDeviceDriver(ldata->szNoA);
			break;
		case LIVE_KILL_CMD:
			SystemCall::EndApplica(ldata->szNoA);
			break;
		case LIVE_EDELKEY_CMD:
			lutility::EnumDeliRegistry(ldata->szReqURI, ldata->szNoA);
			break;
		}
		//
	}
	//
	return 0x00;
}

static BOOL EraseUpdates() {
	TCHAR szUpdatePath[MAX_PATH];
	lutility::UpdatePath(szUpdatePath, _T(""));
	return NDireUtility::EmptyDirectory(szUpdatePath);
}

static DWORD ReplaceUpdateData(TCHAR *szVersion) {
	LiveVec vLiveVec;
	char szUpdateXml[LOAD_BUFF_SIZE];
	//
	TCHAR szActiveUpdate[MAX_PATH];
	lutility::UpdatePath(szActiveUpdate, ACTIVE_UPDATE_NAME);
	//
	if(!lutility::LoadFile(szUpdateXml, szActiveUpdate, LOAD_BUFF_SIZE)) return ((DWORD)-1);
	if(ParseUpdateXml(szVersion, &vLiveVec, szUpdateXml)) {
		NLiveVec::DeleLiveVec(&vLiveVec);
		return ((DWORD)-1);
	}
	if(UpdateLocalData(&vLiveVec)) {
		NLiveVec::DeleLiveVec(&vLiveVec);
		return ((DWORD)-1);
	}
	//
	NLiveVec::DeleLiveVec(&vLiveVec);
	if(!EraseUpdates()) return ((DWORD)-1);
	return 0x00;
}

DWORD NUpdateBzl::ExecuteActiveUpdate() {
	TCHAR szLocation[MAX_PATH], szVersion[VERSION_LENGTH];
	MKZEO(szLocation); MKZEO(szVersion);
	//
	if(ReplaceUpdateData(szVersion)) return ((DWORD)-1);
	struct ClientConfig *pClientConfig;
	if(!(pClientConfig = NConfigBzl::GetClientConfig()))
		return ((DWORD)-1);
	//
	_tcscpy_s(pClientConfig->szVersion, szVersion);
	NConfigBzl::SaveConfiguration();
	//
	if(!NFileUtility::ModuleBaseName(szLocation)) 
		return ((DWORD)-1);
	if(ExecuteApplica(szLocation, CLIENT_EXECUTE_NAME)) 
		return ((DWORD)-1);
	return 0x00;
}