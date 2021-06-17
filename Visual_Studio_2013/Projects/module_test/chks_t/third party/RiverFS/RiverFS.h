#pragma once

#include "CommonUtility.h"
#include "ChksUtility.h"
#include "ChksList.h"
#include "FilesRiver.h"
#include "RiverFolder.h"

//
namespace RiverFS
{
	//
	DWORD Initialize(TCHAR *szLocation, const TCHAR *szDriveRoot);
	DWORD Finalize();
	//
	DWORD FolderIdenti(TCHAR *szExistsPath, const TCHAR *szFolderPath, FILETIME *ftCreationTime);
	DWORD PassFolderValid(const TCHAR *szFolderPath, FILETIME *ftCreationTime);
#define IS_DIRECTORY(FOLDER_PATH)			objRiverFolder.IsDirectory(FOLDER_PATH)
	DWORD FolderAppend(const TCHAR *szFolderPath, FILETIME *ftCreationTime);
	static DWORD FolderAppend(const TCHAR *szFolderPath, const TCHAR *szDriveRoot);
#define ERASE_FOLDER(FOLDER_PATH)			objRiverFolder.EntryEraseRecu(FOLDER_PATH)
	DWORD FolderRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime);
	DWORD FolderMoved(const TCHAR *szExistsPath, const TCHAR *szMovePath);
	//
	static BOOL FileExists(const TCHAR *szFileName);
	DWORD FileIdenti(TCHAR *szExistsFile, struct FileID *pFileID);
	DWORD PassFileValid(const TCHAR *szExistsFile, struct FileID *pFileID);
	ULONG FileAppend(struct FileID *pFileID);
	ULONG DeleteFile(const TCHAR *szFileName);
	ULONG FileRestore(const TCHAR *szFileName);
	ULONG FileRestore(struct FileID *pFileID);
	ULONG FileMoved(const TCHAR *szExistsFile, struct FileID *pFileID);
	ULONG FileMoved(const TCHAR *szExistsFile, const TCHAR *szMoveFile);
	VOID VoidRecycled();
	//
	DWORD InitFileID(struct FileID *pFileID, const TCHAR *szFileName, BOOL bFileExists);
#define CloseFileID(tFileID) if(tFileID.hRiveChks) CloseHandle(tFileID.hRiveChks)
	DWORD ChunkWriteUpdate(const TCHAR *szFileName, DWORD iListPosit, FILETIME *ftLastWrite); // no root
	static BOOL FolderMaker(const TCHAR *szFolderPath, const TCHAR *szDriveRoot);
	DWORD ChunkUpdate(const TCHAR *szFileName, DWORD iListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite, const TCHAR *szDriveRoot);
	//
	DWORD FileSha1(unsigned char *pSha1Chks, TCHAR *szFileName);
	DWORD ValidFileChks(TCHAR *szFileName, DWORD iListPosit); // 0:pass 1:error
	DWORD FileChks(HANDLE hRiveChks, TCHAR *szFileName, DWORD iListPosit);
	DWORD ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, DWORD iListPosit);
};

#define RIVER_OPERATE_FOUND		0x0000
#define RIVER_OPERATE_COPY		0x0001
#define RIVER_OPERATE_MOVE		0x0002
#define OPERATE_MOVE_MODIFY		0x0004
#define RIVER_OPERATE_ADD		0x0008