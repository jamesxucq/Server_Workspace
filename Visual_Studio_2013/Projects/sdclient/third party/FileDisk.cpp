#include "StdAfx.h"
#include <winioctl.h>

#include "third_party.h"
// #include "StringUtility.h"
#include "FileDisk.h"

DWORD NFileDisk::Mount(DWORD dwDeviNumb, const char *szImageName, DWORD dwFileSize, char cDriveLetter) {
	char FileName[MAX_PATH];
	char DriveLetter = cDriveLetter;
	strcpy_s(FileName, szImageName);
    BOOLEAN                 bCdImage = FALSE;
    POPEN_FILE_INFORMATION  OpenFileInformation;
    int                     DeviceNumber = dwDeviNumb;
    //
    OpenFileInformation = (POPEN_FILE_INFORMATION)
		malloc(sizeof(OPEN_FILE_INFORMATION)+strlen(FileName)+7);
	if(!OpenFileInformation) return ((DWORD)-1);
    memset(OpenFileInformation, '\0', sizeof(OPEN_FILE_INFORMATION)+strlen(FileName)+7);
    //
    // strcpy(OpenFileInformation->szFileName, "\\??\\");
    // strcat(OpenFileInformation->szFileName, szFileName);
    // OpenFileInformation->FileNameLength =
    //	(USHORT) strlen(OpenFileInformation->szFileName);
    char *OpenFileName = (char *)OpenFileInformation->FileName;
    strcpy_s(OpenFileName, strlen(FileName)+7, "\\??\\"); // modify by james 20100906
    strcat_s(OpenFileName, strlen(FileName)+7, FileName); // modify by james 20100906
    OpenFileInformation->FileNameLength = (USHORT) strlen(OpenFileName);
    //
    unsigned __int64 qwFileSize = dwFileSize << 30;
    OpenFileInformation->FileSize.QuadPart = qwFileSize;
    OpenFileInformation->DriveLetter = DriveLetter;
    if(FileDiskMount(DeviceNumber, OpenFileInformation, bCdImage)) {
        free(OpenFileInformation);
        return ((DWORD)-1);
    }
    //
    free(OpenFileInformation);
    return 0x00;
}

#define AUTO_CLOSE_WINDOWS_DELAY	330
DWORD NFileDisk::Dismount(char cDriveLetter, DWORD dwWindowsVersion) {
	char DriveLetter = cDriveLetter;
    //
	if(_WIN_7_ <= dwWindowsVersion)
		CloseVolumeExplorerWindows7(DriveLetter);
	else if(_WIN_VISTA_ >= dwWindowsVersion)
		CloseVolumeExplorerWindowsVS(DriveLetter);
	//
    Sleep(AUTO_CLOSE_WINDOWS_DELAY);
    if(FileDiskUnmount(DriveLetter)) return ((DWORD)-1);
    //
    return 0x00;
}

DWORD NFileDisk::Status(char cDriveLetter) {
	char DriveLetter = cDriveLetter;
    if (FileDiskStatus(DriveLetter)) return ((DWORD)-1);
    return 0x00;
}

//
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

    _tprintf(_T("%s %s"), Prefix, (LPTSTR) lpMsgBuf);

    LocalFree(lpMsgBuf);
}

DWORD
FileDiskMount(int                     DeviceNumber,
              POPEN_FILE_INFORMATION  OpenFileInformation,
              BOOLEAN                 bCdImage)
{
    TCHAR   VolumeName[] = _T("\\\\.\\ :");
    TCHAR   DriveName[] = _T(" :\\");
    TCHAR   DeviceName[255];
    HANDLE  Device;
    DWORD   BytesReturned;
    //
    VolumeName[4] = OpenFileInformation->DriveLetter;
    DriveName[0] = OpenFileInformation->DriveLetter;
    //
    Device = CreateFile(VolumeName,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_NO_BUFFERING,
                        NULL);
    //
    if (INVALID_HANDLE_VALUE != Device) {
        CloseHandle(Device);
        SetLastError(ERROR_BUSY);
        PrintLastError(&VolumeName[4]);
        return ((DWORD)-1);
    }
    //
    if (bCdImage) {
        _stprintf_s(DeviceName, DEVICE_NAME_PREFIX _T("Cd") _T("%u"), DeviceNumber);
    } else {
        _stprintf_s(DeviceName, DEVICE_NAME_PREFIX _T("%u"), DeviceNumber);
    }
    //
    if (!DefineDosDevice(DDD_RAW_TARGET_PATH,
                         &VolumeName[4],
                         DeviceName)) {
        PrintLastError(&VolumeName[4]);
        return ((DWORD)-1);
    }
    //
    Device = CreateFile(VolumeName,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_NO_BUFFERING,
                        NULL);

    if (INVALID_HANDLE_VALUE == Device) {
        PrintLastError(&VolumeName[4]);
        DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL);
        return ((DWORD)-1);
    }
    //
    if (!DeviceIoControl(Device,
                         IOCTL_FILE_DISK_OPEN_FILE,
                         OpenFileInformation,
                         sizeof(OPEN_FILE_INFORMATION) + OpenFileInformation->FileNameLength - 1,
                         NULL,
                         0,
                         &BytesReturned,
                         NULL)) {
        PrintLastError(_T("FileDisk:"));
        DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL);
        CloseHandle(Device);
        return ((DWORD)-1);
    }
    //
    CloseHandle(Device);
    SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH|SHCNF_FLUSHNOWAIT, DriveName, NULL);
    return 0x00;
}

