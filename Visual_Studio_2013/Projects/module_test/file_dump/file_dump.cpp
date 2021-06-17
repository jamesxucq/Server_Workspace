// file_dump.cpp : 定义控制台应用程序的入口点。
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
	DWORD lFileIndex;
	for(lFileIndex = 0; ; lFileIndex++) {
		if(INVA_INDE_VALU == EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileIndex))
			break;
		EntryPrint(&tFileEntry, lFileIndex);
	}
	/////////////////////////////////////////
	if(hFilesRiver != INVALID_HANDLE_VALUE) {
		CloseHandle( hFilesRiver );
		hFilesRiver = INVALID_HANDLE_VALUE;
	}
	return 0;
}

void EntryPrint(struct FileEntry *pFileEntry, DWORD lFileIndex) {
	wprintf(_T("lFileIndex:%d isonym_node:%d parent:%d sibling:%d chks:%d:%d name:%s\n"), 
		lFileIndex,
		pFileEntry->isonym_node,
		pFileEntry->parent,
		pFileEntry->sibling,
		pFileEntry->list_hinde,
		pFileEntry->chkslen,
		pFileEntry->szFileName);
}