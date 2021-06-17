#include "StdAfx.h"
#include "CommonUtility.h"

#include "FolderUtility.h"
#include "RiverUtility.h"
#include "../../clientcom/utility/ulog.h"

DWORD EntryUtility::FillEmptyEntry(HANDLE hFilesRiver) {
	struct FileEntry tFileEntry;
	DWORD wlen;

	INIT_FILE_ENTRY(tFileEntry)
	DWORD lEmptyOffset = SetFilePointer(hFilesRiver, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return -1;
	if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, NULL))
		return -1;

	return 0;
}

DWORD EntryUtility::FindEmptyEntry(HANDLE hFilesRiver) {
	struct FileEntry tFillEntry, tFileEntry;
	LONG lEmptyOffset, lEmptyIndex = 0;
	DWORD rlen, wlen;

	lEmptyOffset = SetFilePointer(hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}

	if(INVALID_INDEX_VALUE != tFillEntry.nsake_bin) {
		lEmptyIndex = tFillEntry.nsake_bin;
		if(INVALID_INDEX_VALUE == ReadNode(hFilesRiver, &tFileEntry, lEmptyIndex))
			 return INVALID_INDEX_VALUE;

		if(INVALID_INDEX_VALUE != tFileEntry.sibling) {
			tFillEntry.nsake_bin = tFileEntry.sibling;
			ModifyNode(hFilesRiver, tFileEntry.sibling, FILE_INDEX_NSAKE, tFileEntry.nsake_bin);
		} else tFillEntry.nsake_bin = tFileEntry.nsake_bin;

		OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		OverLapped.Offset = lEmptyOffset;
		if(!WriteFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
			return INVALID_INDEX_VALUE;
	} else {
		if(FillEmptyEntry(hFilesRiver)) return INVALID_INDEX_VALUE;
		lEmptyIndex = lEmptyOffset / sizeof(struct FileEntry);
	}

	return lEmptyIndex;
}

DWORD EntryUtility::AddEmptyEntry(HANDLE hFilesRiver, DWORD lFileIndex) {
	struct FileEntry tFillEntry;
	DWORD rlen, wlen;
	/////////////////////////////////////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	/////////////////////////////////////////////////////////////////////////////
	ModifyNode(hFilesRiver, lFileIndex, FILE_INDEX_NSAKE, tFillEntry.nsake_bin);
	/////////////////////////////////////////////////////////////////////////////
	tFillEntry.nsake_bin = lFileIndex;
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lEmptyOffset;
	if(!WriteFile(hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	// FlushFileBuffers(hFilesRiver);
	return lFileIndex;
}

DWORD EntryUtility::ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, DWORD lFileIndex) {
	DWORD rlen;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
	if(!ReadFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &rlen, &OverLapped)) {
		// if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVALID_INDEX_VALUE;
	}

	return lFileIndex;
}

DWORD EntryUtility::WriteNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, DWORD lFileIndex) {
	DWORD wlen;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
	if(!WriteFile(hFilesRiver, pFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lFileIndex;
}

DWORD EntryUtility::ModifyNode(HANDLE hFilesRiver, DWORD lFileIndex, DWORD dwIndexType, DWORD lNextIndex) {
	DWORD rlen, wlen;
	struct FileEntry tFileEntry;
	DWORD lExistsIndex = INVALID_INDEX_VALUE;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);
	if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, &OverLapped)) {
		// if(ERROR_HANDLE_EOF != GetLastError()) 
		return INVALID_INDEX_VALUE;
	}

	switch(dwIndexType) {
	case FILE_INDEX_NSAKE:
		lExistsIndex = tFileEntry.nsake_bin;
		tFileEntry.nsake_bin = lNextIndex;
		break;
	case FILE_INDEX_SIBLING:
		lExistsIndex = tFileEntry.sibling;
		tFileEntry.sibling = lNextIndex;
		break;
	}

