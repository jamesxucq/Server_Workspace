#include "StdAfx.h"
#include "CommonUtility.h"

// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
//
#include "INodeUtility.h"

//
DWORD INodeUtili::ResetINodeHandle(HANDLE hRecursINode, BOOL bTruncate) {
    DWORD dwResult = SetFilePointer(hRecursINode, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return ((DWORD)-1);
	}
	if(bTruncate) {
		dwResult = SetEndOfFile(hRecursINode);
		if(FALSE==dwResult && NO_ERROR!=GetLastError()) {	
			return ((DWORD)-1);
		}
	}
	//
	return 0x00;
}

DWORD INodeUtili::INodeANext(HANDLE hINodeA, struct INodeA *pINodeA) {
    DWORD slen;
    if(!ReadFile(hINodeA, pINodeA, sizeof(struct INodeA), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return ((DWORD)-1);
    }
    return 0x00;
}

DWORD INodeUtili::INodeAApp(HANDLE hINodeA, const TCHAR *szINodeAName, DWORD dwINodeAType){
	DWORD dwResult = SetFilePointer(hINodeA, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return ((DWORD)-1);
	}
	//
	struct INodeA tINodeA;
    DWORD wlen;
    //
	tINodeA.dwINodeType = dwINodeAType;
	_tcscpy_s(tINodeA.szINodeName, MAX_PATH, szINodeAName);
    if(!WriteFile(hINodeA, &tINodeA, sizeof(struct INodeA), &wlen, NULL)){
        // if(ERROR_HANDLE_EOF != GetLastError())
        return ((DWORD)-1);
    }
    //
	return 0x00;
}

ULONG INodeUtili::FindFolderINodeA(HANDLE hINodeA, struct INodeA *pINodeA, ULONG lINodeAInde) {
	ULONG lFolderInde = lINodeAInde;
    DWORD slen;
    //
	for (; ; ++lFolderInde) {
	    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		OverLapped.Offset = lFolderInde * sizeof(struct INodeA);
	    if(!ReadFile(hINodeA, pINodeA, sizeof(struct INodeA), &slen, &OverLapped) || !slen) {
		    // if(ERROR_HANDLE_EOF != GetLastError())
			return INVA_INDE_VALU;
		}
		//
		if(INTYPE_DIRECTORY == pINodeA->dwINodeType)
			break;
	}
    //
// _LOG_DEBUG(_T("find folder inde:%d name:%s"), lFolderInde, pINodeA->szINodeName);
    return lFolderInde;
}


//
//
ULONG INodeUtili::ReadNodeV(HANDLE hINodeV, ULONG lINodeVInde, struct INodeV *pINodeV) {
    DWORD slen;
	//
	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lINodeVInde * sizeof(struct INodeV);
    if(!ReadFile(hINodeV, pINodeV, sizeof(struct INodeV), &slen, &OverLapped) || !slen) {
        return INVA_INDE_VALU;
    }
    return lINodeVInde;
}

DWORD INodeUtili::INodeVApp(HANDLE hINodeV, struct INodeV *pINodeV) {
	DWORD dwResult = SetFilePointer(hINodeV, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError()) {
        return ((DWORD)-1);
	}
	//
    DWORD wlen;
    if(!WriteFile(hINodeV, pINodeV, sizeof(struct INodeV), &wlen, NULL))
        return ((DWORD)-1);
    return 0x00;
}

ULONG INodeUtili::ModifyNodeV(HANDLE hINodeV, ULONG lINodeVInde, DWORD dwINodeVType) {
    DWORD slen, wlen;
    struct INodeV tINodeV;
    //
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lINodeVInde * sizeof(struct INodeV);
    if(!ReadFile(hINodeV, &tINodeV, sizeof(struct INodeV), &slen, &OverLapped) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError())
        return INVA_INDE_VALU;
    }
    //
	tINodeV.dwINodeType = dwINodeVType;
    if(!WriteFile(hINodeV, &tINodeV, sizeof(struct INodeV), &wlen, &OverLapped))
        return INVA_INDE_VALU;
    //
    return lINodeVInde;
}

DWORD INodeUtili::FileAdd(HANDLE hINodeV, TCHAR *szFileName, FILETIME *ftLastWrite) {
	struct INodeV tINodeAdd;
	memcpy(&tINodeAdd.ftLastWrite, ftLastWrite, sizeof(FILETIME));
	tINodeAdd.dwINodeType = INTYPE_FILE;
	_tcscpy_s(tINodeAdd.szINodeName, MAX_PATH, szFileName);
	//
	return INodeVApp(hINodeV, &tINodeAdd);
}

DWORD INodeUtili::FolderAdd(HANDLE hINodeV, TCHAR *szFolderName) {
	struct INodeV tINodeAdd;
	tINodeAdd.dwINodeType = INTYPE_DIRECTORY;
	_tcscpy_s(tINodeAdd.szINodeName, MAX_PATH, szFolderName);
	//
	return INodeVApp(hINodeV, &tINodeAdd);
}

ULONG INodeUtili::FindFolderINodeV(HANDLE hINodeV, struct INodeV *pINodeV, ULONG lINodeVInde) {
	ULONG lFolderInde = lINodeVInde;
    DWORD slen;
    //
	for (; ; ++lFolderInde) {
	    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		OverLapped.Offset = lFolderInde * sizeof(struct INodeV);
	    if(!ReadFile(hINodeV, pINodeV, sizeof(struct INodeV), &slen, &OverLapped) || !slen) {
		    // if(ERROR_HANDLE_EOF != GetLastError())
			return INVA_INDE_VALU;
		}
		//
		if(INTYPE_DIRECTORY == pINodeV->dwINodeType)
			break;
	}
    //
// _LOG_DEBUG(_T("find folder inde:%d name:%s"), lFolderInde, pINodeA->szINodeName);
    return lFolderInde;
}