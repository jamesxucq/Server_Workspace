#pragma once

namespace RecursiveUtility
{
	static HANDLE CreatRecursiveEnviro(TCHAR *szDrive);
	static DWORD HandRecursiveFile(HANDLE hPathFile, HANDLE hEntryFile, DWORD dwMaxRecursive);
	//
	DWORD RecursiveFile(HANDLE hEntryFile, TCHAR *szDrive); // 0:ok 0x01:error 0x02:processing 0x03:not_found
};
