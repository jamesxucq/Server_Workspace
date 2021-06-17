#ifndef _STRPTIME_H_
#define _STRPTIME_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <time.h>
    
char * _strptime(const char *buf, const char *fmt, struct tm *tm);

#ifdef	__cplusplus
}
#endif

#endif /* _STRPTIME_H_ */