#include "StdAfx.h"
#include <dbt.h>
#include "VolumeUtility.h"

#include "third_party.h"
#include "StringUtility.h"
#include "FileUtility.h"

#define CREATE_HUGEFILE_DELAY		2048
#define ENABLE_DRIVER_DELAY			512
#define MOUNT_VOLUME_DELAY			2000
#define UNMOUNT_VOLUME_DELAY		2000
#define FORMAT_VOLUME_DELAY			8000

VOID BroadcastDeviceChange (WPARAM message) {
	DEV_BROADCAST_VOLUME dbv;
	DWORD_PTR dwResult;

	dbv.dbcv_size = sizeof (dbv); 
	dbv.dbcv_devicetype = DBT_DEVTYP_VOLUME; 
	dbv.dbcv_reserved = 0;
	dbv.dbcv_unitmask = 0;
	dbv.dbcv_flags = 0; 

	UINT timeOut = 1000;
	SendMessageTimeout (HWND_BROADCAST, WM_DEVICECHANGE, message, (LPARAM)(&dbv), SMTO_ABORTIFHUNG, timeOut, &dwResult);
}

DWORD NVolumeUtility::CreatVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize) {
	TCHAR	szUserPoolPath[MAX_PATH];
	if(!szLocation || !szFileName) return ((DWORD)-1);
	//
	NDireUtility::MakeFolderExist(szLocation);
	//
	struti::get_name(szUserPoolPath, szLocation, szFileName);
	if(NFileUtility::CreateHugeFile(szUserPoolPath, dwFileSize)) {
		_LOG_FATAL(_T("create huge file error!"));
		return ((DWORD)-1);
	} else { 
		_LOG_FATAL(_T("create huge file Ok!")); 
	}
	Sleep(CREATE_HUGEFILE_DELAY);

	return 0x00;
}

DWORD NVolumeUtility::FormatVolume(const LPWSTR szDriveLetter,  const LPWSTR szDiskLabel) {
	if(!szDriveLetter || !szDiskLabel) return ((DWORD)-1);

	if(DiskUtility::FormatDisk(szDriveLetter, szDiskLabel)) {
		_LOG_FATAL(_T("format disk error!"));
		return ((DWORD)-1);
	} else { 
		_LOG_FATAL(_T("format disk Ok!")); 
	}
	Sleep(FORMAT_VOLUME_DELAY);

	return 0x00;
}

VOID BulidDriverPath(TCHAR *szToName, TCHAR *szFromName, TCHAR *szDriverName) {
	switch(SystemCall::ProcessorArchitecture()) {
	case PROCESS_INTEL_IA64:
		_tcscpy_s(szDriverName, MAX_PATH, IA64_DRIVER_FNAME);
		break;
	case PROCESS_AMD64:
		_tcscpy_s(szDriverName, MAX_PATH, AMD64_DRIVER_FNAME);
		break;
	case PROCESS_INTEL_I386:
	default:
		_tcscpy_s(szDriverName, MAX_PATH, I386_DRIVER_FNAME);
		break;
	}
	NFileUtility::SystemDriverPath(szToName);
	NFileUtility::ModuleBaseName(szFromName);
}

DWORD NVolumeUtility::MountVolumeEx(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveLetter) {
	if(!szLocation || !szFileName || !szDriveLetter) return ((DWORD)-1);
//
	TCHAR szToName[MAX_PATH], szFromName[MAX_PATH], szDriverName[MAX_PATH];
	BulidDriverPath(szToName, szFromName, szDriverName);
	LoadDriver::CopyDriver(szToName, szFromName, szDriverName);
	//
	DWORD dwEnableValue = ENABLE_EXCEPTION;
	_tcscat_s(szToName, _T("\\"));
	_tcscat_s(szToName, szDriverName);
	if(LoadDriver::EnableDeviceDriver(szToName, DRIVER_SERVICE_NAME)) {
		_LOG_INFO(_T("enable device driver  Ok!"));
		Sleep(ENABLE_DRIVER_DELAY);
		dwEnableValue = ENABLE_SUCCESS;
	} else { 
		_LOG_FATAL(_T("enable device driver error!")); 
	}
//
	DWORD dwMountValue = MOUNT_EXCEPTION;
    DWORD deviceNumber;
	deviceNumber = SystemCall::GetDeviceNumber(_T("DeviceNumber"));
_LOG_INFO(_T("mount file.deviceNumber:%d"), deviceNumber);
	if(((DWORD)-1) == deviceNumber) return ((DWORD)-1);
	char poolPath[MAX_PATH];
	struti::get_name_ex(poolPath, szLocation, szFileName);
	if(!dwEnableValue) {
		if(!NFileDisk::Mount(deviceNumber, poolPath, dwFileSize, szDriveLetter[0])) {
			_LOG_INFO(_T("mount file Ok!"));
			BroadcastDeviceChange (DBT_DEVICEARRIVAL);
			Sleep(MOUNT_VOLUME_DELAY);
			dwMountValue = MOUNT_SUCCESS;
		} else { 
			_LOG_FATAL(_T("mount file error!")); 
		}
	}

	return dwEnableValue? dwEnableValue: dwMountValue;
}

