#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common_macro.h"
#include "string_utility.h"

char *strreplace(const char *mode, char *srcstr, const char *tokstr, const char *word) {
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

    /* decide whether newing the memory or replacing into exists one */
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

char *strsplit(char *str, const char *delim) {
    char *tokensp;
    static char *tokenep;

    if (!str) {
        if (!tokenep) return NULL;
        tokensp = tokenep + strlen(delim);
    } else tokensp = str;

    tokenep = strstr(tokensp, delim);
    if (tokenep) *tokenep = '\0';

    return tokensp;
}

char *split_value(char *str) {
    char *tokensp, *tokenep;

    tokensp = strchr(str, ' ');
    if (tokensp) *tokensp = '\0';

    ++tokensp;
    tokenep = strrchr(tokensp, ' ');
    if (tokenep) *tokenep = '\0';

    return tokensp;
}

char *split_line(char *str) {
    char *tokensp;

    tokensp = strstr(str, ": ");
    if (tokensp) *tokensp = '\0';

    return tokensp + 2;
}

char *name_value(char **name, char *str, const char delim) {
    char *tokensp;
    static char *tokenep;

    if (!str) {
        if (!tokenep) return NULL;
        tokensp = tokenep + 1;
    } else tokensp = str;

    tokenep = strchr(tokensp, delim);
    if (tokenep) *tokenep = '\0';

    if (name) *name = tokensp;
    char *value = strchr(tokensp, '=');
    if (value) {
        *value = '\0';
        ++value;
    } else value = tokensp;

    return value;
}

inline char *append_path(char *path, char *subdir) {
    static char fupath[MAX_PATH * 3];
    sprintf(fupath, "%s%s", path, subdir + 1);
    return fupath;
}

inline char *full_path(char *path, char *subdir) {
    static char fupath[MAX_PATH * 3];
    sprintf(fupath, "%s/user_pool%s", path, subdir);
    return fupath;
}

inline char *full_name(char *location, char *path, char *file) {
    static char name[MAX_PATH * 3];
    sprintf(name, "%s/user_pool%s/%s", location, path, file);
    return name;
}

inline char *full_name_ex(char *location, char *path, char *file) {
    static char name[MAX_PATH * 3];
    sprintf(name, "%s/user_pool%s%s", location, path, file);
    return name;
}

inline char *last_backup(char *path, char *subdir, time_t bak_time) {
    static char bakpath[MAX_PATH * 3];
    sprintf(bakpath, "%s/backup%s_%d", path, strrchr(subdir, '/'), bak_time);
    return bakpath;
}

inline char *next_dirpath(char *path, char *subdir) {
    static char *posi;
    static char *tokensp;
    static char fupath[MAX_PATH * 3];
    char dirpath[MAX_PATH * 3];

    if (!subdir) {
        if (!posi) return NULL;
    } else tokensp = posi = subdir;
    posi = strchr(++posi, '/');

    strcpy(fupath, path);
    if (!posi) strcat(fupath, tokensp);
    else {
        strncpy(dirpath, tokensp, posi - tokensp);
        strcat(fupath, dirpath);
    }

    return fupath;
}

inline char *utc_time_text(time_t utctime) { // Wed, 15 Nov 1995 04:58:08 GMT
    static char timestr[TIME_LENGTH];
    struct tm *utctm;

    if (!utctime) utctime = time(NULL);
    utctm = gmtime(&utctime);
    if (!strftime(timestr, TIME_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", utctm)) return NULL;

    return timestr;
}

inline time_t utc_time_value(char *timestr) { // Wed, 15 Nov 1995 04:58:08 GMT
    struct tm utctm;

    if (!timestr) return 0;
    memset(&utctm, '\0', sizeof (struct tm));
    if (!strptime(timestr, "%a, %d %b %Y %H:%M:%S GMT", &utctm)) return 0;
    time_t calender = mktime(&utctm);
    // calender -= timezone;

    return calender;
}

char *random_seion_id() {
    static char rand_str[MIN_TEXT_LEN];
    static unsigned int count;
    snprintf(rand_str, MIN_TEXT_LEN, "%d-%d-%d", getpid(), time(NULL), ++count);
    return rand_str;
}

char *random_key() {
    static char rand_str[MIN_TEXT_LEN];
    static long int last_seed;
    char code_array[62] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

    if (!last_seed) last_seed = time(NULL);
    int count;
    for (count = 0; count < 16; count++) {
        srandom((unsigned int) last_seed);
        last_seed = random();
        rand_str[count] = code_array[last_seed % 62];
    }
    rand_str[count] = '\0';

    return rand_str;
}

int address_split(serv_addr *saddr, char * addr_txt) {
    char *token = NULL;

    if (!saddr || !addr_txt) return -1;
    memset(saddr, '\0', sizeof (serv_addr));

    token = strchr(addr_txt, ':');
    if (token) {
        strncpy(saddr->sin_addr, addr_txt, token - addr_txt);
        saddr->sin_port = atoi(++token);
    } else strcpy(saddr->sin_addr, addr_txt);

    return 0;
}
