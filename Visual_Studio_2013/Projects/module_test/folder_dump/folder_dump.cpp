// folder_dump.cpp : 定义控制台应用程序的入口点。
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
DWORD FolderEntryPrint(struct FolderEntry *pFolderEntry, DWORD lFolderIndex);
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

void FolderPrint(struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	wprintf(_T("lFolderIndex:%d parent:%d child:%d sibling:%d leaf_inde:%d name:%s\n"), 
		lFolderIndex, 
		pFolderEntry->parent, 
		pFolderEntry->child, 
		pFolderEntry->sibling, 
		pFolderEntry->leaf_inde, 
		pFolderEntry->szFolderName);
}

void EntryPrint(struct FileEntry *pFileEntry, DWORD lFileIndex) {
	wprintf(_T("    lFileIndex:%d isonym_node:%d parent:%d sibling:%d name:%s\n"), 
		lFileIndex,
		pFileEntry->isonym_node,
		pFileEntry->parent,
		pFileEntry->sibling,
		pFileEntry->szFileName);
}

DWORD FolderEntryPrint(struct FolderEntry *pFolderEntry, DWORD lFolderIndex) {
	FolderPrint(pFolderEntry, lFolderIndex);
	////////////////////////////////////////
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
	DWORD lFileIndex = pFolderEntry->leaf_inde;
	while(INVA_INDE_VALU != EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileIndex)) {
		EntryPrint(&tFileEntry, lFileIndex);
		lFileIndex = tFileEntry.sibling;
	}
	/////////////////////////////////////////
	if(hFilesRiver != INVALID_HANDLE_VALUE) {
		CloseHandle( hFilesRiver );
		hFilesRiver = INVALID_HANDLE_VALUE;
	}
	return 0;
}