// 0 succ; -1 error; 1 dismout error
DWORD NVolumeUtility::DismntVolumeEx(const LPCTSTR szDriveLetter) {
	DWORD dwDismntValue = 0x00;
	if(!szDriveLetter) return ((DWORD)-1);
	//
	if(!NFileDisk::Dismount(szDriveLetter[0], WindVers::GetWinVerEx())) {
		_LOG_INFO(_T("dismount file Ok!"));
		BroadcastDeviceChange (DBT_DEVICEREMOVECOMPLETE);
		Sleep(UNMOUNT_VOLUME_DELAY);
		dwDismntValue = UNMOUNT_SUCCESS;
	} else {
		_LOG_FATAL(_T("dismount file error!"));
		dwDismntValue = UNMOUNT_EXCEPTION;
	}
//
	if(!dwDismntValue) {
		// delete by james 20110307 for create new branch
		if(!LoadDriver::DisableDeviceDriver(DRIVER_SERVICE_NAME)) { // name of service LPCTSTR szDriverName
			_LOG_FATAL(_T("disable device driver error!"));
			dwDismntValue = ((DWORD)-1);
		} else { 
			_LOG_FATAL(_T("disable device driver Ok!")); 
		}
	}
//
	return dwDismntValue;
}

// 0 succ; -1 error
DWORD NVolumeUtility::MountVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveLetter)  {
	DWORD dwMountValue = 0x00;
	if(!szLocation || !szFileName || !szDriveLetter) return ((DWORD)-1);
	//
    DWORD deviceNumber;
	deviceNumber = SystemCall::GetDeviceNumber(_T("DeviceNumber"));
_LOG_INFO(_T("mount file.deviceNumber:%d"), deviceNumber);
	if(((DWORD)-1) == deviceNumber) return ((DWORD)-1);
	char poolPath[MAX_PATH];
	struti::get_name_ex(poolPath, szLocation, szFileName);
	//
	if(!NFileDisk::Mount(deviceNumber, poolPath, dwFileSize, szDriveLetter[0]))  {
		_LOG_FATAL(_T("mount file Ok!"));
		BroadcastDeviceChange (DBT_DEVICEARRIVAL);
		Sleep(MOUNT_VOLUME_DELAY);
		dwMountValue = MOUNT_SUCCESS;
	} else {
		_LOG_FATAL(_T("mount file error!"));
		dwMountValue = MOUNT_EXCEPTION;
	}
//
	return dwMountValue;
}

// 0 succ; -1 error; 1 dismout error
DWORD NVolumeUtility::DismntVolume(const TCHAR *szDriveLetter) {
	DWORD dwDismntValue = 0x00;
	if(!szDriveLetter) return ((DWORD)-1);
	//
	if(!NFileDisk::Dismount(szDriveLetter[0], WindVers::GetWinVerEx())) { // successful
		_LOG_FATAL(_T("dismount file Ok!"));
		BroadcastDeviceChange (DBT_DEVICEREMOVECOMPLETE);
		Sleep(UNMOUNT_VOLUME_DELAY);
		dwDismntValue = UNMOUNT_SUCCESS;
	} else { // fail
		_LOG_FATAL(_T("dismount file error!"));
		dwDismntValue = UNMOUNT_EXCEPTION;
	}

	return dwDismntValue;
}