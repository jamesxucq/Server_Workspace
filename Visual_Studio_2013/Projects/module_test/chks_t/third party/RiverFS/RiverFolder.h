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
	FolderHmap m_mRiverFolder;
	FolderHmap m_mIsonymFolder;
	IndexHmap m_mFolderIndex;
	//
	TCHAR m_szRiverFolder[MAX_PATH];
	HANDLE m_hRiverFolder;
private:
	VOID DelFolderEntry(struct FolderShadow *shadow);
	VOID EntryDelete(struct FolderShadow *shadow);
	struct FolderShadow *RebuildFolderEntryI(struct FolderEntry *pFolderEntry, ULONG lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath);
	struct FolderShadow *EntryRebuildI(ULONG lFolderIndex, FolderShadow *parent, DWORD dwExistsLength, const TCHAR *szFolderPath);
	VOID RebuildFolderEntryII(struct FolderShadow *shadow, DWORD dwExistsLength, const TCHAR *szFolderPath);
	VOID EntryRebuildII(struct FolderShadow *shadow, DWORD dwExistsLength, const TCHAR *szFolderPath);
public:
	DWORD IsDirectory(const TCHAR *szFolderPath);
	DWORD InsEntry(const TCHAR *szFolderPath, FILETIME *ftCreationTime, FolderShadow *pshado);
	DWORD EntryEraseRecu(const TCHAR *szFolderPath);
	DWORD EntryRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime, FolderShadow *pshado);
	DWORD MoveEntry(const TCHAR *szExistsPath, const TCHAR *szMovePath, FolderShadow *pshado);
public:
	ULONG FileSiblingIndex(ULONG *lParentIndex, const TCHAR *szFolderPath);
	ULONG FileSiblingIndex(ULONG lFolderIndex);
#define LEAF_INDEX_SET 0x0001
#define LEAF_INDEX_DEL 0x0002
	DWORD SetChildIndex(ULONG lFolderIndex, ULONG lLeafIndex);
	DWORD FolderNameIndex(TCHAR *szFolderPath, ULONG lFolderIndex);
	struct FolderShadow *FolderShadowName(const TCHAR *szFolderPath);
	struct FolderShadow *FolderShadowPtr(ULONG lFolderIndex);
public:
	DWORD FindIsonym(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime);
	DWORD FindRecycled(TCHAR *szExistsPath, const TCHAR *szFolderName, FILETIME *ftCreationTime);
	DWORD VoidRecycled();
};

extern CRiverFolder objRiverFolder;
