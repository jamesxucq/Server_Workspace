#include "StdAfx.h"
#include "AnchorFile.h"

//#include "Logger.h"
#include "FileOpt.h"
//#include "ParseConf.h"

CAnchorFile::CAnchorFile(void)
{
}

CAnchorFile::~CAnchorFile(void)
{
}

CAnchorFile OAnchorFile;

bool CAnchorFile::Create(wchar_t *sSyncingType, wchar_t *sAnchorIndex, wchar_t *sAnchorData, wchar_t *sLocation)
{
	CStringOpt::homepath(m_sSyncingType, sLocation, sSyncingType);
	m_hSyncingTypeFile = CreateFile( m_sSyncingType, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hSyncingTypeFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return false;
	} 

	CStringOpt::homepath(m_sAnchorIndex, sLocation, sAnchorIndex);
	m_hAnchIndexFile = CreateFile( m_sAnchorIndex, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hAnchIndexFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return false;
	} 

	CStringOpt::homepath(m_sAnchorData, sLocation, sAnchorData);
	m_hAnchDataFile = CreateFile( m_sAnchorData, 
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hAnchDataFile == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return false;
	}

	CStringOpt::homepath(m_sSyncingLockedFiles, sLocation, SYNCING_LOCKED_DATA);
	m_hSyncingLockedFiles = INVALID_HANDLE_VALUE;

	return true;
}

bool CAnchorFile::Destroy()
{
	if(m_hSyncingTypeFile != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hSyncingTypeFile );
		m_hSyncingTypeFile = INVALID_HANDLE_VALUE;
	}
	if(m_hAnchIndexFile != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hAnchIndexFile );
		m_hAnchIndexFile = INVALID_HANDLE_VALUE;
	}
	if(m_hAnchDataFile != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hAnchDataFile );
		m_hAnchDataFile = INVALID_HANDLE_VALUE;
	}
	if(m_hSyncingLockedFiles != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hSyncingLockedFiles );
		m_hSyncingLockedFiles = INVALID_HANDLE_VALUE;
	}
	
	return true;
}

DWORD CAnchorFile::LoadLastFilesVec(FilesAttribVec *vpFilesVecCache, DWORD *uipAnchor)
{
	struct IndexrowStruct stIndexrow;

	if(!vpFilesVecCache) 
		return -1;

	memset(&stIndexrow, '\0', sizeof(struct IndexrowStruct));
	if(LoadLastIndexrow(&stIndexrow))
		return -1;

	if(LoadFilesVecByIndexrow(vpFilesVecCache, &stIndexrow))
		return -1;

	memcpy(&m_stLastIndexrow, &stIndexrow, sizeof(struct IndexrowStruct));
	*uipAnchor = stIndexrow.AnchNumber;

	return 0; //succ
}

