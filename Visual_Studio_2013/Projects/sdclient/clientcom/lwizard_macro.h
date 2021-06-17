#ifndef _LWIZARD_MACRO_H_
#define	_LWIZARD_MACRO_H_


// #define LONGIN_WIZARD
//
// all // Path // Anchor
#define GLIST_INVALID                0x0000
#define GLIST_RECU_FILES             0x0001
#define GLIST_LIST_DIREC             0x0002
#define GLIST_RECU_DIRES             0x0004
#define GLIST_ANCH_FILES             0x0008

#define INVALID_CHKS            0x0000
#define SIMPLE_CHKS				0x0001
#define COMPLEX_CHKS			0x0002
#define WHOLE_CHKS				0x0004

#define FINAL_STATUS_INVALID			0x0000
#define FINAL_STATUS_END				0x0001
#define FINAL_STATUS_SUCCESS			0x0002
#define FINAL_STATUS_FINISH				0x0004

//
#define APPLICATION_NAME				_T("sdclient")
#define CLIENT_EXECUTE_NAME				_T("sdclient.exe")

#define LWIZARD_APPLICATION				_T("logiwiza")
#define LWIZARD_EXECUTE_NAME			_T("logiwiza.exe")

#define UPDATE_APPLICATION				_T("liveupda")
#define UPDATE_EXECUTE_NAME				_T("liveupda.exe")

#define BUILD_CACHE						_T("cache")
#define UPDATE_DIRECTORY				_T("updates")

// const ExplorerName = 'explorer.exe';
// const ExplorerProcessName = 'explorer.exe';
#define EXPLORER_EXECUTE_NAME			_T("explorer.exe")
#define EXPLORER_PROCESS_NAME			_T("explorer.exe")

#endif	/* _LWIZARD_MACRO_H_ */
