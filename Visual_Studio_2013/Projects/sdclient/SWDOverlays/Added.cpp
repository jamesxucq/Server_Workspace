// Added.cpp : CAdded µÄÊµÏÖ

#include "stdafx.h"
#include "Added.h"

#include "CommonHeader.h"
#include "./utility/ulog.h"

// CAdded

// IShellIconOverlayIdentifier Method Implementation 
// IShellIconOverlayIdentifier::GetOverlayInfo
// returns The Overlay Icon Location to the system
STDMETHODIMP CAdded::GetOverlayInfo(LPWSTR pszIconFile,
									int cchMax,int* pIndex,
									DWORD* pdwFlags)
{
	NOverlayBzl::Initialize();
	CREATE_ICON_FILE(pszIconFile, ADDED_ICON, cchMax);
// logger(_T("c:\\added.log"), _T("%s\r\n"), pszIconFile);
	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE;
	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation 

// returns the priority of this overlay 0 being the highest. 
// this overlay is always selected do to its high priority 
STDMETHODIMP CAdded::GetPriority(int* pPriority)
{
	// highest priority
	*pPriority = 3;
	return S_OK;
}

// IShellIconOverlayIdentifier Method Implementation
// IShellIconOverlayIdentifier::IsMemberOf
// Returns Whether the object should have this overlay or not 
STDMETHODIMP CAdded::IsMemberOf(LPCWSTR pszPath, DWORD dwAttrib)
{
	// if(/*FILE_SYNCDISK_FORBID  & */dwAttrib)
	if(FILE_STATUS_ADDED & NOverlayBzl::HandleRequestBzl(pszPath, dwAttrib)) 
		return S_OK;
	return S_FALSE;
}


