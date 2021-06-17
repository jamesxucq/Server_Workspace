#include "StdAfx.h"
#include <winioctl.h>
// #include "StringUtility.h"
#include "FileDisk.h"


DWORD NFileDisk::Mount(const TCHAR *szImageName, const TCHAR *pDrive, DWORD dwFileSize) {
	USES_CONVERSION;    
	char*                   szFileName = W2A(szImageName);
	char*                   DriveLetter = W2A(pDrive);
	BOOLEAN                 CdImage = FALSE;
	POPEN_FILE_INFORMATION  OpenFileInformation;
	int                     DeviceNumber = 0; // just for debug 20110307

	OpenFileInformation = (POPEN_FILE_INFORMATION)malloc(sizeof(OPEN_FILE_INFORMATION)+strlen(szFileName)+7);
	memset(OpenFileInformation, '\0', sizeof(OPEN_FILE_INFORMATION)+strlen(szFileName)+7);

	//strcpy(OpenFileInformation->szFileName, "\\??\\");
	//strcat(OpenFileInformation->szFileName, szFileName);
	//OpenFileInformation->FileNameLength =
	//	(USHORT) strlen(OpenFileInformation->szFileName);
	char *OpenFileName = (char *)OpenFileInformation->FileName;
	char *sp = "\\??\\";
	strcpy_s(OpenFileName, strlen(szFileName)+7, sp); //modify by james 20100906
	strcat_s(OpenFileName, strlen(szFileName)+7, szFileName); //modify by james 20100906
	//strcpy_s(OpenFileName, strlen(sp), sp);
	//strcat_s(OpenFileName, strlen(szFileName), szFileName);
	OpenFileInformation->FileNameLength = (USHORT) strlen(OpenFileName);

	unsigned __int64 i64FileSize = dwFileSize<<30;
	OpenFileInformation->FileSize.QuadPart = i64FileSize;
	OpenFileInformation->DriveLetter = DriveLetter[0];
	if(FileDiskMount(DeviceNumber, OpenFileInformation, CdImage)) return -1;

	return 0;
}

DWORD NFileDisk::Dismount(const TCHAR *pDrive) {
	USES_CONVERSION;
	char*                   DriveLetter = W2A(pDrive);

	CloseVolumeExplorerWindows(DriveLetter[0]);
	Sleep(AUTO_CLOSE_WINDOWS_DELAY);
	if(FileDiskUnmount(DriveLetter[0])) return -1;

	return 0;
}

DWORD NFileDisk::Status(const TCHAR *pDrive) {
	USES_CONVERSION;
	char*                   DriveLetter = W2A(pDrive);

	if (FileDiskStatus(DriveLetter[0])) return -1;
	return 0;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void PrintLastError(TCHAR* Prefix) {
	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0,
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);

	_ftprintf(stderr, _T("%s %s"), Prefix, (LPTSTR) lpMsgBuf);

	LocalFree(lpMsgBuf);
}

DWORD
FileDiskMount(int                     DeviceNumber,
			  POPEN_FILE_INFORMATION  OpenFileInformation,
			  BOOLEAN                 CdImage)
{
	TCHAR    VolumeName[] = _T("\\\\.\\ :");
	TCHAR    DriveName[] = _T(" :\\");
	TCHAR    DeviceName[255];
	HANDLE  Device;
	DWORD   BytesReturned;

	VolumeName[4] = OpenFileInformation->DriveLetter;
	DriveName[0] = OpenFileInformation->DriveLetter;

	Device = CreateFile(VolumeName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL);

	if (Device != INVALID_HANDLE_VALUE) {
		CloseHandle(Device);
		SetLastError(ERROR_BUSY);
		PrintLastError(&VolumeName[4]);
		return -1;
	}

	if (CdImage) {
		_stprintf_s(DeviceName, DEVICE_NAME_PREFIX _T("Cd") _T("%u"), DeviceNumber);
	} else {
		_stprintf_s(DeviceName, DEVICE_NAME_PREFIX _T("%u"), DeviceNumber);
	}

	if (!DefineDosDevice(DDD_RAW_TARGET_PATH,
		&VolumeName[4],
		DeviceName))
	{
		PrintLastError(&VolumeName[4]);
		return -1;
	}

	Device = CreateFile(VolumeName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL);

	if (Device == INVALID_HANDLE_VALUE) {
		PrintLastError(&VolumeName[4]);
		DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL);
		return -1;
	}

	if (!DeviceIoControl(Device,
		IOCTL_FILE_DISK_OPEN_FILE,
		OpenFileInformation,
		sizeof(OPEN_FILE_INFORMATION) + OpenFileInformation->FileNameLength - 1,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		PrintLastError(_T("FileDisk:"));
		DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL);
		CloseHandle(Device);
		return -1;
	}

	CloseHandle(Device);

	SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH|SHCNF_FLUSHNOWAIT, DriveName, NULL);

	return 0;
}

