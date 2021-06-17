#ifndef _CLIENT_MACRO_H_
#define	_CLIENT_MACRO_H_


//#define SD_APPLICATION
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define GANCH_UNKNOWN                   0x0000
#define GANCH_LAST_ANCH                 0x0001

//all //Path //Anchor
#define GLIST_UNKNOWN                0x0000
#define GLIST_RECU_FILES             0x0001
#define GLIST_LIST_FILES             0x0002
#define GLIST_RECU_DIRES             0x0004
#define GLIST_ANCH_FILES             0x0008

#define RANGE_TYPE_UNKNOWN              0x0000 // unknown
#define RANGE_TYPE_FILE                 0x0001 // file
#define RANGE_TYPE_CHUNK                0x0002 // chunk
#define RANGE_TYPE_BLOCK                0x0004 // block 

#define UNKNOWN_CSUM            0x0000
#define SIMPLE_CSUM				0x0001
#define COMPLEX_CSUM			0x0002

#define FINAL_STATUS_UNKNOWN           0x0000
#define FINAL_STATUS_END               0x0001
#define FINAL_STATUS_SUCCESS           0x0002
#define FINAL_STATUS_FINISH			   0x0004

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define CONTENT_TYPE_UNKNOWN            0x0000
#define CONTENT_TYPE_XML                0x0001
#define CONTENT_TYPE_OCTET              0x0002
#define CONTENT_TYPE_PLAIN              0x0004
// text/xml\r\n  // application/octet-stream text/plain

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define AGENT_SERVICE_NAME				_T("sdcache")
#define SERVICE_CACHE_NAME				_T("sdcache.exe")

#define APPLICATION_NAME				_T("sdclient")
#define AGENT_EXECUTE_NAME				_T("sdclinet.exe")

#define LWIZARD_APPLICATION					_T("loginwizard")
#define LWIZARD_EXECUTE_NAME				_T("loginwizard.exe")

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// #define NO_ROOT(FILE_PATH)			(FILE_PATH) + 2


#endif	/* _CLIENT_MACRO_H_ */
