//InstallService.h
#ifndef _INSTALLSERVICE_H_
#define _INSTALLSERVICE_H_

	static BOOL SCMInstallService(LPCSTR sServFileName, //驱动程序完整路径名称
						 LPCSTR sServiceName); //name of service LPCTSTR sDriverName

	static BOOL SCMUninstallService(LPCSTR sServiceName);//Name of service
	static BOOL KillService(LPCSTR sServiceName);
	static BOOL SCMCheckServiceExist(LPCSTR sServiceName);//Name of service
//////////////////////////////////////////////////////////////////////////////
	static BOOL SCMStartService(LPCSTR sServiceName);
	static BOOL SCMStopService(LPCSTR sServiceName);
	static BOOL SCMQueryServiceStatus(LPCSTR sServiceName);
//////////////////////////////////////////////////////////////////////////////
	BOOL InstallServiceEx(LPCSTR sServFileName, //驱动程序完整路径名称
						 LPCSTR sServiceName); //name of service LPCTSTR sDriverName
#define ENABLE_MAXTIMES			5
	BOOL EnableService(LPCSTR sServFileName, //驱动程序完整路径名称
						 LPCSTR sServiceName); //name of service LPCTSTR sDriverName
#define DISABLE_MAXTIMES		5
	BOOL DisableService(LPCSTR sServiceName); //name of service LPCTSTR sDriverName


#endif /* _INSTALLSERVICE_H_ */