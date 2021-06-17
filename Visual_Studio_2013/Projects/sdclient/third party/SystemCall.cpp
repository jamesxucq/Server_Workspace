#include "StdAfx.h"
#include <Tlhelp32.h>
#include <Atlbase.h>
#include <Windows.h>

#include <lm.h>
#pragma comment(lib, "netapi32.lib")

#include "BinaryLogger.h"
#include "SystemCall.h"

#define MKZEO(TEXT) TEXT[0] = _T('\0')

DWORD SystemCall::CheckNormalPoweroff(TCHAR *szFlagFilePath) {
	// = CHECK_NORMAL_POWEROFF
	return 0x00;
}

DWORD SystemCall::CheckApplicaAuto(TCHAR *szValueName) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项
	DWORD dwCheckValue = ((DWORD)-1);
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_READ, &hKey); // 打开启动项Key
	if(ERROR_SUCCESS == lResult) {
		DWORD	dwType = REG_SZ; 
		DWORD   dwCount = MAX_PATH;
		TCHAR strValue[MAX_PATH];// 得到程序自身的全路径
		TCHAR szFileName[MAX_PATH];// 得到程序自身的全路径
//
		MKZEO(strValue);
		MKZEO(szFileName);
		DWORD dwPathLength = GetModuleFileName(NULL, szFileName, MAX_PATH);
		lResult = RegQueryValueEx(hKey, szValueName, NULL, &dwType, (LPBYTE)strValue, &dwCount);
		if(ERROR_SUCCESS==lResult && !_tcsicmp(strValue, szFileName)) dwCheckValue = 0x00;
		// 关闭注册表
		RegCloseKey(hKey);
	}
//
	return dwCheckValue;
}

static TCHAR *ModuleBaseNameEx(TCHAR *lpFilePath) {
	TCHAR szDirePath[MAX_PATH];
	if(!lpFilePath) return NULL;
	//
	if (!GetModuleFileName(NULL, szDirePath, MAX_PATH)) return NULL;
	TCHAR *Token = _tcsrchr(szDirePath, _T('\\'));
	if (!Token) return NULL;
	*(++Token) = _T('\0');
	_tcscpy_s(lpFilePath, MAX_PATH, szDirePath);
	//
	return lpFilePath;
}

DWORD SystemCall::AddiApplicaAuto(TCHAR *szValueName, TCHAR *szApplicaPath) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_WRITE, &hKey); // 打开启动项Key
	if(ERROR_SUCCESS == lResult) {
		TCHAR szFileName[MAX_PATH];// 得到程序自身的全路径
//
		MKZEO(szFileName);
		ModuleBaseNameEx(szFileName);// 添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
		_tcscat_s(szFileName, szApplicaPath);
		lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (BYTE *)szFileName, _tcslen(szFileName + 1)*sizeof(WCHAR));
		// 关闭注册表
		RegCloseKey(hKey);
		if(ERROR_SUCCESS != lResult) AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}
//
	return 0x00;
}

DWORD SystemCall::AddiApplicaAutoEx(TCHAR *szValueName) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_WRITE, &hKey); // 打开启动项Key
	if(ERROR_SUCCESS == lResult) {
		TCHAR szFileName[MAX_PATH];// 得到程序自身的全路径
//
		MKZEO(szFileName);
		DWORD dwPathLength = GetModuleFileName(NULL, szFileName, MAX_PATH);// 添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
		lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (BYTE *)szFileName, (dwPathLength + 1)*sizeof(WCHAR));
		// 关闭注册表
		RegCloseKey(hKey);
		if(ERROR_SUCCESS != lResult) AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}
//
	return 0x00;
}

DWORD SystemCall::DeliApplicaAuto(TCHAR *szValueName) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"); // 找到系统的启动项
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_WRITE, &hKey); // 打开启动项Key
	if(ERROR_SUCCESS == lResult) {
		lResult = RegDeleteValue(hKey, szValueName);
		// 关闭注册表
		RegCloseKey(hKey);
		if(ERROR_SUCCESS != lResult) AfxMessageBox(_T("系统参数错误,不能随系统启动"));
	}
