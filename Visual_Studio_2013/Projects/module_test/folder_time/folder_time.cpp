// folder_time.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "windows.h"


#define RIVER_DIRECTORY_DEFAULT				_T(".\\river_folder.sdo")
#define INVA_INDE_VALU ((ULONG)-1)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FolderEntry {
	union {
		ULONG sibling;
		ULONG recycled;
	};
	ULONG parent;
	ULONG child, leaf_inde;
	//
	TCHAR szFolderName[MAX_PATH];
	FILETIME ftCreationTime;
};

namespace FolderUtility {
	ULONG ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderInde);
}

ULONG FolderUtility::ReadNode(HANDLE hRiverFolder, struct FolderEntry *pFolderEntry, ULONG lFolderInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFolderInde * sizeof(struct FolderEntry);
    if(!ReadFile(hRiverFolder, pFolderEntry, sizeof(struct FolderEntry), &slen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lFolderInde;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FolderEntryPrint(struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hRiverFolder;
	/////////////////////////////////////////
	hRiverFolder = CreateFile( RIVER_DIRECTORY_DEFAULT, 
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hRiverFolder == INVALID_HANDLE_VALUE ) {
		printf("create file failed: %d\n", GetLastError() );
		printf(" File: %s Line: %d\n", _T(__FILE__), __LINE__);
		return -1;
	}
	/////////////////////////////////////////
	struct FolderEntry tFolderEntry;
	DWORD lFolderIndex;
	for(lFolderIndex = 0; ; lFolderIndex++) {
		if(INVA_INDE_VALU == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderIndex))
			break;
		FolderEntryPrint(&tFolderEntry, lFolderIndex);
	}
	/////////////////////////////////////////
	if(hRiverFolder != INVALID_HANDLE_VALUE) {
		CloseHandle( hRiverFolder );
		hRiverFolder = INVALID_HANDLE_VALUE;
	}
	return 0;
}

static TCHAR *ftim_unis(TCHAR *timestr, FILETIME *filetime) {  // Wed, 15 Nov 1995 04:58:08 GMT
    SYSTEMTIME st;
    if(!FileTimeToSystemTime(filetime, &st)) return NULL;
    struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
    _tcsftime(timestr, 32, _T("%a, %d %b %Y %H:%M:%S GMT"), &gm);
    return timestr;
}

static TCHAR *slawi(FILETIME *last_write) {
    static TCHAR timestr[64];
    ftim_unis(timestr, last_write);
	return timestr;
}

void FolderEntryPrint(struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	wprintf(_T("lFolderIndex:%d Creation:%s name:%s\n"), 
		lFolderIndex, 
		slawi(&pFolderEntry->ftCreationTime), 
		pFolderEntry->szFolderName);
}
