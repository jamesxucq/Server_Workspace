#include "StdAfx.h"
#include "ExecuteUtility.h"
#include "WatcherBzl.h"
#include "AnchorBzl.h"
#include "ShellLinkBzl.h"
#include "ExecuteTransmitBzl.h"
#include "LocalBzl.h"

#include "LockAnchor.h"

//

DWORD NLockAnchor::UserOperationFinished(TCHAR *szDriveLetter) {
	FilesVec vFilesVec;
	static FilesVec lockedVec;
	DWORD dwFinished = ((DWORD)-1);
	// 解除上一个图标状态
	NShellLinkBzl::DisplayPreviousVector(&lockedVec, szDriveLetter, FILE_STATUS_LOCKED);
	// 检查文件锁定状态
	NLockAnchor::BuildValidFiles(&vFilesVec, NAnchorBzl::GetLastActioVec());
	NFilesVec::DeleFilesVec(&lockedVec);
	dwFinished = NLockAnchor::IsFilesLocked(&lockedVec, &vFilesVec, szDriveLetter);
	NFilesVec::DeleFilesVec(&vFilesVec);
	// 刷新图标
	if(dwFinished) NShellLinkBzl::DisplayLockedVector(&lockedVec, szDriveLetter);
	return dwFinished;
}

VOID NLockAnchor::FileLockedTip(TCHAR *szFileName) {
	_LOG_WARN(_T("lock anchor, file locked:%s"), szFileName);
	TCHAR szMessage[MAX_TEXT_LEN];
	_stprintf_s(szMessage, _T("文件%s被锁定,请关闭!"), szFileName);
	NLocalBzl::BalloonTip(BALLOON_TIP_INFORM, szMessage);
}

DWORD NLockAnchor::IsFilesLocked(FilesVec *pLkAiVec, FilesVec *pFilesVec, TCHAR *szDriveLetter) {
	FilesVec::iterator iter;
	TCHAR szFileName[MAX_PATH];
	//
	if(pFilesVec->empty()) return 0x00;
	DRIVE_LETTE(szFileName, szDriveLetter)
	for(iter=pFilesVec->begin(); pFilesVec->end()!=iter; ) { // ++iter
		//
		_tcscpy_s(NO_LETT(szFileName), MAX_PATH-LETT_LENGTH, (*iter)->file_name);
		if(FILE_LOCKED == NFileUtility::IsFileLocked(szFileName)) { // const TCHAR *szFileName
			NLockAnchor::FileLockedTip(szFileName);
			pLkAiVec->push_back(*iter);
			iter = pFilesVec->erase(iter);
		} else ++iter;
	}
	//
	return !pLkAiVec->empty();
}

DWORD NLockAnchor::BuildValidFiles(FilesVec *pToVec, FilesVec *pFromVec) {
	FilesVec::iterator iter;
	//
	if(!pToVec || !pFromVec) return ((DWORD)-1);
	//
	pToVec->clear();
	for(iter = pFromVec->begin(); pFromVec->end() != iter; ++iter) {
		//
		if(!(DELE&(*iter)->action_type) && !(EXIST&(*iter)->action_type))
			NFilesVec::AppNewNode(pToVec, *iter);
	}
	//
	return 0x00;
}

//
#define FLOCKED_RETRY_TIMES			12 // 24*12/60 4.8m
#define USER_OPERATE_DELAY			24000 // 24s
DWORD NLockAnchor::LockAnchorObject(TCHAR *szDriveLetter) {
	DWORD dwLockedValue = 1;
	//
_LOG_DEBUG(_T("lock anchor object"));
	for(int retry_times=0; FLOCKED_RETRY_TIMES>retry_times; retry_times++) { // 检查用户操作是否完成 add by james 20120330
		// todo: show lock false message. 20131102
		if(!UserOperationFinished(szDriveLetter)) {
			dwLockedValue = 0;
			break;
		} else Sleep(USER_OPERATE_DELAY);
_LOG_DEBUG(_T("lock retry:%d"), retry_times);
	}
	//
	return dwLockedValue;
}

DWORD NLockAnchor::UnlockAnchorObject(DWORD dwFinalType) {
	DWORD dwUnlockValue = 0x00;
	//
	switch(dwFinalType) {
	case ANCHOR_FINISH_FAILED:
		_LOG_WARN(_T("unlock anchor object failed!"));
		dwUnlockValue = 0x00;
		break;
	case ANCHOR_FINISH_SUCCESS:
	case ANCHOR_FINISH_UPDATE:
		if(NAnchorBzl::AddNewAnchor(NExecTransBzl::GetServAnchor()))
			dwUnlockValue = ((DWORD)-1);	
		break;
	case ANCHOR_FINISH_LEAVE:
		if(NAnchorBzl::DiscardAction())
			dwUnlockValue = ((DWORD)-1);
		break;	
	}
_LOG_DEBUG(_T("unlock anchor object, final type:%d serv anchor:%d"), dwFinalType, NExecTransBzl::GetServAnchor());
	//
	return dwUnlockValue;
}
