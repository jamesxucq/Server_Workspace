#pragma once

#include "clientcom/clientcom.h"
#include "LockedAction.h"

namespace NLkAiUtili {
	DWORD ValidateProcessFinished();
	//
	DWORD InitActionConve(const TCHAR *szDriveLetter, const TCHAR *szLocation);
	DWORD LockActionConve(const TCHAR *szDriveLetter, const TCHAR *szLocation);
	//
	DWORD ActioPrepaProcess(const TCHAR *szDriveLetter);  // 0:pass 1:undone 2:fatal
	VOID ClearActioDisplay(const TCHAR *szDriveLetter);
};