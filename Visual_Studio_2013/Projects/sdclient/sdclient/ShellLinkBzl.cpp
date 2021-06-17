#include "StdAfx.h"

#include "WatcherBzl.h"
#include "ShellLinkBzl.h"


DWORD NShellLinkBzl::Initialize(FileFobdVec *pFobdVec, FileColiVec *pColiVec, FilesVec *pActioVec, const TCHAR *szDriveLetter) {
	if(!pFobdVec || !szDriveLetter) return ((DWORD)-1);
//
	// Open SDCache Pipe
	objCommandLinker.Initialize();
//
	if(objCommandLinker.CacheClear(FILE_STATUS_INVALID)) 
		return ((DWORD)-1);
	if(objCommandLinker.SetWatchDire(szDriveLetter)) 
		return ((DWORD)-1);
	if(DisplayForbidVector(pFobdVec, szDriveLetter)) 
		return ((DWORD)-1);
	if(DisplayConfliVector(pColiVec, szDriveLetter)) 
		return ((DWORD)-1);
	if(DisplayFilesVector(pActioVec, szDriveLetter)) 
		return ((DWORD)-1);
	//	objCommandLinker.CacheSeta(_T("H:\\4.txt"), FILE_STATUS_ADDED);
	//	objCommandLinker.CacheSeta(_T("H:\\5.txt"), FILE_STATUS_CONFLICT);
	//	objCommandLinker.CacheSeta(_T("H:\\4.txt"), FILE_STATUS_SYNCING);
	//	objCommandLinker.CacheErase(_T("H:\\4.txt"), FILE_STATUS_SYNCING);
	//	objCommandLinker.CacheErase(_T("H:\\4.txt"), FILE_STATUS_NORMAL);
//
	return 0x00;
}

DWORD NShellLinkBzl::Finalize() {
	if(objCommandLinker.SetWatchDire(_T(""))) {
		// _LOG_WARN( _T("set cache watch path failed.") );
	}
	if(objCommandLinker.CacheClear(FILE_STATUS_INVALID)) {
		// _LOG_WARN( _T("set cache clear failed.") );
	}
	// Exit the Cache Close SDCache Pipe
	objCommandLinker.Finalize();
//
	return 0x00;	
}

DWORD NShellLinkBzl::DisplayDenyVector(FileFobdVec *pFobdVec, const TCHAR *szDriveLetter) {
	FileFobdVec::iterator iter;
	TCHAR szFilePath[MAX_PATH];
//
	DRIVE_LETTE(szFilePath, szDriveLetter)
	for(iter=pFobdVec->begin(); pFobdVec->end()!=iter; ++iter) {
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, (*iter)->szFilePath);
		objCommandLinker.CacheSeta(szFilePath, (*iter)->dwAttrib);
	}
	ShellUtility::RefreshExplorerWindows (szDriveLetter);
//
	return 0x00;
}

DWORD NShellLinkBzl::DisplayForbidVector(FileFobdVec *pFobdVec, const TCHAR *szDriveLetter) {
	FileFobdVec::iterator iter;
	TCHAR szFilePath[MAX_PATH];
//
	DRIVE_LETTE(szFilePath, szDriveLetter)
	for(iter=pFobdVec->begin(); pFobdVec->end()!=iter; ++iter) {
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, (*iter)->szFilePath);
		objCommandLinker.CacheDireSet(szFilePath, (*iter)->dwAttrib);
	}
//
	return 0x00;
}

DWORD NShellLinkBzl::DisplayConfliVector(FileColiVec *pColiVec, const TCHAR *szDriveLetter) {
	FileColiVec::iterator iter;
	TCHAR szFilePath[MAX_PATH];
//
	DRIVE_LETTE(szFilePath, szDriveLetter)
	for(iter=pColiVec->begin(); pColiVec->end()!=iter; ++iter) {
		_tcscpy_s(NO_LETT(szFilePath), MAX_PATH-LETT_LENGTH, (*iter)->szFileName);
		objCommandLinker.CacheSeta(szFilePath, FILE_STATUS_CONFLICT);
	}
//
	return 0x00;
}

