#include "StdAfx.h"

#include "LoginWizdBzl.h"
#include "LoginWizdDlg.h"
#include "CVolThread.h"

#define ERROR_EXIT { \
	LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_FAULT); \
	return ((DWORD)-1); \
}

#define FAILED_EXIT { \
	LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_FAILED); \
	return 0x00; \
}

UINT CVolThread(LPVOID lpParam) {
	LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_WAITING);
	struct ClientConfig *pClientConfig = NConfigBzl::GetClientConfig();
	if(!pClientConfig) ERROR_EXIT;
//
	DWORD dwCreateValue = NVolumeUtility::CreatVolume(pClientConfig->szLocation, pClientConfig->szUserPool, 
		pClientConfig->dwPoolLength);
_LOG_DEBUG(_T("create volume, dwCreateValue:%08X"), dwCreateValue); 
	if(dwCreateValue) FAILED_EXIT
	LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_CREATE);
//
	DWORD dwMountValue = NVolumeUtility::MountVolume(pClientConfig->szLocation, pClientConfig->szUserPool, 
		pClientConfig->dwPoolLength, pClientConfig->szDriveLetter);
_LOG_DEBUG(_T("create volume, dwMountValue:%08X"), dwMountValue); 
	LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_MOUNT);
//
	if(!dwMountValue) {
		NVolumeUtility::FormatVolume(pClientConfig->szDriveLetter, pClientConfig->szDiskLabel);
		LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_FORMAT);
	}
//
	// DWORD dwDismntValue = NVolumeUtility::DismntVolume(pClientConfig->szDriveLetter);
	DWORD dwDismntValue = NVolumeUtility::DismntVolume(pClientConfig->szDriveLetter);
_LOG_DEBUG(_T("create volume, dwDismntValue:%08X"), dwDismntValue); 
	if(dwDismntValue) FAILED_EXIT
	LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_UNMOUNT);
//
	if(dwCreateValue || dwMountValue || dwDismntValue)
		LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_FAILED);
	else LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_SUCCESS);
	// for test
	// LoginWizdBzl::SetFormatStatus(FORMAT_STATUS_FAILED);
	return 0x00;
}

void CALLBACK ProgressTimerFunction(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime)
{   
	CFormatDlg *pCFmtWnd = (CFormatDlg *)CWnd::FromHandle(hWnd);
	CProgressCtrl *pCVolProgress = &(pCFmtWnd->m_proFormatStatus);
//
	DWORD dwFormatStatus = LoginWizdBzl::GetFormatStatus();
	switch(dwFormatStatus) {
	case FORMAT_STATUS_FAULT:
	case FORMAT_STATUS_WAITING:
	case FORMAT_STATUS_FAILED:
		pCVolProgress->SetPos(0); 
		break;
	case FORMAT_STATUS_CREATE:
		pCVolProgress->SetPos(32); 
		break;
	case FORMAT_STATUS_MOUNT:
		pCVolProgress->SetPos(40); 
		break;
	case FORMAT_STATUS_FORMAT:
		pCVolProgress->SetPos(56); 
		break;
	case FORMAT_STATUS_UNMOUNT:
	case FORMAT_STATUS_SUCCESS:
		pCVolProgress->SetPos(64); 
		break;
	}
	if(FORMAT_STATUS_FAULT==dwFormatStatus 
		|| FORMAT_STATUS_SUCCESS==dwFormatStatus 
		|| FORMAT_STATUS_FAILED==dwFormatStatus)
		LoginWizdBzl::FormatFinishAction();
}