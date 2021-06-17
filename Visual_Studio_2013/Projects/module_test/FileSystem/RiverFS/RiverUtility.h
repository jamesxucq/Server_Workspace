#pragma once

///////////////////////////////////////////////////////////////////////
#define INVALID_INDEX_VALUE ((DWORD)-1)
#define ROOT_INDEX_VALUE	0

#define FILE_INDEX_NSAKE	0x0001
#define FILE_INDEX_SIBLING	0x0002
#define FILE_INDEX_LEAF		0x0004
#define FILE_INDEX_CHILD	0x0008

///////////////////////////////////////////////////////////////////////
#define FILESUM_LENGTH 16

struct FileEntry {
	DWORD nsake_bin, parent, sibling;
	////////////////////////////////////////
    FILETIME ftLastWriteTime;
	unsigned char sFileCsum[FILESUM_LENGTH];
	TCHAR szFileName[MAX_PATH];
};

#define INIT_FILE_ENTRY(ENTRY) \
	memset(&ENTRY, '\0', sizeof(struct FileEntry)); \
	ENTRY.nsake_bin = INVALID_INDEX_VALUE;\
	ENTRY.parent = INVALID_INDEX_VALUE;\
	ENTRY.sibling = INVALID_INDEX_VALUE;

namespace EntryUtility {
	DWORD FillEmptyEntry(HANDLE hFilesRiver);
	DWORD FindEmptyEntry(HANDLE hFilesRiver);
	DWORD AddEmptyEntry(HANDLE hFilesRiver, DWORD lEmptyIndex);
	DWORD ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, DWORD lFileIndex);
	DWORD WriteNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, DWORD lFileIndex);
	DWORD ModifyNode(HANDLE hFilesRiver, DWORD lFileIndex, DWORD dwIndexType, DWORD lNextIndex);
}

////////////////////////////////////////////////////////////////////////////////////////
namespace RiverUtility {
	DWORD FillEmptyEntry(HANDLE hFilesRiver, HANDLE hRiverFolder);
	DWORD InitializeEntry(HANDLE hRiverFolder, const TCHAR *szRootPath);
	DWORD InsFolderLeaf(HANDLE hRiverFolder, DWORD lFolderIndex, DWORD lChildIndex);
	DWORD InsFileItem(HANDLE hFilesRiver, DWORD lFileIndex, DWORD lSiblingIndex, DWORD lParentIndex, LPWIN32_FIND_DATA pFileData);
	DWORD InsFolderItem(HANDLE hRiverFolder, DWORD lFolderIndex, DWORD lParentIndex, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData);
	DWORD InsFileNsake(HANDLE hFilesRiver, DWORD lFileIndex, DWORD lNsakeIndex);
};
