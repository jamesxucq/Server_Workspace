#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#include "common_macro.h"
#include "utility/strptime.h"
#include "string_utility.h"

//
char *lsprif(char *dest, const char *format, ...) {
    va_list arg;
    int done;
    //
    if (!dest) return NULL;
    va_start(arg, format);
    done = vsprintf(dest, format, arg);
    va_end(arg);
    if (0x00 > done) return NULL;
    //
    return dest + done;
}

char *lscpy(char * dest, const char * src) {
    if (!dest) return NULL;
    register char *d = dest;
    register const char *s = src;
    while ((*d = *s++)) d++;
    return d;
}

char *lsncpy(char *dest, const char *src, size_t n) {
    char c;
    //
    if (!dest) return NULL;
    if (n >= 4) {
        size_t n4 = n >> 2;
        for (;;) {
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            if (--n4 == 0) goto last_chars;
        }
        return dest;
    }
last_chars:
    n &= 3;
    if (n == 0) {
        *dest = '\0';
        return dest;
    }
    for (;;) {
        c = *src++;
        *dest = c;
        if (c == '\0') break;
        dest++;
        if (--n == 0) {
            *dest = '\0';
            return dest;
        }
    }
    //
    return dest;
}

char *lszcpy(char *dest, const char *source, size_t count) {
    while (count && (*dest++ = *source++)) count--; /* copy string */
    dest[0x00] = '\0';
    return (dest);
}

char *lscat(char * dest, const char * src) {
    if (!dest) return NULL;
    return lscpy(dest + strlen(dest), src);
}

char *lsncat(char *dest, const char *src, size_t n) {
    if (!dest) return NULL;
    char *destptr = dest + strlen(dest);
    for (; n > 0 && (*destptr = *src) != '\0'; src++, destptr++, n--);
    if (n == 0) *destptr = '\0';
    return destptr;
}

//

char *strsplit(char *str, const char *delim) {
    char *toksp;
    static char *tokep;
    //
    if (!str) {
        if (!tokep) return NULL;
        toksp = tokep + strlen(delim);
    } else toksp = str;
    //
    tokep = strstr(toksp, delim);
    if (tokep) *tokep = '\0';
    //
    return toksp;
}

inline char *utc_time_text(time_t utctime) { // Wed, 15 Nov 1995 04:58:08 GMT
    static char timestr[TIME_LENGTH];
    struct tm *utctm;
    //
    if (!utctime) utctime = time(NULL);
    utctm = gmtime(&utctime);
    if (!strftime(timestr, TIME_LENGTH, "%a, %d %b %Y %H:%M:%S %Z", utctm)) return NULL;
    //
    return timestr;
}

inline time_t utc_time_value(char *timestr) { // Wed, 15 Nov 1995 04:58:08 GMT
    struct tm utctm;
    //
    if (!timestr) return 0;
    memset(&utctm, '\0', sizeof (struct tm));
    _strptime(&utctm, timestr);
    time_t calender = _mktime(&utctm);
    //
    return calender;
}

char *random_seion_id() {
    static char rand_str[MIN_TEXT_LEN];
    static unsigned int tatol;
    snprintf(rand_str, MIN_TEXT_LEN, "%d-%u-%d", getpid(), (uint32) time(NULL), ++tatol);
    return rand_str;
}

char *random_valid() {
    static char rand_str[MIN_TEXT_LEN];
    static long int last_seed;
    char code_array[62] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    //
    if (!last_seed) last_seed = time(NULL);
    int tatol;
    for (tatol = 0; tatol < 20; tatol++) {
        srandom((unsigned int) last_seed);
        last_seed = random();
        rand_str[tatol] = code_array[last_seed % 62];
    }
    rand_str[tatol] = '\0';
    //
    return rand_str;
}

char *random_accesskey() {
    static char rand_str[MIN_TEXT_LEN];
    static long int last_seed;
    char code_array[62] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    //
    if (!last_seed) last_seed = time(NULL);
    int tatol;
    for (tatol = 0; tatol < 20; tatol++) {
        srandom((unsigned int) last_seed);
        last_seed = random();
        rand_str[tatol] = code_array[last_seed % 62];
    }
    rand_str[tatol] = '\0';
    //
    return rand_str;
}

