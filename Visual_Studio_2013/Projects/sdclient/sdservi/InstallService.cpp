// InstallService.cpp

#include "StdAfx.h"
#include <windows.h>
#include <Winsvc.h>
#include <tchar.h>

#include "ulog.h"
#include "InstallService.h"

//*********************************************************************
//利用服务控制器(SCM)加载驱动程序(该模块已经测试成功)
//*********************************************************************
static BOOL SCMInstallService(LPCTSTR sServFileName, // 驱动程序完整路径名称
					   LPCTSTR sServiceName) { // name of service
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    // Open a handle to the SC Manager database
    schSCManager = OpenSCManager(NULL, // local machine
                                 NULL, // ServicesActive database
                                 SC_MANAGER_ALL_ACCESS); // full access rights
    if (NULL == schSCManager) {
        // OutputDebugString("OpenSCManager Error!");
        return FALSE;
    }

    // install a service in a SCM database
    schService = CreateService(schSCManager, // SCManager database
                               sServiceName, // name of service
                               sServiceName, // service name to display
                               SERVICE_ALL_ACCESS, // desired access
                               SERVICE_WIN32_OWN_PROCESS, // service type
                               SERVICE_AUTO_START, // start type
                               SERVICE_ERROR_NORMAL, // error control type
                               sServFileName, // path to service’s binary,TEXT("c:\\boot.sys")
                               NULL, // no load ordering group
                               NULL, // no tag identifier
                               NULL, // no dependencies
                               NULL, // LocalSystem account
                               NULL); // no password
    if (NULL == schService) {
        if (ERROR_SERVICE_EXISTS == GetLastError()) {
            // service exist
            schService = OpenService(schSCManager,
                                     sServiceName, // sServiceName
                                     SERVICE_ALL_ACCESS);
            if (NULL == schService) {
                // OutputDebugString("OpenService Error!");
                CloseServiceHandle(schService);
                return FALSE;
            }
        } else {
            // OutputDebugString("CreateService Error!");
            CloseServiceHandle(schService);
            return FALSE;
        }
    }

    // Start the driver service
    if ( !StartService(schService, // handle to service
                       0, // number of arguments
                       NULL) ) { // no arguments
        // An instance of the service is already running.
        if ( ERROR_SERVICE_ALREADY_RUNNING == GetLastError() ) {
            // no real problem
        } else {
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return FALSE;
        }
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return TRUE;
}

//**************************************************************************
// 通过SCM卸载驱动程序
// If the operation is successful, returns ERROR_SUCCESS. Otherwise,
// returns a system error code.
//**************************************************************************
static BOOL SCMUninstallService(LPCTSTR sServiceName) { // Name of service
    SC_HANDLE hSCManager; // Handle to the service control manager
    SC_HANDLE hService;// Handle to the service to be stopped
    SERVICE_STATUS ss;

    // OutputDebugString("Unloading Rootkit Driver.\n");

    // Open a handle to the SC Manager database
    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == hSCManager) {
        // OutputDebugString("OpenSCManager Error.\n");
// logger(_T("c:\\install.log"), _T("OpenSCManager Error.\r\n"));
        // return GetLastError();
        return FALSE;
    }

    // Open a handle to the SC Manager database
    hService = OpenService( hSCManager, // SCManager database
                            sServiceName, // Name of service
                            SERVICE_ALL_ACCESS);
    if( NULL == hService ) {
        // OutputDebugString("OpenService Error.");
// logger(_T("c:\\install.log"), _T("OpenService Error.\r\n"));
        CloseServiceHandle(hSCManager);
        return FALSE;
    }
    // Sends a stop code to the main service.
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss)) {
        // OutputDebugString("warning: could not stop service.");
        // return GetLastError();
		if(ERROR_SERVICE_NOT_ACTIVE != GetLastError()) {
// logger(_T("c:\\install.log"), _T("warning: could not stop service. GetLastError:%d\r\n"), GetLastError());
			return FALSE;		
		}
// logger(_T("c:\\install.log"), _T("warning: service stopped.\r\n"));
    }
    // Marks the specified service for deletion from the service
    // control manager database
    if (!DeleteService(hService)) {
        // OutputDebugString("warning: could not delete service");
// logger(_T("c:\\install.log"), _T("warning: could not delete service.\r\n"));
        return FALSE;
    }
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
	//
    return TRUE;
}

