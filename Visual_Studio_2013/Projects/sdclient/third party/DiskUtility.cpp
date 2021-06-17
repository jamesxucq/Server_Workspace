
#include "stdafx.h"
#include <windows.h>
#include "fmifs.h"
#include "DiskUtility.h"


DWORD DiskUtility::GetVolumeName(LPCTSTR *lpVolumeName, LPCTSTR *lpFileSystem, LPCTSTR lpDriveLetter) {  // "C:\\"
    TCHAR szVolume[260];// 卷标名
    TCHAR szFileSystem[260];
    DWORD dwSerialNum;// 序列号
    DWORD dwFileNameLength;
    DWORD dwFileSysflag;
    BOOL boolValue = FALSE;
//
    boolValue = GetVolumeInformation(lpDriveLetter, szVolume, 260, &dwSerialNum,
                                     &dwFileNameLength,
                                     &dwFileSysflag,
                                     szFileSystem, 260);
    if (!boolValue) return ((DWORD)-1);
//
    *lpVolumeName = _tcsdup(szVolume);
    *lpFileSystem = _tcsdup(szFileSystem);
//
    return 0x00;
}

DWORD DiskUtility::GetVolumeFileSystem(LPWSTR lpFileSystem, LPWSTR lpDriveLetter) {  // "C:\\"
    TCHAR szVolume[260];// 卷标名
    TCHAR szFileSystem[260];
    DWORD dwSerialNum;// 序列号
    DWORD dwFileNameLength;
    DWORD dwFileSysflag;
    BOOL boolValue = FALSE;
//
    boolValue = GetVolumeInformation(lpDriveLetter, szVolume, MAX_PATH, &dwSerialNum,
                                     &dwFileNameLength,
                                     &dwFileSysflag,
                                     szFileSystem, MAX_PATH);
    if (!boolValue) return ((DWORD)-1);
    _tcscpy_s(lpFileSystem, MAX_PATH, szFileSystem);
//
    return 0x00;
}

DWORD DiskUtility::GetFreeDiskSpaceSWD(DWORD *dwFreeSpace, LPCTSTR lpDriveLetter) {  // "C:\\"
	DWORD dwTotalDiskSpace,dwFreeDiskSpace,dwUsedDiskSpace;
	ULARGE_INTEGER uiFreeBytesAvailableToCaller;
	ULARGE_INTEGER uiTotalNumberOfBytes;
	ULARGE_INTEGER uiTotalNumberOfFreeBytes;
	BOOL boolValue = FALSE;
	TCHAR szDriveLetter[MAX_PATH];
	//
	if(!dwFreeSpace || !lpDriveLetter) return ((DWORD)-1);
	//
	_tcsncpy_s(szDriveLetter, MAX_PATH, lpDriveLetter, 3);
	boolValue = GetDiskFreeSpaceEx(szDriveLetter,
		&uiFreeBytesAvailableToCaller,
		&uiTotalNumberOfBytes,
		&uiTotalNumberOfFreeBytes);
	if (!boolValue) return ((DWORD)-1);

	dwTotalDiskSpace = (DWORD)(uiTotalNumberOfBytes.QuadPart>>30);
	dwFreeDiskSpace = (DWORD)(uiFreeBytesAvailableToCaller.QuadPart>>30);
	dwUsedDiskSpace = dwTotalDiskSpace-dwFreeDiskSpace;
	TRACE("硬盘%s::总空间%dGB, 已用%dGB, 可用%dGB\n", lpDriveLetter, dwTotalDiskSpace, dwUsedDiskSpace, dwFreeDiskSpace);
	TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
	//
	*dwFreeSpace = dwFreeDiskSpace;
	//
	return 0x00;
}

