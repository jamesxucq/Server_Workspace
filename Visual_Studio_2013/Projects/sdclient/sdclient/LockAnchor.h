#pragma once

#include "clientcom/clientcom.h"
#include "TRANSObject.h"

namespace NLockAnchor {
	// static
	static DWORD BuildValidFiles(FilesVec *pToVec, FilesVec *pFromVec);
	static VOID FileLockedTip(TCHAR *szFileName);
	static DWORD IsFilesLocked(FilesVec *pLkAiVec, FilesVec *pFilesVec, TCHAR *szDriveLetter);
	static DWORD UserOperationFinished(TCHAR *szDriveLetter);
	//
	DWORD LockAnchorObject(TCHAR *szDriveLetter);
#define ANCHOR_FINISH_FAILED	0x0000
#define ANCHOR_FINISH_SUCCESS	0x0001
#define ANCHOR_FINISH_UPDATE	0x0002
#define ANCHOR_FINISH_LEAVE		0x0004
	DWORD UnlockAnchorObject(DWORD dwFinalType);
}