
#include "stdafx.h"
#include <windows.h>
#include "fmifs.h"
#include "DiskUtility.h"


DWORD DiskUtility::GetVolumeName(OUT LPCTSTR lpVolumeName, OUT LPCTSTR lpFileSystem, IN LPCTSTR lpDriveRoot) { // "C:\\"
	TCHAR     szVolume[256];//卷标名  
	TCHAR     szFileSystem[256];  
	DWORD   dwSerialNum;//序列号  
	DWORD   dwFileNameLength;  
	DWORD   dwFileSysflag; 
	BOOL boolValue = FALSE;

	boolValue = GetVolumeInformation(lpDriveRoot, szVolume, 256, &dwSerialNum, 
		&dwFileNameLength, 
		&dwFileSysflag, 
		szFileSystem, 256);
	if (!boolValue) return -1;

	lpVolumeName = _tcsdup(szVolume);
	lpFileSystem = _tcsdup(szFileSystem);

	return 0;
}

DWORD DiskUtility::GetVolumeFileSystem(OUT LPWSTR lpFileSystem, IN LPWSTR lpDriveRoot) { // "C:\\"
	TCHAR     szVolume[256];//卷标名  
	TCHAR     szFileSystem[256];  
	DWORD   dwSerialNum;//序列号  
	DWORD   dwFileNameLength;  
	DWORD   dwFileSysflag; 
	BOOL boolValue = FALSE;

	boolValue = GetVolumeInformation(lpDriveRoot, szVolume, MAX_PATH, &dwSerialNum, 
		&dwFileNameLength, 
		&dwFileSysflag, 
		szFileSystem, MAX_PATH);
	if (!boolValue) return -1;
	_tcscpy_s(lpFileSystem, MAX_PATH, szFileSystem);

	return 0;
}

DWORD DiskUtility::GetDiskFreeSpace_SD(OUT DWORD *dwFreeSpace, IN LPCTSTR lpDriveRoot) { // "C:\\"
	DWORD   dwTotalDiskSpace,dwFreeDiskSpace,dwUsedDiskSpace;  
	ULARGE_INTEGER   uiFreeBytesAvailableToCaller;  
	ULARGE_INTEGER   uiTotalNumberOfBytes;  
	ULARGE_INTEGER   uiTotalNumberOfFreeBytes;  
	BOOL boolValue = FALSE;
	TCHAR szDriveRoot[MAX_PATH];

	if(!dwFreeSpace || !lpDriveRoot) return -1;

	_tcsncpy_s(szDriveRoot, MAX_PATH, lpDriveRoot, 3);
	boolValue = GetDiskFreeSpaceEx(szDriveRoot, 
		&uiFreeBytesAvailableToCaller,  
		&uiTotalNumberOfBytes,  
		&uiTotalNumberOfFreeBytes);
	if (!boolValue) return -1;

	dwTotalDiskSpace = (DWORD)(uiTotalNumberOfBytes.QuadPart>>30);  
	dwFreeDiskSpace = (DWORD)(uiFreeBytesAvailableToCaller.QuadPart>>30);  
	dwUsedDiskSpace = dwTotalDiskSpace-dwFreeDiskSpace;  
	TRACE("硬盘%s::总空间%dGB, 已用%dGB, 可用%dGB\n", lpDriveRoot, dwTotalDiskSpace, dwUsedDiskSpace, dwFreeDiskSpace); 
	TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);

	*dwFreeSpace = dwFreeDiskSpace;

	return 0;
}


// Globals
//BOOL FormatDone = FALSE;
BOOL FormatError = FALSE;

//======================================================================
// Formatx
// Format clone that demonstrates the use of the FMIFS file system
// utility library.
//======================================================================
void PrintWin32Error( PWCHAR Message, DWORD ErrorCode );
BOOLEAN __stdcall FormatExtCallback( CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument );
BOOLEAN FormatDiskExt(LPWSTR szDriveRoot, LPWSTR lpLabel);

#define QUICK_FORMAT				TRUE
#define NTFS						_T("NTFS")

PFORMATEX							FormatExt;
PENABLEVOLUMECOMPRESSION			EnableVolumeCompression; 

DWORD DiskUtility::FormatDisk(const LPWSTR lpDriveRoot, LPWSTR lpLabel) {
	TCHAR szDriveRoot[MAX_PATH];

	_tcscpy_s(szDriveRoot, lpDriveRoot);
	_tcscat_s(szDriveRoot, _T("\\"));

	if (!FormatDiskExt(szDriveRoot, lpLabel)) return -1;
	SetVolumeLabel(szDriveRoot, // 目标逻辑驱动器根目录名称。如果要设置C盘卷标，应指定为“C:\”
		lpLabel);         // 要设置的卷标名称。如果为NULL，函数删除卷标

	return 0;
}

//----------------------------------------------------------------------
// PrintWin32Error
// Takes the win32 error code and prints the text version.
//----------------------------------------------------------------------
void PrintWin32Error( PWCHAR Message, DWORD ErrorCode ) {
	LPVOID lpMsgBuf;

	FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, ErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(PWCHAR) &lpMsgBuf, 0, NULL );
	_tprintf(L"%s: %s\n", Message, lpMsgBuf );
	LocalFree( lpMsgBuf );
}

//----------------------------------------------------------------------
// FormatExtCallback
// The file system library will call us back with commands that we
// can interpret. If we wanted to halt the chkdsk we could return FALSE.
//----------------------------------------------------------------------
BOOLEAN __stdcall FormatExtCallback(CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument) {
	PDWORD percent;
	PTEXTOUTPUT output;
	PBOOLEAN status;
	static BOOL createStructures = FALSE;

	//
	// We get other types of commands, but we don't have to pay attention to them
	//
	switch(Command) {
	case PROGRESS:
		percent = (PDWORD) Argument;
		_tprintf(L"%d percent completed.\r", *percent);
		break;
	case OUTPUT:
		output = (PTEXTOUTPUT) Argument;
		fprintf(stdout, "%s", output->Output);
		break;
	case DONE:
		status = (PBOOLEAN) Argument;
		if(*status == FALSE) {
			_tprintf(L"FormatExt was unable to complete successfully.\n\n");
			FormatError = TRUE;
		}
		break;
	}

	return TRUE;
}

//----------------------------------------------------------------------
// LoadFMIFSEntryPoints
// Loads FMIFS.DLL and locates the entry point(s) we are going to use
//----------------------------------------------------------------------
BOOLEAN LoadFMIFSEntryPoints() {
	LoadLibrary(_T("fmifs.dll"));

	if(!(FormatExt = (PFORMATEX)GetProcAddress(GetModuleHandle(_T("fmifs.dll")),"FormatEx")))
		return FALSE;

	if(!(EnableVolumeCompression = (PENABLEVOLUMECOMPRESSION)GetProcAddress(GetModuleHandle(_T("fmifs.dll")),"EnableVolumeCompression")))
		return FALSE;

	return TRUE;
}

BOOLEAN FormatDiskExt(LPWSTR szDriveRoot, LPWSTR lpLabel) {
	if (!LoadFMIFSEntryPoints())
		return FALSE;

	FormatExt(szDriveRoot, FMIFS_HARDDISK, NTFS, lpLabel, QUICK_FORMAT, 4<<10, FormatExtCallback);
	if(FormatError) return FALSE;

	// 压缩驱动器
	EnableVolumeCompression(szDriveRoot, TRUE);

	return TRUE;
}

