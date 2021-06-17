#pragma once

#include "CommonUtility.h"
#include "FilesRiver.h"
#include "RiverFolder.h"

///////////////////////////////////////////////////////////////////////
namespace RiverFS {
///////////////////////////////////////////////////
	DWORD Initialize(TCHAR *szLocation, const TCHAR *szDriveRoot);
	DWORD Finalize();
///////////////////////////////////////////////////
	DWORD FolderIdenti(TCHAR *szExistsPath, const TCHAR *szFolderPath, FILETIME *ftCreationTime);
	DWORD ValidFolderDup(const TCHAR *szFolderPath, FILETIME *ftCreationTime);
#define IS_DIRECTORY(FOLDER_PATH)			objRiverFolder.IsDirectory(NO_ROOT(FOLDER_PATH))	
	DWORD FolderAppend(const TCHAR *szFolderPath, FILETIME *ftCreationTime);
#define ERASE_FOLDER(FOLDER_PATH)			objRiverFolder.EntryEraseRecu(NO_ROOT(FOLDER_PATH))
	DWORD FolderRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime);
	DWORD FolderMoved(const TCHAR *szExistsPath, const TCHAR *szMovePath);
///////////////////////////////////////////////////
	DWORD FindIdenti(TCHAR *szExistsFile, struct FileID *pFileID);
	DWORD ValidFileDup(struct FileID *pFileID);
	DWORD FileAppend(struct FileID *pFileID);
	DWORD DeleteFile(const TCHAR *szFilePath);
	DWORD FileRestore(struct FileID *pFileID);
	DWORD FileMoved(const TCHAR *szExistsPath, const TCHAR *szMovePath);
///////////////////////////////////////////////////
	VOID VoidRecycled();
};

#define RIVER_OPERATE_COPY	0x0001
#define RIVER_OPERATE_MOVE	0x0002
#define RIVER_OPERATE_ADD		0x0004