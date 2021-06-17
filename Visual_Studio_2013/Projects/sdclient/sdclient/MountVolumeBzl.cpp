#include "StdAfx.h"
#include "LocalBzl.h"
#include "../third party/RiverFS/RiverFS.h"
#include "ShellLinkBzl.h"

#include "MountVolumeBzl.h"

CMountVolume::CMountVolume(void):
m_dwProcessStatus(PROCESS_STATUS_WAITING)
{
	INIT_MOUNT_FLAGS(m_dwMountInitFlags);
}

CMountVolume::~CMountVolume(void) {
}
class CMountVolume objMountVolume;

DWORD CMountVolume::MountInitialize(const TCHAR *szDriveLetter) {
	// uninit syncing module
	if(SYNCING_CHECK_INIT(m_dwMountInitFlags)) {
		m_dwProcessStatus = NExecTransBzl::GetTransmitProcessStatus();
		NLocalBzl::StopTransmitProcess();
		SYNCING_FINAL_OKAY(m_dwMountInitFlags);
	}

	// stop the dir watcher thread
	if(DWATCH_CHECK_INIT(m_dwMountInitFlags)) {
		if(!NWatcherBzl::Finalize())
			DWATCH_FINAL_OKAY(m_dwMountInitFlags);
	}

	// stop the river file system
	if(RIVER_CHECK_INIT(m_dwMountInitFlags)) {
		if(!RiverFS::Finalize())
			RIVER_FINAL_OKAY(m_dwMountInitFlags);
	}

	// uninit display cache module
	if(MLINK_CHECK_INIT(m_dwMountInitFlags)) {
		if(!NShellLinkBzl::Finalize())
			MLINK_FINAL_OKAY(m_dwMountInitFlags);
	}

	// uninit client gui module	
	DWORD dwDismntValue;
	if(MOUNT_CHECK_INIT(m_dwMountInitFlags)) {
		if(!(dwDismntValue=NVolumeUtility::DismntVolume(szDriveLetter))) 
			MOUNT_FINAL_OKAY(m_dwMountInitFlags);
		if(dwDismntValue) return dwDismntValue;
	}
//
	return 0x00;
}

DWORD CMountVolume::MountFinalize(const TCHAR *szNewLetter) {
	FileFobdVec *pFobdVec = NFileFobd::GetFileFobdPoint();
	FileColiVec *pColiVec = NFileColi::GetFileColiPoint();
	struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();

	// init client gui module
	if(!MOUNT_CHECK_INIT(m_dwMountInitFlags)) {
		if(!NVolumeUtility::MountVolume(pClientConfig->szLocation, pClientConfig->szUserPool, pClientConfig->dwPoolLength, szNewLetter))
			MOUNT_INIT_OKAY(m_dwMountInitFlags);
	}

	// init display cache module
	FilesVec *pActioVec = NAnchorBzl::GetLastActioVec();
	if(!MLINK_CHECK_INIT(m_dwMountInitFlags)) {
		if(!NShellLinkBzl::Initialize(pFobdVec, pColiVec, pActioVec, szNewLetter))
			MLINK_INIT_OKAY(m_dwMountInitFlags);
	}

	// start the river file system
	if(!RIVER_CHECK_INIT(m_dwMountInitFlags)) {
		if(!RiverFS::Initialize(pClientConfig->szLocation, szNewLetter))
			RIVER_INIT_OKAY(m_dwMountInitFlags);
	}

	// start the dir watcher thread
	if(!DWATCH_CHECK_INIT(m_dwMountInitFlags)) {
		if(!NWatcherBzl::Initialize(pClientConfig->szLocation, szNewLetter))
			DWATCH_INIT_OKAY(m_dwMountInitFlags);
	}

	// init syncing module
	if(!SYNCING_CHECK_INIT(m_dwMountInitFlags)) {
		if(!NLocalBzl::ResumeTransmitProcess(m_dwProcessStatus))
			SYNCING_INIT_OKAY(m_dwMountInitFlags);
	}
//
	return 0x00;
}


DWORD NMountVolumeBzl::RemountVolume(TCHAR *szDriveLetter, const TCHAR *szNewLetter) {
	if(!szDriveLetter || !szNewLetter) return ERR_FAULT;
//
	if(objMountVolume.MountInitialize(szDriveLetter)) { // Ð¶ÔØ³ö´í,¹ÒÔØÔ­ÓÐ¾í.
		if(objMountVolume.MountFinalize(szDriveLetter)) // ¹ÒÔØ¾í³ö´í
			return ERR_EXIST_MOUNT;
		return ERR_UNMOUNT;
	}
	if(objMountVolume.MountFinalize(szNewLetter)) // ¹ÒÔØÐÂ¾í³ö´í
		return ERR_NEW_MOUNT;
//
	return ERR_SUCCESS;
}
