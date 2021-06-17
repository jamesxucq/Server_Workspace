#include "StdAfx.h"

#include "clientcom/lupdatecom.h"
#include "HttpUtility.h"
#include "LiveVec.h"
#include "LiveUtility.h"
#include "UpdateXml.h"
#include "DowndBzl.h"


CDowndObject::CDowndObject(VOID):
    m_dwDowndThreadStatus(0)
{
    memset(update_xml, 0, LOAD_BUFF_SIZE);
}

CDowndObject::~CDowndObject(VOID)
{
}

class CDowndObject objDowndObject;

BOOL ActiveUpdate(TCHAR *szActiveUpdate, ServAddress *pUpdateAddress, struct NetworkConfig *pNetworkConfig) {
    HANDLE hActive = CreateFile( szActiveUpdate,
                          GENERIC_WRITE,
                          NULL,
                          NULL,
                          CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    if( INVALID_HANDLE_VALUE == hActive ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return FALSE;
    }
    DWORD dwResult = SetFilePointer(hActive, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        CloseHandle(hActive);
        return FALSE;
    }
    //
    char active_update[URI_LENGTH];
    strcpy_s(active_update, "/");
    strcat_s(active_update, UPDATE_CHAR_NAME);
    if(NHttpUtility::HttpDownd(hActive, active_update, pUpdateAddress, pNetworkConfig)) {
        CloseHandle(hActive);
        return FALSE;
    }
    //
    CloseHandle(hActive);
    return TRUE;
}

DWORD CreatUpdateActive(TCHAR *szUpdateBackup) {
    TCHAR szActiveUpdate[MAX_PATH];
	lutility::UpdatePath(szActiveUpdate, ACTIVE_UPDATE_NAME);
    return MoveFileEx(szUpdateBackup, szActiveUpdate, MOVEFILE_REPLACE_EXISTING);
}

DWORD CreateUpdatesDirectory() {
    TCHAR szUpdateDirectory[MAX_PATH];
	lutility::UpdatePath(szUpdateDirectory, _T(""));
    return NDireUtility::RecursMakeFolderW(szUpdateDirectory);
}

static DWORD ParseUpdateXml(LiveVec *pLiveVec, char *szUpdateXml) {
    TCHAR szVersion[VERSION_LENGTH];
//
    objUpdateXml.Initialize(szVersion, pLiveVec);
    if(objUpdateXml.ParseUpdateXml(szUpdateXml, strlen(szUpdateXml)))
        return ((DWORD)-1);
    objUpdateXml.Finalize();
//
    return 0x00;
}

BOOL ReplaceData(struct LiveData *ldata, ServAddress *pUpdateAddress, struct NetworkConfig *pNetworkConfig) {
    HANDLE hActive;
    TCHAR szActiveUpdate[MAX_PATH];
//
    lutility::Location(szActiveUpdate, ldata->szNoA);
    hActive = CreateFile( szActiveUpdate,
                          GENERIC_WRITE,
                          NULL,
                          NULL,
                          CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);
    if( INVALID_HANDLE_VALUE == hActive ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return FALSE;
    }
    DWORD dwResult = SetFilePointer(hActive, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        CloseHandle(hActive);
        return FALSE;
    }
    //
    if(NHttpUtility::HttpDownd(hActive, ldata->szReqURI, pUpdateAddress, pNetworkConfig)) {
        CloseHandle(hActive);
        return FALSE;
    }
    //
    CloseHandle(hActive);
    return TRUE;
}

DWORD NDowndBzl::PerfromDownload() {
    ServAddress *pUpdateAddress = &(NConfigBzl::GetServParam())->tUpdateAddress;
    struct NetworkConfig *pNetworkConfig = NConfigBzl::GetNetworkConfig();
    //
    TCHAR szActiveUpdate[MAX_PATH];
	lutility::UpdatePath(szActiveUpdate, ACTIVE_PATH_BACKUP);
    //
    CreateUpdatesDirectory();
    if(!ActiveUpdate(szActiveUpdate, pUpdateAddress, pNetworkConfig)) return DOWND_VALUE_ACTIVE;
    //
    LiveVec vLiveVec;
    char szUpdateXml[LOAD_BUFF_SIZE];
    if(!lutility::LoadFile(szUpdateXml, szActiveUpdate, LOAD_BUFF_SIZE)) return DOWND_LOAD_FILE;
    if(ParseUpdateXml(&vLiveVec, szUpdateXml)) {
        NLiveVec::DeleLiveVec(&vLiveVec);
        return DOWND_VALUE_UPDATE;
    }
    //
    vector <struct LiveData *>::iterator iter;
    for(iter = vLiveVec.begin(); vLiveVec.end() != iter; ++iter) {
        if(MUST_DOWND_OPE((*iter)->command)) ReplaceData(*iter, pUpdateAddress, pNetworkConfig);
    }
    //
    NLiveVec::DeleLiveVec(&vLiveVec);
    CreatUpdateActive(szActiveUpdate);
    return DOWND_VALUE_SUCCESS;
}
