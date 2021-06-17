#include "StdAfx.h"

#include "third_party.h"
#include "StringUtility.h"

#include "logger_ansi.h"

void LOG_TRACE_ANSI(char *fmt, char *message)
{
	TCHAR szFormat[512], szMessage[1024];
	log_printf(log_lvl_trace, nstriconv::ansi_unicode(szFormat, fmt), nstriconv::ansi_unicode(szMessage, message));
}
