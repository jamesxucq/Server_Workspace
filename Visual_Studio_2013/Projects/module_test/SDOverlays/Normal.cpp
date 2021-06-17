// Normal.cpp : CNormal µÄÊµÏÖ

#include "stdafx.h"
#include "Normal.h"

#include "CommonHeader.h"

// CNormal

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CNormal::GetOverlayInfo(
									 LPWSTR pwszIconFile,
									 int cchMax,int* pIndex,
									 DWORD* pdwFlags)
{
	NOverlayBzl::Initialize();
	CREATE_ICON_FILE(pwszIconFile, NORMAL_ICON, cchMax);

	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CNormal::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority = 3;

	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CNormal::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
	DWORD file_status;

	//just for test
	NOverlayBzl::HandleRequestBzl(&file_status, pwszPath, dwAttrib);

	//if(/*FILE_SYNCDISK_FORBID  & dwAttrib)*/
	if(file_status & FILE_STATUS_NORMAL) return S_OK;
	else return S_FALSE;

	return S_OK;
}


