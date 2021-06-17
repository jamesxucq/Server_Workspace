// anch_dump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

#define ANCHOR_DATA_DEFAULT				_T(".\\anchor_data.acd")
#define ANCHOR_INDEX_DEFAULT			_T(".\\anchor_inde.ndx")

#pragma	pack(1)
struct IndexValue {
    DWORD AnchorNumber;
    unsigned __int64 FileOffset;
};
#pragma	pack()

struct attent {
	DWORD action_type;		/* add mod del list recu */
	TCHAR file_name[MAX_PATH];
	unsigned __int64 file_size;
	FILETIME last_write;		/* When the item was last modified */
	DWORD reserved;  /* reserved */
};

int index_dump(HANDLE hAnchorIndex) {
	struct IndexValue tIndexValue;
	DWORD dwFileSize, dwFileHigh;
	DWORD dwReadSize = 0;

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	dwFileSize = GetFileSize(hAnchorIndex, &dwFileHigh);
	if(INVALID_FILE_SIZE==dwFileSize  && NO_ERROR!=GetLastError()) 
		return -1;
	if(0x00 == dwFileSize) {
		wprintf(_T("not found index.\n"));
		return 0;
	}

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	DWORD result = SetFilePointer(hAnchorIndex, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;

	ReadFile(hAnchorIndex, &tIndexValue, sizeof(struct IndexValue), &dwReadSize, NULL);
	while (sizeof(struct IndexValue) == dwReadSize) {
		wprintf(_T("AnchorNumber: %d FileOffset: %lld\n"), tIndexValue.AnchorNumber, tIndexValue.FileOffset);
		ReadFile(hAnchorIndex, &tIndexValue, sizeof(struct IndexValue), &dwReadSize, NULL);
	}

	return 0;
}

DWORD LoadLastIndexValue(struct IndexValue *pIndexValue, HANDLE hAnchorIndex) {
	DWORD dwFileSize, dwFileHigh;
	DWORD dwReadSize = 0;

	memset(pIndexValue, '\0', sizeof(struct IndexValue));
	dwFileSize = GetFileSize(hAnchorIndex, &dwFileHigh);
	if(INVALID_FILE_SIZE==dwFileSize  && NO_ERROR!=GetLastError()) 
		return -1;
	if(0x00 == dwFileSize) return 0;

	DWORD result = SetFilePointer(hAnchorIndex, -(LONG)sizeof(struct IndexValue), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	if(!ReadFile(hAnchorIndex, pIndexValue, sizeof(struct IndexValue), &dwReadSize, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}

	return 0;
}

///////////////////////////////////////////////////////
#define NONE					0x00000000
///////////////////////////////////////////////////////
// file real status
#define ADD                     0x00000001 // addition
#define MODIFY                  0x00000002 // modify
#define DEL                     0x00000004 // delete
#define EXIST                   0x00000008 // exist
#define COPY                    0x00000010 // copy
#define MOVE                    0x00000020 // move
///////////////////////////////////////////////////////
// list status
#define RECURSIVE               0x00000040 // recursive
#define LIST                    0x00000080 // list
///////////////////////////////////////////////////////
// dir virtual status
#define DIRECTORY	            0x00001000 // directory
//////////////////////////////////////////////////////////////
#define PROPERTY                0x00010000 // property
////////////////////////////////
#define LOCK					PROPERTY | 0x0001 // lock 
#define UNLOCK					PROPERTY | 0x0002 // unlink
#define READONLY				PROPERTY | 0x0004 // read only
#define WRITEABLE				PROPERTY | 0x0008 // writeable
#define SHARED					PROPERTY | 0x0010 // shared
#define EXCLUSIVE				PROPERTY | 0x0020 // exclusive
///////////////////////////////////////////////////////
// #define ERASE	                0x00020000 // erase
///////////////////////////////////////////////////////

static TCHAR *GetActionTxt(DWORD action_type) {
	static TCHAR szActionTxt[128];

	if(ADD & action_type)_tcscpy_s(szActionTxt, _T("add"));
	else if(MODIFY & action_type) _tcscpy_s(szActionTxt, _T("modify"));
	else if(DEL & action_type)_tcscpy_s(szActionTxt, _T("delete"));
	else if(EXIST & action_type) _tcscpy_s(szActionTxt, _T("exist"));
	else if(COPY & action_type) _tcscpy_s(szActionTxt, _T("copy"));
	else if(MOVE & action_type)_tcscpy_s(szActionTxt, _T("move"));
	if(DIRECTORY & action_type) _tcscat_s(szActionTxt, _T(" dire"));

	return szActionTxt;
}


DWORD LoadFilesByIndexValue(struct IndexValue *pIndexValue, HANDLE hAnchorData) {
	struct attent tFileAttrib;
	DWORD dwReadSize = 0;

	memset(&tFileAttrib, '\0', sizeof(struct attent));
    LONG lDistanceToMoveHigh = (pIndexValue->FileOffset>>32)&0xffffffff;
	DWORD result = SetFilePointer(hAnchorData, (LONG)pIndexValue->FileOffset&0xffffffff, &lDistanceToMoveHigh, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	do {
		if(!ReadFile(hAnchorData, &tFileAttrib, sizeof(struct attent), &dwReadSize, NULL)) {
			if(ERROR_HANDLE_EOF != GetLastError()) return -1;
		}
		if(dwReadSize == sizeof(struct attent)) {
			wprintf(_T("%s %s\n"), GetActionTxt(tFileAttrib.action_type), tFileAttrib.file_name);
		}
	}while(dwReadSize == sizeof(struct attent));

	return 0;
}


int data_dump(int index, HANDLE hAnchorIndex, HANDLE hAnchorData) {
	struct IndexValue tIndexValue;

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	if(0 < index) {
		if(LoadLastIndexValue(&tIndexValue, hAnchorIndex))
			return -1;
	}
	LoadFilesByIndexValue(&tIndexValue, hAnchorData);

	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
	/////////////////////////////////////////////////////////////////////////////////
	if(2 > argc) {
		printf("input error! argc:%d\n", argc);
		return -1;
	}
	/////////////////////////////////////////////////////////////////////////////////
	HANDLE hAnchorIndex = CreateFile( ANCHOR_INDEX_DEFAULT, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if(hAnchorIndex == INVALID_HANDLE_VALUE ) {
		wprintf( _T("create file failed.\n") );
		wprintf(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 
	HANDLE hAnchorData = CreateFile( ANCHOR_DATA_DEFAULT, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if(hAnchorData == INVALID_HANDLE_VALUE ) {
		wprintf( _T("create file failed.\n") );
		wprintf(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}
	/////////////////////////////////////////////////////////////////////////////////
	if(!_tcscmp(_T("ndx"), argv[1])) index_dump(hAnchorIndex);
	else if(!_tcscmp(_T("dat"), argv[1])) {
		if(3 == argc) data_dump(_ttoi(argv[2]), hAnchorIndex, hAnchorData);
		else data_dump(0, hAnchorIndex, hAnchorData);
	}
	/////////////////////////////////////////////////////////////////////////////////
	if(hAnchorIndex != INVALID_HANDLE_VALUE) {
		CloseHandle( hAnchorIndex );
		hAnchorIndex = INVALID_HANDLE_VALUE;
	}
	if(hAnchorData != INVALID_HANDLE_VALUE) {
		CloseHandle( hAnchorData );
		hAnchorData = INVALID_HANDLE_VALUE;
	}
	return 0;
}