if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lExistsIndex;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD RiverUtility::InitializeEntry(HANDLE hRiverFolder, const TCHAR *szRootPath) {
	struct FolderEntry tFolderEntry;
	DWORD lFolderIndex = ROOT_INDEX_VALUE;

	INIT_FOLDER_ENTRY(tFolderEntry)
	_tcscpy_s(tFolderEntry.szFolderName, MAX_PATH, szRootPath);
	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderIndex))
		return INVALID_INDEX_VALUE;

	return lFolderIndex;
}

DWORD RiverUtility::FillEmptyEntry(HANDLE hFilesRiver, HANDLE hRiverFolder) {
	struct FileEntry tFileEntry;
	DWORD wlen;

	INIT_FILE_ENTRY(tFileEntry)
	DWORD lEmptyOffset = SetFilePointer(hFilesRiver, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return -1;
	if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, NULL))
		return -1;
	//////////////////////////////////////////////////////////////	
	struct FolderEntry tFolderEntry;

	INIT_FOLDER_ENTRY(tFolderEntry)
	lEmptyOffset = SetFilePointer(hRiverFolder, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return -1;
	if(!WriteFile(hRiverFolder, &tFolderEntry, sizeof(struct FolderEntry), &wlen, NULL))
		return -1;

	return 0;
}

DWORD RiverUtility::InsFolderLeaf(HANDLE hRiverFolder, DWORD lFolderIndex, DWORD lChildIndex) {
	struct FolderEntry tFolderEntry;
	DWORD lFileIndex;

	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lFolderIndex))
		return INVALID_INDEX_VALUE;

	lFileIndex = tFolderEntry.leaf_index;
	tFolderEntry.leaf_index = lChildIndex;

	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderIndex))
		return INVALID_INDEX_VALUE;

	return lFileIndex;
}

DWORD RiverUtility::InsFileItem(HANDLE hFilesRiver, DWORD lFileIndex, DWORD lSiblingIndex, DWORD lParentIndex, LPWIN32_FIND_DATA pFileData) {
	struct FileEntry tFileEntry;

	INIT_FILE_ENTRY(tFileEntry)
	tFileEntry.ftCreationTime = pFileData->ftCreationTime;
	tFileEntry.sibling = lSiblingIndex;
	tFileEntry.parent = lParentIndex;
	_tcscpy_s(tFileEntry.szFileName, MAX_PATH, pFileData->cFileName);

	if(INVALID_INDEX_VALUE == EntryUtility::WriteNode(hFilesRiver, &tFileEntry, lFileIndex))
		return INVALID_INDEX_VALUE;

	return lFileIndex;
}

DWORD RiverUtility::InsFolderItem(HANDLE hRiverFolder, DWORD lFolderIndex, DWORD lParentIndex, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
	struct FolderEntry tFolderEntry;
	DWORD lChildIndex;
	/////////////////////////////////////////////////////////
	if(INVALID_INDEX_VALUE == FolderUtility::ReadNode(hRiverFolder, &tFolderEntry, lParentIndex))
		return INVALID_INDEX_VALUE;

	lChildIndex = tFolderEntry.child;
	tFolderEntry.child = lFolderIndex;
	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lParentIndex))
		return INVALID_INDEX_VALUE;
	/////////////////////////////////////////////////////////
	INIT_FOLDER_ENTRY(tFolderEntry)
	tFolderEntry.ftCreationTime = pFileData->ftCreationTime;
	tFolderEntry.sibling = lChildIndex;
	tFolderEntry.parent = lParentIndex;
	CommonUtility::full_name(tFolderEntry.szFolderName, szPathString, pFileData->cFileName);

	if(INVALID_INDEX_VALUE == FolderUtility::WriteNode(hRiverFolder, &tFolderEntry, lFolderIndex))
		return INVALID_INDEX_VALUE;
	/////////////////////////////////////////////////////////
	return lFolderIndex;
}

DWORD RiverUtility::InsFileNsake(HANDLE hFilesRiver, DWORD lFileIndex, DWORD lNsakeIndex) {
	struct FileEntry tFileEntry;
	DWORD rlen, wlen;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lFileIndex * sizeof(struct FileEntry);

	if(!ReadFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, &OverLapped)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	tFileEntry.nsake_bin = lNsakeIndex;
	if(!WriteFile(hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
		return INVALID_INDEX_VALUE;

	return lFileIndex;
}