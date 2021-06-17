#pragma once

#include "common_macro.h"
#include "FileUtility.h"
#include "FilesVec.h"

#include <vector>
using std::vector;

// #define NORMAL_MODIFIED_DEFAULT			_T("~\\ModifiedData.dat")
#define LOCKED_FILES_DEFAULT				_T("~\\LockedDefault.tmp")

#pragma	pack(1)
struct IndexValue {
	DWORD AnchorNumber;
	off_t FileOffset;
};

struct anchor_attrib {
    WORD wLength;
	///////////////////////////////////////////////////////////////////
	DWORD modify_type;		/* add mod del list recu */
	///////////////////////////////////////////////////////////////////
	DWORD dwFileSize;
	FILETIME ftLastWrite;		/* When the item was last modified */
	///////////////////////////////////////////////////////////////////
	DWORD dwReserved;  /* filehigh */
	WORD wNameLength;
	TCHAR szFileName[1];
};
#pragma	pack()

class CAnchor {
public:
	CAnchor(void);
	~CAnchor(void);
	// canchor base operate;
public:
	DWORD Initialize(TCHAR *szAnchorIndex, TCHAR *szAnchorData, TCHAR *szLocation);
	DWORD Finalize();
	// anchor data file operate;
private:
	//TCHAR m_szLocation[MAX_PATH];
	TCHAR	m_szAnchorIndex[MAX_PATH];
	TCHAR	m_szAnchorData[MAX_PATH];
	TCHAR m_szLockedFiles[MAX_PATH];
private:
	HANDLE m_hAnchorIndex;
	HANDLE m_hAnchorData;
	HANDLE m_hLockedFiles;
public:
	DWORD LoadLastFiles(FilesVec *pFilesCache, DWORD *uipAnchor);
	DWORD SaveFilesStructVec(DWORD dwAnchor);
	DWORD FlushFilesVecFile(FilesVec *pFilesCache);
private:
	struct IndexValue m_tLastIndex;
public:
	DWORD LoadLastIndexValue(OUT struct IndexValue *pLastIndex);
	DWORD LoadLastAnchor(OUT DWORD *uipAnchor);
public:
	DWORD AddNewAnchor(DWORD dwNewAnchor);
	DWORD LoadFilesByIndexValue(FilesVec *pFilesVec, struct IndexValue *pIndexValue);
	//sync locked watch dir files modify insert to temp file
public:
	DWORD InsertLockedFiles(const TCHAR *szFileName, const DWORD modify_type);
	DWORD LoadLockedFiles(FilesVec *pLockedFiles);
	DWORD LoadLockedFilesTurnon(FilesVec *pLockedFiles);
public:
	inline DWORD IsLockedFilesExists() 
	{ return NFileUtility::FileExists(m_szLockedFiles)? 0: -1; }
	//for lan sync
public: 
#define SEARCH_FAULT        -1
#define SEARCH_FOUND        0x0000
#define SEARCH_NOT_FOUND    0x0001
	DWORD SearchAnchor(struct IndexValue *pIndexStart, struct IndexValue *pIndexEnd, DWORD dwAnchor);
	DWORD LoadFilesByIndexValue(FilesVec *pFilesVec, struct IndexValue *pIndexStart, struct IndexValue *pIndexEnd);
};

extern CAnchor objAnchor;