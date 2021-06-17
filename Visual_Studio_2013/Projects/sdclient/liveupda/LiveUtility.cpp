#include "StdAfx.h"
#include "clientcom/lupdatecom.h"
#include "LiveUtility.h"

char *lutility::LoadFile(char *szContent, TCHAR *szFileName, DWORD dwLength) {
	HANDLE hLoadFile;

	hLoadFile = CreateFile( szFileName, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( INVALID_HANDLE_VALUE == hLoadFile ) {
		_LOG_WARN( _T("create file failed: %d"), GetLastError() );
		return NULL;
	}
	DWORD dwFileSize = GetFileSize(hLoadFile, NULL);
	if(INVALID_FILE_SIZE==dwFileSize && NO_ERROR!=GetLastError()) {
		CloseHandle(hLoadFile);
		return NULL;
	}
	if(dwLength < dwFileSize) {
		CloseHandle(hLoadFile);
		return NULL;
	}
	DWORD dwResult = SetFilePointer(hLoadFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hLoadFile);
		return NULL;
	}
//
	DWORD dwReadSize = 0x00;
	if(!ReadFile(hLoadFile, szContent, dwFileSize, &dwReadSize, NULL) || dwFileSize!=dwReadSize) {
		CloseHandle(hLoadFile);
		return NULL;
	}
	szContent[dwReadSize] = '\0';
//
	CloseHandle(hLoadFile);
	return szContent;
}

