#pragma once
#include "clientcom/clientcom.h"
#include "AnchorBzl.h"

namespace NShellLinkBzl {
	// return 0; succ/ return -1 error
	DWORD Initialize(FileFobdVec *pFobdVec, FileColiVec *pColiVec, FilesVec *pActioVec, const TCHAR *szDriveLetter);
	DWORD Finalize();
	// 文件状态图标显示
	DWORD DisplayDenyVector(FileFobdVec *pFobdVec, const TCHAR *szDriveLetter);
	DWORD DisplayForbidVector(FileFobdVec *pFobdVec, const TCHAR *szDriveLetter);
	DWORD DisplayConfliVector(FileColiVec *pColiVec, const TCHAR *szDriveLetter);
	static DWORD DisplayFilesVector(FilesVec *pFilesVec, const TCHAR *szDriveLetter);
	//
	VOID ExploSleep(DWORD dwSleepTime, const TCHAR *szDriveLetter);
	VOID BatchIconDisplay(const TCHAR *szFileName, DWORD isdire, DWORD dwIconsStatus);
#define BATCH_ICON_DISPLAY(FILE_NAME, ICONS_STATUS) { \
	objCommandLinker.SendCommand(ICONS_STATUS, FILE_NAME); \
/* _LOG_DEBUG(_T("+++ szFileName:%s dwIconsStatus:%X"), FILE_NAME, ICONS_STATUS); */ \
}
	VOID DisplayFileSyncing(const TCHAR *szFileName, DWORD isdire);
	VOID DisplayFileNormal(const TCHAR *szFileName, DWORD isdire);
	VOID DisplayFilePrevious(const TCHAR *szFileName, DWORD isdire);
	//
	inline VOID ClearDisplayDirectory() 
	{ objCommandLinker.CacheClear(FILE_STATUS_DIRECTORY); }
	//
	DWORD DisplayLockedVector(FilesVec *pFilesVec, const TCHAR *szDriveLetter);
	DWORD DisplayPreviousVector(FilesVec *pFilesVec, const TCHAR *szDriveLetter, DWORD dwIconsStatus);
};

//
#define FILE_ICON_DISPLAY(FILE_NAME, ICON_STATUS) \
	objCommandLinker.FastSend(ICON_STATUS, FILE_NAME); \
/*_LOG_DEBUG(_T("+++ FILE_NAME:%s ICON_STATUS:%X"), FILE_NAME, ICON_STATUS);*/

//
#define LINKER_DISPLAY_CONFLI(FILE_NAME) \
	objCommandLinker.CacheSeta(szFileName, FILE_STATUS_FILE|FILE_STATUS_CONFLICT);

#define LINKER_REVOKE_CONFLI(FILE_NAME) \
	objCommandLinker.CacheErase(szFileName, FILE_STATUS_FILE|FILE_STATUS_CONFLICT);