#pragma once

#include "third_party.h"

#define CREATE_HUGEFILE_DELAY		2048
#define ENABLE_DRIVER_DELAY			512
#define MOUNT_VOLUME_DELAY			2000
#define UNMOUNT_VOLUME_DELAY		2000
#define FORMAT_VOLUME_DELAY			8000

namespace NVolumeUtility {
	// 0 succ; -1 error
	DWORD CreatVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize);
	// 0 succ; -1 error	
	DWORD FormatVolume(const LPWSTR szDriveRoot,  const LPWSTR sLabel);	

	// 0 succ; -1 error
#define MOUNT_SUCCESS			0x0000
#define MOUNT_EXCEPTION			0x0001
#define ENABLE_SUCCESS			0x0000
#define ENABLE_EXCEPTION		0x0002
	DWORD MountVolumeExt(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveRoot);
	// 0 succ; -1 error
	DWORD MountVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveRoot);

	// 0 succ; -1 error; 1 dismout error
#define UNMOUNT_SUCCESS		0x0000
#define UNMOUNT_EXCEPTION	0x0001
	DWORD DismntVolume(const LPWSTR szDriveRoot);
	// 0 succ; -1 error; 1 dismout error
	DWORD DismntVolumeExt(const TCHAR *szDriveRoot);
};

static void BroadcastDeviceChange (WPARAM message);
//extern NVolumeUtility objVolumeUtility;