#pragma once

namespace lutility {
	char *LoadFile(char *szContent, TCHAR *szFileName, DWORD dwLength);
	BOOL SaveFile(TCHAR *szFileName, char *szContent, DWORD dwLength);
	//
	BOOL EnumDeliRegistry(char *subKey, TCHAR *szPrefix);
	DWORD AddiSubkey(char *subKey, TCHAR *szValueName, TCHAR *szData);
	DWORD SetRegistry(char *subKey, TCHAR *szValueName, TCHAR *szData);
	DWORD DeliSubkey(char *subKey, TCHAR *szSubkey);
	DWORD DeliRegistry(char *subKey, TCHAR *szValueName);
	//
	BOOL RegistLibrary(TCHAR *szLibraryPath);
	BOOL ExecuteExplorer();
	BOOL KillExplorer();
	//
	TCHAR *Location(TCHAR *szFileName, TCHAR *szFilePath);
	TCHAR *UpdatePath(TCHAR *szFilePath, TCHAR *szFileName);
	// TCHAR *UpdatePath(TCHAR *szFileName, TCHAR *szLocation, TCHAR *szFilePath);
	BOOL URL_split(char *address, int *port, char *szLinkTxt, TCHAR *szReqURL);
};
