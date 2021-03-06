// Delete.cpp : CDelete ??ʵ??

#include "stdafx.h"
#include "Delete.h"

#include "CommonHeader.h"

// CDelete

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CDelete::GetOverlayInfo(
									 LPWSTR pwszIconFile,
									 int cchMax,int* pIndex,
									 DWORD* pdwFlags)
{
	NOverlayBzl::Initialize();
	CREATE_ICON_FILE(pwszIconFile, DELETE_ICON, cchMax);

	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CDelete::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority = 2;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CDelete::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
	DWORD file_status;

	//just for test
	NOverlayBzl::HandleRequestBzl(&file_status, pwszPath, dwAttrib);

	//if(/*FILE_SYNCDISK_FORBID  & */dwAttrib)
	if(file_status & FILE_STATUS_DELETE) return S_OK;
	else return S_FALSE;
}


