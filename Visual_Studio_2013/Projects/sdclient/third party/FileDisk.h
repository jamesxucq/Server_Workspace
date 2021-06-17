#pragma once


#define DEVICE_BASE_NAME    _T("\\FileDisk")
#define DEVICE_DIR_NAME     _T("\\Device")			DEVICE_BASE_NAME
#define DEVICE_NAME_PREFIX  DEVICE_DIR_NAME     DEVICE_BASE_NAME

#define IOCTL_FILE_DISK_OPEN_FILE   CTL_CODE(FILE_DEVICE_DISK, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_FILE_DISK_CLOSE_FILE  CTL_CODE(FILE_DEVICE_DISK, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_FILE_DISK_QUERY_FILE  CTL_CODE(FILE_DEVICE_DISK, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS)

typedef struct _OPEN_FILE_INFORMATION {
    LARGE_INTEGER   FileSize;
    BOOLEAN         ReadOnly;
    UCHAR           DriveLetter;
    USHORT          FileNameLength;
    CHAR            FileName[1];
} OPEN_FILE_INFORMATION, *POPEN_FILE_INFORMATION;

//
DWORD FileDiskUnmount(char DriveLetter);
DWORD FileDiskStatus(char DriveLetter);

void PrintLastError(TCHAR *Prefix);
DWORD FileDiskMount(int                     DeviceNumber,
					POPEN_FILE_INFORMATION  OpenFileInformation,
					BOOLEAN                 bCdImage);

//
BOOL CloseVolumeExplorerWindows7 (int driveNo);
BOOL CloseVolumeExplorerWindowsVS (int driveNo);

//
namespace NFileDisk {
	DWORD Mount(DWORD dwDeviNumb, const char *szImageName, DWORD dwFileSize, char cDriveLetter);
	DWORD Dismount(char cDriveLetter, DWORD dwWindowsVersion);
	DWORD Status(char cDriveLetter);
};