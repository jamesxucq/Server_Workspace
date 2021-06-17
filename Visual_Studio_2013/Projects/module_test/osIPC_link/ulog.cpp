#include "StdAfx.h"

#include <stdio.h>
#include "ulog.h"

#define MKZEO(TEXT) TEXT[0] = _T('\0')
#define BUFFER_SIZE          0x1000  // 4K

int logger(TCHAR *logfile, const TCHAR *fmt, ...) {
	FILE *_log_fp_ = NULL;
	if (!logfile) return -1;
	_tfopen_s(&_log_fp_, logfile, _T("ab"));
	if (!_log_fp_) return -1;
//
	TCHAR buffer[BUFFER_SIZE];
	int size = 0;
//
	va_list ap;
	va_start(ap, fmt);
	size = _vsntprintf_s(buffer, BUFFER_SIZE - 1, BUFFER_SIZE, fmt, ap);
	va_end(ap);
//
	if (_log_fp_) {
		int wlen = fwrite(buffer, 2, size, _log_fp_);
		if (wlen != size) {
			printf("can not write to log file!\n%s", buffer);
			return -1;
		}
		fflush(_log_fp_);
	} else printf("not open log file!\n%s", buffer);
//
	if (!_log_fp_) return -1;
	fclose(_log_fp_); _log_fp_ = NULL;
//
	return 0;
}
