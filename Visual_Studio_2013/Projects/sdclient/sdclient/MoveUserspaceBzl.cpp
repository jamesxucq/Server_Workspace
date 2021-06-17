#include "StdAfx.h"

#include "clientcom/clientcom.h"
#include "../third party/RiverFS/RiverFS.h"
#include "AnchorBzl.h"
#include "LocalBzl.h"
#include "MoveUserspaceDlg.h"
#include "MoveUserspaceBzl.h"

CMoveUserspace::CMoveUserspace(void) {
	INIT_MOVE_FLAGS(m_dwMoveInitFlags);
}

CMoveUserspace::~CMoveUserspace(void) {
}
class CMoveUserspace objMoveUserspace;


DWORD CMoveUserspace::MoveInitialize() {
	// stop the dir watcher thread
	if(DWATCH_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!NWatcherBzl::Finalize())
			DWATCH_FINAL_OKAY(m_dwMoveInitFlags);
	}

	// stop the river file system
	if(RIVER_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!RiverFS::Finalize())
			RIVER_FINAL_OKAY(m_dwMoveInitFlags);
	}

	struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
	// uninit client gui module
	DWORD dwDismntValue;
	if(MOUNT_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!(dwDismntValue=NVolumeUtility::DismntVolume(pClientConfig->szDriveLetter))) 
			MOUNT_FINAL_OKAY(m_dwMoveInitFlags);
		if(dwDismntValue) return dwDismntValue;
	}

	// uninit anchor module
	if(ANCH_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!NAnchorBzl::Finalize())
			ANCH_FINAL_OKAY(m_dwMoveInitFlags);
	}

	// uninit log module
	if(SYSLOG_CHECK_INIT(m_dwMoveInitFlags)) {
		log_fini();
		SYSLOG_FINAL_OKAY(m_dwMoveInitFlags);
	}

	return 0x00;
}

DWORD CMoveUserspace::MoveFinalize(TCHAR *szLocation) {
	struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
	struct LogConfig *pLogConfig = NConfigBzl::GetLogConfig();

	// init log module
	TCHAR szLogfile[MAX_PATH];
	if(!SYSLOG_CHECK_INIT(m_dwMoveInitFlags)) {
		log_init(struti::get_name(szLogfile, pClientConfig->szLocation, pLogConfig->szLogfile), (log_lvl_t)pLogConfig->dwDebugLevel);
		SYSLOG_INIT_OKAY(m_dwMoveInitFlags);
	}

	// init anchor module
	if(!ANCH_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!NAnchorBzl::Initialize(pClientConfig->szAnchorInde, pClientConfig->szAnchorData, szLocation))
			ANCH_INIT_OKAY(m_dwMoveInitFlags);
	}

	// init client gui module
	if(!MOUNT_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!NVolumeUtility::MountVolume(szLocation, pClientConfig->szUserPool, pClientConfig->dwPoolLength, pClientConfig->szDriveLetter))
			MOUNT_INIT_OKAY(m_dwMoveInitFlags);
	}

	// start the river file system
	if(!RIVER_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!RiverFS::Initialize(pClientConfig->szLocation, pClientConfig->szDriveLetter))
			RIVER_INIT_OKAY(m_dwMoveInitFlags);
	}

	// start the dir watcher thread
	if(!DWATCH_CHECK_INIT(m_dwMoveInitFlags)) {
		if(!NWatcherBzl::Initialize(pClientConfig->szLocation, pClientConfig->szDriveLetter))
			DWATCH_INIT_OKAY(m_dwMoveInitFlags);
	}

	return 0x00;
}


static DWORD CALLBACK CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,            // total file size, in bytes
	LARGE_INTEGER TotalBytesTransferred,    // total number of bytes transferred
	LARGE_INTEGER StreamSize,               // total number of bytes for this stream
	LARGE_INTEGER StreamBytesTransferred,   // total number of bytes transferred for this stream
	DWORD dwStreamNumber,                   // the current stream
	DWORD dwCallbackReason,                 // reason for callback
	HANDLE hFromFile,                       // handle to the source file
	HANDLE hDestinationFile,                // handle to the destination file
	LPVOID lpData)                          // passed by CopyFileEx
{
	static int iRecord = 0;
	TCHAR szProgressText[MID_TEXT_LEN];
//
	if(iRecord++ % 1024) return PROGRESS_CONTINUE;
//
	int iTotalFileSize = (int)(TotalFileSize.QuadPart>>20);
	int iTotalTransferred = (int)(TotalBytesTransferred.QuadPart>>20);
	_stprintf_s(szProgressText, _T("正在移动文件:\n\n        已移动:%d MB    文件大小:%d MB"),
		iTotalTransferred,
		iTotalFileSize);
	((CStatic*)lpData)->SetWindowText(szProgressText);
//
	return PROGRESS_CONTINUE;
}