DWORD NShellLinkBzl::DisplayFilesVector(FilesVec *pFilesVec, const TCHAR *szDriveLetter) {
	FilesVec::iterator iter;
	TCHAR szFileName[MAX_PATH];
//
// _LOG_DEBUG(_T("+++ in display files vector!"));
	DRIVE_LETTE(szFileName, szDriveLetter)
	for(iter=pFilesVec->begin(); pFilesVec->end()!=iter; ++iter) {
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->file_name);
		switch((*iter)->action_type) {
			case DELE:
				objCommandLinker.CacheSeta(szFileName, FILE_STATUS_DELETE);	
				break;
			case ADDI:
				objCommandLinker.CacheSeta(szFileName, FILE_STATUS_ADDED);	
				break;
			case EXIST:
			case MODIFY:
				objCommandLinker.CacheSeta(szFileName, FILE_STATUS_MODIFIED);
				break;	
		}
// _LOG_DEBUG(_T("+++ display file name:%s, action_type%08X"), szFileName, (*iter)->action_type);
	}
//
	return 0x00;	
}

//
// it modify the file_name.
#define COARSE_REFRESH_ICONS(FILE_NAME) { \
	TCHAR *szPath = (TCHAR *)_tcsrchr(FILE_NAME, _T('\\')); \
	if(FILE_NAME+LETT_LENGTH == szPath) { \
		(++szPath)[0] = _T('\0'); \
		ShellUtility::RefreshExplorerIcons(FILE_NAME); \
	} else if(szPath) { \
		szPath[0] = _T('\0'); \
		ShellUtility::RefreshExplorerIcons(FILE_NAME); \
	} \
}

//
static DWORD REFRE_EXPLO_INTERV[4] = {600, 2400, 5000, 5000};
#define REFRE_INTERV_TYPE(REFRE_TIMES) (0xFFFFFFFC & REFRE_TIMES)
VOID NShellLinkBzl::ExploSleep(DWORD dwSleepTime, const TCHAR *szDriveLetter) {
	static TCHAR szPrevFile[MAX_PATH];
	static DWORD dwRefreTimes;
	static TCHAR szExploFile[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
//
	if(dwSleepTime) {
		DWORD dwContinTime = 0x00;
		for (; dwSleepTime > dwContinTime; dwRefreTimes++) {
			if(REFRE_INTERV_TYPE(dwRefreTimes)) {
				Sleep(REFRE_EXPLO_INTERV[0x03]);
				dwContinTime += REFRE_EXPLO_INTERV[0x03];		
			} else {
				Sleep(REFRE_EXPLO_INTERV[dwRefreTimes]);
				dwContinTime += REFRE_EXPLO_INTERV[dwRefreTimes];
			}
			LAST_ACTION_COPY(szFileName)
// _LOG_DEBUG(_T("--- szPrevFile:%s szFileName:%s"), szPrevFile, szFileName);
			if(_tcscmp(szPrevFile, szFileName)) {
				_tcscpy_s(szPrevFile, MAX_PATH, szFileName);
				_tcscpy_s(NO_LETT(szExploFile), MAX_PATH-LETT_LENGTH, szFileName);
// _LOG_DEBUG(_T("--- szExploFile:%s"), szExploFile);
				COARSE_REFRESH_ICONS(szExploFile)
			}
		}
	} else {
		dwRefreTimes = 0x00;
		LAST_ACTION_COPY(szFileName)
		_tcscpy_s(szPrevFile, MAX_PATH, szFileName);
		DRIVE_LETTE(szExploFile, szDriveLetter)
		_tcscpy_s(NO_LETT(szExploFile), MAX_PATH-LETT_LENGTH, szFileName);
// _LOG_DEBUG(_T("+++ szPrevFile:%s szFileName:%s"), szPrevFile, szFileName);
// _LOG_DEBUG(_T("--- szExploFile:%s"), szExploFile);
		COARSE_REFRESH_ICONS(szExploFile)
	}
}

//
VOID NShellLinkBzl::BatchIconDisplay(const TCHAR *szFileName, DWORD isdire, DWORD dwIconsStatus) {
	if(isdire) CLEAR_LACHR(szFileName)
// _LOG_DEBUG(_T("+++ szFileName:%s dwIconsStatus:%X"), szFileName, dwIconsStatus);
	objCommandLinker.SendCommand(dwIconsStatus, szFileName);
}

VOID NShellLinkBzl::DisplayFileSyncing(const TCHAR *szFileName, DWORD isdire) {
	if(isdire) {
		CLEAR_LACHR(szFileName)
		objCommandLinker.CacheSeta(szFileName, FILE_STATUS_DIRECTORY|FILE_STATUS_SYNCING);
	} else objCommandLinker.CacheSeta(szFileName, FILE_STATUS_FILE|FILE_STATUS_SYNCING);
// _LOG_DEBUG(_T("+++ display syncing file name:%s"), szFileName);
	
}

//
// it modify the file_name.
#define FINE_REFRESH_ICONS(FILE_NAME, IS_DIRE) { \
	TCHAR *szPath; \
	if(IS_DIRE) { \
		szPath = (TCHAR *)sdstr::wcslchr(szFileName); \
		if(szPath) szPath[0] = _T('\0'); \
	} \
	szPath = (TCHAR *)_tcsrchr(FILE_NAME, _T('\\')); \
	if(FILE_NAME+LETT_LENGTH == szPath) { \
		(++szPath)[0] = _T('\0'); \
		ShellUtility::RefreshExplorerIcons(FILE_NAME); \
	} else { \
		szPath[0] = _T('\0'); \
		ShellUtility::RefreshExplorerIcons(FILE_NAME); \
	} \
}

