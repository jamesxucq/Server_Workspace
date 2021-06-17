#include "StdAfx.h"
// #include "sdclient/LocalBzl.h"
// #include "AnchorBzl.h"
#include "FileUtility.h"
#include "CheckSum.h"
#include "../third party/RiverFS/RiverFS.h"

#include "FileHandler.h"
#include "./utility/ulog.h"

// MFC临界区类对象
// CCriticalSection gcs_HandlePath;
// static int modify_time;

CFileHandler::CFileHandler(void): CDirectoryHandler() {
}

CFileHandler::~CFileHandler(void) {
}

///////////////////////////////////////////////////////////////////////////////////////////
void CFileHandler::On_FileAdded(const TCHAR *szFileName) {
	EXCEPT_RECYCLER(szFileName)
	///////////////////////////////////////
	if(NFileUtility::IsDirectory(szFileName))
		m_objValidLayer.InsValidate(NValidVec::FillValid(szFileName, VALID_ADDITION|VALID_DIRECTORY));
	else m_objValidLayer.InsValidate(NValidVec::FillValid(szFileName, VALID_ADDITION));
}

void HandleDirAdded(struct Valid *pValid) {
	TCHAR szExistsPath[MAX_PATH];
	FILETIME ftCreationTime;
	///////////////////////////////////////////////////////////////
	TCHAR *szFileName = pValid->szFileName1;
	NFileUtility::CreationTime(&ftCreationTime, szFileName);
	///////////////////////////////////////////////////////////////
	switch(RiverFS::FolderIdenti(szExistsPath, szFileName, &ftCreationTime)) {
	case RIVER_OPERATION_COPY:
		logger(_T("c:\\river.log"), _T("dir copy:%s %s\r\n"), szExistsPath, szFileName);
		RiverFS::FolderAppend(szFileName, &ftCreationTime);
//		if(!NLocalBzl::ExceptCopy(szFileName)) NAnchorBzl::InsFolderCopy(szExistsPath, szFileName);
		break;
	case RIVER_OPERATION_MOVE:
		logger(_T("c:\\river.log"), _T("dir move:%s %s\r\n"), szExistsPath, szFileName);
		RiverFS::FolderRestore(szFileName, &ftCreationTime);
//		if(!NLocalBzl::ExceptMoved(szExistsPath)) NAnchorBzl::InsFolderMoved(szExistsPath, szFileName);
		break;
	case RIVER_OPERATION_ADD:
		logger(_T("c:\\river.log"), _T("dir add:%s\r\n"), szFileName);
		RiverFS::FolderAppend(szFileName, &ftCreationTime);
		// if(!NLocalBzl::ExceptAddition(szFileName)) NAnchorBzl::InsDirectory(szFileName);
		break;
	}	
	///////////////////////////////////////////////////////////////		
}