/*
if(!MoveFileWithProgress(szExistDirectory, 
szNewDirectory, 
(LPPROGRESS_ROUTINE)CopyProgressRoutine,
stcProgressCtrl, 
MOVEFILE_REPLACE_EXISTING
|MOVEFILE_COPY_ALLOWED
|MOVEFILE_WRITE_THROUGH))
return false;

if(!MoveFileEx(szExistDirectory, szNewDirectory, 
MOVEFILE_REPLACE_EXISTING |MOVEFILE_COPY_ALLOWED
|MOVEFILE_WRITE_THROUGH))
return false;
*/

static BOOL MoveDirectory(VOID *stcProgressCtrl, TCHAR *szToDirectory, TCHAR *szFromDirectory) {
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	TCHAR szFindDirectory[MAX_PATH];
	//
	_stprintf_s(szFindDirectory, _T("%s\\*.*"), szFromDirectory);
	hFind = FindFirstFile(szFindDirectory, &wfd);
	if(INVALID_HANDLE_VALUE == hFind) return FALSE;
	//
	CreateDirectory(szToDirectory, NULL);
	do {
		TCHAR destFilePath[MAX_PATH];
		_stprintf_s(destFilePath, _T("%s\\%s"), szToDirectory, wfd.cFileName);

		TCHAR foundFilePath[MAX_PATH];
		_stprintf_s(foundFilePath, _T("%s\\%s"), szFromDirectory, wfd.cFileName);
		//
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(_tcscmp(wfd.cFileName,_T(".")) && _tcscmp(wfd.cFileName,_T(".."))) {
				MoveDirectory(stcProgressCtrl, destFilePath, foundFilePath);
			}
		} else {
			if(!MoveFileWithProgress(foundFilePath, destFilePath, 
				(LPPROGRESS_ROUTINE)CopyProgressRoutine,
				stcProgressCtrl, 
				MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED|MOVEFILE_WRITE_THROUGH))
				return FALSE;
		}
	} while(FindNextFile(hFind, &wfd));
	FindClose(hFind);
	RemoveDirectory(szFromDirectory);
	//
	return TRUE;
}

DWORD NMoveSpaceBzl::MoveFileSWD(VOID *stcProgressCtrl, TCHAR *szNewDirectory, TCHAR *szExistDirectory) {
	if(!stcProgressCtrl || !szNewDirectory || !szExistDirectory) 
		return ((DWORD)-1);
	//
	TCHAR *szFromDirectory = szExistDirectory;
	TCHAR *szToDirectory = szNewDirectory;
	if(!MoveDirectory(stcProgressCtrl, szToDirectory, szFromDirectory))
		return ((DWORD)-1);
	//
	return 0x00;
}

DWORD NMoveSpaceBzl::MoveUserspace(CWnd* pDisplayWnd, 
									   TCHAR *szExistLocation, 
									   TCHAR *szNewLocation, 
									   TCHAR *szFileName, 
									   DWORD dwFileSize,  
									   const TCHAR *szDriveLetter)
{
	DWORD dwFreeSpace;
	int move_result;
//
	if(DiskUtility::GetFreeDiskSpaceSWD(&dwFreeSpace, szNewLocation)) return ERR_FAULT;
	if(dwFreeSpace < (dwFileSize + 1)) return ERR_FREE_SPACE;
//
	CStatic *stcDisplsyStatus = &((CMoveUserspacelg*)pDisplayWnd)->m_stcMoveStatus;
	stcDisplsyStatus->SetWindowText(_T("状态:\n\n        正在卸载卷."));
	if((move_result = objMoveUserspace.MoveInitialize())) { // MoveInitialize 失败
		// 重新初始化
		stcDisplsyStatus->SetWindowText(_T("状态:\n\n        卸载出错,挂载原有卷."));
		if(objMoveUserspace.MoveFinalize(szExistLocation)) return ERR_EXIST_MOUNT;
		if(-1 == move_result) return ERR_FAULT;
		else if(0x0001 == move_result)  return ERR_UNMOUNT;
	} else { // MoveInitial 成功
		stcDisplsyStatus->SetWindowText(_T("状态:\n\n        正在移动用户数据."));
		if(NMoveSpaceBzl::MoveFileSWD(stcDisplsyStatus, szNewLocation, szExistLocation)) {
			objMoveUserspace.MoveFinalize(szExistLocation);
			return ERR_COPY_DATA;
		}
		stcDisplsyStatus->SetWindowText(_T("状态:\n\n        正在挂载新卷."));
		if(objMoveUserspace.MoveFinalize(szNewLocation)) return ERR_NEW_MOUNT;
	}
	stcDisplsyStatus->SetWindowText(_T("状态:\n\n        完成."));
//
	return ERR_SUCCESS;
}