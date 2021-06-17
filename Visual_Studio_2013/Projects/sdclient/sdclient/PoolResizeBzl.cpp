#include "StdAfx.h"
#include "../third party/RiverFS/RiverFS.h"
#include "PoolResizeBzl.h"

CPoolResize::CPoolResize(void)
{
	INIT_PRESIZ_FLAGS(m_dwResizInitFlags);
}

CPoolResize::~CPoolResize(void) {
}
class CPoolResize objPoolResize;


DWORD CPoolResize::ResizeInitialize() {
	struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();

	// init client gui module
	if(!MOUNT_CHECK_INIT(m_dwResizInitFlags)) {
		if(!NVolumeUtility::MountVolume(pClientConfig->szLocation, pClientConfig->szUserPool, pClientConfig->dwPoolLength, pClientConfig->szDriveLetter))
			MOUNT_INIT_OKAY(m_dwResizInitFlags);
	}

	// start the river file system
	if(!RIVE_CHECK_INIT(m_dwResizInitFlags)) {
		if(!RiverFS::Initialize(pClientConfig->szLocation, pClientConfig->szDriveLetter))
			RIVE_INIT_OKAY(m_dwResizInitFlags);
	}

	// start the dir watcher thread
	if(!WATCH_CHECK_INIT(m_dwResizInitFlags)) {
		if(!NWatcherBzl::Initialize(pClientConfig->szLocation, pClientConfig->szDriveLetter))
			WATCH_INIT_OKAY(m_dwResizInitFlags);
	}

	return 0x00;
}

DWORD CPoolResize::ResizeFinalize() {
	// stop the dir watcher thread
	if(WATCH_CHECK_INIT(m_dwResizInitFlags)) {
		if(!NWatcherBzl::Finalize())
			WATCH_FINAL_OKAY(m_dwResizInitFlags);
	}

	// stop the river file system
	if(RIVE_CHECK_INIT(m_dwResizInitFlags)) {
		if(!RiverFS::Finalize())
			RIVE_FINAL_OKAY(m_dwResizInitFlags);
	}

	struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
	// uninit client gui module
	DWORD dwDismntValue;
	if(MOUNT_CHECK_INIT(m_dwResizInitFlags)) {
		if(!(dwDismntValue=NVolumeUtility::DismntVolume(pClientConfig->szDriveLetter))) 
			MOUNT_FINAL_OKAY(m_dwResizInitFlags);
		if(dwDismntValue) return dwDismntValue;
	}

	return 0x00;
}

DWORD NPoolResizeBzl::ShrinkPoolLength(TCHAR *szFileName, int dwLocalNewSize) {
	DWORD dwShrinkPool = ERR_SUCCESS;
	if(NTFSLibrary::NTFSResize(szFileName, dwLocalNewSize)) dwShrinkPool = ERR_DISK_RESIZE;
	else {
		if (NFileUtility::ShrinkHugeFile(szFileName, dwLocalNewSize)) dwShrinkPool = ERR_SHRINK_FILE;
		else dwShrinkPool = ERR_SUCCESS;
	}
	// logger(_T("c:\\setting.log"), _T("shrink pool length:%X\r\n"), dwShrinkPool);
	return dwShrinkPool;
}

DWORD NPoolResizeBzl::ExpandPoolLength(TCHAR *szFileName, int dwLocalNewSize) {
	DWORD dwExpandPool = ERR_SUCCESS;
	if(NFileUtility::ExpandHugeFile(szFileName, dwLocalNewSize)) dwExpandPool = ERR_EXPAND_FILE;
	else {
		if(NTFSLibrary::NTFSResize(szFileName, dwLocalNewSize)) dwExpandPool = ERR_DISK_RESIZE;
		else dwExpandPool = ERR_SUCCESS;
	}
	// logger(_T("c:\\setting.log"), _T("expand pool length:%X\r\n"), dwExpandPool);
	return dwExpandPool;
}

DWORD NPoolResizeBzl::AdjustPoolLength(struct ClientConfig *pClientConfig, int dwLocalExistSize, int dwLocalNewSize) {
	TCHAR szFileName[MAX_PATH];
	TCHAR szTipMessage[MID_TEXT_LEN];
	DWORD dwResizeValue = 0;
	//
	if(dwLocalNewSize == dwLocalExistSize) return 0x00;
	struti::get_name(szFileName, pClientConfig->szLocation, pClientConfig->szUserPool);
	//
	if(dwLocalNewSize > dwLocalExistSize) {
		DWORD dwDiskFreeSpace;
		if(!DiskUtility::GetFreeDiskSpaceSWD(&dwDiskFreeSpace, pClientConfig->szLocation)) {
			while(!dwResizeValue && ((int)dwDiskFreeSpace < (dwLocalNewSize-dwLocalExistSize))) {
				_stprintf_s(szTipMessage, _T("用户空间由现在的: %d(GB) 扩大为: %d(GB).\n\n    请确定磁盘有相应的空闲空间!"), dwLocalExistSize, dwLocalNewSize);
				if(IDOK != AfxMessageBox(szTipMessage, MB_OKCANCEL|MB_ICONINFORMATION|MB_SYSTEMMODAL)) dwResizeValue = ((DWORD)-1);
				if(DiskUtility::GetFreeDiskSpaceSWD(&dwDiskFreeSpace, pClientConfig->szLocation)) dwResizeValue = ((DWORD)-1);
			}
			if(!dwResizeValue) {
				if(ExpandPoolLength(szFileName, dwLocalNewSize)) {
					AfxMessageBox(_T("扩大用户空间出错!\n\n    请重新启动软件."), MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
					dwResizeValue = ((DWORD)-1);
				}		
			}
		} else dwResizeValue = ((DWORD)-1);
		//
	} else if(dwLocalNewSize < dwLocalExistSize) {
		DWORD dwLastDisksize = dwLocalExistSize;
		while(!dwResizeValue && NTFSLibrary::ValidateReduce(&dwLastDisksize, szFileName, dwLocalNewSize)) {
			if(!objPoolResize.ResizeInitialize()) { // mount initial successful
				_stprintf_s(szTipMessage, _T("用户空间已占用: %d(MB) 缩小为: %d(GB).\n\n    请删除文件并做磁盘整理后确定!\n\n    或者取消后重新生成用户空间."), dwLastDisksize, dwLocalNewSize);
				if(IDOK != AfxMessageBox(szTipMessage, MB_OKCANCEL|MB_ICONINFORMATION|MB_SYSTEMMODAL)) dwResizeValue = ((DWORD)-1);
				if(objPoolResize.ResizeFinalize()) dwResizeValue = ((DWORD)-1);
			} else { //  mount initial false 
				objPoolResize.ResizeFinalize();
				dwResizeValue = ((DWORD)-1);
			}
		}
		if(!dwResizeValue) {
			if(ShrinkPoolLength(szFileName, dwLocalNewSize)) {
				AfxMessageBox(_T("缩小用户空间出错!\n\n    请重新启动软件."), MB_OK|MB_ICONINFORMATION|MB_SYSTEMMODAL);
				dwResizeValue = ((DWORD)-1);
			}		
		}
	}
	//
	return dwResizeValue;
}