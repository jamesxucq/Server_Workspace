#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "CommonMacro.h"
//#include "AuthMacro.h"
#include "StringOpt.h"

char *StrReplace(const char *mode, char *srcstr, const char *tokstr, const char *word) {
    if (mode == NULL || strlen(mode) != 2 || srcstr == NULL || tokstr == NULL || word == NULL) {
        //DEBUG("Unknown mode \"%s\".", mode);
        return NULL;
    }

    char *newstr, *newp, *srcp, *tokenp, *retp;
    newstr = newp = srcp = tokenp = retp = NULL;

    char method = mode[0], memuse = mode[1];
    int maxstrlen, tokstrlen;

    /* Put replaced string into malloced 'newstr' */
    if (method == 't') { /* Token replace */
        maxstrlen = strlen(srcstr) * ((strlen(word) > 0) ? strlen(word) : 1);
        newstr = (char*) malloc(maxstrlen + 1);

        for (srcp = (char*) srcstr, newp = newstr; *srcp; srcp++) {
            for (tokenp = (char*) tokstr; *tokenp; tokenp++) {
                if (*srcp == *tokenp) {
                    char *wordp;
                    for (wordp = (char*) word; *wordp; wordp++) *newp++ = *wordp;
                    break;
                }
            }
            if (!*tokenp) *newp++ = *srcp;
        }
        *newp = '\0';
    } else if (method == 's') { /* String replace */
        if (strlen(word) > strlen(tokstr)) maxstrlen = ((strlen(srcstr) / strlen(tokstr)) * strlen(word)) + (strlen(srcstr) % strlen(tokstr));
        else maxstrlen = strlen(srcstr);
        newstr = (char*) malloc(maxstrlen + 1);
        tokstrlen = strlen(tokstr);

        for (srcp = srcstr, newp = newstr; *srcp; srcp++) {
            if (!strncmp(srcp, tokstr, tokstrlen)) {
                char *wordp;
                for (wordp = (char*) word; *wordp; wordp++) *newp++ = *wordp;
                srcp += tokstrlen - 1;
            } else *newp++ = *srcp;
        }
        *newp = '\0';
    } else {
        //DEBUG("Unknown mode \"%s\".", mode);
        return NULL;
    }

    /* decide whether newing the memory or replacing into exist one */
    if (memuse == 'n') retp = newstr;
    else if (memuse == 'r') {
        strcpy(srcstr, newstr);
        free(newstr);
        retp = srcstr;
    } else {
        //DEBUG("Unknown mode \"%s\".", mode);
        free(newstr);
        return NULL;
    }

    return retp;
}

char *StrSplit(char *str, const char *delim) {
    char *tokensp;
    static char *tokenep;

    if (!str) {
        if (!tokenep) return NULL;
        tokensp = tokenep + strlen(delim);
    } else tokensp = str;

    tokenep = strstr(tokensp, delim);
    if (!tokenep) return tokensp;
    *tokenep = '\0';

    return tokensp;
}

inline char *appendpath(char *path, char *subdir) {
    static char fupath[(MAX_PATH + 1) * 2];

    if (!path || !subdir) return NULL;
    memset(fupath, '\0', MAX_PATH + 1);
    if (*subdir == '~') sprintf(fupath, "%s%s", path, subdir + 1);
    else sprintf(fupath, "%s%s", path, subdir);

    return fupath;
}

inline char *fullpath(char *path, char *subdir) {
    static char fupath[(MAX_PATH + 1) * 2];

    if (!path || !subdir) return NULL;
    memset(fupath, '\0', MAX_PATH + 1);
    sprintf(fupath, "%s/UserData%s", path, subdir);

    return fupath;
}

inline char *fullname(char *path, char *file) {
    static char name[(MAX_PATH + 1) * 2];

    if (!path || !file) return NULL;
    memset(name, '\0', MAX_PATH + 1);
    sprintf(name, "%s/%s", path, file);

    return name;
}

inline char *fullname_ex(char *path, char *file) {
    static char name[(MAX_PATH + 1) * 2];

    if (!path || !file) return NULL;
    memset(name, '\0', MAX_PATH + 1);
    sprintf(name, "%s%s", path, file);

    return name;
}

inline char *NextDirPath(char *path, char *subdir) {
    static char *pPos;
    static char *pStart;
    static char fupath[MAX_PATH + 1];
    char dirpath[MAX_PATH + 1];

    if (!subdir) {
        if (!pPos) return NULL;
    } else pStart = pPos = subdir;
    pPos = strchr(++pPos, '/');

    strcpy(fupath, path);
    if (!pPos) strcat(fupath, pStart);
    else {
        strncpy(dirpath, pStart, pPos - pStart);
        strcat(fupath, dirpath);
    }

    return fupath;
}

inline char *UtcTimeToString(time_t utctime) { // 2010-11-11 10:10:10
    static char timestr[TIME_STRINT_LEN];
    struct tm *utctm;

    if (!utctime) utctime = time(NULL);
    utctm = gmtime(&utctime);
    if (!strftime(timestr, TIME_STRINT_LEN, "%m-%d-%Y %H:%M:%S", utctm)) return NULL;

    return timestr;
}

inline time_t StringToUtcTime(char *timestr) { // 2010-11-11 10:10:10
    char *s = "- :";
    char *pos, *prev;
    struct tm utctm;

    //if (!strptime(timestr, "%m-%d-%Y %H:%M:%S", &tm)) return 0;
    if (!timestr) return 0;
    memset(&utctm, '\0', sizeof(struct tm));

    int count = 0;
    prev = timestr;
    do {
        pos = strpbrk(prev, s);
        if (pos) *pos = '\0';
        switch (count) {
            case 0:
                utctm.tm_mon = atoi(prev) - 1;
                break;
            case 1:
                utctm.tm_mday = atoi(prev);
                break;
            case 2:
                utctm.tm_year = atoi(prev) - 1900;
                break;
            case 3:
                utctm.tm_hour = atoi(prev);
                break;
            case 4:
                utctm.tm_min = atoi(prev);
                break;
            case 5:
                utctm.tm_sec = atoi(prev);
                break;
        }
        ++count;
        if (pos) prev = pos + 1;
    } while (pos);
    time_t calender = mktime(&utctm);
    calender -= timezone;

    return calender;
}

char *GetRandSessionNum() {
    static char rand_str[MIN_STRING_LEN];
    static unsigned int count;

    snprintf(rand_str, MIN_STRING_LEN, "%d-%d-%d", getpid(), time(NULL), ++count);

    return rand_str;
}