// folder_bin.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "windows.h"


#define RIVER_DIRECTORY_DEFAULT				_T(".\\river_folder.sdo")
#define FILES_RIVER_DEFAULT				_T(".\\files_river.sdo")
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
void FolderPrint(struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
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
	LONG lEmptyOffset;

	int entry_len = sizeof(struct FolderEntry);
	lEmptyOffset = SetFilePointer(hRiverFolder, -entry_len, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVA_INDE_VALU;
	/////////////////////////////////////////
	DWORD lFolderIndex = lEmptyOffset / entry_len;
	while(INVA_INDE_VALU != FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderIndex)) {
		FolderPrint(&tFolderEntry, lFolderIndex);
		lFolderIndex = tFolderEntry.sibling;
	}
	/////////////////////////////////////////
	if(hRiverFolder != INVALID_HANDLE_VALUE) {
		CloseHandle( hRiverFolder );
		hRiverFolder = INVALID_HANDLE_VALUE;
	}
	return 0;
}

void FolderPrint(struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	wprintf(_T("lFolderIndex:%d parent:%d child:%d sibling:%d leaf_inde:%d name:%s\n"), 
		lFolderIndex, 
		pFolderEntry->parent, 
		pFolderEntry->child, 
		pFolderEntry->sibling, 
		pFolderEntry->leaf_inde, 
		pFolderEntry->szFolderName);
}


