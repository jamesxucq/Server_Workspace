#pragma once

//
//
namespace NPrepaCache {
	static HANDLE RecursInitial(HashDB *pReallyDB, HANDLE *hRiverINodeV, const TCHAR *szLocation);
	static VOID FinalRecurs(HANDLE hReallyINodeV, HashDB *pReallyDB, const TCHAR *szLocation, HANDLE hRiverINodeV);
	static DWORD BuildReallyDB(HashDB *pReallyDB, HANDLE hReallyINodeV);
	//
	static DWORD BuildLockActi(HANDLE hLockActio, HANDLE hReallyINodeV, HashDB *pReallyDB, HANDLE hRiverINodeV);
	DWORD PrepaActionCache(HANDLE hLockActio, const TCHAR *szDriveLetter, const TCHAR *szLocation); // >0:finish 0:nothing <0:exception
};