DWORD FileDiskUnmount(char DriveLetter) {
    TCHAR    VolumeName[] = _T("\\\\.\\ :");
    TCHAR    DriveName[] = _T(" :\\");
    HANDLE  Device;
    DWORD   BytesReturned;
    //
    VolumeName[4] = DriveLetter;
    DriveName[0] = DriveLetter;
    //
    Device = CreateFile(VolumeName,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_NO_BUFFERING,
                        NULL);

    if (INVALID_HANDLE_VALUE == Device) {
        PrintLastError(&VolumeName[4]);
        return ((DWORD)-1);
    }
    //
    if (!DeviceIoControl(Device,
                         FSCTL_LOCK_VOLUME,
                         NULL,
                         0,
                         NULL,
                         0,
                         &BytesReturned,
                         NULL)) {
        PrintLastError(&VolumeName[4]);
        CloseHandle(Device);
        return ((DWORD)-1);
    }
    //
    if (!DeviceIoControl(Device,
                         IOCTL_FILE_DISK_CLOSE_FILE,
                         NULL,
                         0,
                         NULL,
                         0,
                         &BytesReturned,
                         NULL)) {
        PrintLastError(_T("FileDisk:"));
        CloseHandle(Device);
        return ((DWORD)-1);
    }
    //
    if (!DeviceIoControl(Device,
                         FSCTL_DISMOUNT_VOLUME,
                         NULL,
                         0,
                         NULL,
                         0,
                         &BytesReturned,
                         NULL)) {
        PrintLastError(&VolumeName[4]);
        CloseHandle(Device);
        return ((DWORD)-1);
    }
    //
    if (!DeviceIoControl(Device,
                         FSCTL_UNLOCK_VOLUME,
                         NULL,
                         0,
                         NULL,
                         0,
                         &BytesReturned,
                         NULL)) {
        PrintLastError(&VolumeName[4]);
        CloseHandle(Device);
        return ((DWORD)-1);
    }
    //
    CloseHandle(Device);
    //
    if (!DefineDosDevice(DDD_REMOVE_DEFINITION,
                         &VolumeName[4],
                         NULL)) {
        PrintLastError(&VolumeName[4]);
        return ((DWORD)-1);
    }
    //
    SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH|SHCNF_FLUSHNOWAIT, DriveName, NULL);
    return 0x00;
}

DWORD FileDiskStatus(char DriveLetter) {
    TCHAR                   VolumeName[] = _T("\\\\.\\ :");
    HANDLE                  Device;
    POPEN_FILE_INFORMATION  OpenFileInformation;
    DWORD                   BytesReturned;
    //
    VolumeName[4] = DriveLetter;
    //
    Device = CreateFile(VolumeName,
                        GENERIC_READ,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_NO_BUFFERING,
                        NULL);
    //
    if (INVALID_HANDLE_VALUE == Device) {
        PrintLastError(&VolumeName[4]);
        return ((DWORD)-1);
    }
    //
    OpenFileInformation = (POPEN_FILE_INFORMATION)
		malloc(sizeof(OPEN_FILE_INFORMATION) + MAX_PATH);
	if(!OpenFileInformation) return ((DWORD)-1);
	//
    if (!DeviceIoControl(Device,
                         IOCTL_FILE_DISK_QUERY_FILE,
                         NULL,
                         0,
                         OpenFileInformation,
                         sizeof(OPEN_FILE_INFORMATION) + MAX_PATH,
                         &BytesReturned,
                         NULL)) {
        PrintLastError(&VolumeName[4]);
        CloseHandle(Device);
        free(OpenFileInformation);
        return ((DWORD)-1);
    }
    //
    if (BytesReturned < sizeof(OPEN_FILE_INFORMATION)) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        PrintLastError(&VolumeName[4]);
        CloseHandle(Device);
        free(OpenFileInformation);
        return ((DWORD)-1);
    }
    //
    CloseHandle(Device);
    //
