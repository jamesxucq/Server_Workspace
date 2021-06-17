//LoadDriver.cpp

#include "StdAfx.h"
#include <windows.h>
#include <Winsvc.h>

#include <stdio.h>
#include <tchar.h>

#include "LoadDriver.h"


//*********************************************************************
//利用服务控制器(SCM)加载驱动程序(该模块已经测试成功)
//*********************************************************************
BOOL SCMLoadDeviceDriver(LPCSTR sDriverFileName, //驱动程序完整路径名称
						LPCSTR sDriverName) //name of service
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	//Open a handle to the SC Manager database
	schSCManager = OpenSCManager(NULL, //local machine
								NULL, //ServicesActive database
								SC_MANAGER_ALL_ACCESS); //full access rights
	if (NULL == schSCManager)
	{
		//OutputDebugString(”OpenSCManager Error!!!”);
		return FALSE;
	}

	//install a service in a SCM database
	schService = CreateService(schSCManager, //SCManager database
								sDriverName, //name of service
								sDriverName, //service name to display
								SERVICE_ALL_ACCESS, //desired access
								SERVICE_KERNEL_DRIVER, //service type
								SERVICE_SYSTEM_START, //start type
								SERVICE_ERROR_NORMAL, //error control type
								sDriverFileName, //path to service’s binary,TEXT(”c:\\boot.sys”)
								NULL, // no load ordering group
								NULL, // no tag identifier
								NULL, // no dependencies
								NULL, // LocalSystem account
								NULL); // no password
	if (NULL == schService)
	{
		if (GetLastError() == ERROR_SERVICE_EXISTS)
		{
			//service exist
			schService = OpenService(schSCManager,
									sDriverName, //sDriverName
									SERVICE_ALL_ACCESS);
			if (NULL == schService)
			{
				//OutputDebugString(”OpenService Error!!!”);
				CloseServiceHandle(schService);
				return FALSE;
			}
		}
		else
		{
			//OutputDebugString(”CreateService Error!!!”);
			CloseServiceHandle(schService);
			return FALSE;
		}
	}

	//Start the driver service
	if ( !StartService(schService, // handle to service
						0, // number of arguments
						NULL) ) // no arguments
	{
		//An instance of the service is already running.
		if ( ERROR_SERVICE_ALREADY_RUNNING == GetLastError() )
		{
			// no real problem
		}
		else
		{
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
BOOL SCMUnloadDeviceDriver(LPCSTR sDriverName)//Name of service
{
	SC_HANDLE hSCManager; // Handle to the service control manager
	SC_HANDLE hService;// Handle to the service to be stopped
	SERVICE_STATUS ss;

	//OutputDebugString(”Unloading Rootkit Driver.\n”);

	// Open a handle to the SC Manager database
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager)
	{
		//OutputDebugString(”OpenSCManager Error.\n”);
		//return GetLastError();
		return FALSE;
	}

	// Open a handle to the SC Manager database
	hService = OpenService( hSCManager, //SCManager database
							sDriverName, //Name of service
							SERVICE_ALL_ACCESS);
	if( NULL == hService )
	{
		//OutputDebugString(”OpenService Error.”);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// Sends a stop code to the main service.
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss))
	{
		//OutputDebugString(”warning: could not stop service”);
		//return GetLastError();
		return FALSE;
	}

	// Marks the specified service for deletion from the service
	// control manager database
	if (!DeleteService(hService))
	{
		//OutputDebugString(”warning: could not delete service”);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return TRUE;
}


BOOL SCMCheckServiceExist(LPCSTR sDriverName)//Name of service
{
	SC_HANDLE hSCManager; // Handle to the service control manager
	SC_HANDLE hService;// Handle to the service to be stopped
	//SERVICE_STATUS ss;

	//OutputDebugString(”Unloading Rootkit Driver.\n”);

	// Open a handle to the SC Manager database
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager)
	{
		//OutputDebugString(”OpenSCManager Error.\n”);
		//return GetLastError();
		return FALSE;
	}

	// Open a handle to the SC Manager database
	hService = OpenService( hSCManager, //SCManager database
							sDriverName, //Name of service
							SERVICE_ALL_ACCESS);
	if( NULL == hService )
	{
		//OutputDebugString(”OpenService Error.”);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return TRUE;
}

BOOL SCMStartService(LPCSTR sDriverName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	//Open a handle to the SC Manager database
	schSCManager = OpenSCManager(NULL, //local machine
								NULL, //ServicesActive database
								SC_MANAGER_ALL_ACCESS); //full access rights
	if (NULL == schSCManager)
	{
		//OutputDebugString(”OpenSCManager Error!!!”);
		return FALSE;
	}

	// Open a handle to the SC Manager database
	schService = OpenService( schSCManager, //SCManager database
							sDriverName, //Name of service
							SERVICE_ALL_ACCESS);
	if( NULL == schService )
	{
		//OutputDebugString(”OpenService Error.”);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}


	//Start the driver service
	if ( !StartService(schService, // handle to service
						0, // number of arguments
						NULL) ) // no arguments
	{
		//An instance of the service is already running.
		if ( ERROR_SERVICE_ALREADY_RUNNING == GetLastError() )
		{
			// no real problem
		}
		else
		{
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return FALSE;
		}
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return TRUE;
}


BOOL SCMQueryServiceStatus(LPCSTR sDriverName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	//Open a handle to the SC Manager database
	schSCManager = OpenSCManager(NULL, //local machine
								NULL, //ServicesActive database
								SC_MANAGER_ALL_ACCESS); //full access rights
	if (NULL == schSCManager)
	{
		//OutputDebugString(”OpenSCManager Error!!!”);
		return FALSE;
	}

	// Open a handle to the SC Manager database
	schService = OpenService( schSCManager, //SCManager database
							sDriverName, //Name of service
							SERVICE_ALL_ACCESS);
	if( NULL == schService )
	{
		//OutputDebugString(”OpenService Error.”);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////
    SERVICE_STATUS ssStatus;  
    DWORD dwOldCheckPoint;  
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwStatus;
	if(!QueryServiceStatus(
		schService, // handle to service  
		&ssStatus)) // address of status information structure
    {
		//MyErrorExit( "QueryServiceStatus ");  
		printf( "QueryServiceStatus.\n ");
		return FALSE;
    }
 
    // Save the tick count and initial checkpoint.
    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;
    while(ssStatus.dwCurrentState == SERVICE_START_PENDING)  
    {  
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

		if(ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
			// The service is making progress.
			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
		else
		{
			if(GetTickCount()-dwStartTickCount > ssStatus.dwWaitHint)
			{
				// No progress made within the wait hint
				break;
			}
		}
	}  

	if (ssStatus.dwCurrentState == SERVICE_RUNNING)  
	{
		//printf( "StartService SUCCESS.\n ");  
		//dwStatus = NO_ERROR;
		dwStatus = TRUE;
	}
	else  
	{  
		//printf( "\nService not started. \n ");
		//printf( "   Current State: %d\n ", ssStatus.dwCurrentState);  
		//printf( "   Exit Code: %d\n ", ssStatus.dwWin32ExitCode);  
		//printf( "   Service Specific Exit Code: %d\n ",  
		//	ssStatus.dwServiceSpecificExitCode);  
		//printf( "   Check Point: %d\n ", ssStatus.dwCheckPoint);  
		//printf( "   Wait Hint: %d\n ", ssStatus.dwWaitHint);  
		//dwStatus = GetLastError();
		dwStatus = FALSE;
	}  

	//////////////////////////////////////////////////////////////////
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return dwStatus;
}

#define ENABLE_DRIVER_MAX_AUTO_RETRIES				16
#define DISABLE_DRIVER_MAX_AUTO_RETRIES				16
#define ENABLE_DRIVER_AUTO_RETRY_DELAY				64
#define DISABLE_DRIVER_AUTO_RETRY_DELAY				64
BOOL EnableDeviceDriver(LPCSTR sDriverFileName, //驱动程序完整路径名称
						 LPCSTR sDriverName) //name of service LPCTSTR sDriverName
{
	BOOL bEnableReturn = FALSE;
	int iEnableCount;

	if(!sDriverFileName || !sDriverName)
		return FALSE;

	if(!SCMCheckServiceExist(sDriverName)) // 驱动程序不存在
		SCMLoadDeviceDriver(sDriverFileName, sDriverName); // 加载并启动驱动程序

	iEnableCount = 0;
	while(!(bEnableReturn=SCMQueryServiceStatus(sDriverName)) && ENABLE_DRIVER_MAX_AUTO_RETRIES>iEnableCount++) {
		SCMStartService(sDriverName);
	}
	// modify by james 20110320
/*
	while(!(bEnableReturn=SCMStartService(sDriverName)) && ENABLE_DRIVER_MAX_AUTO_RETRIES>iEnableCount++) 
		Sleep(ENABLE_DRIVER_AUTO_RETRY_DELAY);
*/

	return bEnableReturn;
}

BOOL DisableDeviceDriver(LPCSTR sDriverName) //name of service LPCTSTR sDriverName
{ 
	BOOL bDisableReturn = FALSE;
	int iDisableCount;

	if(!sDriverName) return FALSE;

	iDisableCount = 0;
	while((bDisableReturn=SCMCheckServiceExist(sDriverName)) && DISABLE_DRIVER_MAX_AUTO_RETRIES>iDisableCount++) {
		SCMUnloadDeviceDriver(sDriverName); 
		Sleep(DISABLE_DRIVER_AUTO_RETRY_DELAY);
	}
	// modify by james 20110320
/*
	while(!(bDisableReturn=SCMUnloadDeviceDriver(sDriverName)) && DISABLE_DRIVER_MAX_AUTO_RETRIES>iDisableCount++) 
		Sleep(DISABLE_DRIVER_AUTO_RETRY_DELAY);
*/

	return !bDisableReturn;
}

BOOL CopyDriver(char *sDestDriverPath, char *sSourceDriverPath, char *sDriverName)
{
	char sDestDriverName[MAX_PATH];
	char sSourceDriverName[MAX_PATH];

	if(!sDriverName || !sDestDriverPath || !sSourceDriverPath)
		return false;
	_tcscpy_s(sDestDriverName, sDestDriverPath);
	_tcscat_s(sDestDriverName, _T("\\"));
	_tcscat_s(sDestDriverName, sDriverName);

	_tcscpy_s(sSourceDriverName, sSourceDriverPath);
	_tcscat_s(sSourceDriverName, _T("\\"));
	_tcscat_s(sSourceDriverName, sDriverName);

	return CopyFile(sSourceDriverName, sDestDriverName, TRUE);
}