
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

#include "access_log.h"
//
#define BUFFER_SIZE          0x1000  // 4K
static char _log_file_[PATH_MAX];
static int _log_fd_ = -1;

//

void access_inital(char *logfile) {
    if (logfile) {
        strcpy(_log_file_, logfile);
        //_log_fd_ = open64(_log_file_, O_WRONLY|O_APPEND|O_CREAT|O_TRUNC, 0666);
        _log_fd_ = open64(_log_file_, O_WRONLY | O_APPEND | O_CREAT, 0666);
    }
}

void access_final() {
    if (-1 != _log_fd_) close(_log_fd_);
}
//

void access_printf(const char *fmt, ...) {
    if (-1 == _log_fd_) return;
    //
    char buffer[BUFFER_SIZE];
    time_t current_time = time(NULL);
    struct tm current_tm;
    int time_len = 0;
    memset(&current_tm, '\0', sizeof (struct tm));
    time_len = strftime(buffer, BUFFER_SIZE - 1,
            "[%Y-%m-%d %H:%M:%S]", localtime_r(&current_time, &current_tm));
    //
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(buffer + time_len, BUFFER_SIZE - time_len - 1, fmt, ap) + time_len;
    va_end(ap);
    //
#ifdef _DEBUG
    fprintf(stdout, buffer);
#else
    int wlen = write(_log_fd_, buffer, size);
    if (wlen != size) fprintf(stderr, "can not write to log file!\n%s", buffer);
#endif
}
