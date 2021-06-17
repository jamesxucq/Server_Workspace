#include "stdafx.h"
#include <windows.h>
#include <ShellAPI.h>
#include <WinUser.h>
#include <Tlhelp32.h>
#include <ShlObj.h>

#include <stdio.h>
#include <tchar.h>

#include "ApplicaUtility.h"

#define MKZEO(TEXT) TEXT[0] = _T('\0')

DWORD __stdcall ExecuteApplica(TCHAR *szApplicaPath) {
	if((int)ShellExecute(NULL, _T("open"), szApplicaPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
		return ((DWORD)-1);
	return 0x00;
}

DWORD KillProcessFromName(LPCTSTR lpProcessName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return ((DWORD)-1);
//
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) {
		if (!_tcscmp(pe.szExeFile, lpProcessName)) {
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
			::TerminateProcess(hProcess,0);
			CloseHandle(hProcess);
			return 0x00;
		}
	}
//
	return ((DWORD)-1);
}

DWORD __stdcall EndApplica(TCHAR *szProcessName) {
	if(!szProcessName) return ((DWORD)-1);
	return KillProcessFromName(szProcessName);
}

DWORD GetProcessID(LPCTSTR szProcessName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return 0x00;
//
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) {
		if (!_tcscmp(pe.szExeFile, szProcessName)) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}
//
	return 0x00;
}


DWORD __stdcall ExecuteApplicaEx(TCHAR *szApplicaPath, TCHAR *szProcessName, bool fFailIfExist) {
	if(!szApplicaPath || !szProcessName)
		return ((DWORD)-1);
	if(!GetProcessID(szProcessName) || !fFailIfExist) {
		if((int)ShellExecute(NULL, _T("open"), szApplicaPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
			return ((DWORD)-1);
	}
	return 0x00;
}

/*
typedef struct tagWNDINFO {
	DWORD dwProcessId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

BOOL CALLBACK YourEnumProc(HWND hWnd,LPARAM lParam) {
	DWORD dwProcessId;
//
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	LPWNDINFO   pInfo = (LPWNDINFO)lParam;
	if(dwProcessId == pInfo-> dwProcessId)
	{
		pInfo-> hWnd = hWnd;
		return false;
	}
	return true;
}
*/

static DWORD CheckApplicaAuto(TCHAR *szApplicaPath) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Run"); // 找到系统的启动项
	DWORD retValue = ((DWORD)-1);
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_READ, &hKey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = MAX_PATH;
		TCHAR strValue[MAX_PATH]; // 得到程序自身的全路径
		TCHAR szValueName[MAX_PATH];
//
		MKZEO(strValue);
		_tcscpy_s(szValueName, _T("Userinit"));
		lResult = RegQueryValueEx(hKey, szValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(lResult==ERROR_SUCCESS && _tcsstr(strValue, szApplicaPath)) retValue = 0x00;
//
		//关闭注册表
		RegCloseKey(hKey);
	}
//
	return retValue;
}


DWORD __stdcall AddiApplicaAuto(TCHAR *szApplicaPath) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Run"); // 找到系统的启动项
	DWORD retValue = 0x00;
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_READ|KEY_WRITE, &hKey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = MAX_PATH;
		TCHAR strValue[MAX_PATH]; // 得到程序自身的全路径
		TCHAR szValueName[MAX_PATH];
//
		MKZEO(strValue);
		_tcscpy_s(szValueName, _T("Userinit"));
		lResult = RegQueryValueEx(hKey, szValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(lResult==ERROR_SUCCESS && !_tcsstr(strValue, szApplicaPath)) {
			TCHAR szDupValue[MAX_PATH];
			_tcscpy_s(szDupValue, strValue);

			_tcscpy_s(strValue, szApplicaPath);
			_tcscat_s(strValue, _T(","));
			_tcscat_s(strValue, szDupValue);	
			lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (BYTE *)strValue, (_tcslen(strValue)+1)*sizeof(char));
		}

		// 关闭注册表
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS) retValue = ((DWORD)-1); // AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}
//
	return retValue;
}

DWORD __stdcall AddiApplicaAutoEx(TCHAR *szApplicaPath) {
	DWORD retValue = ((DWORD)-1);
//
	if(!szApplicaPath) return ((DWORD)-1);
//
	if(CheckApplicaAuto(szApplicaPath)) {
		if(AddiApplicaAuto(szApplicaPath))
			retValue = 0x00;
	}
//
	return retValue;
}

DWORD __stdcall DeliApplicaAuto(TCHAR *szApplicaPath) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Run"); // 找到系统的启动项
	DWORD retValue = 0x00;
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_READ|KEY_WRITE, &hKey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = MAX_PATH;
		TCHAR strValue[MAX_PATH]; // 得到程序自身的全路径
		TCHAR szValueName[MAX_PATH];
		TCHAR *szSubvalue = NULL;
//
		MKZEO(strValue);
		_tcscpy_s(szValueName, _T("Userinit"));
		lResult = RegQueryValueEx(hKey, szValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(lResult==ERROR_SUCCESS && (szSubvalue=_tcsstr(strValue, szApplicaPath))) {
			TCHAR szDupValue[MAX_PATH];
			_tcscpy_s(szDupValue, strValue);
//
			*(szSubvalue) = '\0';
			TCHAR *szEndDupSubvalue = _tcsstr(szDupValue, szApplicaPath) + (_tcslen(szApplicaPath)*sizeof(char) + 1);
			_tcscat_s(strValue, szEndDupSubvalue);	
			lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (BYTE *)strValue, (_tcslen(strValue)+1)*sizeof(char));
		}
//
		// 关闭注册表
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS) retValue = ((DWORD)-1); // AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}
//
	return retValue;
}
