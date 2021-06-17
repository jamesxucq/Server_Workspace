#ifndef COMMON_MACRO_H_
#define COMMON_MACRO_H_

#include <limits.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef unsigned char BYTE;

#ifndef OFF_T
#define OFF_T off_t
#endif

#ifndef int32
#define int32 int
#endif

#ifndef int64
#define int64 long long
#endif

#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef uint64
#define uint64 unsigned long long
#endif

#ifndef uchar
#define uchar unsigned char
#endif

////////////////////////////////////////////////////////////////////////////////
#define POW2N_MOD(NUA, NUB)  (NUA & (NUB -1)) // x=2^n a=b%x-->a=b&(x-1)

#define FCLOSE_SAFE(FPTR) if(FPTR) fclose(FPTR);
#define DELETE_SAFE(MPTR) if(MPTR) free(MPTR);

#define MKZERO(TEXT) TEXT[0] = '\0'

////////////////////////////////////////////////////////////////////////////////
#ifndef OUT
#define OUT
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef LDECLTYPE
#define LDECLTYPE(x) __typeof(x)
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef MAX_HOST_LEN
#define MAX_HOST_LEN 256
#endif

#ifndef PATH_LEN
#define PATH_LEN 128
#endif

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 1024
#endif

////////////////////////////////////////////////////////////////////////////////

#ifndef EVD_SOCKET_BUFF
#define EVD_SOCKET_BUFF                 0x2000  // 8K
#endif

#define AUTH_SOCKS_DATSIZ               1024
#define DEFAULT_BUFFER			0x2000  // 8K
#define SOCKET_BUFF_SIZE                0x2000  // 8K

////////////////////////////////////////////////////////////////////////////////
#define BLOCK_SIZE                      0x2000  // 8K
#define CHUNK_SIZE                      ((int32)1 << 22)
#define MAX_BLOCK_SIZE                  ((int32)1 << 17)
#define MAX_MAP_SIZE                    (256*1024)

////////////////////////////////////////////////////////////////////////////////
#define XML_LENGTH                    0x2000  // 8K
#define LINE_LENGTH                   1024

#define AUTH_TEXT_LENGTH                   0x2000  // 8K
#define AUTH_LINE_LENGTH                   512
#define REGIST_LINE_LENGTH                 128
#define LINKED_LINE_LEN                    256
#define SERV_LINE_LENGTH                   1024

//////////////////////////////////////////////////////////////////
#define MAX_TEXT_LEN		512
#define MID_TEXT_LEN		128
#define MIN_TEXT_LEN		64

#define LARGE_TEXT_SIZE         2048
#define TIME_LENGTH             32

////////////////////////////////////////////////////////////////////////////////
#define CONTENT_TYPE_LENGTH		32
#define AUTHOR_LENGTH                   128
#define PASSWORD_LENGTH			128
#define SALT_LENGTH			32
#define _USERNAME_LENGTH_               128
#define DATABASE_LENGTH			128
#define KEY_LENGTH			32
#define VALID_LENGTH                    32
#define SESSION_LENGTH                  32
#define SERVNAME_LENGTH			256
#define VERSION_LENGTH			32

#define IDENTI_LENGTH                   64
#define DIGIT_LENGTH                    32
#define EXECUTE_LENGTH                  32

////////////////////////////////////////////////////////////////////////////////
#define		ERR_SUCCESS                     0
#define		ERR_FAULT			-1
#define		ERR_EXCEPTION                   1   

#endif /* COMMON_MACRO_H_ */