DWORD CAnchorFile::SaveFilesStructVec(DWORD uiAnchor)
{


	return 0; //succ
}

	
DWORD CAnchorFile::FlushFilesVecFile(FilesAttribVec *vpFilesVecCache)
{
	FilesAttribVec::iterator iter;
	DWORD retValue;
    DWORD uiWritenSize = 0;

	int iFilesAttSize = sizeof(struct FileAttrib);
	retValue = SetFilePointer(m_hAnchDataFile, m_stLastIndexrow.FileOffset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
	for(iter=vpFilesVecCache->begin(); iter!=vpFilesVecCache->end(); iter++) {
		if(!WriteFile(m_hAnchDataFile, *iter, iFilesAttSize, &uiWritenSize, NULL))
			return -1;
	}
	retValue = SetEndOfFile(m_hAnchDataFile);
	if(0x00==retValue && NO_ERROR!=GetLastError()) 
		return -1;
    FlushFileBuffers(m_hAnchDataFile);

	return 0; //succ
}

DWORD  CAnchorFile::LoadLastIndexrow(OUT struct IndexrowStruct *pLastIndexrow)
{
	struct IndexrowStruct stIndexrow;
	DWORD uiFileSize, uiFileHigh;
    DWORD uiReadSize = 0;

	memset(&stIndexrow, '\0', sizeof(struct IndexrowStruct));
	int iIndexrowSize = sizeof(struct IndexrowStruct);
	uiFileSize = GetFileSize(m_hAnchIndexFile, &uiFileHigh);
	if(INVALID_FILE_SIZE==uiFileSize  && NO_ERROR!=GetLastError()) 
		return FALSE;
	if(0x00 == uiFileSize) {
		memset(pLastIndexrow, '\0', iIndexrowSize);
		return 0;
	}

	memset(&stIndexrow, '\0', iIndexrowSize);
	DWORD retValue = SetFilePointer(m_hAnchIndexFile, -iIndexrowSize, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
    if(!ReadFile(m_hAnchIndexFile, &stIndexrow, iIndexrowSize, &uiReadSize, NULL))
		return -1;
    FlushFileBuffers(m_hAnchIndexFile);

	memcpy(pLastIndexrow, &stIndexrow, iIndexrowSize);
	return 0;
}


DWORD CAnchorFile::LoadLastAnchor(OUT DWORD *uipAnchor)
{
	struct IndexrowStruct stIndexrow;

	memset(&stIndexrow, '\0', sizeof(struct IndexrowStruct));
	if(LoadLastIndexrow(&stIndexrow))
		return -1;
	*uipAnchor = stIndexrow.AnchNumber;

	return 0;
}


int CAnchorFile::AddNewAnchor(DWORD uiAnchor)
{
	struct IndexrowStruct stIndexrow;
	DWORD uiFileSize, uiFileHigh;
    DWORD uiWritenSize = 0;

	memset(&stIndexrow, '\0', sizeof(struct IndexrowStruct));
	int iIndexrowSize = sizeof(struct IndexrowStruct);

    FlushFileBuffers(m_hAnchDataFile);
	uiFileSize = GetFileSize(m_hAnchDataFile, &uiFileHigh);
	if(INVALID_FILE_SIZE==uiFileSize  && NO_ERROR!=GetLastError()) 
		return -1;
	memset(&stIndexrow, '\0', iIndexrowSize);
	stIndexrow.AnchNumber = uiAnchor;
	stIndexrow.FileOffset = uiFileSize;

	DWORD retValue = SetFilePointer(m_hAnchIndexFile, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
    if(!WriteFile(m_hAnchIndexFile, &stIndexrow, iIndexrowSize, &uiWritenSize, NULL))
		return -1;
    FlushFileBuffers(m_hAnchIndexFile);

	memcpy(&m_stLastIndexrow, &stIndexrow, sizeof(struct IndexrowStruct));
	return 0;
}


int CAnchorFile::LoadFilesVecByIndexrow(FilesAttribVec *vpFilesAttVec, struct IndexrowStruct *pIndexrow)
{
	struct FileAttrib stFileAttrib;
	struct FileAttrib *fstw;
    DWORD uiReadSize = 0;

	memset(&stFileAttrib, '\0', sizeof(struct FileAttrib));
	int iFilesAttSize = sizeof(struct FileAttrib);
	DWORD retValue = SetFilePointer(m_hAnchDataFile, pIndexrow->FileOffset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
	do {
		if(!ReadFile(m_hAnchDataFile, &stFileAttrib, iFilesAttSize, &uiReadSize, NULL))
			return -1;
		if(uiReadSize == iFilesAttSize) {
			fstw = CFilesVec::AppendNewFilesNode(vpFilesAttVec);
			memcpy(fstw, &stFileAttrib, iFilesAttSize);
		}
	}while(uiReadSize == iFilesAttSize);

	return 0;
}

/*
int CAnchorFile::AddNewAmd(char *sChangeName, char file_status)
{
	struct FileAttrib AmdFilesAttSt = {0};
    DWORD uiWritenSize = 0;

	int iFilesAttSize = sizeof(struct FileAttrib);
	DWORD retValue = SetFilePointer(m_hAnchDataFile, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
	
	memset(&AmdFilesAttSt, '\0', iFilesAttSize);
	AmdFilesAttSt.status = file_status;
	strcpy_s(AmdFilesAttSt.filename, sChangeName);
    if(!WriteFile(m_hAnchDataFile, &AmdFilesAttSt, iFilesAttSize, &uiWritenSize, NULL))
		return -1;
    FlushFileBuffers(m_hAnchDataFile);

	return 0;
}

int CAnchorFile::AddNewEn(char *sExistName, char *sNewName)
{
	DWORD uiFileSize;
	struct FileAttrib EnFilesAttSt = {0};
    DWORD uiWritenSize = 0;

	uiFileSize = FileSizeN(m_hAnchDataFile, NULL);
	if(INVALID_FILE_SIZE==uiFileSize  && NO_ERROR!=GetLastError()) 
		return FALSE;

	int iFilesAttSize = sizeof(struct FileAttrib);
	DWORD retValue = SetFilePointer(m_hAnchDataFile, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;

	memset(&EnFilesAttSt, '\0', iFilesAttSize);
	EnFilesAttSt.status = EXIST;
	strcpy_s(EnFilesAttSt.filename, sExistName);
	EnFilesAttSt.Reserved = (WORD)uiFileSize;
    if(!WriteFile(m_hAnchDataFile, &EnFilesAttSt, iFilesAttSize, &uiWritenSize, NULL))
		return -1;
	memset(&EnFilesAttSt, '\0', iFilesAttSize);
	EnFilesAttSt.status = NEW;
	strcpy_s(EnFilesAttSt.filename, sNewName);
	EnFilesAttSt.Reserved = (WORD)uiFileSize;
    if(!WriteFile(m_hAnchDataFile, &EnFilesAttSt, iFilesAttSize, &uiWritenSize, NULL))
		return -1;
    FlushFileBuffers(m_hAnchDataFile);

	return 0;
}

DWORD FlushFilesVecFile()
{



	return 0;
}
*/


/*
FilesAttribVec FindFromFilesVecEx(FilesAttribVec vFilesVec, struct FileAttrib *fstw)
{
	static char sFileName[MAX_PATH];
	static FilesAttribVec item_pos = NULL;
	FilesAttribVec item = NULL;

	if(!flist && !fstw)
		item_pos = item_pos->next;
	else{
		item_pos = flist;
		strcpy_s(sFileName, fstw->filename);
	}
	LL_FOREACH(item_pos, item){
		if(!strcmp(item->files->filename, sFileName))
			break;	
	}
	item_pos = item;

	return item;
}
*/

int CAnchorFile::LockedFilesInsert(wchar_t *sChangeName, char file_status)
{
	struct FileAttrib stFileAttrib;
    DWORD uiWritenSize = 0;

	memset(&stFileAttrib, '\0', sizeof(struct FileAttrib));
	_tcscpy_s(stFileAttrib.filename, MAX_PATH, sChangeName);
	stFileAttrib.mod_type = file_status;
	if(m_hSyncingLockedFiles == INVALID_HANDLE_VALUE) {
		m_hSyncingLockedFiles = CreateFile( m_sSyncingLockedFiles, 
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ, 
			NULL, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if( m_hSyncingLockedFiles == INVALID_HANDLE_VALUE ) {
			TRACE( _T("create file failed.\n") );
			TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
			return FALSE;
		} 
	}
	DWORD retValue = SetFilePointer(m_hSyncingLockedFiles, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
    if(!WriteFile(m_hSyncingLockedFiles, &stFileAttrib, sizeof(struct FileAttrib), &uiWritenSize, NULL))
		return -1;
    FlushFileBuffers(m_hSyncingLockedFiles);

	return 0; //succ
}

DWORD CAnchorFile::LoadLockedFilesVec(FilesAttribVec *vpLockedFilesVec)
{
	struct FileAttrib stFileAttrib;
	struct FileAttrib *fstw;
    DWORD uiReadSize = 0;

	if(m_hSyncingLockedFiles == INVALID_HANDLE_VALUE) return 0x02;

	memset(&stFileAttrib, '\0', sizeof(struct FileAttrib));
	int iFilesAttSize = sizeof(struct FileAttrib);
	DWORD retValue = SetFilePointer(m_hSyncingLockedFiles, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
	do {
		if(!ReadFile(m_hSyncingLockedFiles, &stFileAttrib, iFilesAttSize, &uiReadSize, NULL))
			return -1;
		if(uiReadSize == iFilesAttSize) {
			fstw = CFilesVec::AppendNewFilesNode(vpLockedFilesVec);
			memcpy(fstw, &stFileAttrib, iFilesAttSize);
		}
	}while(uiReadSize == iFilesAttSize);

	CloseHandle( m_hSyncingLockedFiles );
	m_hSyncingLockedFiles = INVALID_HANDLE_VALUE;
	DeleteFile(m_sSyncingLockedFiles);

	return 0; //succ
}

DWORD CAnchorFile::LoadLockedFilesVecTurnon(FilesAttribVec *vpLockedFilesVec)
{
	if(m_hSyncingLockedFiles == INVALID_HANDLE_VALUE) {
		m_hSyncingLockedFiles = CreateFile( m_sSyncingLockedFiles, 
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ, 
			NULL, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if( m_hSyncingLockedFiles == INVALID_HANDLE_VALUE ) {
			TRACE( _T("create file failed.\n") );
			TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
			return FALSE;
		} 
	}
	return LoadLockedFilesVec(vpLockedFilesVec);
}


int CAnchorFile::SearchAnchor(struct IndexrowStruct *pIndexrowStart, struct IndexrowStruct *pIndexrowEnd, DWORD uiAnchor)
{
	HANDLE hMapFile;
	DWORD uiFileSize, uiFileHigh;
	struct IndexrowStruct *pIndexrow = NULL;
	struct IndexrowStruct *MidPos, *MinPos, *MaxPos;

	if(!pIndexrowStart || !pIndexrowEnd || 0>uiAnchor)
		return -1;
	int iIndexrowSize = sizeof(struct IndexrowStruct);
	uiFileSize = GetFileSize(m_hAnchIndexFile, &uiFileHigh);
	if(INVALID_FILE_SIZE==uiFileSize  && NO_ERROR!=GetLastError()) 
		return FALSE;
	if(0x00==uiFileSize && 0x00==uiAnchor) {
		memset(pIndexrowStart, '\0', iIndexrowSize);		
		memset(pIndexrowEnd, '\0', iIndexrowSize);
		return 0;
	}

	hMapFile = CreateFileMapping(m_hAnchIndexFile, NULL, PAGE_READWRITE, 0, uiFileSize, NULL);
	if(hMapFile == NULL) {
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}
	pIndexrow = (struct IndexrowStruct *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, uiFileSize);

	UINT iIndexNum = uiFileSize/iIndexrowSize;
	MaxPos = pIndexrow + iIndexNum-1;
	memset(pIndexrowEnd, '\0', iIndexrowSize);
	memcpy(pIndexrowEnd, MaxPos, iIndexrowSize);
	if (0 == pIndexrowEnd->AnchNumber){
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		UnmapViewOfFile(pIndexrow);
		CloseHandle( hMapFile );
		return -1;
	}

	MaxPos = pIndexrow + iIndexNum-1;
	MinPos = pIndexrow;
	MidPos = MinPos + iIndexNum/2;
	while(true){
		if(uiAnchor > MidPos->AnchNumber){
			MinPos = MidPos;
			iIndexNum = (UINT)(((MaxPos-MinPos)/iIndexrowSize) >> 1);
			MidPos = MinPos + iIndexNum;
		}
		else if(uiAnchor < MidPos->AnchNumber){
			MaxPos = MidPos;
			iIndexNum = (UINT)(((MaxPos-MinPos)/iIndexrowSize) >> 1);
			MidPos = MinPos + iIndexNum;
		}
		else if(uiAnchor == MidPos->AnchNumber){
			memset(pIndexrowStart, 0, iIndexrowSize);
			memcpy(pIndexrowStart, MidPos, iIndexrowSize);
			break;
		}
	}

	UnmapViewOfFile(pIndexrow);
	CloseHandle( hMapFile );

	return 0;
}

int CAnchorFile::LoadFilesVecByIndexrow(FilesAttribVec *vpFilesAttVec, struct IndexrowStruct *pIndexrowStart, struct IndexrowStruct *pIndexrowEnd)
{
	HANDLE hMapFile;
	DWORD uiFileSize, uiFileHigh;
	struct FileAttrib *fstw;
	struct FileAttrib *pDataFilesAtt = NULL;
	struct FileAttrib *pFilesAttPos = NULL;

	uiFileSize = GetFileSize(m_hAnchDataFile, &uiFileHigh);
	if(INVALID_FILE_SIZE==uiFileSize  && NO_ERROR!=GetLastError()) 
		return FALSE;
	if(0x00==uiFileSize) return 0;
	int iFilesAttSize = sizeof(struct FileAttrib);

	hMapFile = CreateFileMapping(m_hAnchDataFile, NULL, PAGE_READWRITE, 0, uiFileSize, NULL);
	if(hMapFile == NULL) {
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}
	int mlen = pIndexrowEnd->FileOffset - pIndexrowStart->FileOffset;
	pFilesAttPos = pDataFilesAtt = (struct FileAttrib *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, pIndexrowStart->FileOffset, mlen);
	for(int iIndex=0; iIndex<mlen/iFilesAttSize; iIndex++){
		fstw = CFilesVec::AppendNewFilesNode(vpFilesAttVec);
		memcpy(fstw, pFilesAttPos, iFilesAttSize);
		pFilesAttPos ++;
	}

	UnmapViewOfFile(pDataFilesAtt);
	CloseHandle( hMapFile );

	return 0;
}

DWORD CAnchorFile::LoadSyncingType()
{
	DWORD iSyncingType;
    DWORD uiReadSize = 0;

	DWORD retValue = SetFilePointer(m_hSyncingTypeFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
	if(!ReadFile(m_hSyncingTypeFile, &iSyncingType, sizeof(DWORD), &uiReadSize, NULL))
		return -1;
	if(uiReadSize != sizeof(DWORD)) 
		return -1;

	return iSyncingType;
}

DWORD CAnchorFile::SaveSyncingType(DWORD iSyncingType)
{
    DWORD uiWritenSize = 0;

	if(SYNCING_TYPE_SLOW!=iSyncingType && SYNCING_TYPE_FAST!=iSyncingType)
		return -1;

	DWORD retValue = SetFilePointer(m_hSyncingTypeFile, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==retValue && NO_ERROR!=GetLastError()) 
		return -1;
    if(!WriteFile(m_hSyncingTypeFile, &iSyncingType, sizeof(DWORD), &uiWritenSize, NULL))
		return -1;
	if(uiWritenSize != sizeof(DWORD)) 
		return -1;
    FlushFileBuffers(m_hSyncingTypeFile);

	return 0;
}