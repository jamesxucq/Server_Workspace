#pragma once

#include "third_party.h"

namespace NVolumeUtility {
	// 0 succ; -1 error
	DWORD CreatVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize);
	// 0 succ; -1 error	
	DWORD FormatVolume(const LPWSTR szDriveLetter,  const LPWSTR szDiskLabel);	

	// 0 succ; -1 error
#define MOUNT_SUCCESS			0x0000
#define MOUNT_EXCEPTION			0x0001
#define ENABLE_SUCCESS			0x0000
#define ENABLE_EXCEPTION		0x0002
	DWORD MountVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveLetter);
	// 0 succ; -1 error
	DWORD MountVolumeEx(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveLetter);

	// 0 succ; -1 error; 1 dismout error
#define UNMOUNT_SUCCESS		0x0000
#define UNMOUNT_EXCEPTION	0x0001
	DWORD DismntVolume(const LPCTSTR szDriveLetter);
	// 0 succ; -1 error; 1 dismout error
	DWORD DismntVolumeEx(const TCHAR *szDriveLetter);
};

// extern NVolumeUtility objVolumeUtility;