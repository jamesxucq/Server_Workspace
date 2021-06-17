#pragma once

#include "FolderUtility.h"

#define RIVER_FOLDER_DEFAULT				_T("~\\river_folder.sdo")

class CRiverFolder {
public:
	CRiverFolder(void);
	~CRiverFolder(void);
public:
	DWORD Initialize(const TCHAR *szLocation);
	DWORD Finalize();
private:
	vector <struct FolderShadow *> m_vRiverFolder;
	FolderHmap m_hmRiverFolder;
	FolderHmap m_hmNsakeFolder;
	IndexHmap m_hmFolderIndex;
	/////////////////////////////////////////////
	TCHAR m_szRiverFolder[MAX_PATH];
	HANDLE m_hRiverFolder;
private:
	VOID DelFolderEntry(struct FolderShadow *shadow);
	VOID EntryDelete(struct FolderShadow *shadow);
	struct FolderShadow *RebuildFolderEntryI(struct FolderEntry *pFolderEntry, DWORD lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath);
	struct FolderShadow *EntryRebuildI(DWORD lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath);
	VOID RebuildFolderEntryII(struct FolderShadow *shadow, DWORD dwExistsLength, const TCHAR *szFolderPath);
	VOID EntryRebuildII(struct FolderShadow *shadow, DWORD dwExistsLength, const TCHAR *szFolderPath);
public:
	DWORD IsDirectory(const TCHAR *szFolderPath);
	DWORD InsEntry(const TCHAR *szFolderPath, FILETIME *ftCreationTime, DWORD lParentIndex);
	DWORD EntryEraseRecu(const TCHAR *szFolderPath);
	DWORD EntryRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime, DWORD lParentIndex);
	DWORD MoveEntry(const TCHAR *szExistsPath, const TCHAR *szMovePath, DWORD lParentIndex);
public:
	DWORD FileSiblingIndex(DWORD *lParentIndex, const TCHAR *szFolderPath);
	DWORD FileSiblingIndex(DWORD lFolderIndex);
#define LEAF_INDEX_SET 0x0001
#define LEAF_INDEX_DEL 0x0002
	DWORD SetChildIndex(DWORD lFolderIndex, DWORD lLeafIndex);
	DWORD FolderNameIndex(TCHAR *szFolderPath, DWORD lFolderIndex);
	DWORD FolderIndexName(const TCHAR *szFolderPath);
private:
	struct FolderShadow *FolderShadowPtr(DWORD lFolderIndex);
public:
	DWORD FindNsake(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime);
	DWORD FindRecycled(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime);
	DWORD VoidRecycled();
};

extern CRiverFolder objRiverFolder;
