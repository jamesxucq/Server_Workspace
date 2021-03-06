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

#define BUFFER_SIZE          0x1000  // 4K
log_lvl_t _log_lvl_;
wchar_t _log_file_[MAX_PATH];
FILE *_log_fp_ = NULL;

int log_init(wchar_t *logfile, log_lvl_t log_lvl) {
    if (!logfile || log_lvl > log_lvl_max) return -1;

    _tcscpy_s(_log_file_, logfile);
#ifdef DEBUG
    //_log_fp_ = open(logfile, O_WRONLY|O_APPEND|O_CREAT|O_TRUNC, 0666);
     _tfopen_s(&_log_fp_, logfile, _T("wb"));
    if (!_log_fp_) return -1;
#endif
    _log_lvl_ = log_lvl;

    return 0;
}

void log_fini() {
    if (!_log_fp_) return;
    fclose(_log_fp_); _log_fp_ = NULL;
}


int log_printf(log_lvl_t log_lvl, const wchar_t *fmt, ...) {
    wchar_t buffer[BUFFER_SIZE];
    int size = 0;

    if (log_lvl > _log_lvl_) return 0;
    memset(buffer, _T('\0'), BUFFER_SIZE);

	SYSTEMTIME st;
	CString LocalTime;
	GetLocalTime(&st);
	LocalTime.Format(_T("[%4d-%2d-%2d %2d:%2d:%2d]"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	_tcscpy_s(buffer, LocalTime);
	int time_len = LocalTime.GetLength();

    va_list ap;
    va_start(ap, fmt);
    size = _vsntprintf_s(buffer + time_len, BUFFER_SIZE - time_len - 1, BUFFER_SIZE, fmt, ap) +
            time_len;
    va_end(ap);

#ifndef DEBUG
    _tprintf(buffer);
#else
    if (_log_fp_) {
        int wlen = fwrite(buffer, 2, size, _log_fp_);
        if (wlen != size) {
            printf("Can not write to log file!!!\n%s", buffer);
            return -1;
        }
		fflush(_log_fp_);
    } else printf("Not open log file!!!\n%s", buffer);
#endif
    return 0;
}