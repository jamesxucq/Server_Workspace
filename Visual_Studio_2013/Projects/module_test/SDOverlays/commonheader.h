
#pragma once

#include "clientcom/CacheInterface.h"
#include "clientcom/CacheLinker.h"
#include "OverlayBzl.h"

#define SYNCING_ICON		_T("SyncingIcon.ico") 
#define NORMAL_ICON			_T("NormalIcon.ico") 
#define MODIFIED_ICON		_T("ModifiedIcon.ico")
#define CONFLICT_ICON		_T("ConflictIcon.ico") 
#define DELETE_ICON			_T("DeletedIcon.ico") 
#define READONLY_ICON		_T("ReadOnlyIcon.ico")
#define LOCKED_ICON			_T("LockedIcon.ico")
#define ADDED_ICON			_T("AddedIcon.ico")
#define FORBID_ICON			_T("IgnoredIcon.ico")
//#define FORBID_ICON			_T("ForbidIcon.ico")
//#define UNVERSIONED_ICON		_T("UnversionedIcon") 

#define CREATE_ICON_FILE(pwszIconFile, wsIconName, chrMax)									\
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, chrMax);			\
	TCHAR *strend = wcsrchr(pwszIconFile, _T('\\'));										\
	if(strend) *strend = _T('\0');															\
	_tcscat_s(pwszIconFile, chrMax, _T("\\icons\\XPStyle\\"));								\
	_tcscat_s(pwszIconFile, chrMax, wsIconName);