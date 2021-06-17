#include "StdAfx.h"
#include <Tlhelp32.h>
#include <Atlbase.h>
#include <Windows.h>

#include "BinaryLogger.h"
#include "SystemCall.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#define MKZERO(TEXT)					TEXT[0] = _T('\0')

DWORD SystemCall::CheckNormalPoweroff(TCHAR *szFlagFilePath) {
	//= CHECK_NORMAL_POWEROFF
	return 0;
}

DWORD SystemCall::CheckApplicationAutorun(TCHAR *szValueName) {
	HKEY hkey;
	LPCTSTR lprun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项
	DWORD dwCheckValue = -1;

	long lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lprun, 0, KEY_READ, &hkey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = MAX_PATH;
		TCHAR strValue[MAX_PATH];//得到程序自身的全路径
		TCHAR szFileName[MAX_PATH];//得到程序自身的全路径

		MKZERO(strValue);
		MKZERO(szFileName);
		DWORD dwPathLength = GetModuleFileName(NULL, szFileName, MAX_PATH);
		lResult = RegQueryValueEx(hkey, szValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(lResult==ERROR_SUCCESS && !_tcscmp(strValue, szFileName)) dwCheckValue = 0;

		//关闭注册表
		RegCloseKey(hkey);
	}

	return dwCheckValue;
}
static TCHAR *ModuleBaseName(TCHAR *lpFilePath) {
	TCHAR szDirectoryPath[MAX_PATH];

	if(!lpFilePath) return NULL;
	if (!GetModuleFileName(NULL, szDirectoryPath, MAX_PATH)) return NULL;
	if (!PathRemoveFileSpec(szDirectoryPath)) return NULL;
	_tcscpy_s(lpFilePath, wcslen(szDirectoryPath)+1, szDirectoryPath);

	return lpFilePath;
}

DWORD SystemCall::AddApplicationAutorun(TCHAR *szValueName, TCHAR *szApplicaPath) {
	HKEY hkey;
	LPCTSTR lprun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项

	long lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lprun, 0, KEY_WRITE, &hkey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		TCHAR szFileName[MAX_PATH];//得到程序自身的全路径

		MKZERO(szFileName);
		ModuleBaseName(szFileName);//添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
		_tcscat_s(szFileName, _T("\\"));
		_tcscat_s(szFileName, szApplicaPath);
		lResult = RegSetValueEx(hkey, szValueName, 0, REG_SZ, (BYTE *)szFileName, _tcslen(szFileName + 1)*sizeof(WCHAR));

		//关闭注册表
		RegCloseKey(hkey);
		if(lResult != ERROR_SUCCESS) AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}

	return 0;
}

DWORD SystemCall::AddApplicationAutorunExt(TCHAR *szValueName) {
	HKEY hkey;
	LPCTSTR lprun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项

	long lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lprun, 0, KEY_WRITE, &hkey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		TCHAR szFileName[MAX_PATH];//得到程序自身的全路径

		MKZERO(szFileName);
		DWORD dwPathLength = GetModuleFileName(NULL, szFileName, MAX_PATH);//添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
		lResult = RegSetValueEx(hkey, szValueName, 0, REG_SZ, (BYTE *)szFileName, (dwPathLength + 1)*sizeof(WCHAR));

		//关闭注册表
		RegCloseKey(hkey);
		if(lResult != ERROR_SUCCESS) AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}

	return 0;
}

DWORD SystemCall::DelApplicationAutorun(TCHAR *szValueName) {
	HKEY hkey;
	LPCTSTR lprun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项

	long lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lprun, 0, KEY_WRITE, &hkey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		lResult = RegDeleteValue(hkey, szValueName);
		//关闭注册表
		RegCloseKey(hkey);
		if(lResult != ERROR_SUCCESS) AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}

	return 0;
}

