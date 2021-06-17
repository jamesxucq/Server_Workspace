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
log_lvl_t _log_lvl_;
TCHAR _log_file_[MAX_PATH];
//
void log_init(TCHAR *logfile, log_lvl_t log_lvl)
{
	if (logfile && (log_lvl < log_lvl_max)) {
		_tcscpy_s(_log_file_, logfile);
		_log_lvl_ = log_lvl;
	}
}

void log_printf(log_lvl_t log_lvl, const TCHAR *fmt, ...)
{
	if (log_lvl > _log_lvl_) return;
	//
	static TCHAR buffer[BUFFER_SIZE];
	buffer[0] = _T('\0');
	SYSTEMTIME st;
	wchar_t szLocalTime[32];
	GetLocalTime(&st);
	_stprintf_s(szLocalTime,_T("[%4d-%2d-%2d %2d:%2d:%2d]"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	int time_length = _tcslen(szLocalTime);
	_tcscpy_s(buffer, szLocalTime);
	//
	va_list ap;
	va_start(ap, fmt);
	int size = _vsntprintf_s(buffer + time_length, BUFFER_SIZE - time_length - 1, BUFFER_SIZE, fmt, ap) + time_length;
	va_end(ap);
	//
	// _log_fp_ = open(logfile, O_WRONLY|O_APPEND|O_CREAT|O_TRUNC, 0666);
	FILE *_log_fp_;
	_tfopen_s(&_log_fp_, _log_file_, _T("wb"));
	if (_log_fp_) {
		int wlen = fwrite(buffer, 2, size, _log_fp_);
		if (wlen != size) printf("can not write to log file!\n%s", buffer);
		fclose(_log_fp_);
	} else printf("not open log file!\n%s", buffer);
}
