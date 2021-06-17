// ServiceTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include "InstallService.h"


int _tmain(int argc, _TCHAR* argv[])
{
	char *option = {_T("install")};
	char *servfile = {_T("C:\\Program Files\\SDClientT\\SDCache.exe")};
	char *servname = {_T("SDCache")};

	//if(argc < 3 ) printf("error param!\n");

	if(!_tcscmp(_T("install"), option)) {
		//if(!InstallServiceEx(servfile, servname)) printf("install error!\n");
		//else printf("install Ok!\n");
		EnableService(servfile, servname);
	}	
	else if(!_tcscmp(_T("uninstall"), option)) {
		if(!DisableService(servname)) printf("uninstall error!\n");	
		else printf("uninstall Ok!\n");
	}
	else printf("error option!\n");

	return 0;
}

