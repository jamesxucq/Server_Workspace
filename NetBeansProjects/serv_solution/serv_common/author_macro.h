#ifndef _AUTHOR_MACRO_H_
#define	_AUTHOR_MACRO_H_

#ifdef	__cplusplus
extern "C" {
#endif

//
//
#define CONTENT_TYPE_INVALID               0x0000
#define CONTENT_TYPE_XML                0x0001
#define CONTENT_TYPE_OCTET              0x0002
#define CONTENT_TYPE_PLAIN              0x0003
// text/xml\r\n  // application/octet-stream text/plain

//
//
#define ADDI_TYPE_INVALID                0x0000
#define ADDI_TYPE_SERVER                 0x0001
#define ADDI_TYPE_ADMIN                  0x0002

#define CLEAR_TYPE_INVALID                 0x0000
#define CLEAR_TYPE_ALLUSRS              0x0001
#define CLEAR_TYPE_SINGLEUSR            0x0002
#define CLEAR_TYPE_SERVER               0x0003
#define CLEAR_TYPE_ADMIN                0x0004

#define LIST_TYPE_INVALID                  0x0000
#define LIST_TYPE_SERVER                0x0001
#define LIST_TYPE_ADMIN                 0x0002

#define EXECUTE_EXCEPTION               0x0000
#define EXECUTE_SUCCESS                 0x0001

//
//
#define DEFAULT_HOST_PORT 8888
#define DEFAULT_CONTENT_LENGTH (100 * 1024) /* bytes */
#define DEFAULT_KEEP_ALIVE 5 /* seconds */
#define DEFAULT_MAX_CONNECTION_AGE 300 /* seconds */
#define BUG_REPORT_EMAIL "james.xucq@gmail.com"

#define AUTH_SERV_DEFAULT "auth_serv.xml"
#define ASEV_LOCK_DEFAULT "auth_serv.lock"
#define ASEV_PID_DEFAULT "auth_serv.pid"
#define ASEV_LOG_DEFAULT "auth_serv.log"
#define ASEV_ACCESS_DEFAULT "auth_access.log"
#define ASEV_USER_DEFAULT "nobody"
#define ASEV_GROUP_DEFAULT "nobody"
    
#define VERSION 	"1.0"
#define COPYRIGHT 	"Copyright (C) 2012 Pu Shan"
#define DISCLAIMER      "This is not free software.  "
#define FUNCTION	"authorize server" 
#define USAGE		"Usage: %s [options] <input>\n"
#define PROGRAM_NAME	"author_serv"

//
//#define LOOP_STOP                       0x00000000 // stop
//#define LOOP_CONTINUE                   0x00000001 // continue

#ifdef	__cplusplus
}
#endif
    
#endif	/* _AUTHOR_MACRO_H_ */

