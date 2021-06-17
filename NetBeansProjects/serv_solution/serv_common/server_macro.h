#ifndef _SERVER_MACRO_H_
#define	_SERVER_MACRO_H_

#ifdef	__cplusplus
extern "C" {
#endif
//
#define GANCH_INVA                      0x0000
#define GANCH_LAST_ANCH                 0x0001

// all // Path // Anchor
#define GLIST_INVA                      0x0000
#define GLIST_RECU_FILES                0x0001
#define GLIST_LIST_DIREC                0x0002
#define GLIST_RECU_DIRES                0x0003
#define GLIST_ANCH_FILES                0x0004

#define RANGE_TYPE_INVA                 0x0000 // invalid
#define RANGE_TYPE_FILE                 0x0001 // file
#define RANGE_TYPE_CHUNK                0x0002 // chunk
#define RANGE_TYPE_BLOCK                0x0003 // block 

#define INVA_CHKS               0x0000
#define SIMPLE_CHKS             0x0001
#define COMPLEX_CHKS            0x0002
#define WHHLE_CHKS		0x0003

#define INVA_CACHE              0x0000
#define UPDATE_CACHE		0x0001
#define RIVER_CACHED		0x0002
    
#define FINAL_TYPE_INVA               0x0000
#define FINAL_TYPE_END                0x0001
#define FINAL_TYPE_SUCCESS            0x0002
#define FINAL_TYPE_FINISH             0x0003

#define CONTENT_TYPE_INVA               0x0000
#define CONTENT_TYPE_XML                0x0001
#define CONTENT_TYPE_OCTET              0x0002
#define CONTENT_TYPE_PLAIN              0x0003
// text/xml\r\n  // application/octet-stream text/plain

//
#define DEFAULT_HOST_PORT 8888
#define DEFAULT_CONTENT_LENGTH (100 * 1024) /* bytes */
#define DEFAULT_KEEP_ALIVE 5 /* seconds */
#define DEFAULT_MAX_CONNECTION_AGE 300 /* seconds */
#define BUG_REPORT_EMAIL "james.xucq@gmail.com"

#define SYNC_SERV_DEFAULT "sync_serv.xml"
#define SSEV_LOCK_DEFAULT "sync_serv.lock"
#define SSEV_PID_DEFAULT "sync_serv.pid"
#define SSEV_LOG_DEFAULT "sync_serv.log"
#define SSEV_ACCESS_DEFAULT "sync_access.log"
#define ANCH_INDE_DEFAULT "~/anchor_inde.ndx"
#define ANCH_DATA_DEFAULT "~/anchor_data.acd"
#define SSEV_USER_DEFAULT "nobody"
#define SSEV_GROUP_DEFAULT "nobody"

#define VERSION 	"1.0"
#define COPYRIGHT 	"Copyright (C) 2012 Pu Shan"
#define DISCLAIMER      "This is not free software.  "
#define FUNCTION	"sync server" 
#define USAGE		"Usage: %s [options] <input>\n"
#define PROGRAM_NAME	"sync_serv_t"

//
#define LOOP_STOP                       0x00000000 // stop
#define LOOP_CONTINUE                   0x00000001 // continue
#define HAND_FINISH                     LOOP_STOP // stop
#define HAND_UNDONE                     LOOP_CONTINUE // continue
// #define HAND_EXCEPT                     -1 // exception
//
#define SEND_SUCCESS                    0x0000ffff
#define SEND_CONTINUE                   0x00010000
//
    
#ifdef	__cplusplus
}
#endif
    
#endif	/* _SERVER_MACRO_H_ */

