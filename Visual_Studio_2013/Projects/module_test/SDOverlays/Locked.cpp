// Locked.cpp : CLocked µÄÊµÏÖ

#include "stdafx.h"
#include "Locked.h"

#include "CommonHeader.h"

// CLocked

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CLocked::GetOverlayInfo(
									 LPWSTR pwszIconFile,
									 int cchMax,int* pIndex,
									 DWORD* pdwFlags)
{
	NOverlayBzl::Initialize();
	CREATE_ICON_FILE(pwszIconFile, LOCKED_ICON, cchMax);

	*pIndex = 0;
	//*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
	*pdwFlags = ISIOI_ICONFILE;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CLocked::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority = 6;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CLocked::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
	DWORD file_status;

	//just for test
	NOverlayBzl::HandleRequestBzl(&file_status, pwszPath, dwAttrib);

	//if(/*FILE_SYNCDISK_FORBID  & */dwAttrib)
	if(file_status & FILE_STATUS_LOCKED) return S_OK;
	else return S_FALSE;
}


