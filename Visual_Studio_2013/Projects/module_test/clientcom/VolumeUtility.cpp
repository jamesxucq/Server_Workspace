#include "StdAfx.h"
#include <dbt.h>
#include "VolumeUtility.h"

#include "third_party.h"
#include "StringUtility.h"
#include "FileUtility.h"


void BroadcastDeviceChange (WPARAM message)
{
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

DWORD NVolumeUtility::CreatVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize)
{
	TCHAR	szUserPoolPath[MAX_PATH];

	if(!szLocation || !szFileName) return -1;

	///////////////////////////////////////////////////////////////
	NFileUtility::MakeSureDirectoryPathExists(szLocation);

	///////////////////////////////////////////////////////////////
	nstriutility::get_name(szUserPoolPath, szLocation, szFileName);
	if(NFileUtility::CreateHugeFile(szUserPoolPath, dwFileSize)) {
		LOG_FATAL(_T("create huge file error!"));
		return -1;
	} else LOG_FATAL(_T("create huge file Ok!"));
	Sleep(CREATE_HUGEFILE_DELAY);

	return 0;
}

DWORD NVolumeUtility::FormatVolume(const LPWSTR szDriveRoot,  const LPWSTR sLabel)
{
	if(!szDriveRoot || !sLabel) return -1;

	if(DiskUtility::FormatDisk(szDriveRoot, sLabel)) {
		LOG_FATAL(_T("format disk error!"));
		return -1;
	} else LOG_FATAL(_T("format disk Ok!"));
	Sleep(FORMAT_VOLUME_DELAY);

	return 0;
}

DWORD NVolumeUtility::MountVolume(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveRoot)
{
	TCHAR	szUserPoolPath[MAX_PATH];
	if(!szLocation || !szFileName || !szDriveRoot) return -1;

	///////////////////////////////////////////////////////////////
	TCHAR	szDestiName[MAX_PATH], szSourceName[MAX_PATH];
	NFileUtility::SystemDriverPath(szDestiName);
	NFileUtility::ModuleBaseName(szSourceName);
	LoadDriver::CopyDriver(szDestiName, szSourceName, I386_DRIVER_FNAME);

	///////////////////////////////////////////////////////////////
	DWORD dwEnableValue = ENABLE_EXCEPTION;
	_tcscat_s(szDestiName, _T("\\"));
	_tcscat_s(szDestiName, I386_DRIVER_FNAME);
	if(LoadDriver::EnableDeviceDriver(szDestiName, DRIVER_SERVICE_NAME)) {
		LOG_FATAL(_T("enable device driver  Ok!"));
		Sleep(ENABLE_DRIVER_DELAY);
		dwEnableValue = ENABLE_SUCCESS;
	} else LOG_FATAL(_T("enable device driver error!"));

	///////////////////////////////////////////////////////////////
	DWORD dwMountValue = MOUNT_EXCEPTION;
	nstriutility::get_name(szUserPoolPath, szLocation, szFileName);
	if(!dwEnableValue) {
		if(!NFileDisk::Mount(szUserPoolPath, szDriveRoot, dwFileSize)) {
			LOG_FATAL(_T("mount file Ok!"));
			BroadcastDeviceChange (DBT_DEVICEARRIVAL);
			Sleep(MOUNT_VOLUME_DELAY);
			dwMountValue = MOUNT_SUCCESS;
		} else LOG_FATAL(_T("mount file error!"));
	}

	return dwEnableValue? dwEnableValue: dwMountValue;
}

// 0 succ; -1 error; 1 dismout error
DWORD NVolumeUtility::DismntVolume(const LPWSTR szDriveRoot) 
{
	DWORD dwDismntValue = 0;

	if(!szDriveRoot) return -1;
	///////////////////////////////////////////////////////////////
	if(!NFileDisk::Dismount(szDriveRoot)) {
		LOG_FATAL(_T("dismount file Ok!"));
		BroadcastDeviceChange (DBT_DEVICEREMOVECOMPLETE);
		Sleep(UNMOUNT_VOLUME_DELAY);
		dwDismntValue = UNMOUNT_SUCCESS;
	} else {
		LOG_FATAL(_T("dismount file error!"));
		dwDismntValue = UNMOUNT_EXCEPTION;
	}

	///////////////////////////////////////////////////////////////
	if(!dwDismntValue) {
		// delete by james 20110307 for create new branch
		if(!LoadDriver::DisableDeviceDriver(DRIVER_SERVICE_NAME)) { // name of service LPCTSTR szDriverName
			LOG_FATAL(_T("disable device driver error!"));
			dwDismntValue = -1;
		} else LOG_FATAL(_T("disable device driver Ok!"));
	}

	return dwDismntValue;
}

// 0 succ; -1 error
DWORD NVolumeUtility::MountVolumeExt(const TCHAR *szLocation, const TCHAR *szFileName, DWORD dwFileSize, const TCHAR *szDriveRoot) 
{
	TCHAR	szUserPoolPath[MAX_PATH];
	DWORD dwMountValue = 0;

	if(!szLocation || !szFileName || !szDriveRoot) return -1;

	///////////////////////////////////////////////////////////////
	nstriutility::get_name(szUserPoolPath, szLocation, szFileName);
	if(!NFileDisk::Mount(szUserPoolPath, szDriveRoot, dwFileSize))  {
		LOG_FATAL(_T("mount file Ok!"));
		BroadcastDeviceChange (DBT_DEVICEARRIVAL);
		Sleep(MOUNT_VOLUME_DELAY);
		dwMountValue = MOUNT_SUCCESS;
	} else {
		LOG_FATAL(_T("mount file error!"));
		dwMountValue = MOUNT_EXCEPTION;
	}

	return dwMountValue;	
}

// 0 succ; -1 error; 1 dismout error
DWORD NVolumeUtility::DismntVolumeExt(const TCHAR *szDriveRoot) 
{
	DWORD dwDismntValue = 0;

	if(!szDriveRoot) return -1;

	///////////////////////////////////////////////////////////////
	if(!NFileDisk::Dismount(szDriveRoot)) { // successful
		LOG_FATAL(_T("dismount file Ok!"));
		BroadcastDeviceChange (DBT_DEVICEREMOVECOMPLETE);
		Sleep(UNMOUNT_VOLUME_DELAY);
		dwDismntValue = UNMOUNT_SUCCESS;
	} else { // fail
		LOG_FATAL(_T("dismount file error!"));
		dwDismntValue = UNMOUNT_EXCEPTION;
	}

	return dwDismntValue;
}