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


bool ExecuteApplicationEx(char *sApplicationPath, char *sProcessName, bool fFailIfExists)
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