#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "Logger.h"

#define BUFFER_SIZE          0x1000  // 4K
log_lvl_t _log_lvl_;
char _log_file_[PATH_MAX];
int _log_fd_ = -1;

int log_init(char *logfile, log_lvl_t log_lvl) {
    if (!logfile || log_lvl > log_lvl_max) return -1;

    strcpy(_log_file_, logfile);
#ifndef DEBUG
    //_log_fd_ = open(logfile, O_WRONLY|O_APPEND|O_CREAT|O_TRUNC, 0666);
    _log_fd_ = open(logfile, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (_log_fd_ < 0) return -1;
#endif
    _log_lvl_ = log_lvl;

    return 0;
}

inline void log_fini() {
    if (-1 == _log_fd_) return;
    close(_log_fd_);
    _log_fd_ = -1;
}

int log_printf(log_lvl_t log_lvl, const char *fmt, ...) {
    char buffer[BUFFER_SIZE];
    int size = 0;

    if (log_lvl > _log_lvl_) return 0;
    memset(buffer, '\0', BUFFER_SIZE);

    time_t current_time = time(NULL);
    struct tm current_tm = {0};
    int time_len = 0;
    memset(&current_tm, '\0', sizeof(struct tm));
    time_len = strftime(buffer, BUFFER_SIZE - 1,
            "[%Y-%m-%d %H:%M:%S]", localtime_r(&current_time, &current_tm));

    va_list ap;
    va_start(ap, fmt);
    size = vsnprintf(buffer + time_len, BUFFER_SIZE - time_len - 1, fmt, ap) +
            time_len;
    va_end(ap);

#ifdef DEBUG
    printf(buffer);
#else
    if (-1 != _log_fd_) {
        int wlen = write(_log_fd_, buffer, size);
        if (wlen != size) {
            printf("Can not write to log file!!!\n%s", buffer);
            return -1;
        }
    } else printf("Not open log file!!!\n%s", buffer);
#endif
    return 0;
}