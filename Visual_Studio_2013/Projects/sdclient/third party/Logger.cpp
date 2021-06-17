#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "Logger.h"
//
#define BUFFER_SIZE          0x1000  // 4K
static log_lvl_t _log_lvl_;
static TCHAR _log_file_[MAX_PATH];
static HANDLE _log_hand_ = INVALID_HANDLE_VALUE;
//
void log_init(TCHAR *logfile, log_lvl_t log_lvl) {
    if (logfile && (log_lvl <= log_lvl_max)) {
        _tcscpy_s(_log_file_, logfile);
        _log_hand_ = CreateFile( _log_file_,
                                 GENERIC_WRITE, // | GENERIC_READ,
                                 FILE_SHARE_READ,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);
        if( INVALID_HANDLE_VALUE == _log_hand_ ) {
            fprintf(stderr, "create file failed: %d\n", GetLastError() );
        }
        _log_lvl_ = log_lvl;
    }
}

void log_fini() {
    if(INVALID_HANDLE_VALUE != _log_hand_) {
        CloseHandle( _log_hand_ );
        _log_hand_ = INVALID_HANDLE_VALUE;
    }
}

static TCHAR *LEVEL_TIPS[] = {_T("[FATAL]"), _T("[ERROR]"), _T("[WARN]"), _T("[INFO]"), _T("[DEBUG]"), _T("[TRACE]"), _T("[MAX]")};
static int TIPS_LEN[] = {0x07, 0x07, 0x06, 0x06, 0x07, 0x07, 0x05};

void log_printf(log_lvl_t log_lvl, const TCHAR *fmt, ...) {
    if (log_lvl > _log_lvl_) return;
    //
    TCHAR buffer[BUFFER_SIZE];
    SYSTEMTIME st;
    GetLocalTime(&st);
    _stprintf_s(buffer,_T("[%04d-%02d-%02d %02d:%02d:%02d]"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
    int prev_len = _tcslen(buffer);
    _tcscat_s(buffer, BUFFER_SIZE-prev_len, LEVEL_TIPS[log_lvl]);
    prev_len += TIPS_LEN[log_lvl];
    //
    va_list ap;
    va_start(ap, fmt);
    int size = _vsntprintf_s(buffer+prev_len, BUFFER_SIZE-prev_len, BUFFER_SIZE-prev_len, fmt, ap) + prev_len;
    va_end(ap);
    //
    DWORD wlen;
    if (INVALID_HANDLE_VALUE != _log_hand_) {
        WriteFile(_log_hand_, buffer, size << 1, &wlen, NULL);
        if (wlen != size << 1) fprintf(stderr, "can not write to log file!\n%s", buffer);
    } else fprintf(stderr, "not open log file!\n%s", buffer);
}
