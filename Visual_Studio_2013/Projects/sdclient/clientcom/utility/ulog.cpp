#include "StdAfx.h"

#include "ulog.h"

#define MKZEO(TEXT) TEXT[0] = _T('\0')
#define BUFFER_SIZE          0x1000  // 4K

int logger(TCHAR *logfile, const TCHAR *fmt, ...) {
	if (!logfile) return -1;
    HANDLE log_hand = CreateFile( logfile,
                             GENERIC_WRITE, // | GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
    if( INVALID_HANDLE_VALUE == log_hand ) {
        fprintf(stderr, "create file failed: %d\n", GetLastError() );
		return -1;
    }
    DWORD result = SetFilePointer(log_hand, 0x00, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
		CloseHandle( log_hand );
        return -1;	
	}
//
	TCHAR buffer[BUFFER_SIZE];
	int size = 0;
//
	va_list ap;
	va_start(ap, fmt);
	size = _vsntprintf_s(buffer, BUFFER_SIZE, BUFFER_SIZE, fmt, ap);
	va_end(ap);
//
    DWORD wlen;
    WriteFile(log_hand, buffer, size<<0x01, &wlen, NULL);
	if (wlen != size<<0x01) {
		fprintf(stderr, "can not write to log file!\n%s", buffer);
		CloseHandle( log_hand );
		return -1;
	}
//
    CloseHandle( log_hand );
	return 0;
}