DWORD SystemCall::LaunchProcess(TCHAR *szProgramPath) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	//DETACHED_PROCESS
	if(!CreateProcess(szProgramPath,NULL,NULL,NULL,FALSE,DETACHED_PROCESS,0,0,&si,&pi)) {
		//TRACE(_T("CreatProcess failed on error %d\n"), GetLastError());
		//TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} else {
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		//cout<<"NWE PROCESSID:"<<pi.dwProcessId;
		//cout<<"NEW THREADID:"<<pi.dwThreadId;
	}

	return 0;
}

DWORD SystemCall::OpenLink(TCHAR *szURL) {
	if((int)ShellExecute(NULL, _T("open"), szURL, NULL, NULL, SW_SHOWNORMAL) <= 32)
		return -1;
	return 0;
}

DWORD SystemCall::ExecuteApplication(TCHAR *szApplicaPath) {
	if((int)ShellExecute(NULL, _T("open"), szApplicaPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
		return -1;
	return 0;
}

DWORD KillProcessFromName(LPCWSTR lpProcessName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return -1;

	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) {
		if (!_tcscmp(pe.szExeFile, lpProcessName)) {
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);
			::TerminateProcess(hProcess,0);
			CloseHandle(hProcess);
			return 0;
		}
	}

	return -1;
}

DWORD SystemCall::EndApplication(TCHAR *szProcessName) {
	if(!szProcessName) return -1;
	return KillProcessFromName(szProcessName);
}

DWORD GetProcessID(LPCTSTR szProcessName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return 0;

	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) {
		if (!_tcscmp(pe.szExeFile, szProcessName)) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}

	return 0;
}


DWORD SystemCall::ExecuteApplicationExt(TCHAR *szApplicaPath, TCHAR *szProcessName, bool fFailIfExists) {
	if(!szApplicaPath || !szProcessName)
		return -1;

	if(!GetProcessID(szProcessName) || !fFailIfExists) {
		if((int)ShellExecute(NULL, _T("open"), szApplicaPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
			return -1;
	}

	return 0;
}

VOID SystemCall::RefreshExplorer() {
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST|SHCNF_FLUSHNOWAIT, NULL, NULL);
}


VOID SystemCall::RefreshExplorerIcons(LPCWSTR szIconDirectory) {
	SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH|SHCNF_FLUSHNOWAIT, (LPCTSTR)szIconDirectory, NULL);
}

DWORD SystemCall::RebuildIcons() {
	const int BUFFER_SIZE = 1024;
	TCHAR *buf = NULL;
	HKEY hRegKey = 0;
	DWORD dwRegValue;
	DWORD dwRegValueTemp;
	DWORD dwSize;
	DWORD_PTR dwResult;
	LONG lRegResult;
	std::wstring sRegValueName;
	std::wstring sDefaultIconSize;
	int iDefaultIconSize;
	DWORD ret_value = -1;

	lRegResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Desktop\\WindowMetrics"),
		0, KEY_READ | KEY_WRITE, &hRegKey);
	if (lRegResult != ERROR_SUCCESS)
		goto Cleanup;

	buf = new TCHAR[BUFFER_SIZE];
	if(buf == NULL)
		goto Cleanup;

	// we're going to change the Shell Icon Size value
	sRegValueName = _T("Shell Icon Size");

	// Read registry value
	dwSize = BUFFER_SIZE;
	lRegResult = RegQueryValueEx(hRegKey, sRegValueName.c_str(), NULL, NULL, 
		(LPBYTE) buf, &dwSize);
	if (lRegResult != ERROR_FILE_NOT_FOUND) {
		// If registry key doesn't exist create it using system current setting
		iDefaultIconSize = ::GetSystemMetrics(SM_CXICON);
		if (0 == iDefaultIconSize)
			iDefaultIconSize = 32;
		_sntprintf_s(buf, BUFFER_SIZE, BUFFER_SIZE, _T("%d"), iDefaultIconSize); 
	}
	else if (lRegResult != ERROR_SUCCESS)
		goto Cleanup;

	// Change registry value
	dwRegValue = _ttoi(buf);
	dwRegValueTemp = dwRegValue-1;

	dwSize = _sntprintf_s(buf, BUFFER_SIZE, BUFFER_SIZE, _T("%d"), dwRegValueTemp) + sizeof(TCHAR); 
	lRegResult = RegSetValueEx(hRegKey, sRegValueName.c_str(), 0, REG_SZ, 
		(LPBYTE) buf, dwSize); 
	if (lRegResult != ERROR_SUCCESS)
		goto Cleanup;


	// Update all windows
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, 
		0, SMTO_ABORTIFHUNG, 5000, &dwResult);

	// Reset registry value
	dwSize = _sntprintf_s(buf, BUFFER_SIZE, BUFFER_SIZE, _T("%d"), dwRegValue) + sizeof(TCHAR); 
	lRegResult = RegSetValueEx(hRegKey, sRegValueName.c_str(), 0, REG_SZ, 
		(LPBYTE) buf, dwSize); 
	if(lRegResult != ERROR_SUCCESS)
		goto Cleanup;

	// Update all windows
	SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETNONCLIENTMETRICS, 
		0, SMTO_ABORTIFHUNG, 5000, &dwResult);

	ret_value = 0;

