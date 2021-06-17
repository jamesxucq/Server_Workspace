#pragma once

#include "..\dbz\kchashdb.h"
#include "RiverUtility.h"

#include <vector>
using  std::vector;

using namespace std;
using namespace kyotocabinet;

#define FILES_RIVER_DEFAULT				_T("~\\files_river.sdo")
#define FILES_NSAKE_DEFAULT				_T("~\\files_nsake_bin.kct")


///////////////////////////////////////////////////////////////////////
struct FileID{
	TCHAR szFileName[MAX_PATH];
	unsigned char sFileCsum[FILESUM_LENGTH];
	unsigned long lFileSize;
};

class CFilesRiver {
public:
	CFilesRiver(void);
	~CFilesRiver(void);
public:
	DWORD Initialize(const TCHAR *szLocation);
	DWORD Finalize();
private:
	TreeDB db;
	/////////////////////////////////////////////
	TCHAR m_szFilesRiver[MAX_PATH];
	HANDLE m_hFilesRiver;
public:
	DWORD InsEntry(const TCHAR *szFileName, unsigned char *sFileCsum, DWORD lParentIndex, DWORD lSiblingIndex);
	DWORD DelFileEntry(DWORD *lNextIndex, const TCHAR *szFileName, DWORD lSiblingIndex); // step 1;
	DWORD EntryRestore(const TCHAR *szFileName, unsigned char *sFileCsum, DWORD lParentIndex, DWORD lSiblingIndex);
	DWORD ExistsEntry(DWORD *lNextIndex, const TCHAR *szFileName, DWORD lSiblingIndex); // step 1;
	DWORD MoveEntry(DWORD lFileIndex, DWORD lParentIndex, DWORD lSiblingIndex);
public:
	DWORD EntryRestore(DWORD lSiblingIndex);
	DWORD DelFolderEntry(DWORD lSiblingIndex);
public:
	DWORD FindNsake(const TCHAR *szFileName, struct FileID *pFileID);
	DWORD FindRecycled(const TCHAR *szFileName, unsigned char *sFileCsum);
	DWORD VoidRecycled();
};

extern CFilesRiver objFilesRiver;