void HandleFileAdded(struct Valid *pValid) {
	TCHAR szExistsPath[MAX_PATH];
	struct FileID tAddedID;
	///////////////////////////////////////////////////////////////
	TCHAR *szFileName = pValid->szFileName1;
	file_csum(tAddedID.sFileCsum, szFileName);
	tAddedID.lFileSize = NFileUtility::FileSize(szFileName);
	_tcscpy_s(tAddedID.szFileName, MAX_PATH, szFileName);
	///////////////////////////////////////////////////////////////
	switch(RiverFS::FindIdenti(szExistsPath, &tAddedID)) {
	case RIVER_OPERATION_COPY:
		logger(_T("c:\\river.log"), _T("file copy:%s %s\r\n"), szExistsPath, szFileName);
		RiverFS::FileAppend(&tAddedID);
//		if(!NLocalBzl::ExceptCopy(szFileName)) NAnchorBzl::InsFileCopy(szExistsPath, szFileName);
		break;
	case RIVER_OPERATION_MOVE:
		logger(_T("c:\\river.log"), _T("file move:%s %s\r\n"), szExistsPath, szFileName);
		RiverFS::FileRestore(&tAddedID);
//		if(!NLocalBzl::ExceptMoved(szFileName)) NAnchorBzl::InsFileMoved(szExistsPath, szFileName);
		break;
	case RIVER_OPERATION_ADD:
		logger(_T("c:\\river.log"), _T("file add:%s\r\n"), szFileName);
		RiverFS::FileAppend(&tAddedID);
//		if(!NLocalBzl::ExceptAddition(szFileName)) NAnchorBzl::InsSingleAnchor(szFileName, ADD);
		break;
	}
	///////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////
void CFileHandler::On_FileRemoved(const TCHAR *szFileName) {
	EXCEPT_RECYCLER(szFileName)
	///////////////////////////////////////
	m_objValidLayer.InsValidate(NValidVec::FillValid(szFileName, VALID_DELETE));
}

void HandleRemoved(const TCHAR *szFileName) {
	/////////////////////////////
	if(IS_DIRECTORY(szFileName)) {
		logger(_T("c:\\river.log"), _T("dir del:%s\r\n"), szFileName);
		ERASE_FOLDER(szFileName);
//		if(!NLocalBzl::ExceptDelete(szFileName)) NAnchorBzl::InsSingleAnchor(szFileName, DEL|DIRECTORY);
	} else { //wprintf_s( L"Removed   - %s\n", szFileName );
		logger(_T("c:\\river.log"), _T("file del:%s\r\n"), szFileName);
		RiverFS::DeleteFile(szFileName);
//		if(!NLocalBzl::ExceptDelete(szFileName)) NAnchorBzl::InsSingleAnchor(szFileName, DEL);	
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
void CFileHandler::On_FileModified(const TCHAR *szFileName) {
	EXCEPT_RECYCLER(szFileName)
	///////////////////////////////////////
	if(!IS_DIRECTORY(szFileName))
		m_objValidLayer.InsValidate(NValidVec::FillValid(szFileName, VALID_MODIFY));
}

void HandleModified(const TCHAR *szFileName) {
	logger(_T("c:\\river.log"), _T("file modify:%s\r\n"), szFileName);
//	if(!NLocalBzl::ExceptModify(szFileName)) NAnchorBzl::InsSingleAnchor(szFileName, MODIFY);
}

///////////////////////////////////////////////////////////////////////////////////////////
void CFileHandler::On_FileNameChanged(const TCHAR *szExistsName, const TCHAR *szNewName) {
	m_objValidLayer.InsValidate(NValidVec::FillValid(szExistsName, szNewName, VALID_MOVE));
}

void HandleMoved(const TCHAR *szExistsName, const TCHAR *szNewName) {
	if(IS_DIRECTORY(szExistsName)) {
		RiverFS::FolderMoved(szExistsName, szNewName);
		logger(_T("c:\\river.log"), _T("dir move:%s %s\r\n"), szExistsName, szNewName);
//		if(!NLocalBzl::ExceptMoved(szExistsName)) NAnchorBzl::InsFolderMoved(szExistsName, szNewName);
	} else { // LOG_DEBUG(_T("modify handler, exists:%s new:%s"), szExistsName, szNewName);
		logger(_T("c:\\river.log"), _T("file move:%s %s\r\n"), szExistsName, szNewName);
		RiverFS::FileMoved(szExistsName, szNewName);
//		if(!NLocalBzl::ExceptMoved(szExistsName)) NAnchorBzl::InsFileMoved(szExistsName, szNewName);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
void CFileHandler::On_WatchStarted(DWORD dwErrorCode, const TCHAR *szDirectory) {
	//if( dwError == 0 ) m_listBox.AddString( _T("Directory Watch Started: ") + szDirectory);
	//else.m_listBox.AddString( _T("Directory watch failed: ") + szDirectory);
	if( dwErrorCode == 0 ) LOG_DEBUG(_T("handler directory watch started: %s"), szDirectory);
	else LOG_DEBUG(_T("handler directory watch failed: %s"), szDirectory);
	////////////////////////////////////////
	m_objValidLayer.Initialize();
}

void CFileHandler::On_WatchStopped(const TCHAR *szDirectory) {
	LOG_DEBUG(_T("handler directory watch stoped: %s"), szDirectory);
	////////////////////////////////////////
	m_objValidLayer.Finalize();
}
