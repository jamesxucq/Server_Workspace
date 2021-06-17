#ifndef STROPT_H_
#define STROPT_H_


#include <time.h>

/**
 * Replace string or tokens as word from source string with given mode.
 *
 * @param mode		replacing mode
 * @param srcstr	source string
 * @param tokstr	token or string
 * @param word		target word to be replaced
 *
 * @return		a pointer of malloced or source string depending on the mode if rsuccessful, otherewise returns NULL
 *
 * @note
 * The mode argument has two separated character. First character
 * is used to decide replacing method and can be 't' or 's'.
 * The character 't' and 's' stand on [t]oken and [s]tring.
 *
 * When 't' is given each character of the token string(third argument)
 * will be compared with source string individually. If matched one
 * is found. the character will be replaced with given work.
 *
 * If 's' is given instead of 't'. Token string will be analyzed
 * only one chunk word. So the replacement will be occured when
 * the case of whole word matched.
 *
 * Second character is used to decide returning memory type and
 * can be 'n' or 'r' which are stand on [n]ew and [r]eplace.
 *
 * When 'n' is given the result will be placed into new array so
 * you should free the return string after using. Instead of this,
 * you can also use 'r' character to modify source string directly.
 * In this case, given source string should have enough space. Be
 * sure that untouchable value can not be used for source string.
 *
 * So there are four associatable modes such like below.
 *
 * Type "tn" : [t]oken replacing & putting the result into [n]ew array.
 * Type "tr" : [t]oken replacing & [r]eplace source string directly.
 * Type "sn" : [s]tring replacing & putting the result into [n]ew array.
 * Type "sr" : [s]tring replacing & [r]eplace source string directly.
 *
 * @code
 *   char srcstr[256], *retstr;
 *   char mode[4][2+1] = {"tn", "tr", "sn", "sr"};
 *
 *   for(i = 0; i < 4; i++) {
 *     strcpy(srcstr, "Welcome to the qDecoder project.");
 *     LOG_INFO("before %s : srcstr = %s\n", mode[i], srcstr);
 *
 *     retstr = qStrReplace(mode[i], srcstr, "the", "_");
 *     LOG_INFO("after  %s : srcstr = %s\n", mode[i], srcstr);
 *     LOG_INFO("            retstr = %s\n\n", retstr);
 *     if(mode[i][1] == 'n') free(retstr);
 *   }
 *
 *   --[Result]--
 *   before tn : srcstr = Welcome to the qDecoder project.
 *   after  tn : srcstr = Welcome to the qDecoder project.
 *               retstr = W_lcom_ _o ___ qD_cod_r proj_c_.
 *
 *   before tr : srcstr = Welcome to the qDecoder project.
 *   after  tr : srcstr = W_lcom_ _o ___ qD_cod_r proj_c_.
 *               retstr = W_lcom_ _o ___ qD_cod_r proj_c_.
 *
 *   before sn : srcstr = Welcome to the qDecoder project.
 *   after  sn : srcstr = Welcome to the qDecoder project.
 *               retstr = Welcome to _ qDecoder project.
 *
 *   before sr : srcstr = Welcome to the qDecoder project.
 *   after  sr : srcstr = Welcome to _ qDecoder project.
 *               retstr = Welcome to _ qDecoder project.
 * @endcode
 */
char *StrReplace(const char *mode, char *srcstr, const char *tokstr, const char *word);

char *StrSplit(char *str, const char *delim);

inline char *appendpath(char *path, char *subdir);
inline char *fullpath(char *path, char *subdir);
inline char *fullname(char *path, char *file);
inline char *fullname_ex(char *path, char *file);

inline char *NextDirPath(char *path, char *subdir);

inline char *UtcTimeToString(time_t utctime);
inline time_t StringToUtcTime(char *timestr);

char *GetRandSessionNum();

#endif /* STROPT_H_ */
