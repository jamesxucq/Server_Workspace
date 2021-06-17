// LoadDriver.cpp

#include "StdAfx.h"
#include <windows.h>
#include <Winsvc.h>

#include <stdio.h>
#include <tchar.h>

#include "ulog.h"
#include "LoadDriver.h"


//*********************************************************************
//利用服务控制器(SCM)加载驱动程序(该模块已经测试成功)
//*********************************************************************
static BOOL SCMLoadDeviceDriver(LPCTSTR sDriverFileName, // 驱动程序完整路径名称
                         LPCTSTR szDriverName) // name of service
{
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
                               szDriverName, // name of service
                               szDriverName, // service name to display
                               SERVICE_ALL_ACCESS, // desired access
                               SERVICE_KERNEL_DRIVER, // service type
                               SERVICE_SYSTEM_START, // start type
                               SERVICE_ERROR_NORMAL, // error control type
                               sDriverFileName, // path to service’s binary,TEXT("c:\\boot.sys")
                               NULL, // no load ordering group
                               NULL, // no tag identifier
                               NULL, // no dependencies
                               NULL, // LocalSystem account
                               NULL); // no password
    if (NULL == schService) {
        if (GetLastError() == ERROR_SERVICE_EXISTS) {
            // service exist
            schService = OpenService(schSCManager,
                                     szDriverName, //  szDriverName
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
static BOOL SCMUnloadDeviceDriver(LPCTSTR szDriverName) // Name of service
{
    SC_HANDLE hSCManager; // Handle to the service control manager
    SC_HANDLE hService;// Handle to the service to be stopped
    SERVICE_STATUS ss;

    // OutputDebugString("Unloading Rootkit Driver.\n");

    // Open a handle to the SC Manager database
    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == hSCManager) {
        // OutputDebugString("OpenSCManager Error.\n");
        // return GetLastError();
        return FALSE;
    }

    // Open a handle to the SC Manager database
    hService = OpenService( hSCManager, // SCManager database
                            szDriverName, // Name of service
                            SERVICE_ALL_ACCESS);
    if( NULL == hService ) {
        // OutputDebugString("OpenService Error.");
        CloseServiceHandle(hSCManager);
        return FALSE;
    }

    // Sends a stop code to the main service.
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss)) {
        // OutputDebugString("warning: could not stop service");
        // return GetLastError();
        return FALSE;
    }

    // Marks the specified service for deletion from the service
    // control manager database
    if (!DeleteService(hService)) {
        // OutputDebugString("warning: could not delete service");
        return FALSE;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return TRUE;
}


static BOOL SCMCheckServiceExist(LPCTSTR szDriverName) // Name of service
{
    SC_HANDLE hSCManager; // Handle to the service control manager
    SC_HANDLE hService;// Handle to the service to be stopped
    // SERVICE_STATUS ss;

    // OutputDebugString("Unloading Rootkit Driver.\n");

    // Open a handle to the SC Manager database
    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == hSCManager) {
        // OutputDebugString("OpenSCManager Error.\n");
        // return GetLastError();
        return FALSE;
    }

    // Open a handle to the SC Manager database
    hService = OpenService( hSCManager, // SCManager database
                            szDriverName, // Name of service
                            SERVICE_ALL_ACCESS);
    if( NULL == hService ) {
        // OutputDebugString("OpenService Error.”);
        CloseServiceHandle(hSCManager);
        return FALSE;
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return TRUE;
}

static BOOL SCMStartService(LPCTSTR szDriverName)
{
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
                              szDriverName, // Name of service
                              SERVICE_ALL_ACCESS);
    if( NULL == schService ) {
        // OutputDebugString("OpenService Error.");
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

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return TRUE;
}


static BOOL SCMQueryServiceStatus(LPCTSTR szDriverName)
{
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
                              szDriverName, // Name of service
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
        fprintf(stderr, "QueryServiceStatus.\n ");
        return FALSE;
    }

    // Save the tick count and initial checkpoint.
    ulStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;
    while(ssStatus.dwCurrentState == SERVICE_START_PENDING) {
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

    if (ssStatus.dwCurrentState == SERVICE_RUNNING) {
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
// logger(_T("c:\\install.log"), _T("query driver dwStatus:%d\r\n"), dwStatus);
    return dwStatus;
}

#define ENABLE_DRIVER_MAX_AUTO_RETRIES				16
#define DISABLE_DRIVER_MAX_AUTO_RETRIES				16
#define ENABLE_DRIVER_AUTO_RETRY_DELAY				64
#define DISABLE_DRIVER_AUTO_RETRY_DELAY				64
BOOL __stdcall EnableDeviceDriver(LPCTSTR sDriverFileName, // 驱动程序完整路径名称
                        LPCTSTR szDriverName) // name of service LPCTSTR szDriverName
{
    BOOL bEnableValue = FALSE;
    DWORD dwEnableCount;

    if(!sDriverFileName || !szDriverName)
        return FALSE;

    if(!SCMCheckServiceExist(szDriverName)) // 驱动程序不存在
        SCMLoadDeviceDriver(sDriverFileName, szDriverName); // 加载并启动驱动程序

    dwEnableCount = 0;
    while(!(bEnableValue=SCMQueryServiceStatus(szDriverName)) && ENABLE_DRIVER_MAX_AUTO_RETRIES>dwEnableCount++) {
        SCMStartService(szDriverName);
// logger(_T("c:\\install.log"), _T("driver dwEnableCount:%d bEnableValue:%d\r\n"), dwEnableCount, bEnableValue);
    }
// logger(_T("c:\\install.log"), _T("driver bEnableValue:%d\r\n"), bEnableValue);
    return bEnableValue;
}

BOOL __stdcall DisableDeviceDriver(LPCTSTR szDriverName) // name of service LPCTSTR szDriverName
{
    BOOL bDisableValue = FALSE;
    DWORD dwEnableCount;

    if(!szDriverName) return FALSE;

    dwEnableCount = 0;
    while((bDisableValue=SCMCheckServiceExist(szDriverName)) && DISABLE_DRIVER_MAX_AUTO_RETRIES>dwEnableCount++) {
        SCMUnloadDeviceDriver(szDriverName);
        Sleep(DISABLE_DRIVER_AUTO_RETRY_DELAY);
// logger(_T("c:\\install.log"), _T("driver dwEnableCount:%d bDisableValue:%d\r\n"), dwEnableCount, bDisableValue);
    }
// logger(_T("c:\\install.log"), _T("driver bDisableValue:%d\r\n"), bDisableValue);
    return !bDisableValue;
}

BOOL __stdcall CopyDriver(TCHAR *szToDriverDir, TCHAR *szFromDriverDir, TCHAR *szDriverName) {
    TCHAR szToDriver[MAX_PATH];
    TCHAR szFromDriver[MAX_PATH];

    if(!szDriverName || !szToDriverDir || !szFromDriverDir)
        return false;
    _stprintf_s(szFromDriver, _T("%s\\%s"), szFromDriverDir, szDriverName);
    _stprintf_s(szToDriver, _T("%s\\%s"), szToDriverDir, szDriverName);

    return CopyFile(szFromDriver, szToDriver, TRUE);
}