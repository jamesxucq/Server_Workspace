//LoadDriver.h
#ifndef _LOADDRIVER_H_
#define _LOADDRIVER_H_

static BOOL SCMLoadDeviceDriver(LPCSTR sDriverFileName, //驱动程序完整路径名称
						 LPCSTR sDriverName); //name of service LPCTSTR sDriverName
static BOOL SCMUnloadDeviceDriver(LPCSTR sDriverName);//Name of service
static BOOL SCMCheckServiceExist(LPCSTR sDriverName);//Name of service

//////////////////////////////////////////////////////////////////////////////
static BOOL SCMStartService(LPCSTR sDriverName);
static BOOL SCMQueryServiceStatus(LPCSTR sDriverName);

//////////////////////////////////////////////////////////////////////////////
BOOL EnableDeviceDriver(LPCSTR sDriverFileName, //驱动程序完整路径名称
						 LPCSTR sDriverName); //name of service LPCTSTR sDriverName
BOOL DisableDeviceDriver(LPCSTR sDriverName); //name of service LPCTSTR sDriverName
BOOL CopyDriver(char *sDestDriverPath, char *sSourceDriverPath, char *sDriverName);

#define DRIVER_SERVICE_NAME  _T("sdclient")//name of service LPCTSTR sDriverName
#define I386_DRIVER_FNAME  _T("sdclient-i386.sys")
#define IA64_DRIVER_FNAME  _T("sdclient-ia64.sys")
#define AMD64_DRIVER_FNAME  _T("sdclient-amd64.sys")

#endif