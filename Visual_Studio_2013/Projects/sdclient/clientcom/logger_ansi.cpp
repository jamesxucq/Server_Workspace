#include "StdAfx.h"

#include "third_party.h"
#include "StringUtility.h"

#include "logger_ansi.h"

VOID _LOG_ANSI(const char *fmt, const char *message) {
	TCHAR szFormat[1024], szMessage[2048];
	log_printf(log_lvl_trace, strcon::ansi_ustr(szFormat, fmt), strcon::ansi_ustr(szMessage, message));
}