DWORD FileDiskUnmount(char DriveLetter) {
	char    VolumeName[] = "\\\\.\\ :";
	char    DriveName[] = " :\\";
	HANDLE  Device;
	DWORD   BytesReturned;

	VolumeName[4] = DriveLetter;
	DriveName[0] = DriveLetter;

	USES_CONVERSION;  
	Device = CreateFile(A2W(VolumeName),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL);

	if (Device == INVALID_HANDLE_VALUE) {
		PrintLastError(A2W(&VolumeName[4]));
		return -1;
	}

	if (!DeviceIoControl(Device,
		FSCTL_LOCK_VOLUME,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		PrintLastError(A2W(&VolumeName[4]));
		CloseHandle(Device);
		return -1;
	}

	if (!DeviceIoControl(Device,
		IOCTL_FILE_DISK_CLOSE_FILE,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		PrintLastError(_T("FileDisk:"));
		CloseHandle(Device);
		return -1;
	}

	if (!DeviceIoControl(Device,
		FSCTL_DISMOUNT_VOLUME,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		PrintLastError(A2W(&VolumeName[4]));
		CloseHandle(Device);
		return -1;
	}

	if (!DeviceIoControl(Device,
		FSCTL_UNLOCK_VOLUME,
		NULL,
		0,
		NULL,
		0,
		&BytesReturned,
		NULL))
	{
		PrintLastError(A2W(&VolumeName[4]));
		CloseHandle(Device);
		return -1;
	}

	CloseHandle(Device);

	//USES_CONVERSION;
	if (!DefineDosDevice(DDD_REMOVE_DEFINITION,
		A2W(&VolumeName[4]),
		NULL))
	{
		PrintLastError(A2W(&VolumeName[4]));
		return -1;
	}

	SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH|SHCNF_FLUSHNOWAIT, DriveName, NULL);

	return 0;
}

DWORD FileDiskStatus(char DriveLetter) {
	char                    VolumeName[] = "\\\\.\\ :";
	HANDLE                  Device;
	POPEN_FILE_INFORMATION  OpenFileInformation;
	DWORD                   BytesReturned;

	VolumeName[4] = DriveLetter;

	USES_CONVERSION;
	Device = CreateFile(A2W(VolumeName),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL);

	if (Device == INVALID_HANDLE_VALUE) {
		PrintLastError(A2W(&VolumeName[4]));
		return -1;
	}

	OpenFileInformation = (POPEN_FILE_INFORMATION)malloc(sizeof(OPEN_FILE_INFORMATION) + MAX_PATH);

	if (!DeviceIoControl(Device,
		IOCTL_FILE_DISK_QUERY_FILE,
		NULL,
		0,
		OpenFileInformation,
		sizeof(OPEN_FILE_INFORMATION) + MAX_PATH,
		&BytesReturned,
		NULL))
	{
		PrintLastError(A2W(&VolumeName[4]));
		CloseHandle(Device);
		free(OpenFileInformation);
		return -1;
	}

	if (BytesReturned < sizeof(OPEN_FILE_INFORMATION)) {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		PrintLastError(A2W(&VolumeName[4]));
		CloseHandle(Device);
		free(OpenFileInformation);
		return -1;
	}

	CloseHandle(Device);

	TRACE(_T("%c: %.*s %I64u bytes%s\n"),
		DriveLetter,
		OpenFileInformation->FileNameLength,
		OpenFileInformation->FileName,
		OpenFileInformation->FileSize,
		OpenFileInformation->ReadOnly ? _T(" ro") : _T("")
		);

	free(OpenFileInformation);

	return 0;
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
static BOOL explorerCloseSent;
static HWND explorerTopLevelWindow;

static BOOL CALLBACK CloseVolumeExplorerWindowsChildEnum (HWND hwnd, LPARAM driveStr) {
	TCHAR s[MAX_PATH];
	SendMessage (hwnd, WM_GETTEXT, sizeof (s), (LPARAM) s);

	if (_tcsstr (s, (TCHAR *) driveStr) != NULL) {
		PostMessage (explorerTopLevelWindow, WM_CLOSE, 0, 0);
		explorerCloseSent = TRUE;
		return FALSE;
	}

	return TRUE;
}


static BOOL CALLBACK CloseVolumeExplorerWindowsEnum (HWND hwnd, LPARAM driveNo) {
	TCHAR driveStr[10];
	TCHAR s[MAX_PATH];

	//_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	_stprintf_s (driveStr, _T("%c:\\"), driveNo);

	GetClassName (hwnd, s, sizeof s);
	if (_tcscmp (s, _T("CabinetWClass")) == 0) {
		GetWindowText (hwnd, s, sizeof s);
		if (_tcsstr (s, driveStr) != NULL) {
			PostMessage (hwnd, WM_CLOSE, 0, 0);
			explorerCloseSent = TRUE;
			return TRUE;
		}

		explorerTopLevelWindow = hwnd;
		EnumChildWindows (hwnd, CloseVolumeExplorerWindowsChildEnum, (LPARAM) driveStr);
	}

	return TRUE;
}


BOOL CloseVolumeExplorerWindows (int driveNo) {
	if (driveNo >= 0) {
		explorerCloseSent = FALSE;
		EnumWindows (CloseVolumeExplorerWindowsEnum, (LPARAM) driveNo);
	}

	return explorerCloseSent;
}