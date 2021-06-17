#pragma once

//
#define INTYPE_FILE			0x0000
#define INTYPE_DIRECTORY	0x0001

//
namespace INodeUtili {
	DWORD ResetINodeHandle(HANDLE hRecursINode, BOOL bTruncate);
	//
	struct INodeA {
		DWORD dwINodeType;
		TCHAR szINodeName[MAX_PATH];
	};
	DWORD INodeANext(HANDLE hINodeA, struct INodeA *pINodeA);
	DWORD INodeAApp(HANDLE hINodeA, const TCHAR *szINodeAName, DWORD dwINodeAType);
	ULONG FindFolderINodeA(HANDLE hINodeA, struct INodeA *pINodeA, ULONG lINodeAInde);
	//
	struct INodeV {
		DWORD dwINodeType;
		FILETIME ftLastWrite;
		TCHAR szINodeName[MAX_PATH];
	};
	DWORD ReadNodeV(HANDLE hINodeV, ULONG lINodeVInde, struct INodeV *pINodeV);
	DWORD INodeVApp(HANDLE hINodeV, struct INodeV *pINodeV);
	ULONG ModifyNodeV(HANDLE hINodeV, ULONG lINodeVInde, DWORD dwINodeVType);
	DWORD FileAdd(HANDLE hINodeV, TCHAR *szFileName, FILETIME *ftLastWrite);
	DWORD FolderAdd(HANDLE hINodeV, TCHAR *szFolderName);
	ULONG FindFolderINodeV(HANDLE hINodeV, struct INodeV *pINodeV, ULONG lINodeVInde);
};

//
//
