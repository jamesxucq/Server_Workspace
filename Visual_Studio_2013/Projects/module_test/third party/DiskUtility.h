
#pragma once

namespace DiskUtility {
	DWORD GetVolumeName(OUT LPCTSTR lpVolumeName, OUT LPCTSTR lpFileSystem, IN LPCTSTR lpDriveRoot);
	DWORD GetVolumeFileSystem(OUT LPWSTR lpFileSystem, IN LPWSTR lpDriveRoot);
	DWORD GetDiskFreeSpace_SD(OUT DWORD *dwFreeSpace, IN LPCTSTR lpDriveRoot); //"C:\\"
	DWORD FormatDisk(const LPWSTR lpDriveRoot, LPWSTR lpLabel);
};

