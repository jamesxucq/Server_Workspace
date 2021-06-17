//ApplicationOpt.cpp

#include "stdafx.h"
#include <windows.h>
#include <ShellAPI.h>
#include <WinUser.h>
#include <Tlhelp32.h>

#include <stdio.h>
#include <tchar.h>

#include "ApplicationOpt.h"


bool ExecuteApplication(char *sApplicationPath)
{
	if((int)ShellExecute(NULL, _T("open"), sApplicationPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
		return false;
	return true;
}

bool KillProcessFromName(LPCSTR lpProcessName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
        return false;

    PROCESSENTRY32 pe = { sizeof(pe) };
    BOOL fOk;
    for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
    {
        if (!_tcscmp(pe.szExeFile, lpProcessName)) {
            DWORD dwProcessID = pe.th32ProcessID;
            HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
            ::TerminateProcess(hProcess,0);
            CloseHandle(hProcess);
            return true;
        }
    }

    return false;
}

bool EndApplication(char *sProcessName)
{
	if(!sProcessName) return false;
	return KillProcessFromName(sProcessName);
}

DWORD GetProcessID(LPCTSTR sProcessName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
        return 0;

    PROCESSENTRY32 pe = { sizeof(pe) };
    BOOL fOk;
    for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
    {
        if (!_tcscmp(pe.szExeFile, sProcessName)) {
            CloseHandle(hSnapshot);
            return pe.th32ProcessID;
        }
    }

    return 0;
}


bool ExecuteApplicationExt(char *sApplicationPath, char *sProcessName, bool fFailIfExists)
{
	if(!sApplicationPath || !sProcessName)
		return false;

	if(!GetProcessID(sProcessName) || !fFailIfExists) {
		if((int)ShellExecute(NULL, _T("open"), sApplicationPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
			return false;
	}

	return true;
}

typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
}WNDINFO, *LPWNDINFO;

BOOL CALLBACK YourEnumProc(HWND hWnd,LPARAM lParam)
{
	DWORD dwProcessId;

	GetWindowThreadProcessId(hWnd, &dwProcessId);
	LPWNDINFO   pInfo = (LPWNDINFO)lParam;
	if(dwProcessId == pInfo-> dwProcessId)
	{
		pInfo-> hWnd = hWnd;
		return false;
	}
	return true;
}

HWND GetProcessMainWnd(DWORD dwProcessId)
{
	WNDINFO   wi;
	wi.dwProcessId = dwProcessId;
	wi.hWnd = NULL;
	EnumWindows(YourEnumProc,(LPARAM)&wi);
	return   wi.hWnd;
} 

bool RefreshExplorer(LPCSTR sProcessName)
{
	if(!sProcessName) return false;

	DWORD dwProcessId = GetProcessID(sProcessName);
	if(!dwProcessId) return false;

	HWND hProcessMainWnd = GetProcessMainWnd(dwProcessId);
	PostMessage(hProcessMainWnd, WM_WININICHANGE, NULL, NULL);

	return true;
}

bool CheckApplicationAutorun(char *sApplicationPath)
{
	HKEY hKey;
	LPCTSTR lpRun = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"); // 找到系统的启动项
	bool boolReturn = false;

	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_READ, &hKey); // 打开启动项Key
	if(lRet == ERROR_SUCCESS) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = (MAX_PATH)*8;
		char strValue[(MAX_PATH)*8]; // 得到程序自身的全路径
		char sValueName[MAX_PATH];
		
		memset(strValue, '\0', MAX_PATH);
		_tcscpy_s(sValueName, _T("Userinit"));
		lRet = RegQueryValueEx(hKey, sValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(lRet==ERROR_SUCCESS && _tcsstr(strValue, sApplicationPath)) boolReturn = true;

		//关闭注册表
		RegCloseKey(hKey);
	}

	return boolReturn;
}


bool AddApplicationAutorun(char *sApplicationPath)
{
	HKEY hKey;
	LPCTSTR lpRun = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"); // 找到系统的启动项
	bool boolReturn = true;

	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_READ|KEY_WRITE, &hKey); // 打开启动项Key
	if(lRet == ERROR_SUCCESS) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = (MAX_PATH)*8;
		char strValue[(MAX_PATH)*8]; // 得到程序自身的全路径
		char sValueName[MAX_PATH];
		
		memset(strValue, '\0', MAX_PATH);
		_tcscpy_s(sValueName, _T("Userinit"));
		lRet = RegQueryValueEx(hKey, sValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(lRet==ERROR_SUCCESS && !_tcsstr(strValue, sApplicationPath)) {
			char sDupValue[(MAX_PATH)*8];
			_tcscpy_s(sDupValue, strValue);

			_tcscpy_s(strValue, sApplicationPath);
			_tcscat_s(strValue, _T(","));
			_tcscat_s(strValue, sDupValue);	
			lRet = RegSetValueEx(hKey, sValueName, 0, REG_SZ, (BYTE *)strValue, (_tcslen(strValue)+1)*sizeof(char));
		}

		// 关闭注册表
		RegCloseKey(hKey);
		if(lRet != ERROR_SUCCESS) boolReturn = false; // AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}

	return boolReturn;
}

bool AddApplicationAutorunExt(char *sApplicationPath)
{
	bool boolReturn = false;

	if(!sApplicationPath) return false;
	
	if(!CheckApplicationAutorun(sApplicationPath)) {
		if(AddApplicationAutorun(sApplicationPath))
			boolReturn = true;
	}

	return boolReturn;
}

bool DelApplicationAutorun(char *sApplicationPath)
{
	HKEY hKey;
	LPCTSTR lpRun = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"); // 找到系统的启动项
	bool boolReturn = true;

	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_READ|KEY_WRITE, &hKey); // 打开启动项Key
	if(lRet == ERROR_SUCCESS) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = (MAX_PATH)*8;
		char strValue[(MAX_PATH)*8]; // 得到程序自身的全路径
		char sValueName[MAX_PATH];
		char *sSubvalue = NULL;
		
		memset(strValue, '\0', MAX_PATH);
		_tcscpy_s(sValueName, _T("Userinit"));
		lRet = RegQueryValueEx(hKey, sValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(lRet==ERROR_SUCCESS && (sSubvalue=_tcsstr(strValue, sApplicationPath))) {
			char sDupValue[(MAX_PATH)*8];
			_tcscpy_s(sDupValue, strValue);

			*(sSubvalue) = '\0';
			char *sEndDupSubvalue = _tcsstr(sDupValue, sApplicationPath) + (_tcslen(sApplicationPath)*sizeof(char) + 1);
			_tcscat_s(strValue, sEndDupSubvalue);	
			lRet = RegSetValueEx(hKey, sValueName, 0, REG_SZ, (BYTE *)strValue, (_tcslen(strValue)+1)*sizeof(char));
		}

		// 关闭注册表
		RegCloseKey(hKey);
		if(lRet != ERROR_SUCCESS) boolReturn = false; // AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}

	return boolReturn;
}
