// Delete.cpp : CDelete µÄÊµÏÖ

#include "stdafx.h"
#include "Delete.h"

#include "CommonHeader.h"

// CDelete

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CDelete::GetOverlayInfo(LPWSTR pszIconFile,
									 int cchMax,int* pIndex,
									 DWORD* pdwFlags)
{
	NOverlayBzl::Initialize();
	CREATE_ICON_FILE(pszIconFile, DELETE_ICON, cchMax);
//
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
	*pPriority = 4;
	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CDelete::IsMemberOf(LPCWSTR pszPath, DWORD dwAttrib)
{
	// if(/*FILE_SYNCDISK_FORBID  & */dwAttrib)
	if(FILE_STATUS_DELETE & NOverlayBzl::HandleRequestBzl(pszPath, dwAttrib))
		return S_OK;
	return S_FALSE;
}


