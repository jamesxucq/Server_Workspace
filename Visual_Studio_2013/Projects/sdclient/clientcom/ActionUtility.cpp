#include "StdAfx.h"

#include "third_party.h"
#include "FileUtility.h"
#include "iattb_type.h"
#include "ActionUtility.h"

#define COPY_BUFFER	0x2000  // 8K
int ActionUtility::ActioCopy(HANDLE hTo, HANDLE hFrom) {
	int iActioValue = 0;
	// 
    DWORD dwResult = SetFilePointer(hTo, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return -1;
    dwResult = SetFilePointer(hFrom, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return -1;
    //
    DWORD dwWritenSize = 0x00;
    DWORD dwReadSize = 0x00;
    CHAR szBuffer[COPY_BUFFER];
    while(ReadFile(hFrom, szBuffer, COPY_BUFFER, &dwReadSize, NULL) && 0<dwReadSize) {
        WriteFile(hTo, szBuffer, dwReadSize, &dwWritenSize, NULL);
        if(dwReadSize != dwWritenSize) {
            _LOG_WARN(_T("fatal write error: %d"), GetLastError());
        }
		iActioValue++;
    }
    //
    dwResult = SetFilePointer(hFrom, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return -1;
    dwResult = SetEndOfFile(hFrom);
    if(FALSE==dwResult && NO_ERROR!=GetLastError())
        return -1;
    //
    FlushFileBuffers(hTo);
    FlushFileBuffers(hFrom);
    //
    return iActioValue;
}


int ActionUtility::EraseActio(HANDLE hErase) {
    DWORD dwResult = SetFilePointer(hErase, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return -1;
    dwResult = SetEndOfFile(hErase);
    if(FALSE==dwResult && NO_ERROR!=GetLastError())
        return -1;
    //
    FlushFileBuffers(hErase);
	//
	return 0;
}

VOID ActionUtility::LockActioApp(HANDLE hLockActio, TCHAR *szFileName, DWORD dwActioType) {
	DWORD dwWritenSize;
	struct Action tLockActio;
	//
	tLockActio.dwActioType = dwActioType;
	_tcscpy(tLockActio.szFileName, szFileName);
	tLockActio.ulTimestamp = 0x00;
	WriteFile(hLockActio, &tLockActio, sizeof(struct Action), &dwWritenSize, NULL);
_LOG_DEBUG( _T("lock addi:%s %08X"), szFileName, dwActioType);
}
