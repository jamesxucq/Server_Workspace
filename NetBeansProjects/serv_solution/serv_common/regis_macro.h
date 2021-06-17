#ifndef _REGIS_MACRO_H_
#define	_REGIS_MACRO_H_

#ifdef	__cplusplus
extern "C" {
#endif

//
#define CONTENT_TYPE_INVALID               0x0000
#define CONTENT_TYPE_XML                0x0001
#define CONTENT_TYPE_OCTET              0x0002
#define CONTENT_TYPE_PLAIN              0x0003
// text/xml\r\n  // application/octet-stream text/plain

//
//
#define DEFAULT_HOST_PORT 8888
#define DEFAULT_CONTENT_LENGTH (100 * 1024) /* bytes */
#define DEFAULT_KEEP_ALIVE 5 /* seconds */
#define DEFAULT_MAX_CONNECTION_AGE 300 /* seconds */
#define BUG_REPORT_EMAIL "james.xucq@gmail.com"

#define RIGIS_SERV_DEFAULT "regis_serv.xml"
#define RSEV_LOCK_DEFAULT "regis_serv.lock"
#define RSEV_PID_DEFAULT "regis_serv.pid"
#define RSEV_LOG_DEFAULT "regis_serv.log"
#define RSEV_ACCESS_DEFAULT "regis_access.log"
#define RSEV_USER_DEFAULT "nobody"
#define RSEV_GROUP_DEFAULT "nobody"

#define VERSION 	"1.0"
#define COPYRIGHT 	"Copyright (C) 2012 Pu Shan"
#define DISCLAIMER      "This is not free software.  "
#define FUNCTION	"register server" 
#define USAGE		"Usage: %s [options] <input>\n"
#define PROGRAM_NAME	"regis_serv"

//
#define LOOP_STOP                       0x00000000 // stop
#define LOOP_CONTINUE                   0x00000001 // continue

#ifdef	__cplusplus
}
#endif
    
#endif	/* _REGIS_MACRO_H_ */