static BOOL SCMCheckServiceExist(LPCTSTR sServiceName) {
    SC_HANDLE hSCManager; // Handle to the service control manager
    SC_HANDLE hService;// Handle to the service to be stopped
    // SERVICE_STATUS ss;

    // OutputDebugString("Unloading Rootkit Driver.\n");

    // Open a handle to the SC Manager database
    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == hSCManager) {
        // OutputDebugString("OpenSCManager Error.\n");
// logger(_T("c:\\install.log"), _T("OpenSCManager Error.GetLastError:%d\r\n"), GetLastError());
        // return GetLastError();
        return FALSE;
    }

    // Open a handle to the SC Manager database
    hService = OpenService( hSCManager, // SCManager database
                            sServiceName, // Name of service
                            SERVICE_ALL_ACCESS);
    if( NULL == hService ) {
        // OutputDebugString("OpenService Error.");
// logger(_T("c:\\install.log"), _T("OpenService Error.\r\n"));
        CloseServiceHandle(hSCManager);
        return FALSE;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
	//
    return TRUE;
}

static BOOL KillService(LPCTSTR sServiceName) {
    // kill service with given name
    SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (0==schSCManager) {
        long lError = GetLastError();
        // TCHAR pTempstring[121];
        // sprintf(pTempstring, "OpenSCManager failed, error code = %d\n", lError);
        // WriteLog(pLogFile, pTempstring);
    } else {
        // open the service
        SC_HANDLE schService = OpenService( schSCManager, sServiceName, SERVICE_ALL_ACCESS);
        if (0==schService) {
            long lError = GetLastError();
            // TCHAR pTempstring[121];
            // sprintf(pTempstring, "OpenService failed, error code = %d\n", lError);
            // WriteLog(pLogFile, pTempstring);
        } else {
            // call ControlService to kill the given service
            SERVICE_STATUS status;
            if(ControlService(schService,SERVICE_CONTROL_STOP,&status)) {
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return TRUE;
            } else {
                long lError = GetLastError();
                // TCHAR pTempstring[121];
                // sprintf(pTempstring, "ControlService failed, error code = %d\n", lError);
                // WriteLog(pLogFile, pTempstring);
            }
            CloseServiceHandle(schService);
        }
        CloseServiceHandle(schSCManager);
    }
    return FALSE;
}

