
#pragma once

#include "clientcom/ShellInterface.h"
#include "clientcom/ShellLinker.h"
#include "OverlayBzl.h"

#define SYNCING_ICON		_T("SyncingIcon.ico")
#define ADDED_ICON			_T("AddedIcon.ico") 
#define DELETE_ICON			_T("DeletedIcon.ico") 
#define MODIFIED_ICON		_T("ModifiedIcon.ico")
#define NORMAL_ICON			_T("NormalIcon.ico") 
#define CONFLICT_ICON		_T("ConflictIcon.ico") 
#define READONLY_ICON		_T("ReadOnlyIcon.ico")
#define LOCKED_ICON			_T("LockedIcon.ico")
#define FORBID_ICON			_T("IgnoredIcon.ico")
//#define FORBID_ICON			_T("ForbidIcon.ico")
//#define UNVERSIONED_ICON		_T("UnversionedIcon") 

#define CREATE_ICON_FILE(ICON_FILE, ICON_NAME, CHR_MAX) \
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), ICON_FILE, CHR_MAX); \
	TCHAR *chrend = _tcsrchr(ICON_FILE, _T('\\')); \
	if(chrend) chrend[0] = _T('\0'); \
	_tcscat_s(ICON_FILE, CHR_MAX, _T("\\icons\\XPStyle\\")); \
	_tcscat_s(ICON_FILE, CHR_MAX, ICON_NAME);