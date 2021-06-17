#ifndef _CLIENT_MACRO_H_
#define	_CLIENT_MACRO_H_


//#define SWD_APPLICATION
//
#define HLIST_INVA                   0x0000
#define HLIST_RECU_FILES             0x0001
#define HLIST_LIST_DIREC             0x0002
#define HLIST_RECU_DIRES             0x0004
#define HLIST_ANCH_FILES             0x0008

// all // Path // Anchor
#define GANCH_INVA                      0x0000
#define GANCH_LAST_ANCH                 0x0001

// all // Path // Anchor
#define GLIST_INVA                   0x0000
#define GLIST_RECU_FILES             0x0001
#define GLIST_LIST_DIREC             0x0002
#define GLIST_RECU_DIRES             0x0004
#define GLIST_ANCH_FILES             0x0008

//
#define INVA_CHKS               0x0000
#define SIMPLE_CHKS				0x0001
#define COMPLEX_CHKS			0x0002
#define WHOLE_CHKS				0x0004

#define INVA_CACHE              0x0000
#define VERIFY_CACHE			0x0001
#define RIVER_CACHED			0x0002

#define FINAL_STATUS_INVA           0x0000
#define FINAL_STATUS_END            0x0001
#define FINAL_STATUS_SUCCESS        0x0002
#define FINAL_STATUS_FINISH			0x0004

//
#define CONTENT_TYPE_INVA            0x0000
#define CONTENT_TYPE_XML             0x0001
#define CONTENT_TYPE_OCTET           0x0002
#define CONTENT_TYPE_PLAIN           0x0004
// text/xml\r\n  // application/octet-stream text/plain

//
#define APPLICATION_NAME				_T("sdclient")
#define CLIENT_EXECUTE_NAME				_T("sdclient.exe")

#define LWIZARD_APPLICATION				_T("logiwiza")
#define LWIZARD_EXECUTE_NAME			_T("logiwiza.exe")

#define UPDATE_APPLICATION				_T("liveupda")
#define UPDATE_EXECUTE_NAME				_T("liveupda.exe")

#define ACTIVE_PATH_NAME				_T("updates//active-update.xml")

#define COMPANY_HOME_LINK				_T("www.pushan.com")
//
// #define NO_LETT(FILE_PATH)			(FILE_PATH) + 2

//
#define SYNP_VALUE_FAULT		-1
#define SYNP_VALUE_RUNNING		0x00000000
#define SYNP_VALUE_WAITING		0x00000001
#define SYNP_VALUE_PAUSED		0x00000002
#define SYNP_VALUE_FORCESTOP	0x00000004
#define SYNP_VALUE_FINISH		0x00000008
#define SYNP_VALUE_FAILED		0x00000010
#define SYNP_VALUE_KILLED		0x00000020
#define SYNP_VALUE_NETWORK		0x00000040
#define SYNP_VALUE_LOCKED		0x00000080
#define SYNP_VALUE_EXCEPTION	0x00010000
#define SYNP_VALUE_UDEFA		0x01000000 // user define
#define SYNP_VALUE_UDEFB 		0x02000000 // user define

#endif	/* _CLIENT_MACRO_H_ */