_LOG_DEBUG(_T("%c: %.*s %I64u bytes%s"),
          DriveLetter,
          OpenFileInformation->FileNameLength,
          OpenFileInformation->FileName,
          OpenFileInformation->FileSize,
          OpenFileInformation->ReadOnly ? _T(" ro") : _T("")
         );
    //
    free(OpenFileInformation);
    return 0x00;
}


//
//
static BOOL explorerCloseSent;
static HWND explorerTopLevelWindow;

static BOOL CALLBACK CloseVolumeExplorerWindowsChildEnumCB(HWND hwnd, LPARAM driveStr) {
    TCHAR s[MAX_PATH];
	//
	GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("ComboBoxEx32"))) {
		SendMessage (hwnd, WM_GETTEXT, MAX_PATH, (LPARAM) s);
		if (NULL == _tcsncmp(s, (TCHAR *) driveStr, 0x03)) {
			PostMessage(explorerTopLevelWindow, WM_CLOSE, 0, 0);
			explorerCloseSent = TRUE;
			return FALSE;
		}
	}
    //
    return TRUE;
}

static BOOL CALLBACK CloseVolumeExplorerWindowsChildEnumTW(HWND hwnd, LPARAM driveStr) {
    TCHAR s[MAX_PATH];
	//
	GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("ToolbarWindow32"))) {
		SendMessage (hwnd, WM_GETTEXT, MAX_PATH, (LPARAM) s);
		if (NULL == _tcsncmp(s+0x04, (TCHAR *) driveStr, 0x03)) {
			PostMessage(explorerTopLevelWindow, WM_CLOSE, 0, 0);
			explorerCloseSent = TRUE;
			return FALSE;
		}
	}
    //
    return TRUE;
}

static BOOL CALLBACK CloseVolumeExplorerWindowsEnumVS(HWND hwnd, LPARAM driveNo) {
    TCHAR driveStr[10] = _T(" :\\");
    TCHAR s[MAX_PATH];
    //_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	driveStr[0] = (TCHAR)driveNo;
    //
    GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
        GetWindowText (hwnd, s, MAX_PATH);
        if (NULL == _tcsncmp(s, driveStr, 0x03)) {
            PostMessage (hwnd, WM_CLOSE, 0, 0);
            explorerCloseSent = TRUE;
            return TRUE;
        }
        //
        explorerTopLevelWindow = hwnd;
        EnumChildWindows(hwnd, CloseVolumeExplorerWindowsChildEnumCB, (LPARAM) driveStr);
    }
    //
    return TRUE;
}

static BOOL CALLBACK CloseVolumeExplorerWindowsEnum7(HWND hwnd, LPARAM driveNo) {
    TCHAR driveStr[10] = _T(" :\\");
    TCHAR s[MAX_PATH];
    //_stprintf_s (driveStr, _T("%c:\\"), driveNo + 'A');
	driveStr[0] = (TCHAR)driveNo;
    //
    GetClassName (hwnd, s, MAX_PATH);
    if (NULL == _tcscmp(s, _T("CabinetWClass"))) {
        GetWindowText (hwnd, s, MAX_PATH);
        if (NULL == _tcsncmp(s, driveStr, 0x03)) {
            PostMessage (hwnd, WM_CLOSE, 0, 0);
            explorerCloseSent = TRUE;
            return TRUE;
        }
        //
        explorerTopLevelWindow = hwnd;
        EnumChildWindows(hwnd, CloseVolumeExplorerWindowsChildEnumTW, (LPARAM) driveStr);
    }
    //
    return TRUE;
}

BOOL CloseVolumeExplorerWindowsVS(int driveNo) {
    if (driveNo >= 'A') {
        explorerCloseSent = FALSE;
        EnumWindows (CloseVolumeExplorerWindowsEnumVS, (LPARAM) driveNo);
    }
    //
    return explorerCloseSent;
}

BOOL CloseVolumeExplorerWindows7(int driveNo) {
    if (driveNo >= 'A') {
        explorerCloseSent = FALSE;
        EnumWindows (CloseVolumeExplorerWindowsEnum7, (LPARAM) driveNo);
    }
    //
    return explorerCloseSent;
}