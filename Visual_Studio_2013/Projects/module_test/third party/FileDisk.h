#pragma once


#define DEVICE_BASE_NAME    _T("\\FileDisk")
#define DEVICE_DIR_NAME     _T("\\Device")      DEVICE_BASE_NAME
#define DEVICE_NAME_PREFIX  DEVICE_DIR_NAME     DEVICE_BASE_NAME

#define FILE_DEVICE_FILE_DISK       0x8000

#define IOCTL_FILE_DISK_OPEN_FILE   CTL_CODE(FILE_DEVICE_FILE_DISK, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_FILE_DISK_CLOSE_FILE  CTL_CODE(FILE_DEVICE_FILE_DISK, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_FILE_DISK_QUERY_FILE  CTL_CODE(FILE_DEVICE_FILE_DISK, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS)

typedef struct _OPEN_FILE_INFORMATION {
	LARGE_INTEGER   FileSize;
	BOOLEAN         ReadOnly;
	UCHAR           DriveLetter;
	USHORT          FileNameLength;
	UCHAR           FileName[1];
} OPEN_FILE_INFORMATION, *POPEN_FILE_INFORMATION;

///////////////////////////////////////////////////////////////////////
DWORD FileDiskUnmount(char DriveLetter);
DWORD FileDiskStatus(char DriveLetter);

void PrintLastError(TCHAR* Prefix);
DWORD FileDiskMount(int                     DeviceNumber,
					POPEN_FILE_INFORMATION  OpenFileInformation,
					BOOLEAN                 CdImage);

///////////////////////////////////////////////////////////////////////
BOOL CloseVolumeExplorerWindows (int driveNo);


///////////////////////////////////////////////////////////////////////
#define AUTO_CLOSE_WINDOWS_DELAY	128
namespace NFileDisk
{
	DWORD Mount(const TCHAR *szImageName, const TCHAR *pDrive, DWORD dwFileSize);
	DWORD Dismount(const TCHAR *pDrive);
	DWORD Status(const TCHAR *pDrive);
};