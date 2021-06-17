//InstallService.h
#ifndef _INSTALLSERVICE_H_
#define _INSTALLSERVICE_H_

namespace InstallService {
	////////////////////////////////////////////////////////////
	static BOOL SCMInstallService(LPCWSTR sServFileName, //驱动程序完整路径名称
		LPCWSTR sServiceName); //name of service LPCTSTR szDriverName

	static BOOL SCMUninstallService(LPCWSTR sServiceName);//Name of service
	static BOOL KillService(LPCWSTR sServiceName);
	static BOOL SCMCheckServiceExists(LPCWSTR sServiceName);//Name of service
	//////////////////////////////////////////////////////////////////////////////
	static BOOL SCMStartService(LPCWSTR sServiceName);
	static BOOL SCMQueryServiceStatus(LPCWSTR sServiceName);
	////////////////////////////////////////////////////////////
	BOOL InstallServiceExt(LPCWSTR sServFileName, //驱动程序完整路径名称
		LPCWSTR sServiceName); //name of service LPCTSTR szDriverName
	BOOL EnableService(LPCWSTR sServFileName, //驱动程序完整路径名称
		LPCWSTR sServiceName); //name of service LPCTSTR szDriverName
	BOOL DisableService(LPCWSTR sServiceName); //name of service LPCTSTR szDriverName
};


#endif /* _INSTALLSERVICE_H_ */