// file_bin.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "windows.h"


#define RIVER_DIRECTORY_DEFAULT				_T(".\\river_folder.sdo")
#define FILES_RIVER_DEFAULT				_T(".\\files_river.sdo")
#define INVA_INDE_VALU		((ULONG)-1)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SHA1_DIGEST_LEN					20

struct FileEntry {
	ULONG sibling, parent;
	union {
		ULONG isonym_node;
		ULONG recycled;
	};
	//
	TCHAR szFileName[MAX_PATH];
    unsigned __int64 qwFileSize;
	unsigned char szFileChks[SHA1_DIGEST_LEN];
	FILETIME ftCreatTime;
	FILETIME ftLastWrite;
	ULONG list_hinde;
	DWORD chkslen; /* chunk length */
};

namespace EntryUtility {
	ULONG ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde);
}

ULONG EntryUtility::ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde) {
    DWORD slen;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lFileInde * sizeof(struct FileEntry);
    if(!ReadFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &slen, &OverLapped)) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
    return lFileInde;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EntryPrint(struct FileEntry *pFileEntry, DWORD lFileIndex) ;
int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hFilesRiver;
	/////////////////////////////////////////
	hFilesRiver = CreateFile( FILES_RIVER_DEFAULT, 
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFilesRiver == INVALID_HANDLE_VALUE ) {
		printf("create file failed: %d\n", GetLastError() );
		printf(" File: %s Line: %d\n", _T(__FILE__), __LINE__);
		return -1;
	}
	/////////////////////////////////////////
	struct FileEntry tFileEntry;
	LONG lEmptyOffset;

	int entry_len = sizeof(struct FileEntry);
	lEmptyOffset = SetFilePointer(hFilesRiver, -entry_len, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVA_INDE_VALU;
	/////////////////////////////////////////
	DWORD lFileIndex = lEmptyOffset / entry_len;
	while(INVA_INDE_VALU != EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileIndex)) {
		EntryPrint(&tFileEntry, lFileIndex);
		lFileIndex = tFileEntry.isonym_node;
	}
	/////////////////////////////////////////
	if(hFilesRiver != INVALID_HANDLE_VALUE) {
		CloseHandle( hFilesRiver );
		hFilesRiver = INVALID_HANDLE_VALUE;
	}
	return 0;
}


void EntryPrint(struct FileEntry *pFileEntry, DWORD lFileIndex) {
	wprintf(_T("lFileIndex:%d recycled:%d parent:%d sibling:%d name:%s\n"), 
		lFileIndex,
		pFileEntry->recycled,
		pFileEntry->parent,
		pFileEntry->sibling,
		pFileEntry->szFileName);
}