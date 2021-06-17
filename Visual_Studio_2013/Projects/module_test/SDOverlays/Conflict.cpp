// Conflict.cpp : CConflict µÄÊµÏÖ

#include "stdafx.h"
#include "Conflict.h"

#include "CommonHeader.h"

// CConflict

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CConflict::GetOverlayInfo(
									   LPWSTR pwszIconFile,
									   int cchMax,int* pIndex,
									   DWORD* pdwFlags)
{
	NOverlayBzl::Initialize();
	CREATE_ICON_FILE(pwszIconFile, CONFLICT_ICON, cchMax);

	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CConflict::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority = 0;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CConflict::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
	DWORD file_status;

	//just for test
	NOverlayBzl::HandleRequestBzl(&file_status, pwszPath, dwAttrib);

	//if(/*FILE_SYNCDISK_FORBID  & */dwAttrib)
	if(file_status & FILE_STATUS_CONFLICT) return S_OK;
	else return S_FALSE;
}


