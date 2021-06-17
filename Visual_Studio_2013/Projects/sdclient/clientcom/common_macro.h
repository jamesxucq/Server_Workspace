
#ifndef _COMMON_MACRO_H_
#define	_COMMON_MACRO_H_

//
#define POW2N_MOD(nua, nub)  (nua & (nub -1)) // x=2^n a=a%x-->a=a&(x-1)

//
#define mkzeo(txt) txt[0] = '\0'
#define MKZEO(TEXT) TEXT[0] = _T('\0')
// #define ISZEO(TEXT) _T('\0') == TEXT[0]
// #define NOZEO(TEXT) _T('\0') != TEXT[0]
#define s_strlen(str) (sizeof(str) - 1)

//
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

//
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 260
#endif

#ifndef PATH_MAX
#define PATH_MAX 260
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

//
#define NTFS_MBYTE (1 << 20)
#define NTFS_GBYTE (1 << 30)

//
#define LOAD_BUFF_SIZE					0x4000  // 16K
#define DEFAULT_BUFFER					0x2000  // 8K
#define SOCKET_BUFFER					0x2000  // 8K
#define RECV_BUFF_SIZE					0x2000  // 8K
//#define SEND_BUFF_SIZE				0x2000  // 8K
#define HEAD_BUFF_LEN					1024
#define BODY_BUFF_LEN					0x2000  // 8K

#define TEXT_LENGTH						2048
#define LINE_LENGTH						1024

//
#define MAX_TEXT_LEN					512
#define MID_TEXT_LEN					128
#define MIN_TEXT_LEN					64

#define URI_LENGTH						512
#define MD5_TEXT_LENGTH					33
#define AUTH_TEXT_LEN					32
#define TIME_LENGTH						32
#define PROXY_TYPE_LEN					32
#define BOOL_TEXT_LEN					8

#define HOSTNAME_LENGTH					260
#define SERVNAME_LENGTH					260
#define SALT_TEXT_LEN					64

#define PASSWORD_LENGTH					128
#define USERNAME_LENGTH					128
#define IDENTI_LENGTH                   64

#define USERAGENT_LENGTH				64
#define CONTENT_TYPE_LENGTH				32
#define CACHE_VERIFY_LENGTH				32

#define VALID_LENGTH					32
#define KEY_LENGTH						32
#define SESSION_LENGTH					32
#define VERSION_LENGTH					32
#define DIGIT_LENGTH                    32
#define EXECUTE_LENGTH                  32

#define MD5_DIGEST_LEN					16
#define SHA1_DIGEST_LEN					20
#define MAX_CHKS_LEN					20

//
#define INVA_FILE_INDE			((DWORD)-1)
#define OPERATION_SUCCESS		((DWORD)-1)
#define INVA_INDE_VALU			((ULONG)-1)

//
#define BLOCK_SIZE						0x2000  // 8K
#define CHUNK_SIZE						((int)1 << 22) // 4M

//
#define NO_LETT(FILE_PATH)				(FILE_PATH) + 2
#define LETT_LENGTH						2
#define DRIVE_LETTE(FILE_PATH, LETTE)	FILE_PATH[0] = LETTE[0]; FILE_PATH[1] = LETTE[1];


#endif	/* _COMMON_MACRO_H_ */