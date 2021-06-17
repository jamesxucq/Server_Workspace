#ifndef _LWIZARD_MACRO_H_
#define	_LWIZARD_MACRO_H_


//#define LONGIN_WIZARD
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
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

#define FINAL_STATUS_UNKNOWN			0x0000
#define FINAL_STATUS_END				0x0001
#define FINAL_STATUS_SUCCESS			0x0002
#define FINAL_STATUS_FINISH				0x0004

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define AGENT_SERVICE_NAME				_T("sdcache")
#define SERVICE_CACHE_NAME				_T("sdcache.exe")

#define APPLICATION_NAME				_T("sdclient")
#define AGENT_EXECUTE_NAME				_T("sdclinet.exe")

#define LWIZARD_APPLICATION				_T("loginwizard")
#define LWIZARD_EXECUTE_NAME				_T("loginwizard.exe")

//const ExplorerName = 'explorer.exe';
//const ExplorerProcessName = 'explorer.exe';
#define EXPLORER_EXECUTE_NAME			_T("explorer.exe")
#define EXPLORER_PROCESS_NAME			_T("explorer.exe")

#endif	/* _LWIZARD_MACRO_H_ */