void saddr_split(serv_addr *saddr, const char *addr_txt) {
    char *toke = NULL;
    memset(saddr, '\0', sizeof (serv_addr));
    toke = strchr(addr_txt, ':');
    if (toke) {
        strncpy(saddr->sin_addr, addr_txt, toke - addr_txt);
        saddr->sin_port = atoi(++toke);
    } else strcpy(saddr->sin_addr, addr_txt);
}

char *name_value(char **name, char *str, const char delim) {
    char *toksp;
    static char *tokep;
    //
    if (!str) {
        if (!tokep) return NULL;
        toksp = tokep + 1;
    } else toksp = str;
    //
    tokep = strchr(toksp, delim);
    if (tokep) *tokep = '\0';
    //
    if (name) *name = toksp;
    char *value = strchr(toksp, '=');
    if (value) {
        *value = '\0';
        ++value;
    } else value = toksp;
    //
    return value;
}

inline char *next_dirpath(char *path, char *subdir) {
    static char *toke, *toksp;
    static char fullpath[MAX_PATH * 3];
    //
    if (!subdir) {
        if (!toke) return NULL;
    } else toksp = toke = subdir;
    toke = strchr(++toke, '/');
    //
    char *last = lscpy(fullpath, path);
    if (toke) {
        lsncpy(last, toksp, toke - toksp);
    } else strcpy(last, toksp);
    //
    return fullpath;
}

char *bakup_folder(time_t bak_time) { // Nov_15_1995_GMT
    static char folder[TIME_LENGTH];
    struct tm *utctm;
    //
    if (!bak_time) bak_time = time(NULL);
    utctm = gmtime(&bak_time);
    if (!strftime(folder, TIME_LENGTH, "%b_%d_%Y_%Z", utctm)) return NULL;
    //
    return folder;
}

const char *strlchr(const char *str) {
    const char *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int longword, himagic, lomagic;
    //
    for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
        if (*char_ptr == '\0')
            return (char_ptr - 1);
    }
    //
    longword_ptr = (unsigned long int *) char_ptr;
    himagic = 0x80808080L;
    lomagic = 0x01010101L;
    if (sizeof (longword) > 4) {
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }
    if (sizeof (longword) > 8) abort();
    //
    for (;;) {
        longword = *longword_ptr++;
        if (((longword - lomagic) & ~longword & himagic) != 0) {
            const char *cp = (const char *) (longword_ptr - 1);
            //
            if (cp[0] == 0)
                return (cp - 1);
            if (cp[1] == 0)
                return cp;
            if (cp[2] == 0)
                return (cp + 1);
            if (cp[3] == 0)
                return (cp + 2);
            if (sizeof (longword) > 4) {
                if (cp[4] == 0)
                    return (cp + 3);
                if (cp[5] == 0)
                    return (cp + 4);
                if (cp[6] == 0)
                    return (cp + 5);
                if (cp[7] == 0)
                    return (cp + 6);
            }
        }
    }
}

const char *strlast(const char *str) {
    const char *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int longword, himagic, lomagic;
    //
    for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
        if (*char_ptr == '\0')
            return char_ptr;
    }
    //
    longword_ptr = (unsigned long int *) char_ptr;
    himagic = 0x80808080L;
    lomagic = 0x01010101L;
    if (sizeof (longword) > 4) {
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }
    if (sizeof (longword) > 8) abort();
    //
    for (;;) {
        longword = *longword_ptr++;
        if (((longword - lomagic) & ~longword & himagic) != 0) {
            const char *cp = (const char *) (longword_ptr - 1);
            //
            if (cp[0] == 0)
                return cp;
            if (cp[1] == 0)
                return (cp + 1);
            if (cp[2] == 0)
                return (cp + 2);
            if (cp[3] == 0)
                return (cp + 3);
            if (sizeof (longword) > 4) {
                if (cp[4] == 0)
                    return (cp + 4);
                if (cp[5] == 0)
                    return (cp + 5);
                if (cp[6] == 0)
                    return (cp + 6);
                if (cp[7] == 0)
                    return (cp + 7);
            }
        }
    }
}

char *strzcpy(char *dest, const char *source, size_t count) {
    char *start = dest;
    while (count && (*dest++ = *source++)) count--; /* copy string */
    if (count) /* pad out with zeroes */
        while (--count) *dest++ = '\0';
    else *dest = '\0';
    return (start);
}
