#pragma once

#include "CommonUtility.h"
#include "ChksUtility.h"
#include "INodeUtility.h"
#include "ChksList.h"
#include "FilesRiver.h"
#include "RiverFolder.h"

//
namespace RiverFS {
	// 初始化接口
	DWORD Initialize(TCHAR *szLocation, const TCHAR *szDriveLetter);
	DWORD Finalize();
	// 本地目录接口
#define IS_DIRECTORY(FOLDER_PATH)			objRiverFolder.IsDirectory(FOLDER_PATH) // no letter
	DWORD FolderAppendVc(const TCHAR *szFolderPath, FILETIME *ftCreatTime, const TCHAR *szDriveLetter); // no letter
#define ERASE_FOLDER_VC(FOLDER_PATH)			objRiverFolder.EntryEraseRecurs(FOLDER_PATH) // no letter
	DWORD FolderRecursINodeA(HANDLE hRecursINodeA, const TCHAR *szFolderPath); // no letter
	DWORD FolderMoved(const TCHAR *szExistPath, const TCHAR *szNewFile, const TCHAR *szDriveLetter); // no letter
	// 本地文件接口
	BOOL FileExist(const TCHAR *szFileName); // no letter
	ULONG FileAppend(struct FileID *pFileID, const TCHAR *szDriveLetter); // no letter
	ULONG DeleFileV(const TCHAR *szFileName); // no letter
	ULONG FileMoveV(const TCHAR *szExistFile, const TCHAR *szNewFile, const TCHAR *szDriveLetter); // no letter
	ULONG FileCopyV(const TCHAR *szExistFile, const TCHAR *szNewFile, const TCHAR *szDriveLetter); // no letter
	// 功能接口
	DWORD FindDupliFile(TCHAR *szExistFile, const TCHAR *szFileName, const TCHAR *szDriveLetter, unsigned __int64 qwFileSize, unsigned char *szSha1Chks);
	VOID InitFolderID(struct FileID *pFolderID, const TCHAR *szFileName);
	VOID InitFileID(struct FileID *pFileID, const TCHAR *szFileName);
#define CloseFileID(tFileID) if(INVALID_HANDLE_VALUE != tFileID.hRiveChks) { \
		CloseHandle(tFileID.hRiveChks); \
		tFileID.hRiveChks = INVALID_HANDLE_VALUE; \
	}
	DWORD RiverRecursINodeV(HANDLE hRiverINodeV);
	// 服务器目录接口
	DWORD FolderAppendVs(const TCHAR *szFolderPath, const TCHAR *szDriveLetter);
	DWORD EraseFolderVs(const TCHAR *szFolderPath);
	// 服务器文件接口
	DWORD ChunkLawiUpdate(const TCHAR *szFileName, DWORD dwListPosit, DWORD chunk_length, FILETIME *ftLastWrite); // no letter
	DWORD FileLawiUpdate(const TCHAR *szFileName, FILETIME *ftLastWrite); // no letter
	DWORD ChunkUpdate(const TCHAR *szFileName, DWORD dwListPosit, DWORD chunk_length, unsigned char *md5_chks, FILETIME *ftLastWrite, const TCHAR *szDriveLetter);
	DWORD FileUpdate(const TCHAR *szFileName, FILETIME *ftLastWrite);
	DWORD FileUpdate(const TCHAR *szFileName, FILETIME *ftLastWrite, const TCHAR *szDriveLetter);
	// 缓存效验值接口
	DWORD FileSha1Relay(unsigned char *szSha1Chks, TCHAR *szFileName);
	DWORD FileAttribCache(unsigned __int64 *qwFileLength, unsigned char *szSha1Chks, FILETIME *ftCreatTime, TCHAR *szFileName); // no letter
	DWORD FileChksRelay(HANDLE hRiveChks, TCHAR *szFileName, DWORD dwListPosit);
	DWORD ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, DWORD dwListPosit);
};

// river identi action type
#define RIVER_OPERATE_FOUND		0x0000
#define RIVER_OPERATE_COPY		0x0001