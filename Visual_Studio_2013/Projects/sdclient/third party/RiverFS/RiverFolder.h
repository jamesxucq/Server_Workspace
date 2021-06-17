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
	IndexHmap m_mFolderInde;
	//
	TCHAR m_szRiverFolder[MAX_PATH];
	HANDLE m_hRiverFolder;
private:
	VOID DeliFolderEntry(struct FolderShadow *shadow);
	VOID EntryDelete(struct FolderShadow *shadow);
	VOID RebuildFolderEntry(struct FolderShadow *shadow, DWORD dwExistLength, const TCHAR *szFolderPath);
	VOID EntryRebuild(struct FolderShadow *shadow, DWORD dwExistLength, const TCHAR *szFolderPath);
public:
	DWORD IsDirectory(const TCHAR *szFolderPath);
	DWORD InsEntry(const TCHAR *szFolderPath, FILETIME *ftCreatTime, FolderShadow *pshado);
	DWORD EntryEraseRecurs(const TCHAR *szFolderPath);
	DWORD MoveEntry(const TCHAR *szExistPath, const TCHAR *szNewFile, FolderShadow *pshado);
public:
	ULONG FileSiblingIndex(ULONG *lFolderInde, const TCHAR *szFolderPath);
	ULONG FileSiblingIndex(ULONG lFolderInde);
#define LEAF_INDEX_SET 0x0001
#define LEAF_INDEX_DEL 0x0002
	DWORD SetChildIndex(ULONG lFolderInde, ULONG lLeafInde);
	DWORD FolderNameIndex(TCHAR *szFolderPath, ULONG lFolderInde);
	struct FolderShadow *FolderShadowName(const TCHAR *szFolderPath);
	struct FolderShadow *FolderShadowPtr(ULONG lFolderInde);
public:
	DWORD FindSibliAppA(HANDLE hINodeA, struct FolderShadow *sibling);
	DWORD FindSibliAppV(HANDLE hINodeV, struct FolderShadow *sibling);
	DWORD FindFolderName(TCHAR *szFolderPath, ULONG lFolderInde, ULONG lFileInde);
public:
	VOID CTimeUpdate(const TCHAR *szFolderPath, const TCHAR *szDriveLetter);
};

extern CRiverFolder objRiverFolder;
