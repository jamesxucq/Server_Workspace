#pragma once

namespace RecursiveUtility {
	static HANDLE CreatRecursiveEnviro(TCHAR *szDriveLetter);
	static DWORD HandRecursiveFile(HANDLE hPathFile, HANDLE hEntryFile, DWORD dwMaxRecursive);
	//
	DWORD RecursiveFile(HANDLE hEntryFile, TCHAR *szDriveLetter); // 0:ok 0x01:error 0x02:processing
};
