#ifndef STRINGUTILITY_H_
#define STRINGUTILITY_H_

#include <time.h>
#include "common_macro.h"

/**
 * Replace string or tokens as word from source string with given mode.
 */
char *strreplace(const char *mode, char *srcstr, const char *tokstr, const char *word);

char *strsplit(char *str, const char *delim);
char *split_value(char *str);
char *split_line(char *str);
char *name_value(char **name, char *str, const char delim);

inline char *append_path(char *path, char *subdir);
inline char *full_path(char *path, char *subdir);
inline char *full_name(char *location, char *path, char *file);
inline char *full_name_ex(char *location, char *path, char *file);

inline char *last_backup(char *path, char *subdir, time_t bak_time);
inline char *next_dirpath(char *path, char *subdir);

inline char *utc_time_text(time_t utctime);
inline time_t utc_time_value(char *timestr);

char *random_seion_id();
char *random_key();

typedef struct {
    char sin_addr[SERVNAME_LENGTH];
    int sin_port;
} serv_addr;
int address_split(serv_addr *saddr, char * addr_txt);

#endif /* STRINGUTILITY_H_ */
