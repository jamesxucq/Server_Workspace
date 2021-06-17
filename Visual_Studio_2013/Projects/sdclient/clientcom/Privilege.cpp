#include "StdAfx.h"

#include "Logger.h"
#include "Privilege.h"

static BOOL	EnablePrivilege(LPCTSTR pszPrivName, BOOL fEnable) {    
	BOOL fOk = FALSE;      
	HANDLE hToken;    

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {            
		TOKEN_PRIVILEGES tp = { 1 };        

		if( LookupPrivilegeValue(NULL, pszPrivName, &tp.Privileges[0].Luid)) {
			tp.Privileges[0].Attributes = fEnable ?  SE_PRIVILEGE_ENABLED : SE_PRIVILEGE_REMOVED;
			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
			fOk = (ERROR_SUCCESS == GetLastError());		
		}
		CloseHandle(hToken);	
	}	
	return(fOk);
}

// these two are required for the FILE_FLAG_BACKUP_SEMANTICS flag used in the call to //  CreateFile() to open the directory handle for ReadDirectoryChangesW
// just to make sure...it's on by default for all users.// <others here as needed>
static void PrivilegeEnabler(LPCTSTR *arPrivelegeNames, BOOL fEnable) {
	for(int inde = 0; NULL != arPrivelegeNames[inde]; ++inde) {
		if( !EnablePrivilege(arPrivelegeNames[inde], fEnable) ) {
			_LOG_DEBUG(_T("Unable to enable privilege: %s	--	GetLastError(): %d\n"), arPrivelegeNames[inde], GetLastError());
		}
	}
}

//
void NPrivilege::DrivVoluPrivi() {
	static int invoke_count;
	if(invoke_count++) return;
	//
	LPCTSTR arPrivelegeNames[] = { SE_LOAD_DRIVER_NAME, SE_MANAGE_VOLUME_NAME, NULL };
	PrivilegeEnabler(arPrivelegeNames, TRUE);
}