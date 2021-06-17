
#pragma once

namespace DiskUtility {
	DWORD GetVolumeName(LPCTSTR *lpVolumeName, LPCTSTR *lpFileSystem, LPCTSTR lpDriveLetter);
	DWORD GetVolumeFileSystem(LPWSTR lpFileSystem, LPWSTR lpDriveLetter);
	DWORD GetFreeDiskSpaceSWD(DWORD *dwFreeSpace, LPCTSTR lpDriveLetter); //"C:\\"
	DWORD GetTotalImageSpace(DWORD *dwTotalSpace, LPCTSTR lpDriveLetter); //"C:\\"
	DWORD FormatDisk(const LPWSTR lpDriveLetter, LPWSTR lpDiskLabel);
};