DWORD DiskUtility::GetTotalImageSpace(DWORD *dwTotalSpace, LPCTSTR lpDriveLetter) { //"C:\\"
	DWORD dwTotalDiskSpace,dwFreeDiskSpace,dwUsedDiskSpace;
	ULARGE_INTEGER uiFreeBytesAvailableToCaller;
	ULARGE_INTEGER uiTotalNumberOfBytes;
	ULARGE_INTEGER uiTotalNumberOfFreeBytes;
	BOOL boolValue = FALSE;
	TCHAR szDriveLetter[MAX_PATH];
	//
	if(!dwTotalSpace || !lpDriveLetter) return ((DWORD)-1);
	//
	_tcsncpy_s(szDriveLetter, MAX_PATH, lpDriveLetter, 3);
	boolValue = GetDiskFreeSpaceEx(szDriveLetter,
		&uiFreeBytesAvailableToCaller,
		&uiTotalNumberOfBytes,
		&uiTotalNumberOfFreeBytes);
	if (!boolValue) return ((DWORD)-1);

	dwTotalDiskSpace = (DWORD)(uiTotalNumberOfBytes.QuadPart>>20);
	dwFreeDiskSpace = (DWORD)(uiFreeBytesAvailableToCaller.QuadPart>>30);
	dwUsedDiskSpace = dwTotalDiskSpace-dwFreeDiskSpace;
	TRACE("硬盘%s::总空间%dGB, 已用%dGB, 可用%dGB\n", lpDriveLetter, dwTotalDiskSpace, dwUsedDiskSpace, dwFreeDiskSpace);
	TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
	//
	*dwTotalSpace = (DWORD)((dwTotalDiskSpace + 0x01) >> 10); // 得到的空间略小,所以加一
	//
	return 0x00;
}

////////////////////////////////////////////////////////////////////////
// Globals
// BOOL FormatDone = FALSE;
BOOL FormatError = FALSE;

// ======================================================================
// Formatx
// Format clone that demonstrates the use of the FMIFS file system
// utility library.
// ======================================================================
void PrintWin32Error( PWCHAR Message, DWORD ErrorCode );
BOOLEAN __stdcall FormatExCallback( CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument );
BOOLEAN FormatDiskEx(LPWSTR szDriveLetter, LPWSTR lpDiskLabel);

#define QUICK_FORMAT				TRUE
#define NTFS						_T("NTFS")

PFORMATEX							FormatEx;
PENABLEVOLUMECOMPRESSION			EnableVolumeCompression;

DWORD DiskUtility::FormatDisk(const LPWSTR lpDriveLetter, LPWSTR lpDiskLabel) {
    TCHAR szDriveLetter[MAX_PATH];
//
    _tcscpy_s(szDriveLetter, lpDriveLetter);
    _tcscat_s(szDriveLetter, _T("\\"));
//
    if (!FormatDiskEx(szDriveLetter, lpDiskLabel)) return ((DWORD)-1);
    SetVolumeLabel(szDriveLetter, // 目标逻辑驱动器根目录名称。如果要设置C盘卷标，应指定为“C:\"
                   lpDiskLabel);         // 要设置的卷标名称。如果为NULL，函数删除卷标
//
    return 0x00;
}

// ----------------------------------------------------------------------
// PrintWin32Error
// Takes the win32 error code and prints the text version.
// ----------------------------------------------------------------------
void PrintWin32Error( PWCHAR Message, DWORD ErrorCode ) {
    LPVOID lpMsgBuf;
//
    FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL, ErrorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (PWCHAR) &lpMsgBuf, 0, NULL );
    _tprintf(L"%s: %s\n", Message, lpMsgBuf );
    LocalFree( lpMsgBuf );
}

// ----------------------------------------------------------------------
// FormatExCallback
// The file system library will call us back with commands that we
// can interpret. If we wanted to halt the chkdsk we could return FALSE.
// ----------------------------------------------------------------------
BOOLEAN __stdcall FormatExCallback(CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument) {
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
        if(FALSE == *status) {
            _tprintf(L"FormatEx was unable to complete successfully.\n");
            FormatError = TRUE;
        }
        break;
    }
//
    return TRUE;
}

// ----------------------------------------------------------------------
// LoadFMIFSEntryPoints
// Loads FMIFS.DLL and locates the entry point(s) we are going to use
// ----------------------------------------------------------------------
BOOLEAN LoadFMIFSEntryPoints() {
    LoadLibrary(_T("fmifs.dll"));
    if(!(FormatEx = (PFORMATEX)GetProcAddress(GetModuleHandle(_T("fmifs.dll")),"FormatEx")))
        return FALSE;
    if(!(EnableVolumeCompression = (PENABLEVOLUMECOMPRESSION)GetProcAddress(GetModuleHandle(_T("fmifs.dll")),"EnableVolumeCompression")))
        return FALSE;
    return TRUE;
}

BOOLEAN FormatDiskEx(LPWSTR szDriveLetter, LPWSTR lpDiskLabel) {
    if (!LoadFMIFSEntryPoints()) return FALSE;
    FormatEx(szDriveLetter, FMIFS_HARDDISK, NTFS, lpDiskLabel, QUICK_FORMAT, 4<<10, FormatExCallback);
    if(FormatError) return FALSE;
    // 压缩驱动器
    EnableVolumeCompression(szDriveLetter, TRUE);
    return TRUE;
}

