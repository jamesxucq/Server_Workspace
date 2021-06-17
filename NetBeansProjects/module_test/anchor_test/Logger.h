#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef enum {
    log_lvl_fatal = 0,
    log_lvl_error,
    log_lvl_warn,
    log_lvl_info,
    /////////////////////
    log_lvl_debug,
    log_lvl_trace,
    log_lvl_max
} log_lvl_t;

////////////////////////////////////////////////////////////////////////////////
extern char _log_file_[PATH_MAX];

int log_init(char *logfile, log_lvl_t log_lvl);
inline void log_fini();
int log_printf(log_lvl_t log_lvl, const char *fmt, ...);

////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
#define LOG_FATAL(fmt, ...)  \
	log_printf(log_lvl_fatal, "[%s %d %s]:"fmt"\n", __FILE__, __LINE__, \
			__FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_FATAL(fmt, ...)  \
	log_printf(log_lvl_fatal, ":"fmt"\n", ##__VA_ARGS__)
#endif

#ifdef DEBUG
#define LOG_ERROR(fmt, ...)  \
	log_printf(log_lvl_error, "[%s %d %s]:"fmt"\n", __FILE__, __LINE__, \
			__FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...)  \
	log_printf(log_lvl_error, ":"fmt"\n", ##__VA_ARGS__)
#endif


#ifdef DEBUG
#define LOG_WARN(fmt, ...)  \
	log_printf(log_lvl_warn, "[%s %d %s]:"fmt"\n", __FILE__, __LINE__, \
			__FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)  \
	log_printf(log_lvl_warn, ":"fmt"\n", ##__VA_ARGS__)
#endif


#ifdef DEBUG
#define LOG_INFO(fmt, ...)  \
	log_printf(log_lvl_info, "[%s %d %s]:"fmt"\n", __FILE__, __LINE__, \
			__FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)  \
	log_printf(log_lvl_info, ":"fmt"\n", ##__VA_ARGS__)
#endif

////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
#define LOG_DEBUG(fmt, ...)  \
	log_printf(log_lvl_debug, "[%s %d %s]:"fmt"\n", __FILE__, __LINE__,\
		   	__FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#ifdef DEBUG
#define LOG_TRACE(fmt, ...)  \
	log_printf(log_lvl_trace, "[%s %d %s]:"fmt"\n", __FILE__, __LINE__, \
			__FUNCTION__, ##__VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...)
#endif

/*
#define LOG_EXIT(status) \
    {LOG_NOTICE(syncat, "exit with status = %d", status); exit(status);}
 */

#endif /* LOGGER_H_ */
