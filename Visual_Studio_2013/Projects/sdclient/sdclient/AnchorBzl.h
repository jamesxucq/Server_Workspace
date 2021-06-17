#pragma once

#include "clientcom/clientcom.h"

#include "LockedAction.h"

namespace NAnchorBzl {
	// return 0; succ/ return -1 error
	DWORD Initialize(TCHAR *szAnchorInde, TCHAR *szAnchorData, TCHAR *szLocation);
	DWORD Finalize();
	//
	inline DWORD GetLastAnchor() { 
		return objAnchorCache.GetLastAnchor(); 
	}
	DWORD AddNewAnchor(DWORD dwNewAnchor);
	DWORD DiscardAction();
	//	
	DWORD InsAnchorAction(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter); // >0:finish 0:nothing
	// static VOID FileLockedTip(TCHAR *szFileName);
	// DWORD InitInsAction(LkAiVec *pLkAiVec, const TCHAR *szDriveLetter); // >0:finish 0:nothing
	//
	static VOID OptimizeActiveAction(FilesVec *pActiveVec, FilesVec *pIndeVec);
	DWORD GetActionByAnchor(FilesVec *pActioVec, DWORD dwAnchor);
	inline FilesVec *GetLastActioVec() {
		return objAnchorCache.GetLastActioVec();
	}
	//
    DWORD OptimizeCached(const TCHAR *szDriveLetter); // >0:finish 0:nothing
	inline DWORD CheckActiveAction() { // >0:finish 0:nothing
		return objAnchorCache.GetLastActioVec()->empty()? 0: 0x0001; 
	}
	VOID ClearAnchorEnviro(BOOL bEraseAnchor);
	VOID InserAnchorIcon(const TCHAR *szDriveLetter);
	VOID EraseAnchorIcon(const TCHAR *szDriveLetter);
};