Cleanup:
	if (hRegKey != 0) {
		RegCloseKey(hRegKey);
	}
	if (buf != NULL) {
		delete [] buf;
	}

	return ret_value;
}


typedef VOID (WINAPI *PGNSI)(LPSYSTEM_INFO);

DWORD SystemCall::GetWindowsVersion() {
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	DWORD dwWindowVersion = _WIN_NONE_;

	ZeroMemory(&osvi, sizeof (OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
	ZeroMemory(&si, sizeof (SYSTEM_INFO));

	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) & osvi))) {
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *) & osvi)) {
			return _WIN_NONE_;
		}
	}

	switch (osvi.dwPlatformId) {
		case VER_PLATFORM_WIN32_NT:
			//Windows NT
			if (osvi.dwMajorVersion == 6) {
				switch (osvi.dwMinorVersion) {
		case 0:
			if (osvi.wProductType == VER_NT_WORKSTATION) { // 操作系统: Microsoft Windows Vista
				dwWindowVersion = _WIN_VISTA_;
			}
			break;
		case 1:
			dwWindowVersion = _WIN_SEVEN_;
			break;
				}
			}
			if (osvi.dwMajorVersion == 5) {
				switch (osvi.dwMinorVersion) {
		case 0:
			// 操作系统: Microsoft Windows 2000
			dwWindowVersion = _WIN_2K_;
			break;
		case 1:
			// 操作系统: Microsoft Windows XP
			dwWindowVersion = _WIN_XP_;
			break;
		case 2:
			pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
			if (NULL != pGNSI) pGNSI(&si);
			if (GetSystemMetrics(SM_SERVERR2)) // Microsoft Windows Server 2003 R2;
				dwWindowVersion = _WIN_2003_;
			else if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) { // 操作系统: Microsoft Windows XP Professional x64 Edition
				dwWindowVersion = _WIN_2003_;
			} else dwWindowVersion = _WIN_2003_; // 操作系统: Microsoft Windows Server 2003
			break;
		default:
			break;
				}
			}
		case VER_PLATFORM_WIN32_WINDOWS:
			//Windows 9X
			if (osvi.dwMajorVersion == 4) {
				switch (osvi.dwMinorVersion) {
		case 0: // 操作系统: Microsoft Windows 95
			dwWindowVersion = _WIN_95_;
			if (osvi.szCSDVersion[1] == L'C' || osvi.szCSDVersion[1] == L'B') // OSR2
				dwWindowVersion = _WIN_95_OSR2_;
			break;
		case 10: // 操作系统: Microsoft Windows 98
			dwWindowVersion = _WIN_98_;
			if (osvi.szCSDVersion[1] == L'A' || osvi.szCSDVersion[1] == L'B') // SE
				dwWindowVersion = _WIN_98_SE_;
			break;
		case 90: // 操作系统: Microsoft Windows Me
			dwWindowVersion = _WIN_ME_;
			break;
		default:
			break;
				}
			}
			break;
		case VER_PLATFORM_WIN32s:
			break;
		default:
			break;
	}

	return dwWindowVersion;
}