static BOOL SCMStartService(LPCTSTR sServiceName) {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    // Open a handle to the SC Manager database
    schSCManager = OpenSCManager(NULL, // local machine
                                 NULL, // ServicesActive database
                                 SC_MANAGER_ALL_ACCESS); // full access rights
    if (NULL == schSCManager) {
        // OutputDebugString("OpenSCManager Error!");
// logger(_T("c:\\install.log"), _T("OpenSCManager Error!\r\n"));
        return FALSE;
    }

    // Open a handle to the SC Manager database
    schService = OpenService( schSCManager, // SCManager database
                              sServiceName, // Name of service
                              SERVICE_ALL_ACCESS);
    if( NULL == schService ) {
        // OutputDebugString("OpenService Error.");
// logger(_T("c:\\install.log"), _T("OpenSCManager Error!\r\n"));
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    // Start the driver service
    if ( !StartService(schService, // handle to service
                       0, // number of arguments
                       NULL) ) { // no arguments
        // An instance of the service is already running.
        if ( ERROR_SERVICE_ALREADY_RUNNING == GetLastError() ) {
            // no real problem
        } else {
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return FALSE;
        }
    }
	//
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return TRUE;
}

static BOOL SCMQueryServiceStatus(LPCTSTR sServiceName) {
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    // Open a handle to the SC Manager database
    schSCManager = OpenSCManager(NULL, // local machine
                                 NULL, // ServicesActive database
                                 SC_MANAGER_ALL_ACCESS); // full access rights
    if (NULL == schSCManager) {
        // OutputDebugString("OpenSCManager Error!");
        return FALSE;
    }

    // Open a handle to the SC Manager database
    schService = OpenService( schSCManager, // SCManager database
                              sServiceName, // Name of service
                              SERVICE_ALL_ACCESS);
    if( NULL == schService ) {
        // OutputDebugString("OpenService Error.");
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    //
    SERVICE_STATUS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD ulStartTickCount;
    DWORD dwWaitTime;
    DWORD dwStatus;
    if(!QueryServiceStatus(
                schService, // handle to service
                &ssStatus)) { // address of status information structure
        // MyErrorExit( "QueryServiceStatus ");
        return FALSE;
    }

    // Save the tick count and initial checkpoint.
    ulStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;
    while(SERVICE_START_PENDING == ssStatus.dwCurrentState) {
        // Do not wait longer than the wait hint. A good interval is
        // one tenth the wait hint, but no less than 1 second and no
        // more than 10 seconds.
        dwWaitTime = ssStatus.dwWaitHint / 10;
        if(dwWaitTime < 1000) dwWaitTime = 1000;
        else if(dwWaitTime > 10000) dwWaitTime = 10000;
        Sleep(dwWaitTime);

        // Check the status again.
        if(!QueryServiceStatus(
                    schService, // handle to service
                    &ssStatus)) // address of structure
            break;

        if(ssStatus.dwCheckPoint > dwOldCheckPoint) {
            // The service is making progress.
            ulStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        } else {
            if(GetTickCount()-ulStartTickCount > ssStatus.dwWaitHint) {
                // No progress made within the wait hint
                break;
            }
        }
    }

    if (SERVICE_RUNNING == ssStatus.dwCurrentState) {
        // fprintf(stderr, "StartService SUCCESS.\n ");
        // dwStatus = NO_ERROR;
        dwStatus = TRUE;
    } else {
        // fprintf(stderr, "\nService not started. \n ");
        // fprintf(stderr, "   Current State: %d\n ", ssStatus.dwCurrentState);
        // fprintf(stderr, "   Exit Code: %d\n ", ssStatus.dwWin32ExitCode);
        // fprintf(stderr, "   Service Specific Exit Code: %d\n ",
        //	ssStatus.dwServiceSpecificExitCode);
        // fprintf(stderr, "   Check Point: %d\n ", ssStatus.dwCheckPoint);
        // fprintf(stderr, "   Wait Hint: %d\n ", ssStatus.dwWaitHint);
        // dwStatus = GetLastError();
        dwStatus = FALSE;
    }

    //
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
// logger(_T("c:\\install.log"), _T("query service dwStatus:%d\r\n"), dwStatus);
    return dwStatus;
}

BOOL __stdcall InstallServiceEx(LPCTSTR sServFileName, // 驱动程序完整路径名称
					LPCTSTR sServiceName) { // name of service LPCTSTR szDriverName
    BOOL bInstallValue = FALSE;
	//
    if(!sServFileName || !sServiceName)
        return FALSE;
	//
    if(!SCMCheckServiceExist(sServiceName)) // 驱动程序不存在
        bInstallValue = SCMInstallService(sServFileName, sServiceName); // 加载并启动驱动程序
	//
// logger(_T("c:\\install.log"), _T("service bInstallValue:%d\r\n"), bInstallValue);
    return bInstallValue;
}

#define ENABLE_SERVICE_MAX_AUTO_RETRIES			16
#define DISABLE_SERVICE_MAX_AUTO_RETRIES		16
#define ENABLE_SERVICE_AUTO_RETRY_DELAY			64
#define DISABLE_SERVICE_AUTO_RETRY_DELAY		64
BOOL __stdcall EnableService(LPCTSTR sServFileName, // 驱动程序完整路径名称
				LPCTSTR sServiceName) { // name of service LPCTSTR sServiceName
    BOOL bEnableValue = FALSE;
	//
    if(!sServFileName || !sServiceName)
        return FALSE;
	//
    if(!SCMCheckServiceExist(sServiceName)) // 驱动程序不存在
        SCMInstallService(sServFileName, sServiceName); // 加载并启动驱动程序
    int dwEnableCount = 0;
    while(!(bEnableValue=SCMQueryServiceStatus(sServiceName)) && ENABLE_SERVICE_MAX_AUTO_RETRIES>dwEnableCount++) {
        SCMStartService(sServiceName);
        Sleep(ENABLE_SERVICE_AUTO_RETRY_DELAY);
    }
	//
// logger(_T("c:\\install.log"), _T("service bEnableValue:%d\r\n"), bEnableValue);
    return bEnableValue;
}

BOOL __stdcall DisableService(LPCTSTR sServiceName) { // name of service LPCTSTR szDriverName
    BOOL bDisableValue = FALSE;
	//
    if(!sServiceName) return FALSE;
	//
    int dwEnableCount = 0;
    while((bDisableValue=SCMCheckServiceExist(sServiceName)) && DISABLE_SERVICE_MAX_AUTO_RETRIES>dwEnableCount++) {
        SCMUninstallService(sServiceName);
        Sleep(DISABLE_SERVICE_AUTO_RETRY_DELAY);
// logger(_T("c:\\install.log"), _T("service dwEnableCount:%d bDisableValue:%d\r\n"), dwEnableCount, bDisableValue);
    }
	//
// logger(_T("c:\\install.log"), _T("service bDisableValue:%d\r\n"), bDisableValue);
    return !bDisableValue;
}