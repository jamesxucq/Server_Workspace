#pragma once
//

struct FileEntry {
	ULONG sibling, parent;
	union {
		ULONG isonym_node;
		ULONG recycled;
	};
	//
	TCHAR szFileName[MAX_PATH];
	unsigned char szFileChks[SHA1_DIGEST_LEN];
	FILETIME ftLastWrite;
	ULONG list_hinde;
	DWORD chkslen; /* chunk length */
};

#define INIT_FILE_ENTRY(entry) \
	memset(&entry, '\0', sizeof(struct FileEntry)); \
	entry.sibling = INVA_INDE_VALU; \
	entry.parent = INVA_INDE_VALU;\
	entry.isonym_node = INVA_INDE_VALU;\
	entry.list_hinde = INVA_INDE_VALU;

namespace EntryUtility
{
	DWORD FillIdleEntry(HANDLE hFilesRiver);
	ULONG FindIdleEntry(ULONG *lListHinde, DWORD *iListLength, HANDLE hFilesRiver);
	ULONG AddIdleEntry(HANDLE hFilesRiver, ULONG lIdleIndex);
	ULONG ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileIndex);
	ULONG WriteNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileIndex);
	ULONG ModifyNode(HANDLE hFilesRiver, ULONG lFileIndex, DWORD dwIndexType, ULONG lNextIndex);
	ULONG VoidNode(HANDLE hFilesRiver, ULONG lFileIndex);
}

//
namespace RiverUtility
{
	DWORD FillIdleEntry(HANDLE hChksList, HANDLE hFilesRiver, HANDLE hRiverFolder);
	ULONG InitializeEntry(HANDLE hRiverFolder, const TCHAR *szRootPath);
	ULONG InsFolderLeaf(HANDLE hRiverFolder, ULONG lFolderIndex, ULONG lChildIndex);
	ULONG InsFileItem(HANDLE hFilesRiver, ULONG lFileIndex, ULONG lSiblingIndex, ULONG lParentIndex, ULONG lListIndex, TCHAR *szFileName, DWORD chks_tally);
	ULONG InsFolderItem(HANDLE hRiverFolder, ULONG lFolderIndex, ULONG lParentIndex, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData);
	ULONG InsFileIsonym(HANDLE hFilesRiver, ULONG lFileIndex, ULONG lIsonymIndex);
	//
	ULONG InsChksListItem(HANDLE hChksList, DWORD chks_tally);
};
//