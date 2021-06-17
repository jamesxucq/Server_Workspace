// ReadOnly.cpp : CReadOnly µÄÊµÏÖ

#include "stdafx.h"
#include "ReadOnly.h"

#include "CommonHeader.h"

// CReadOnly

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CReadOnly::GetOverlayInfo(LPWSTR pszIconFile,
									   int cchMax,int* pIndex,
									   DWORD* pdwFlags)
{
	NOverlayBzl::Initialize();
	CREATE_ICON_FILE(pszIconFile, READONLY_ICON, cchMax);
//
	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE;
	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CReadOnly::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority = 6;
	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CReadOnly::IsMemberOf(LPCWSTR pszPath, DWORD dwAttrib)
{
	// if(/*FILE_SYNCDISK_FORBID  & */dwAttrib)
	if(FILE_STATUS_READONLY & NOverlayBzl::HandleRequestBzl(pszPath, dwAttrib)) 
		return S_OK;
	return S_FALSE;
}


