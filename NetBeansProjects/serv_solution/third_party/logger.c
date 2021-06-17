#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <libgen.h>
#include <errno.h>
// #include "common_macro.h"
#include "logger.h"
//
#define BUFFER_SIZE          0x1000  // 4K
static log_lvl_t _log_lvl_;
static char _log_file_[PATH_MAX];
static int _log_fd_ = -1;

//

void log_inital(char *logfile, log_lvl_t log_lvl) {
    if (logfile && (log_lvl <= log_lvl_max)) {
        strcpy(_log_file_, logfile);
        _log_lvl_ = log_lvl;
        //_log_fd_ = open64(_log_file_, O_WRONLY|O_APPEND|O_CREAT|O_TRUNC, 0666);
        _log_fd_ = open64(_log_file_, O_WRONLY | O_APPEND | O_CREAT, 0666);
    }
}

void log_final() {
    if(-1 != _log_fd_) close(_log_fd_);
}
        
//
static char *LEVEL_TIPS[] = {"[FATAL]", "[ERROR]", "[WARN]", "[INFO]", "[DEBUG]", "[TRACE]", "[MAX]"};
static int TIPS_LEN[] = {0x07, 0x07, 0x06, 0x06, 0x07, 0x07, 0x05};

void log_printf(log_lvl_t log_lvl, const char *fmt, ...) {
#ifndef _DEBUG
    if (log_lvl>_log_lvl_ || -1==_log_fd_) return;
#endif
    //
    char buffer[BUFFER_SIZE];
    time_t current_time = time(NULL);
    struct tm current_tm;
    int prev_len = 0;
    memset(&current_tm, '\0', sizeof (struct tm));
    prev_len = strftime(buffer, BUFFER_SIZE - 1,
            "[%Y-%m-%d %H:%M:%S]", localtime_r(&current_time, &current_tm));
    strcpy(buffer + prev_len, LEVEL_TIPS[log_lvl]);
    prev_len += TIPS_LEN[log_lvl];
    //
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(buffer + prev_len, BUFFER_SIZE - prev_len - 1, fmt, ap) + prev_len;
    va_end(ap);
    //
#ifdef _DEBUG
    fprintf(stdout, buffer);
#else
    int wlen = write(_log_fd_, buffer, size);
    if (wlen != size) fprintf(stderr, "can not write to log file!\n%s", buffer);
#endif
}
