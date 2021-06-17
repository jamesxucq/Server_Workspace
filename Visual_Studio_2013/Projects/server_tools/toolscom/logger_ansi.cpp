#include "StdAfx.h"

#include "logger.h"
#include "StringUtility.h"

#include "logger_ansi.h"

void LOG_TRACE_ANSI(char *fmt, char *message)
{
	wchar_t szFormat[512], szMessage[1024];
	log_printf(log_lvl_trace, striconv::ansi_unicode(szFormat, fmt), striconv::ansi_unicode(szMessage, message));
}