//
	return 0x00;
}

/*
	HKEY hKey = NULL;
	long lResult = 0;
//
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER,   
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),   
		0,   
		KEY_CREATE_LINK | KEY_WRITE | KEY_READ | KEY_NOTIFY,   
		&hKey);
	if(ERROR_SUCCESS != lResult) return ((DWORD)-1); 
//
	DWORD dwType = REG_DWORD; 
	DWORD dwCount = sizeof(DWORD);
	DWORD dwProxyEnable;   
	lResult = RegQueryValueEx(hKey, _T("ProxyEnable"), NULL, &dwType, (LPBYTE)&dwProxyEnable, &dwCount);
	if(ERROR_SUCCESS != lResult) return ((DWORD)-1);
	if(0x01 != dwProxyEnable) return 0x00;
*/

DWORD SystemCall::GetDeviceNumber(TCHAR *szParamName) {
	HKEY hKey;
	LPCTSTR lpSubKey = _T("SOFTWARE\\sdclient"); // 找到系统的启动项
	DWORD dwDeviceNumber = ((DWORD)-1);
//
	long lResult = RegOpenKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, KEY_READ, &hKey); // 打开启动项Key
	if(ERROR_SUCCESS == lResult) {
		DWORD	dwType = REG_DWORD; 
		DWORD   dwCount = sizeof(DWORD);
//
		lResult = RegQueryValueEx(hKey, szParamName, NULL, &dwType, (LPBYTE)&dwDeviceNumber, &dwCount);
		if(ERROR_SUCCESS != lResult) {
			RegCloseKey(hKey);
			return ((DWORD)-1);
		}
		// 关闭注册表
		RegCloseKey(hKey);
	}
//
	return dwDeviceNumber;
}

DWORD SystemCall::LaunchProcess(TCHAR *szProgramPath) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
//
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
//
	// DETACHED_PROCESS
	if(!CreateProcess(szProgramPath,NULL,NULL,NULL,FALSE,DETACHED_PROCESS,0,0,&si,&pi)) {
		// _LOG_WARN(_T("CreatProcess failed on error %d"), GetLastError());
		return ((DWORD)-1);
	} else {
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		// cout<<"NWE PROCESSID:"<<pi.dwProcessId;
		// cout<<"NEW THREADID:"<<pi.dwThreadId;
	}
//
	return 0x00;
}

DWORD SystemCall::OpenLink(TCHAR *szURL) {
	if((int)ShellExecute(NULL, _T("open"), szURL, NULL, NULL, SW_SHOWNORMAL) <= 32)
		return ((DWORD)-1);
	return 0x00;
}

DWORD SystemCall::ExecuteApplica(TCHAR *szApplicaPath) {
	if((int)ShellExecute(NULL, _T("open"), szApplicaPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
		return ((DWORD)-1);
	return 0x00;
}

DWORD KillProcessFromName(LPCWSTR lpProcessName) {
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

DWORD SystemCall::EndApplica(TCHAR *szProcessName) {
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


DWORD SystemCall::ExecuteApplicaEx(TCHAR *szApplicaPath, TCHAR *szProcessName, bool fFailIfExist) {
	if(!szApplicaPath || !szProcessName)
		return ((DWORD)-1);
	if(!GetProcessID(szProcessName) || !fFailIfExist) {
		if((int)ShellExecute(NULL, _T("open"), szApplicaPath, NULL, NULL, SW_SHOWNORMAL) <= 32)
			return ((DWORD)-1);
	}
	return 0x00;
}

DWORD SystemCall::ProcessorArchitecture() {
	SYSTEM_INFO info;
	DWORD dwProcessorArchitecture = PROCESS_INVALID;
//
	GetSystemInfo(&info);
	switch(info.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_INTEL:
		switch(info.dwProcessorType) {
		case PROCESSOR_INTEL_386:
		case PROCESSOR_INTEL_486:
		case PROCESSOR_INTEL_PENTIUM:
			dwProcessorArchitecture = PROCESS_INTEL_I386;
			break;
		case PROCESSOR_INTEL_IA64:
			dwProcessorArchitecture = PROCESS_INTEL_IA64;
			break;
		default:
			dwProcessorArchitecture = PROCESS_INTEL_I386;
			break;
		}
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		dwProcessorArchitecture = PROCESS_INTEL_IA64;
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		if(PROCESSOR_AMD_X8664 == info.dwProcessorType)
			dwProcessorArchitecture = PROCESS_INTEL_IA64;
		else dwProcessorArchitecture = PROCESS_AMD64;
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
	default:
		dwProcessorArchitecture = PROCESS_INVALID;
		break;
	}
//
	return dwProcessorArchitecture;
}

//
#define VER_SUITE_WH_SERVER		0x00008000
typedef VOID (WINAPI *PGNSI)(LPSYSTEM_INFO);

DWORD WindVers::GetWindowsVersion() {
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	DWORD dwWindowVersion = __WIN_NONE__;
//
	ZeroMemory(&osvi, sizeof (OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);
	ZeroMemory(&si, sizeof (SYSTEM_INFO));
//
	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *) & osvi))) {
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *) & osvi)) {
			return __WIN_NONE__;
		}
	}
