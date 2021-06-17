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
    unsigned __int64 qwFileSize;
	unsigned char szFileChks[SHA1_DIGEST_LEN];
	FILETIME ftCreatTime;
	FILETIME ftLastWrite;
	ULONG list_hinde;
	DWORD chkslen; /* chunk length */
};

//
#define INIT_FILE_ENTRY(entry) \
	memset(&entry, '\0', sizeof(struct FileEntry)); \
	entry.sibling = INVA_INDE_VALU; \
	entry.parent = INVA_INDE_VALU;\
	entry.isonym_node = INVA_INDE_VALU;\
	entry.list_hinde = INVA_INDE_VALU;

namespace EntryUtility {
	DWORD FillIdleEntry(HANDLE hFilesRiver);
	ULONG FindIdleEntry(HANDLE hFilesRiver, ULONG *lListHinde, DWORD *dwListLength);
	ULONG AddIdleEntry(HANDLE hFilesRiver, ULONG lIdleInde);
	ULONG ReadNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde);
	ULONG WriteNode(HANDLE hFilesRiver, struct FileEntry *pFileEntry, ULONG lFileInde);
	ULONG ModifyNode(HANDLE hFilesRiver, ULONG lFileInde, DWORD dwIndeType, ULONG lNextInde);
	ULONG VoidNode(HANDLE hFilesRiver, ULONG lFileInde);
	//
	ULONG FindRecyEntry(HANDLE hFilesRiver, ULONG lSibliInde, ULONG lFileInde);
}

//
namespace RiverUtility {
	DWORD FillIdleEntry(HANDLE hChksList, HANDLE hFilesRiver, HANDLE hRiverFolder);
	ULONG InitializeEntry(HANDLE hRiverFolder, const TCHAR *szRootPath);
	ULONG InsFolderLeaf(HANDLE hRiverFolder, ULONG lFolderInde, ULONG lChildInde);
	ULONG InsFileItem(HANDLE hFilesRiver, ULONG lFileInde, ULONG lSibliInde, ULONG lParentInde, ULONG lListInde, TCHAR *szFileName, unsigned __int64 qwFileSize, FILETIME *ftCreatTime, DWORD chks_tatol);
	ULONG InsFolderItem(HANDLE hRiverFolder, ULONG lFolderInde, ULONG lParentInde, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData);
	ULONG InsFileIsonym(HANDLE hFilesRiver, ULONG lFileInde, ULONG lIsonymInde);
	//
	ULONG InsChksListItem(HANDLE hChksList, DWORD chks_tatol);
};
//