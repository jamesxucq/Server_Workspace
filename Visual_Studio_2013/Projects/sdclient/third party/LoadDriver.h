// LoadDriver.h
#ifndef _LOADDRIVER_H_
#define _LOADDRIVER_H_

namespace LoadDriver {
	static BOOL SCMLoadDeviceDriver(LPCWSTR sDriverFileName, // 驱动程序完整路径名称
		LPCWSTR szDriverName); // name of service LPCTSTR szDriverName
	static BOOL SCMUnloadDeviceDriver(LPCWSTR szDriverName);// Name of service
	static BOOL SCMCheckServiceExist(LPCWSTR szDriverName);// Name of service
	//
	static BOOL SCMStartService(LPCWSTR szDriverName);
	static BOOL SCMQueryServiceStatus(LPCWSTR szDriverName);
	//
	BOOL EnableDeviceDriver(LPCWSTR sDriverFileName, // 驱动程序完整路径名称
		LPCWSTR szDriverName); // name of service LPCTSTR szDriverName
	BOOL DisableDeviceDriver(LPCWSTR szDriverName); // name of service LPCTSTR szDriverName
	//
	BOOL CopyDriver(TCHAR *szToDir, TCHAR *szFromDir, TCHAR *szDriverName);
};

#define DRIVER_SERVICE_NAME  _T("sdclient")// name of service LPCTSTR szDriverName
#define I386_DRIVER_FNAME  _T("sdclient-i386.sys")
#define IA64_DRIVER_FNAME  _T("sdclient-ia64.sys")
#define AMD64_DRIVER_FNAME  _T("sdclient-amd64.sys")

#endif