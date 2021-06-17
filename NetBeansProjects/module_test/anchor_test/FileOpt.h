#ifndef FILEOPT_H_
#define FILEOPT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "CommonMacro.h"
//#include "SyncMacro.h"

/**
 * Load file into memory.
 * @param filepath	file path
 * @param nbytes	has two purpost, one is to set how many bytes are readed. the other is actual the number loaded bytes will be stored. nbytes must be point 0 or NULL to read entire file.
 * @return		allocated memory pointer if successful, otherwise returns NULL.
 * @code
 *   // loading text file
 *   char *text = (char *)qFileLoad("/tmp/text.txt", NULL);
 *   // loading binary file
 *   int binlen = 0;
 *   char *bin = (char *)qFileLoad("/tmp/binary.bin", &binlen);
 *   // loading partial
 *   int binlen = 10;
 *   char *bin = (char *)qFileLoad("/tmp/binary.bin", &binlen);
 * @endcode
 * @note
 * This method actually allocates memory more than 1 bytes than filesize then append
 * NULL character at the end. For example, when the file size is 10 bytes long, 10+1
 * bytes will allocated and the last byte is always NULL character. So you can load
 * text file and use without appending NULL character. By the way, *size still will
 * be returned the actual file size of 10.
 */
void *LoadFile(const char *filepath, size_t *nbytes);

/**
 * Get file size.
 * @param filepath	file or directory path
 * @return		the file size if exists, otherwise returns -1.
 */
off_t FileSizeN(const char *filepath);
off_t FileSizeD(int fd);
off_t FileSizeP(FILE *fp);

/**
 * Check path string contains invalid characters.
 * @param path		path string
 * @return		true if ok, otherwise returns false.
 */
bool FileCheckPath(const char *path);

/**
 * Read one line from file.
 * @param fp		FILE pointer
 * @return		allocated memory pointer if successful, otherwise returns NULL.
 * @note
 * Basically it's same as fgets() but can read line without length limitation.
 */
char *FileReadLine(FILE *fp);

/**
 * Get filename from filepath
 * @param filepath	file or directory path
 * @return		malloced filename string
 */
char *FileGetName(const char *filepath);

/**
 * Get directory suffix from filepath
 * @param filepath	file or directory path
 * @return		malloced filepath string
 */
char *FileGetDir(const char *filepath);
char *FileGetDirEx(char *filedir, const char *filepath);
/**
 * Attempts to create a directory recursively.
 * @param dirpath	directory path
 * @param mode		permissions to use
 * @param recursive	whether or not to create parent directories automatically
 * @return		true if successful, otherwise returns false.
 */
bool FileMkdir(const char *dirpath, mode_t mode, bool recursive);
bool CreatFileDirPath(char *sHomePath, char *FileName, mode_t mode);

///////////////////////////////
int FileTimeN(time_t *modify_time, char *name);
int FileSizeTimeN(off_t *filesize, time_t *modify_time, char *name);
int FileSizeTimeP(off_t *filesize, time_t *modify_time, FILE *fp);

/* Determine if two time_t values are equivalent
 * @retval 0 if the times should be treated as the same
 * @retval +1 if the first is later
 * @retval -1 if the 2nd is later
 **/
//inline int cmp_time(time_t timea, time_t timeb);
FILE *BackupFile(FILE *fpm);
int FtruncateP(FILE *fp, off_t length);

int SetFileModtime(char *name, time_t modtime);
#define SetFileModtimeEx(basename, filename, modtime) \
    SetFileModtime(fullpath(basename, filename), modtime)

inline int CheckDirExists(char *dirpath);
inline int CheckFileExists(char *filepath);

int RecuDirRemove(char *dirpath);

#endif /* FILEOPT_H_ */
