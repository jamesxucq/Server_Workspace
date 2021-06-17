#pragma once

//
#include "ShellInterface.h"
#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default

//
extern DWORD FileStatusTab[];
#define CONV_ACTION_SETA(ACTIO_TYPE)		(CACHE_CONTROL_SETATTRI|FileStatusTab[ACTIO_TYPE])
#define CONV_ACTION_ERAS(ACTIO_TYPE)		(CACHE_CONTROL_ERASE|FileStatusTab[FILE_REAL_STAT(ACTIO_TYPE)])

//
inline DWORD CONV_STATUS_SETA(DWORD action_type) {
	DWORD disp_code = FILE_STATUS_INVALID;
	//
	if(ATTRB_IS_DIRECTORY(action_type)) {
		if((ADDI|COPY|MOVE) & action_type) 
			disp_code = CACHE_CONTROL_SETATTRI|FILE_STATUS_DIRECTORY|FILE_STATUS_ADDED;
		else if((EXIST|MODIFY) & action_type) 
			disp_code = CACHE_CONTROL_SETATTRI|FILE_STATUS_DIRECTORY|FILE_STATUS_MODIFIED;
		else if(DELE & action_type) 
			disp_code = CACHE_CONTROL_SETATTRI|FILE_STATUS_DIRECTORY|FILE_STATUS_DELETE;
	} else {
		if((ADDI|COPY|MOVE) & action_type) 
			disp_code = CACHE_CONTROL_SETATTRI|FILE_STATUS_FILE|FILE_STATUS_ADDED;
		else if((EXIST|MODIFY) & action_type) 
			disp_code = CACHE_CONTROL_SETATTRI|FILE_STATUS_FILE|FILE_STATUS_MODIFIED;
		else if(DELE & action_type) 
			disp_code = CACHE_CONTROL_SETATTRI|FILE_STATUS_FILE|FILE_STATUS_DELETE;
	}
	//
	return disp_code;
}

//
inline DWORD CONV_STATUS_ERAS(DWORD action_type) {
	DWORD disp_code = FILE_STATUS_INVALID;
	//
	if(ATTRB_IS_DIRECTORY(action_type)) {
		if((ADDI|COPY|MOVE) & action_type) 
			disp_code = CACHE_CONTROL_ERASE|FILE_STATUS_DIRECTORY|FILE_STATUS_ADDED;
		else if((EXIST|MODIFY) & action_type) 
			disp_code = CACHE_CONTROL_ERASE|FILE_STATUS_DIRECTORY|FILE_STATUS_MODIFIED;
		else if(DELE & action_type) 
			disp_code = CACHE_CONTROL_ERASE|FILE_STATUS_DIRECTORY|FILE_STATUS_DELETE;
	} else {
		if((ADDI|COPY|MOVE) & action_type) 
			disp_code = CACHE_CONTROL_ERASE|FILE_STATUS_FILE|FILE_STATUS_ADDED;
		else if((EXIST|MODIFY) & action_type) 
			disp_code = CACHE_CONTROL_ERASE|FILE_STATUS_FILE|FILE_STATUS_MODIFIED;
		else if(DELE & action_type) 
			disp_code = CACHE_CONTROL_ERASE|FILE_STATUS_FILE|FILE_STATUS_DELETE;
	}
	//
	return disp_code;
}
//

namespace ShellUtility {
	static VOID RefreshExplorer();
	static DWORD RebuildIcons();
	//
	VOID InitiExploWindo();
	BOOL RefreshExplorerWindows(const TCHAR *szDriveLetter);
	VOID RefreshExplorerIcons(const TCHAR *szIconDirectory);
};