BOOL lutility::SaveFile(TCHAR *szFileName, char *szContent, DWORD dwLength) {
	HANDLE hSaveFile;
//
	hSaveFile = CreateFile( szFileName, 
		GENERIC_WRITE,
		NULL,
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( INVALID_HANDLE_VALUE == hSaveFile ) {
		_LOG_WARN( _T("create file failed: %d"), GetLastError() );
		return FALSE;
	}
	DWORD dwResult = SetFilePointer(hSaveFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
		CloseHandle(hSaveFile);
		return FALSE;
	}
//
	DWORD dwWritenSize = 0x00;
	WriteFile(hSaveFile, szContent, dwLength, &dwWritenSize, NULL);
	if(dwWritenSize != dwLength) {
		_LOG_WARN(_T("fatal write error: %08x!"), GetLastError());
		CloseHandle(hSaveFile);
		return FALSE;		
	}
//
	CloseHandle(hSaveFile);
	return TRUE;
}

// $INSTDIR
// $COMMONFILES
// $WINDIR
static DWORD firstInvoke = 0x00;
static TCHAR szWindowsDirectory[MAX_PATH];
static TCHAR szCommonFiles[MAX_PATH];
static TCHAR szSystemRoot[MAX_PATH];
static TCHAR szLocation[MAX_PATH];
static TCHAR szLiveUpdate[MAX_PATH];
TCHAR *lutility::Location(TCHAR *szFileName, TCHAR *szFilePath) {
	TCHAR *szSubstr;
	MKZEO(szFileName);
	//
	if(!firstInvoke) {
		NFileUtility::ModuleBaseName(szLocation);
		lutility::UpdatePath(szLiveUpdate, _T(""));
		_tcscpy_s(szWindowsDirectory, MAX_PATH, _tgetenv(_T("WINDIR")));
		_tcscpy_s(szSystemRoot, MAX_PATH, _tgetenv(_T("SYSTEMROOT")));
		_tcscpy_s(szCommonFiles, MAX_PATH, _tgetenv(_T("COMMONPROGRAMFILES")));
		//
		firstInvoke++;
	}
	//
	if(szSubstr = _tcsstr(szFilePath, _T("$LIVEDIR"))) {
		_tcscpy_s(szFileName, MAX_PATH, szLiveUpdate);
		szSubstr += 8;
	} else if(szSubstr = _tcsstr(szFilePath, _T("$INSTDIR"))) {
		_tcscpy_s(szFileName, MAX_PATH, szLocation);
		szSubstr += 8;
	} else if(szSubstr = _tcsstr(szFilePath, _T("$COMMONFILES"))) {
		_tcscpy_s(szFileName, MAX_PATH, szCommonFiles);
		szSubstr += 12;
	} else if(szSubstr = _tcsstr(szFilePath, _T("$WINDIR"))) {
		_tcscpy_s(szFileName, MAX_PATH, _tgetenv(_T("windir")));
		szSubstr += 7;
	} else if(szSubstr = _tcsstr(szFilePath, _T("$SYSTEMROOT"))) {
		_tcscpy_s(szFileName, MAX_PATH, szSystemRoot);
		szSubstr += 11;
	} else szSubstr = szFilePath;
	_tcscat_s(szFileName, MAX_PATH, szSubstr);
	//
	return szFileName;
}

TCHAR *lutility::UpdatePath(TCHAR *szFilePath, TCHAR *szFileName) {
    static TCHAR szUpdatePath[MAX_PATH];
    //
    if(_T(':') != szUpdatePath[1]) {
        if(!GetEnvironmentVariable(_T("APPDATA"), szUpdatePath, MAX_PATH))
            return NULL;
        _tcscat_s(szUpdatePath, _T("\\sdclient\\updates"));
		NDireUtility::RecursMakeFolderW(szUpdatePath);
    }
	_stprintf_s(szFilePath, MAX_PATH, _T("%s\\%s"), szUpdatePath, szFileName);
    //
    return szFilePath;
}

/*
TCHAR *lutility::UpdatePath(TCHAR *szFileName, TCHAR *szLocation, TCHAR *szFilePath) {
	_stprintf_s(szFileName, MAX_PATH, _T("%s\\updates\\%s"), szLocation, szFilePath);
	return szFileName;
}
*/

BOOL lutility::KillExplorer() {
	return SystemCall::EndApplica(EXPLORER_PROCESS_NAME) ? FALSE: TRUE;
}

//
// Delete a key and all of its descendants.
//
LONG recursiveDeleteKey(HKEY hKeyParent,           // Parent of key to delete
						const TCHAR* lpszKeyChild)  // Key to delete
{
	// Open the child.
	HKEY hKeyChild ;
	LONG lResult = RegOpenKeyEx(hKeyParent, lpszKeyChild, 0,
		KEY_ALL_ACCESS, &hKeyChild) ;
	if (lResult != ERROR_SUCCESS) {
		return lResult ;
	}
	// Enumerate all of the descendants of this child.
	FILETIME time ;
	TCHAR szBuffer[260] ;
	DWORD dwSize = 260 ;
	while (RegEnumKeyEx(hKeyChild, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time) == S_OK) {
		// Delete the descendants of this child.
		lResult = recursiveDeleteKey(hKeyChild, szBuffer) ;
		if (lResult != ERROR_SUCCESS) {
			// Cleanup before exiting.
			RegCloseKey(hKeyChild) ;
			return lResult;
		}
		dwSize = 260 ;
	}
	// Close the child.
	RegCloseKey(hKeyChild) ;
	// Delete this child.
	return RegDeleteKey(hKeyParent, lpszKeyChild) ;
}

// LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers");
BOOL lutility::EnumDeliRegistry(char *subKey, TCHAR *szPrefix) {
	HKEY hKey;
//
	long lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		vector <TCHAR *> vKeyChild;
		// Enumerate all of the descendants of this child.
		TCHAR szBuffer[260] ;
		DWORD dwSize = 260 ;
		DWORD inde = 0;
		while (RegEnumKeyEx(hKey, inde++, szBuffer, &dwSize, NULL, NULL, NULL, NULL) == S_OK) {
			// Delete the descendants of this child.
			if(!_tcsncmp(szPrefix, szBuffer, _tcslen(szPrefix))) {
				TCHAR *szChild = (TCHAR *)malloc(260 * sizeof(TCHAR));
				vKeyChild.push_back(szChild);
				_tcscpy_s(szChild, 260, szBuffer);
			}
			dwSize = 260 ;
		}
		//
		vector <TCHAR *>::iterator iter;
		for(iter = vKeyChild.begin(); vKeyChild.end() != iter; ++iter) {
			lResult |= recursiveDeleteKey(hKey, *iter);
			free(*iter);
		}
		vKeyChild.clear();
		//关闭注册表
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS) return FALSE;
	}
