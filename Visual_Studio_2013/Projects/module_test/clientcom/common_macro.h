
#ifndef _COMMON_MACRO_H_
#define	_COMMON_MACRO_H_

//////////////////////////////////////////////////////////////////
#define POW2N_MOD(NUMA, NUMB)  (NUMA & (NUMB -1)) // x=2^n a=a%x-->a=a&(x-1)

//////////////////////////////////////////////////////////////////
#define EmptyString(sz) *sz = _T('\0') 

//////////////////////////////////////////////////////////////////
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

//////////////////////////////////////////////////////////////////
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

//////////////////////////////////////////////////////////////////
#define NTFS_MBYTE (1 << 20)
#define NTFS_GBYTE (1 << 30)

//////////////////////////////////////////////////////////////////
#define DEFAULT_BUFFER					0x2000  // 8K
#define SOCKET_BUFFER					0x2000  // 8K
#define RECV_BUFF_SIZE					0x2000  // 8K
//#define SEND_BUFF_SIZE				0x2000  // 8K
#define HEAD_BUFF_LEN					1024
#define BODY_BUFF_LEN					0x2000  // 8K

#define TEXT_LENGTH						1024
#define LINE_LENGTH						256

//////////////////////////////////////////////////////////////////
#define MAX_TEXT_LEN					512
#define MID_TEXT_LEN					128
#define MIN_TEXT_LEN					32

#define URL_TEXT_LENGTH					512
#define MD5_TEXT_LENGTH					33
#define AUTH_TEXT_LEN					32
#define TIME_LENGTH						32
#define PROXY_TYPE_LEN					32
#define BOOL_TEXT_LEN					8

#define HOSTNAME_LENGTH					256
#define SERVNAME_LENGTH					256

#define PASSWORD_LENGTH					128
#define USERNAME_LENGTH					128
#define IDENTI_LENGTH                   64

#define USERAGENT_LENGTH				64
#define CONTENT_TYPE_LENGTH				32

#define KEY_LENGTH						32
#define SESSION_LENGTH					32
#define VERSION_LENGTH					32
#define DIGIT_LENGTH                    32
#define EXECUTE_LENGTH                  32
#define SUM_LENGTH						16

//////////////////////////////////////////////////////////////////
#define INVALID_FILE_INDEX ((DWORD)-1)
#define OPERATION_SUCCESS ((DWORD)-1)

//////////////////////////////////////////////////////////////////
#define BLOCK_SIZE						0x2000  // 8K
#define CHUNK_SIZE						((int)1 << 22) // 4M

//////////////////////////////////////////////////////////////////
#define NO_ROOT(FILE_PATH)				(FILE_PATH) + 2
#define ROOT_LENGTH						2


#endif	/* _COMMON_MACRO_H_ */