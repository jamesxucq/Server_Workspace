#ifndef _TOOLS_MACRO_H_
#define	_TOOLS_MACRO_H_

//////////////////////////////////////////////////////////////////
#define RETURN_ERROR					-1

//////////////////////////////////////////////////////////////////
#define DEFAULT_BUFFER					0x2000  // 8K
#define SOCKET_BUFF_SIZE				0x2000  // 8K
#define RECV_BUFF_SIZE					0x2000  // 8K
//#define SEND_BUF_SIZE					0x2000  // 8K
#define HEAD_BUFF_LEN					1024
#define BODY_BUFF_LEN					0x2000  // 8K

#define TEXT_BUFF_SIZE			512
#define LINE_BUFF_SIZE			128

//////////////////////////////////////////////////////////////////
#define MAX_STRING_LEN			512
#define MID_STRING_LEN			128
#define MIN_STRING_LEN			32

#define HOST_NAME				256
#define SERV_NAME				256
#define PROXY_TYPE				32

//////////////////////////////////////////////////////////////////
#define BOOL_STRING_LEN			8
#define PASSWORD_LENGTH			32
#define USERNAME_LENGTH			32

#define SERVNAME_LENGTH			256
#define HOSTNAME_LENGTH			256

//////////////////////////////////////////////////////////////////
#define ADD_TYPE_ADMIN			0x0001
#define ADD_TYPE_WORKER			0x0002

#define CLEAR_TYPE_ALLUSRS      0x0001
#define CLEAR_TYPE_SINGLE		0x0002
#define CLEAR_TYPE_ADMIN	    0x0004
#define CLEAR_TYPE_WORKER	    0x0008

#define LIST_TYPE_ADMIN			0x0001
#define LIST_TYPE_WORKER	    0x0002

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define CONTENT_TYPE_UNKNOWN            0x0000
#define CONTENT_TYPE_XML                0x0001
#define CONTENT_TYPE_OCTET              0x0002
#define CONTENT_TYPE_PLAIN              0x0004

#endif	/* _TOOLS_MACRO_H_ */
