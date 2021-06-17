#include "StdAfx.h"
#include "BinaryLogger.h"

VOID BinaryLogger::LogBinaryCode(char *binary_code, int length) {
	if(!binary_code || length<0) return;
    HANDLE code_hand = CreateFile( _T("e:\\binary_code.bin"),
                             GENERIC_WRITE, // | GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
    if( INVALID_HANDLE_VALUE == code_hand ) {
        fprintf(stderr, "create file failed: %d\n", GetLastError() );
		return;
    }
    DWORD result = SetFilePointer(code_hand, 0x00, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
		CloseHandle( code_hand );
        return;
	}
//
    DWORD wlen;
    WriteFile(code_hand, binary_code, length, &wlen, NULL);
	if (wlen != length) {
		fprintf(stderr, "can not write to log file!\n%s", binary_code);
		CloseHandle( code_hand );
		return;
	}
//
    CloseHandle( code_hand );
}

VOID BinaryLogger::LogBinaryCodeW(TCHAR *binary_code, int length) {
	if(!binary_code || length<0) return;
    HANDLE code_hand = CreateFile( _T("e:\\binary_code.bin"),
                             GENERIC_WRITE, // | GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
    if( INVALID_HANDLE_VALUE == code_hand ) {
        fprintf(stderr, "create file failed: %d\n", GetLastError() );
		return;
    }
    DWORD result = SetFilePointer(code_hand, 0x00, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) {
		CloseHandle( code_hand );
		return;
	}
//
    DWORD wlen;
    WriteFile(code_hand, binary_code, length<<0x01, &wlen, NULL);
	if (wlen != length<<0x01) {
		fprintf(stderr, "can not write to log file!\n%s", binary_code);
		CloseHandle( code_hand );
		return;
	}
//
    CloseHandle( code_hand );
}