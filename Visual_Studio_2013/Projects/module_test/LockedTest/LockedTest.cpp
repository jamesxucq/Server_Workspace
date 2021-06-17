// LockedTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"


DWORD IsFileLocked(const TCHAR *szFileName)   // -1:error 0:not lock 1:locked
{
    HANDLE hFileLocked;
//
    if(!szFileName) return -1;

    hFileLocked = CreateFile( szFileName,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    if( hFileLocked == INVALID_HANDLE_VALUE ) {
        if(ERROR_SHARING_VIOLATION == GetLastError()) return 0x01;
        return -1;
    }
//
    if(hFileLocked != INVALID_HANDLE_VALUE) {
        CloseHandle( hFileLocked );
        hFileLocked = INVALID_HANDLE_VALUE;
    }
//
    return 0;
}
//
// #define FILE_SHARE_READ                 0x00000001  
// #define FILE_SHARE_WRITE                0x00000002  
// #define FILE_SHARE_DELETE               0x00000004

DWORD ShareReadLocked(const TCHAR *szFileName) {
    HANDLE hFileLocked;
//
    if(!szFileName) return -1;

    hFileLocked = CreateFile( szFileName,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    if( hFileLocked == INVALID_HANDLE_VALUE ) {
        if(ERROR_SHARING_VIOLATION == GetLastError()) return 0x01;
        return -1;
    }
//
    if(hFileLocked != INVALID_HANDLE_VALUE) {
        CloseHandle( hFileLocked );
        hFileLocked = INVALID_HANDLE_VALUE;
    }
//
    return 0;
}
//
DWORD OpenFile(const TCHAR *szFileName) {
    HANDLE hFileLocked;
//
    if(!szFileName) return -1;

    hFileLocked = CreateFile( szFileName,
                              GENERIC_WRITE | GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    if( hFileLocked == INVALID_HANDLE_VALUE ) {
        if(ERROR_SHARING_VIOLATION == GetLastError()) return 0x01;
        return -1;
    }
//
//    if(hFileLocked != INVALID_HANDLE_VALUE) {
//        CloseHandle( hFileLocked );
//        hFileLocked = INVALID_HANDLE_VALUE;
//    }
//
    return 0;
}

const TCHAR *szFileName = _T("E:\\PPPD236.avi");
// const TCHAR *szFileName = _T("E:\\Server Workspace\\Visual_Studio_2008\\Projects\\module_test\\LockedTest\\Debug\\cmd.exe");

int _tmain(int argc, _TCHAR* argv[])
{
	if(OpenFile(szFileName)) printf("open file error!\n");
	else printf("open file ok.\n");
	for(;;){
		//
		if(IsFileLocked(szFileName)) printf("all attrib locked!\n");
		else printf("all attrib not locked.\n");
		//
		if(ShareReadLocked(szFileName)) printf("share read locked!\n");
		else printf("share read not locked.\n");
		//
		Sleep(2000);
	}

	return 0;
}