//
	switch (osvi.dwPlatformId) {
	case VER_PLATFORM_WIN32_NT: // Windows NT
		if (10 == osvi.dwMajorVersion) {
			switch (osvi.dwMinorVersion) {
			case 0:
				if (VER_NT_DOMAIN_CONTROLLER==osvi.wProductType || VER_NT_WORKSTATION==osvi.wProductType) { 
					// 操作系统: Microsoft Windows Vista
					dwWindowVersion = __WIN_10__;
				} else if(VER_NT_SERVER == osvi.wProductType) {
					dwWindowVersion = __WIN_2016__;
				}
				break;
			}
		} else if (6 == osvi.dwMajorVersion) {
			switch (osvi.dwMinorVersion) {
			case 0:
				if (VER_NT_DOMAIN_CONTROLLER==osvi.wProductType || VER_NT_WORKSTATION==osvi.wProductType) {
					// 操作系统: Microsoft Windows Vista
					dwWindowVersion = __WIN_VISTA__;
				} else if(VER_NT_SERVER == osvi.wProductType) {
					dwWindowVersion = __WIN_2008__;
				}
				break;
			case 1:
				if (VER_NT_DOMAIN_CONTROLLER==osvi.wProductType || VER_NT_WORKSTATION==osvi.wProductType) {
					dwWindowVersion = __WIN_7__;
				} else if(VER_NT_SERVER == osvi.wProductType) {
					dwWindowVersion = __WIN_2008_R2__;
				}
				break;
			case 2:
				if (VER_NT_DOMAIN_CONTROLLER==osvi.wProductType || VER_NT_WORKSTATION==osvi.wProductType) {
					dwWindowVersion = __WIN_8__;
				} else if(VER_NT_SERVER == osvi.wProductType) {
					dwWindowVersion = __WIN_2012__;
				}
				break;
			case 3:
				if (VER_NT_DOMAIN_CONTROLLER==osvi.wProductType || VER_NT_WORKSTATION==osvi.wProductType) {
					dwWindowVersion = __WIN_8_1__;
				} else if(VER_NT_SERVER == osvi.wProductType) {
					dwWindowVersion = __WIN_2012_R2__;
				}
				break;
			}
		} else if (5 == osvi.dwMajorVersion) {
			switch (osvi.dwMinorVersion) {
			case 0:
				// 操作系统: Microsoft Windows 2000
				dwWindowVersion = __WIN_2K__;
				break;
			case 1:
				// 操作系统: Microsoft Windows XP
				dwWindowVersion = __WIN_XP__;
				break;
			case 2:
				pGNSI = (PGNSI) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
				if (NULL != pGNSI) pGNSI(&si);
				if (GetSystemMetrics(SM_SERVERR2)) // Microsoft Windows Server 2003 R2;
					dwWindowVersion = __WIN_2003_R2__;
				else if (VER_NT_WORKSTATION==osvi.wProductType && PROCESSOR_ARCHITECTURE_AMD64==si.wProcessorArchitecture) { 
					// 操作系统: Microsoft Windows XP Professional x64 Edition
					dwWindowVersion = __WIN_XP_64__;
				} else if(osvi.wSuiteMask & VER_SUITE_WH_SERVER) {
					dwWindowVersion = __WIN_Home_Server__;
				} else if(!GetSystemMetrics(SM_SERVERR2)) {
					 // 操作系统: Microsoft Windows Server 2003
					dwWindowVersion = __WIN_2003__;
				}
				break;
			default:
				break;
			}
		} else if (4 == osvi.dwMajorVersion) {
			dwWindowVersion = __WIN_NT4__;
		} else if (3 == osvi.dwMajorVersion) {
			if(51 == osvi.dwMinorVersion) dwWindowVersion = __WIN_NT3_51__;
		}
	case VER_PLATFORM_WIN32_WINDOWS: // Windows 9X
		if (4 == osvi.dwMajorVersion) {
			switch (osvi.dwMinorVersion) {
			case 0: // 操作系统: Microsoft Windows 95
				dwWindowVersion = __WIN_95__;
				if (L'C' == osvi.szCSDVersion[1] || L'B' == osvi.szCSDVersion[1]) // OSR2
				dwWindowVersion = __WIN_95_OSR2__;
				break;
			case 10: // 操作系统: Microsoft Windows 98
				dwWindowVersion = __WIN_98__;
				if (L'A' == osvi.szCSDVersion[1] || L'B' == osvi.szCSDVersion[1]) // SE
					dwWindowVersion = __WIN_98_SE__;
				break;
			case 90: // 操作系统: Microsoft Windows Me
				dwWindowVersion = __WIN_ME__;
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
//
	return dwWindowVersion;
}

////
#define PACKVERSION(major, minor)	MAKELONG(minor, major)

DWORD WindVers::GetShellVersion(LPCTSTR lpszDllName) {  
    HINSTANCE hinstDll;  
    DWORD dwVersion = 0x00;
    /* For security purposes, LoadLibrary should be provided with a fully-qualified  
       path to the DLL. The lpszDllName variable should be tested to ensure that it  
       is a fully qualified path before it is used. */  
    hinstDll = LoadLibrary(lpszDllName); 
    if(hinstDll) {
        DLLGETVERSIONPROC pDllGetVersion;  
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");  
        /* Because some DLLs might not implement this function, you must test for  
           it explicitly. Depending on the particular DLL, the lack of a DllGetVersion  
           function can be a useful indicator of the version. */  
        if(pDllGetVersion) {  
            DLLVERSIONINFO dvi;  
            HRESULT hr;  
//
            ZeroMemory(&dvi, sizeof(dvi));  
            dvi.cbSize = sizeof(dvi);  
//  
            hr = (*pDllGetVersion)(&dvi);
            if(SUCCEEDED(hr)) {  
               dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);  
            }  
        } 
        FreeLibrary(hinstDll);  
    }  
    return dwVersion;  
}  

DWORD WindVers::GetShellVersionEx() {
	LPCTSTR lpszDllName = _T("C://Windows//System32//Shell32.dll");
	static DWORD dwShellVersion;
	if(_SHELL_NONE_ == dwShellVersion)
		dwShellVersion = GetShellVersion(lpszDllName);	
	return dwShellVersion;
}

DWORD WindVers::GetWinVer() {
	DWORD dwVersion = 0x00;
	//
	WKSTA_INFO_100 *wkstaInfo = NULL;
	NET_API_STATUS netStatus = NetWkstaGetInfo(NULL, 100, (LPBYTE *)&wkstaInfo);
	if (netStatus == NERR_Success) {
		dwVersion = (DWORD)MAKELONG(wkstaInfo->wki100_ver_minor, wkstaInfo->wki100_ver_major);
		netStatus = NetApiBufferFree(wkstaInfo);
	}
	//
	return dwVersion;
}

DWORD WindVers::GetWinVerEx() {
	static DWORD dwWinVer;
	if(_WIN_NONE_ == dwWinVer) 
		dwWinVer = GetWinVer();
	return dwWinVer;
}
