// nsake_dump.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

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

ULONG EntryUtility::ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde)
{
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
DWORD FolderEntryPrint(DWORD lNsakeIndex);
int _tmain(int argc, _TCHAR* argv[])
{
	if(2 != argc) printf("input error! argc:%d\n", argc);
	else FolderEntryPrint(_wtoi(argv[1]));

	return 0;
}

void EntryPrint(struct FileEntry *pFileEntry, DWORD lFileIndex) {
	wprintf(_T("lFileIndex:%d isonym_node:%d parent:%d sibling:%d %s\n"), 
		lFileIndex,
		pFileEntry->isonym_node,
		pFileEntry->parent,
		pFileEntry->sibling,
		pFileEntry->szFileName);
}

DWORD FolderEntryPrint(DWORD lNsakeIndex) {
	////////////////////////////////////////
	HANDLE hFilesRiver;
	/////////////////////////////////////////
	hFilesRiver = CreateFile( FILES_RIVER_DEFAULT, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( hFilesRiver == INVALID_HANDLE_VALUE ) {
		printf( "create file failed.\n");
		printf(" File: %s Line: %d\n", _T(__FILE__), __LINE__);
		return -1;
	}
	/////////////////////////////////////////
	struct FileEntry tFileEntry;
	DWORD lFileIndex = lNsakeIndex;
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