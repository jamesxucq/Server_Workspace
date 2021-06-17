#ifndef MACRO_DEFINE_H_
#define MACRO_DEFINE_H_

#include <limits.h>

//enum BOOL {FALSE = 0,TRUE = !FALSE};
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

typedef char BOOL;
typedef char bool;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;

#ifndef OFF_T
#define OFF_T off_t
#endif

#ifndef int32
#define int32 int
#endif

#ifndef int64
#define int64 long
#endif

#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef uchar
#define uchar unsigned char
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifndef IN
#define IN
#endif

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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

#ifndef PATHLEN
#define PATHLEN 128
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define MAX_STRING_LEN		512
#define MID_STRING_LEN		128
#define MIN_STRING_LEN		32

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifndef EVD_SOCKET_BUFF
#define EVD_SOCKET_BUFF                 0x2000  // 8K
#endif

#define DATSIZ                          1024
#define DEFAULT_BUFFER			0x2000  // 8K
#define SOCKET_BUFF_SIZE                0x2000  // 8K

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define BLOCK_SIZE                      0x2000  // 8K
#define CHUNK_SIZE                      ((int32)1 << 22)
#define MAX_BLOCK_SIZE                  ((int32)1 << 17)
#define MAX_MAP_SIZE                    (256*1024)

#define TIME_STRINT_LEN                  20
#define TEXT_BUFF_SIZE                   512
#define LINE_BUFF_SIZE                   128

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define HANDLE_END                       0x00 // stop
#define HANDLE_CONTINUE                   0x01 // continue

#endif /* MACRO_DEFINE_H_ */
