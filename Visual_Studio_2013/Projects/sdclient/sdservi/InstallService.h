// InstallService.h
#ifndef _INSTALLSERVICE_H_
#define _INSTALLSERVICE_H_
	//
	static BOOL SCMInstallService(LPCTSTR sServFileName, // 驱动程序完整路径名称
								LPCTSTR sServiceName); // name of service LPCTSTR szDriverName

	static BOOL SCMUninstallService(LPCTSTR sServiceName);// Name of service
	static BOOL KillService(LPCTSTR sServiceName);
	static BOOL SCMCheckServiceExist(LPCTSTR sServiceName);// Name of service
	//
	static BOOL SCMStartService(LPCTSTR sServiceName);
	static BOOL SCMQueryServiceStatus(LPCTSTR sServiceName);
	//
	BOOL __stdcall InstallServiceEx(LPCTSTR sServFileName, // 驱动程序完整路径名称
						LPCTSTR sServiceName); // name of service LPCTSTR szDriverName
	BOOL __stdcall EnableService(LPCTSTR sServFileName, // 驱动程序完整路径名称
						LPCTSTR sServiceName); // name of service LPCTSTR szDriverName
	BOOL __stdcall DisableService(LPCTSTR sServiceName); // name of service LPCTSTR szDriverName

#endif /* _INSTALLSERVICE_H_ */