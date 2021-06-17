#pragma once

#include "FolderUtility.h"

#define RIVER_DIRECTORY_DEFAULT				_T("~\\river_folder.sdo")

class CRiverFolder {
public:
	CRiverFolder(void);
	~CRiverFolder(void);
public:
	DWORD Initialize(const TCHAR *szLocation);
	DWORD Finalize();
private:
	vector <struct FolderSoul *> m_vRiverFolder;
	FolderHmap m_hmRiverFolder;
	FolderHmap m_hmNsakeFolder;
	IndexHmap m_hmFolderIndex;
	/////////////////////////////////////////////
	TCHAR m_szRiverFolder[MAX_PATH];
	HANDLE m_hRiverFolder;
private:
	void DelFolderEntry(struct FolderSoul *folderSoul);
	void EntryDelete(struct FolderSoul *folderSoul);
	struct FolderSoul *RebuildFolderEntryI(struct FolderEntry *pFolderEntry, DWORD lFolderIndex, FolderSoul *parent, DWORD dwExistsLength, const TCHAR *szFolderPath);
	struct FolderSoul *EntryRebuildI(DWORD lFolderIndex, FolderSoul *parent, DWORD dwExistsLength, const TCHAR *szFolderPath);
	void RebuildFolderEntryII(struct FolderSoul *folderSoul, DWORD dwExistsLength, const TCHAR *szFolderPath);
	void EntryRebuildII(struct FolderSoul *folderSoul, DWORD dwExistsLength, const TCHAR *szFolderPath);
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
	struct FolderSoul *FolderSoulPtr(DWORD lFolderIndex);
public:
	DWORD FindNsake(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime);
	DWORD FindRecycled(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime);
};

extern CRiverFolder objRiverFolder;