#define FINAL_EXPLO_INTERV	6000 // 6s
VOID NShellLinkBzl::DisplayFileNormal(const TCHAR *szFileName, DWORD isdire) {
	if(isdire) {
		CLEAR_LACHR(szFileName)
		objCommandLinker.CacheErase(szFileName, FILE_STATUS_DIRECTORY|FILE_STATUS_SYNCING);	
	} else objCommandLinker.CacheErase(szFileName, FILE_STATUS_FILE|FILE_STATUS_SYNCING);
// _LOG_DEBUG(_T("+++ display normal file name:%s"), szFileName);
	static DWORD ulTimestamp;
    static volatile LONG lRefreExplo;
	if(!InterlockedExchange(&lRefreExplo, 1L)) {
		if(FINAL_EXPLO_INTERV < (GetTickCount() - ulTimestamp)) {
			FINE_REFRESH_ICONS(szFileName, isdire)
			ulTimestamp = GetTickCount();
		}
		InterlockedExchange(&lRefreExplo, 0L);
	}
}

VOID NShellLinkBzl::DisplayFilePrevious(const TCHAR *szFileName, DWORD isdire) {
	if(isdire) {
		CLEAR_LACHR(szFileName)
		objCommandLinker.CacheErase(szFileName, FILE_STATUS_DIRECTORY|FILE_STATUS_SYNCING);	
	} else objCommandLinker.CacheErase(szFileName, FILE_STATUS_FILE|FILE_STATUS_SYNCING);
// _LOG_DEBUG(_T("+++ display previous file name:%s"), szFileName);
}

/*
objCommandLinker.CacheSeta(_T("H:\\1.txt"), FILE_STATUS_SYNCING);
objCommandLinker.CacheSeta(_T("H:\\2.txt"), FILE_STATUS_DELETE);
objCommandLinker.CacheSeta(_T("H:\\3.txt"), FILE_STATUS_MODIFIED);
objCommandLinker.CacheSeta(_T("H:\\4.txt"), FILE_STATUS_ADDED);
objCommandLinker.CacheSeta(_T("H:\\5.txt"), FILE_STATUS_CONFLICT);
objCommandLinker.CacheSeta(_T("H:\\6.txt"), FILE_STATUS_LOCKED);
objCommandLinker.CacheSeta(_T("H:\\7.txt"), FILE_STATUS_READONLY);
objCommandLinker.CacheSeta(_T("H:\\8.txt"), FILE_STATUS_FORBID);
*/

DWORD NShellLinkBzl::DisplayLockedVector(FilesVec *pFilesVec, const TCHAR *szDriveLetter) {
	TCHAR szFileName[MAX_PATH]; 
	FilesVec::iterator iter;
//
	if(!szDriveLetter) return ((DWORD)-1);
	DRIVE_LETTE(szFileName, szDriveLetter)
//
	for(iter=pFilesVec->begin(); pFilesVec->end()!=iter; ++iter) { 
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->file_name); 
// _LOG_DEBUG(_T("display locked file name:%s"), szFileName); // disable by james 20130408
		objCommandLinker.CacheSeta(szFileName, FILE_STATUS_LOCKED);
	}
	ShellUtility::RefreshExplorerWindows (szDriveLetter);
//
	return 0x00;
}

DWORD NShellLinkBzl::DisplayPreviousVector(FilesVec *pFilesVec, const TCHAR *szDriveLetter, DWORD dwIconsStatus) {
	TCHAR szFileName[MAX_PATH]; 
	FilesVec::iterator iter;
//
	if(!szDriveLetter) return ((DWORD)-1);
	DRIVE_LETTE(szFileName, szDriveLetter)
//
	for(iter=pFilesVec->begin(); pFilesVec->end()!=iter; ++iter) { 
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->file_name); 
// _LOG_DEBUG(_T("display locked file name:%s"), szFileName); // disable by james 20130408
		objCommandLinker.CacheErase(szFileName, dwIconsStatus);	
	}
	ShellUtility::RefreshExplorerWindows (szDriveLetter);
//
	return 0x00;
}

