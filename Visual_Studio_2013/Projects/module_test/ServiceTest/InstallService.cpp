//InstallService.cpp

#include "stdafx.h"
#include <windows.h>
#include <WinSvc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InstallService.h"


//*********************************************************************
//利用服务控制器(SCM)加载驱动程序(该模块已经测试成功)
//*********************************************************************
BOOL SCMInstallService(LPCSTR sServFileName, //驱动程序完整路径名称
						LPCSTR sServiceName) //name of service
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
								sServiceName, //name of service
								sServiceName, //service name to display
								SERVICE_ALL_ACCESS, //desired access
								SERVICE_WIN32_OWN_PROCESS, //service type
								SERVICE_DEMAND_START, //start type
								SERVICE_ERROR_NORMAL, //error control type
								sServFileName, //path to service’s binary,TEXT(”c:\\boot.sys”)
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
									sServiceName, //sServiceName
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
BOOL SCMUninstallService(LPCSTR sServiceName)//Name of service
{
	SC_HANDLE schSCManager; // Handle to the service control manager
	SC_HANDLE schService;// Handle to the service to be stopped
	SERVICE_STATUS ss;

	//OutputDebugString(”Unloading Rootkit Driver.\n”);

	// Open a handle to the SC Manager database
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schSCManager)
	{
		//OutputDebugString(”OpenSCManager Error.\n”);
		//return GetLastError();
		return FALSE;
	}

	// Open a handle to the SC Manager database
	schService = OpenService( schSCManager, //SCManager database
							sServiceName, //Name of service
							SERVICE_ALL_ACCESS);
	if( NULL == schService )
	{
		//OutputDebugString(”OpenService Error.”);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// Sends a stop code to the main service.
	if (!ControlService(schService, SERVICE_CONTROL_STOP, &ss))
	{
		//OutputDebugString(”warning: could not stop service”);
		//return GetLastError();
		return FALSE;
	}

	// Marks the specified service for deletion from the service
	// control manager database
	if (!DeleteService(schService))
	{
		//OutputDebugString(”warning: could not delete service”);
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return TRUE;
}

BOOL SCMCheckServiceExist(LPCSTR sServiceName)//Name of service
{
	SC_HANDLE schSCManager; // Handle to the service control manager
	SC_HANDLE schService;// Handle to the service to be stopped
	//SERVICE_STATUS ss;

	//OutputDebugString(”Unloading Rootkit Driver.\n”);

	// Open a handle to the SC Manager database
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schSCManager)
	{
		//OutputDebugString(”OpenSCManager Error.\n”);
		//return GetLastError();
		return FALSE;
	}

	// Open a handle to the SC Manager database
	schService = OpenService( schSCManager, //SCManager database
							sServiceName, //Name of service
							SERVICE_ALL_ACCESS);
	if( NULL == schService )
	{
		//OutputDebugString(”OpenService Error.”);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return TRUE;
}

BOOL KillService(LPCSTR sServiceName) 
{ 
	// kill service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		long nError = GetLastError();
		char pTempstring[121];
		sprintf_s(pTempstring, "OpenSCManager failed, error code = %d\n", nError);
//		WriteLog(pLogFile, pTempstring);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, sServiceName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			long nError = GetLastError();
			char pTempstring[121];
			sprintf_s(pTempstring, "OpenService failed, error code = %d\n", nError);
//			WriteLog(pLogFile, pTempstring);
		}
		else
		{
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				long nError = GetLastError();
				char pTempstring[121];
				sprintf_s(pTempstring, "ControlService failed, error code = %d\n", nError);
//				WriteLog(pLogFile, pTempstring);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

BOOL SCMStartService(LPCSTR sServiceName)
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
							sServiceName, //Name of service
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

BOOL SCMStopService(LPCSTR sServiceName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	SERVICE_STATUS ss;

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
							sServiceName, //Name of service
							SERVICE_ALL_ACCESS);
	if( NULL == schService )
	{
		//OutputDebugString(”OpenService Error.”);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// Sends a stop code to the main service.
	if (!ControlService(schService, SERVICE_CONTROL_STOP, &ss))
	{
		//OutputDebugString(”warning: could not stop service”);
		//return GetLastError();
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL SCMQueryServiceStatus(LPCSTR sServiceName)
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
							sServiceName, //Name of service
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
		char pTempstring[121];
		sprintf_s(pTempstring, "QueryServiceStatus.\n ");
//		WriteLog(pLogFile, pTempstring);
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

BOOL InstallServiceEx(LPCSTR sServFileName, //驱动程序完整路径名称
						 LPCSTR sServiceName) //name of service LPCTSTR sDriverName
{
	BOOL bInstallReturn = FALSE;

	if(!sServFileName || !sServiceName)
		return FALSE;
	if(!SCMCheckServiceExist(sServiceName)) // 驱动程序不存在
		bInstallReturn = SCMInstallService(sServFileName, sServiceName); // 加载并启动驱动程序

	return bInstallReturn;
}

BOOL EnableService(LPCSTR sServFileName, //驱动程序完整路径名称
						 LPCSTR sServiceName) //name of service LPCTSTR sServiceName
{
	BOOL bEnableReturn = FALSE;
	int iEnableCount;

	if(!sServFileName || !sServiceName)
		return FALSE;

	if(!SCMCheckServiceExist(sServiceName)) // 驱动程序不存在
		SCMInstallService(sServFileName, sServiceName); // 加载并启动驱动程序
	iEnableCount = 0;
	while(!(bEnableReturn=SCMQueryServiceStatus(sServiceName)) && ENABLE_MAXTIMES>iEnableCount++) {
		SCMStartService(sServiceName);
	}

	return bEnableReturn;
}

BOOL DisableService(LPCSTR sServiceName) //name of service LPCTSTR sDriverName
{
	BOOL bDisableReturn = FALSE;
	int iDisableCount;

	if(!sServiceName) return FALSE;

	iDisableCount = 0;
	while((bDisableReturn=SCMCheckServiceExist(sServiceName)) && DISABLE_MAXTIMES>iDisableCount++) {
		SCMUninstallService(sServiceName);
	}

	return !bDisableReturn; 
}

