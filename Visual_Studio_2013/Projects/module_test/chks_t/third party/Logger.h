#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//#define _DEBUG 0

typedef enum {
    log_lvl_fatal = 0,
    log_lvl_error,
    log_lvl_warn,
    log_lvl_info,
    //
    log_lvl_debug,
    log_lvl_trace,
    log_lvl_max
} log_lvl_t;

//
void log_init(TCHAR *logfile, log_lvl_t log_lvl);
void log_printf(log_lvl_t log_lvl, const TCHAR *fmt, ...);

//
#ifdef _DEBUG
#define _LOG_FATAL(fmt, ...)  \
	log_printf(log_lvl_fatal, _T("[%s %d %s]:") fmt _T("\r\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ##__VA_ARGS__)
#else
#define _LOG_FATAL(fmt, ...)  \
	log_printf(log_lvl_fatal, _T(":") fmt _T("\r\n"), ##__VA_ARGS__)
#endif

#ifdef _DEBUG
#define _LOG_ERROR(fmt, ...)  \
	log_printf(log_lvl_error, _T("[%s %d %s]:") fmt _T("\r\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ##__VA_ARGS__)
#else
#define _LOG_ERROR(fmt, ...)  \
	log_printf(log_lvl_error, _T(":") fmt _T("\r\n"), ##__VA_ARGS__)
#endif


#ifdef _DEBUG
#define _LOG_WARN(fmt, ...)  \
	log_printf(log_lvl_warn, _T("[%s %d %s]:") fmt _T("\r\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ##__VA_ARGS__)
#else
#define _LOG_WARN(fmt, ...)  \
	log_printf(log_lvl_warn, _T(":") fmt _T("\r\n"), ##__VA_ARGS__)
#endif


#ifdef _DEBUG
#define _LOG_INFO(fmt, ...)  \
	log_printf(log_lvl_info, _T("[%s %d %s]:") fmt _T("\r\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ##__VA_ARGS__)
#else
#define _LOG_INFO(fmt, ...)  \
	log_printf(log_lvl_info, _T(":") fmt _T("\r\n"), ##__VA_ARGS__)
#endif

//
#ifdef _DEBUG
#define _LOG_DEBUG(fmt, ...)  \
	log_printf(log_lvl_debug, _T("[%s %d %s]:") fmt _T("\r\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ##__VA_ARGS__)
#else
#define _LOG_DEBUG(fmt, ...)  \
	log_printf(log_lvl_debug, _T(":") fmt _T("\r\n"), ##__VA_ARGS__)
#endif

#ifdef _DEBUG
#define _LOG_TRACE(fmt, ...)  \
	log_printf(log_lvl_trace, _T("[%s %d %s]:") fmt _T("\r\n"), _T(__FILE__), __LINE__, _T(__FUNCTION__), ##__VA_ARGS__)
#else
#define _LOG_TRACE(fmt, ...)  \
	log_printf(log_lvl_trace, _T(":") fmt _T("\r\n"), ##__VA_ARGS__)
#endif

/*
#define _LOG_EXIT(status) \
{ LOG_NOTICE(syncat, "exit with status = %d", status); exit(status); }
*/

#endif /* LOGGER_H_ */