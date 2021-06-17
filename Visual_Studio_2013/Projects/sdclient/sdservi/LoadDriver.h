// LoadDriver.h
#ifndef _LOADDRIVER_H_
#define _LOADDRIVER_H_

static BOOL SCMLoadDeviceDriver(LPCTSTR sDriverFileName, // 驱动程序完整路径名称
						 LPCTSTR szDriverName); // name of service LPCTSTR szDriverName
static BOOL SCMUnloadDeviceDriver(LPCTSTR szDriverName);// Name of service
static BOOL SCMCheckServiceExist(LPCTSTR szDriverName);// Name of service

//
static BOOL SCMStartService(LPCTSTR szDriverName);
static BOOL SCMQueryServiceStatus(LPCTSTR szDriverName);

//
BOOL __stdcall EnableDeviceDriver(LPCTSTR sDriverFileName, // 驱动程序完整路径名称
						 LPCTSTR szDriverName); // name of service LPCTSTR szDriverName
BOOL __stdcall DisableDeviceDriver(LPCTSTR szDriverName); // name of service LPCTSTR szDriverName
BOOL __stdcall CopyDriver(TCHAR *szToDriverDir, TCHAR *szFromDriverDir, TCHAR *szDriverName);

#define DRIVER_SERVICE_NAME			_T("sdclient") // name of service LPCTSTR szDriverName
#define I386_DRIVER_FNAME			_T("sdclient-i386.sys")
#define IA64_DRIVER_FNAME			_T("sdclient-ia64.sys")
#define AMD64_DRIVER_FNAME			_T("sdclient-amd64.sys")

#endif