//
	return TRUE;
}

BOOL lutility::RegistLibrary(TCHAR *szLibraryPath) { // Todo: modify
	return SystemCall::ExecuteApplica(szLibraryPath) ? FALSE: TRUE;
}

DWORD lutility::AddiSubkey(char *subKey, TCHAR *szValueName, TCHAR *szData) {
	HKEY hKey;
	DWORD disp;
//
	LONG lResult = RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &disp); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		//添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
		lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (BYTE *)szData, _tcslen(szData + 1)*sizeof(WCHAR));
		//关闭注册表
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS) AfxMessageBox(_T("创建参数错误"));
	}
//
	return 0x00;
}

DWORD lutility::SetRegistry(char *subKey, TCHAR *szValueName, TCHAR *szData) {
	HKEY hKey;
//
	long lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_WRITE, &hKey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		//添加一个子Key,并设置值 // 下面的"getip"是应用程序名字（不加后缀.exe）
		lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (BYTE *)szData, _tcslen(szData + 1)*sizeof(WCHAR));
		//关闭注册表
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS) AfxMessageBox(_T("设置参数错误"));
	}
//
	return 0x00;
}

DWORD lutility::DeliSubkey(char *subKey, TCHAR *szSubkey) {
	HKEY hKey;
//
	long lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_WRITE, &hKey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		lResult = recursiveDeleteKey(hKey, szSubkey);
		//关闭注册表
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS) AfxMessageBox(_T("删除参数错误"));
	}
//
	return 0x00;
}

DWORD lutility::DeliRegistry(char *subKey, TCHAR *szValueName) {
	HKEY hKey;
//
	long lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_WRITE, &hKey); // 打开启动项Key
	if(lResult == ERROR_SUCCESS) {
		lResult = RegDeleteValue(hKey, szValueName);
		//关闭注册表
		RegCloseKey(hKey);
		if(lResult != ERROR_SUCCESS) AfxMessageBox(_T("删除参数错误"));
	}
//
	return 0x00;
}

BOOL lutility::ExecuteExplorer() {
	TCHAR szFileName[MAX_PATH];
//
	_tcscpy_s(szFileName, _tgetenv(_T("windir")));
	_tcscat_s(szFileName, _T("\\"));
	_tcscat_s(szFileName, EXPLORER_EXECUTE_NAME);
//
	return SystemCall::ExecuteApplicaEx(szFileName, EXPLORER_PROCESS_NAME, true) ? FALSE: TRUE;
}

// http://192.168.1.1:80/cgi-bin/inde.html
BOOL lutility::URL_split(char *address, int *port, char *szReqURI, TCHAR *szReqURL) {
	char req_url[URI_LENGTH], strtmp[64];
	char *toksp, *tokep;
//
	strcon::ustr_ansi(req_url, szReqURL);
	tokep = req_url;
//
	toksp = strstr(req_url, "http://");
	if(!toksp) { // return FALSE;
		*address = '\0';
		*port = 0;
		strcpy_s(szReqURI, MAX_PATH, req_url);
	} else {
		tokep = toksp + 7;
//
		toksp = strpbrk(tokep, ":/");
		if(!toksp) return FALSE;
		strncpy_s(address, HOSTNAME_LENGTH, tokep, toksp - tokep);
		address[toksp-tokep] = '\0';
//
		if(':' == *toksp) {
			tokep = toksp + 1;
			toksp = strchr(tokep, '/');
			strncpy_s(strtmp, tokep, toksp - tokep);
			strtmp[toksp-tokep] = '\0';
			*port = atoi(strtmp);
		} else *port = 80;
//
		strcpy_s(szReqURI, MAX_PATH, toksp);
	}
//
	return TRUE;
}