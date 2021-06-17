#pragma once

//#include "CommonMacro.h"
#include "StringOpt.h"
#include "FileOpt.h"
#include "FilesVec.h"

#include <vector>
using std::vector;

#define SYNCING_LOCKED_DATA			_T("~\\SlockedData.dat")
#define NORMAL_MODIFIED_DATA		_T("~\\ModifiedData.dat")

struct IndexrowStruct
{
	DWORD AnchNumber;
	off_t FileOffset;
};

class CAnchorFile
{
public:
	CAnchorFile(void);
	~CAnchorFile(void);
// canchor base operate;
public:
	bool Create(wchar_t *sSyncingType, wchar_t *sAnchorIndex, wchar_t *sAnchorData, wchar_t *sLocation);
	bool Destroy();
// anchor data file operate;
private:
	//wchar_t m_sLocation[MAX_PATH];
	wchar_t	m_sAnchorIndex[MAX_PATH];
	wchar_t	m_sAnchorData[MAX_PATH];
	wchar_t m_sSyncingLockedFiles[MAX_PATH];
	wchar_t m_sSyncingType[MAX_PATH];
private:
	HANDLE m_hSyncingTypeFile;
	HANDLE m_hAnchIndexFile;
	HANDLE m_hAnchDataFile;
	HANDLE m_hSyncingLockedFiles;
public:
	DWORD LoadLastFilesVec(FilesAttribVec *vpFilesVecCache, DWORD *uipAnchor);
	DWORD SaveFilesStructVec(DWORD uiAnchor);
	DWORD FlushFilesVecFile(FilesAttribVec *vpFilesVecCache);
private:
	struct IndexrowStruct m_stLastIndexrow;
public:
	DWORD LoadLastIndexrow(OUT struct IndexrowStruct *pLastIndexrow);
	DWORD LoadLastAnchor(OUT DWORD *uipAnchor);
public:
	int AddNewAnchor(DWORD uiAnchor);
	//int AddNewAmd(char *sChangeName, char file_status);
	//int AddNewEn(char *sExistName, char *sNewName);
	int LoadFilesVecByIndexrow(FilesAttribVec *vpFilesAttVec, struct IndexrowStruct *pIndexrow);
//sync locked watch dir files modify insert to temp file
public:
	int LockedFilesInsert(wchar_t *sChangeName, char file_status);
	DWORD LoadLockedFilesVec(FilesAttribVec *vpLockedFilesVec);
	DWORD LoadLockedFilesVecTurnon(FilesAttribVec *vpLockedFilesVec);
public:
	inline bool CheckSlockedFilesExist() 
		{ return CFileOpt::CheckFileExist(m_sSyncingLockedFiles)? true: false; }
//for lan sync
public: 
	int SearchAnchor(struct IndexrowStruct *pIndexrowStart, struct IndexrowStruct *pIndexrowEnd, DWORD uiAnchor);
	int LoadFilesVecByIndexrow(FilesAttribVec *vpFilesAttVec, struct IndexrowStruct *pIndexrowStart, struct IndexrowStruct *pIndexrowEnd);
public:
#define SYNCING_TYPE_ERROR		-1
#define SYNCING_TYPE_SLOW		0x0000
#define SYNCING_TYPE_FAST		0x0001
	DWORD LoadSyncingType(); //0 is slow; 1 is fast; -1 error
	DWORD SaveSyncingType(DWORD iSyncingType); //0 is slow; 1 is fast; -1 error
};


extern CAnchorFile OAnchorFile;