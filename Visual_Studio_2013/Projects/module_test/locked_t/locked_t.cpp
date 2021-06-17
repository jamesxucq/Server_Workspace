// locked_t.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>


//  Returns: bool
//		true if strPath is a path to a directory
//		false otherwise.
bool IsDirectory(const TCHAR *szFolderPath)
{
	DWORD dwAttrib = GetFileAttributes( szFolderPath );
	return static_cast<bool>((dwAttrib!=0xffffffff)
		&& (dwAttrib&FILE_ATTRIBUTE_DIRECTORY));
}

DWORD IsFileLocked(const TCHAR *szFileName)   // -1:error 0:not lock 1:locked
{
	HANDLE hFileLocked;
	//
	if(!szFileName) return -1;
	hFileLocked = CreateFile( szFileName,
		/* GENERIC_WRITE | */ GENERIC_READ,
		FILE_SHARE_READ /* | FILE_SHARE_WRITE */,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( INVALID_HANDLE_VALUE == hFileLocked ) {
		if(ERROR_SHARING_VIOLATION == GetLastError()) return 0x01;
		return -1;
	}
	//
	if(INVALID_HANDLE_VALUE != hFileLocked) {
		CloseHandle( hFileLocked );
		hFileLocked = INVALID_HANDLE_VALUE;
	}
	//
	return 0;
}

#define ADD                     0x00000001 // addition
#define MODIFY                  0x00000002 // modify
#define DEL                     0x00000004 // delete

struct Action {
	DWORD dwActionType;
	TCHAR szFileName[MAX_PATH];
	DWORD ulTimestamp;
};


DWORD ValidLocked(HANDLE hActionCache, const TCHAR *szDriveRoot) // 0x01 succ
{
	struct Action tAction;
	TCHAR szFileName[MAX_PATH];
	DWORD lock_value = 0x01;
	DWORD dwFileSize = GetFileSize(hActionCache, NULL);
	//
	DWORD dwAttrib, dwReadSize;
	DWORD inde, count;
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; //
	for(count = 0, inde = 1; 0x05 > count; inde++) {
		OverLapped.Offset = dwFileSize - sizeof(struct Action) * inde;
		if(!ReadFile(hActionCache, &tAction, sizeof(struct Action), &dwReadSize, &OverLapped) || 0 >= dwReadSize)
			break;
		if(ADD & tAction.dwActionType) {
			_stprintf_s(szFileName, _T("%s%s"), szDriveRoot, tAction.szFileName);
			dwAttrib = GetFileAttributes( szFileName );
			if((INVALID_FILE_ATTRIBUTES==dwAttrib) || (FILE_ATTRIBUTE_DIRECTORY&dwAttrib))
				continue;
			count++;
			if(IsFileLocked(szFileName)) {
				lock_value = 0x00;
				break;
			}
		}
	}
	//
	DWORD dwResult = SetFilePointer(hActionCache, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return -1;
	//
	return lock_value;
}
//
HANDLE init_env()
{
	HANDLE hActionCache = CreateFile( _T("action_cache.acd"),
		GENERIC_WRITE | GENERIC_READ,
		NULL, /* FILE_SHARE_READ */
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);
	if( INVALID_HANDLE_VALUE == hActionCache ) {
		printf( "create file failed: %d", GetLastError() );
		return INVALID_HANDLE_VALUE;
	}
	//
	struct Action tAction;
	DWORD dwWritenSize;
	//
	tAction.dwActionType = ADD;
	_tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\TEST DATA\\21 Guns.mp3"));
	WriteFile(hActionCache, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

	_tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\TEST DATA\\Alive.mp3"));
	WriteFile(hActionCache, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

	_tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\TEST DATA\\Boom Boom Pow.mp3"));
	WriteFile(hActionCache, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

	tAction.dwActionType = ADD;
	_tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\TEST DATA\\"));
	WriteFile(hActionCache, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

	tAction.dwActionType = DEL;
	_tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\TEST DATA\\arch"));
	WriteFile(hActionCache, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

	tAction.dwActionType = ADD;
	_tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\TEST DATA\\Feeling.mp3"));
	WriteFile(hActionCache, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

	tAction.dwActionType = ADD;
	_tcscpy_s(tAction.szFileName, MAX_PATH, _T("\\TEST DATA\\I Gotta Feeling.mp3"));
	WriteFile(hActionCache, &tAction, sizeof(struct Action), &dwWritenSize, NULL);

	//
	DWORD dwResult = SetFilePointer(hActionCache, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
		return INVALID_HANDLE_VALUE;
	//
	return hActionCache;
}
//
void lock_file(TCHAR *file_name)
{
	HANDLE hLockFile = CreateFile(file_name,
		GENERIC_WRITE | GENERIC_READ,
		NULL, /* FILE_SHARE_READ */
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);
	if( INVALID_HANDLE_VALUE == hLockFile ) {
		printf( "create file failed: %d", GetLastError() );
		return ;
	}
}

//
int _tmain(int argc, _TCHAR* argv[])
{
/*	bool b = IsDirectory(_T("E:\\TEST DATA\\Alive.mp3"));
	b = IsDirectory(_T("E:\\TEST DATA\\"));
	b = IsDirectory(_T("E:\\TEST DAT"));
*/
	HANDLE hActionCache = init_env();
	//
	lock_file(_T("E:\\TEST DATA\\Alive.mp3"));
	//
	DWORD x = ValidLocked(hActionCache, _T("E:"));
	//
	//
		return 0;
}
