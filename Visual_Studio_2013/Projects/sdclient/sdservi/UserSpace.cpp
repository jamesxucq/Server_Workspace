#include "StdAfx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "ulog.h"
#include "UserSpace.h"

//
//
#define CLIENT_CONFIGURATION  _T("Configuration.XML")

// #define BOOL_TEXT_LEN				8
#define MAX_TEXT_LEN					512

#define MKZEO(TEXT) TEXT[0] = _T('\0')

//

DWORD DeleDirectoryA(char *szDirectory) {  // 如删除 DeleDirectory("c:\\aaa")
	WIN32_FIND_DATAA wfda;
	HANDLE hFind;
	char szFindDirectory[MAX_PATH];
	//
	sprintf_s(szFindDirectory, "%s\\*.*", szDirectory);
	hFind = FindFirstFileA(szFindDirectory, &wfda);
	if(INVALID_HANDLE_VALUE == hFind) return ((DWORD)-1);
	while(FindNextFileA(hFind, &wfda)) {
		if(FILE_ATTRIBUTE_DIRECTORY & wfda.dwFileAttributes) {
			if(strcmp(wfda.cFileName,".") && strcmp(wfda.cFileName,"..")) {
				char foundDirectory[MAX_PATH];
				sprintf_s(foundDirectory, "%s\\%s", szDirectory, wfda.cFileName);
				DeleDirectoryA(foundDirectory);
			}
		} else {
			char foundFilePath[MAX_PATH];
			sprintf_s(foundFilePath, "%s\\%s", szDirectory, wfda.cFileName);
			DeleteFileA(foundFilePath);
		}
	}
	FindClose(hFind);
	if(!RemoveDirectoryA(szDirectory)) {
		// MessageBox(0,"删除目录失败!","警告信息",MB_OK);//比如没有找到文件夹,删除失败，可把此句删除
		if(ERROR_SHARING_VIOLATION==GetLastError() || ERROR_LOCK_VIOLATION==GetLastError()) {
			return ((DWORD)-1);
		}
	}
	//
	return 0x00;
}

//
#define s_strlen(str) (sizeof(str) - 1)

#define START_LOCAT_LABEL        "<Location>"
#define END_LOCAT_LABEL          "</Location>"

#define GET_ELEMENT_VALUE(ELEMENT_VALUE, PARSE_TOKEN, LABEL_START, LABEL_END) { \
    char *elem_end, *elem_start; \
    if(elem_start = strstr(PARSE_TOKEN, LABEL_START)) { \
        elem_start += s_strlen(LABEL_START); \
        if (elem_end = strstr(elem_start, LABEL_END)) { \
            int length = elem_end - elem_start; \
            strncpy_s(ELEMENT_VALUE, elem_start, length); \
            ELEMENT_VALUE[length] = '\0'; \
            PARSE_TOKEN = elem_end + s_strlen(LABEL_END); \
        } else PARSE_TOKEN = NULL; \
    } else PARSE_TOKEN = NULL; \
}

#define LOAD_BUFF_SIZE					0x1000  // 4K

static char *LoadFile(char *szContent, TCHAR *szFileName, DWORD dwLength) {
	HANDLE hLoadFile;

	hLoadFile = CreateFile( szFileName, 
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( INVALID_HANDLE_VALUE == hLoadFile ) {
		// TRACE( _T("create file failed: %d"), GetLastError() );
		OutputDebugString(_T("create file failed."));
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

int Parser::ParseLocation(char *szLocation, TCHAR *szFileName) {
	char szContentXml[LOAD_BUFF_SIZE];
	if(!LoadFile(szContentXml, szFileName, LOAD_BUFF_SIZE))
		return -1;
	//
	char element_value[MAX_TEXT_LEN];
	char *parse_toke = szContentXml;
	GET_ELEMENT_VALUE(element_value, parse_toke, START_LOCAT_LABEL, END_LOCAT_LABEL);
	if(!parse_toke) return -1;
	strcpy_s(szLocation, MAX_PATH, element_value);
	//
	return 0;
}

TCHAR *Parser::ApplicaDataPath(TCHAR *szFilePath, const TCHAR *szFileName) {
    static TCHAR szApplicaPath[MAX_PATH];
    if(!szFileName) return NULL;
    //
	if(_T(':') != szApplicaPath[1]) {
		if(!GetEnvironmentVariable(_T("APPDATA"), szApplicaPath, MAX_PATH)) 
			return NULL;
		_tcscat_s(szApplicaPath, _T("\\sdclient"));
		CreateDirectory(szApplicaPath, NULL);
	}
    _stprintf_s(szFilePath, MAX_PATH, _T("%s\\%s"), szApplicaPath, szFileName);
    //
    return szFilePath;
}

// for test
/*
TCHAR* ansi_ustr(TCHAR* ustr, const char* ansi) {
    // ansi to unicode
    int wlen = MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), NULL, 0);
    MultiByteToWideChar(CP_ACP, NULL, ansi, (int)strlen(ansi), ustr, wlen);
    ustr[wlen] = _T('\0');
    //
    return ustr;
}
*/

//
DWORD __stdcall DeleUserSpace() {
	TCHAR szFilePath[MAX_PATH];
	MKZEO(szFilePath);
	if(!Parser::ApplicaDataPath(szFilePath, CLIENT_CONFIGURATION))
		return ((DWORD)-1);
	//
	char szLocation[MAX_PATH];
	if(Parser::ParseLocation(szLocation, szFilePath))
		return ((DWORD)-1);
// for test
// TCHAR ustr[MAX_PATH];	
// ansi_ustr(ustr, szLocation);
// logger(_T("c:\\install.log"), _T("delete user space:%s"), ansi_ustr(ustr, szLocation));
//
	return DeleDirectoryA(szLocation);
}