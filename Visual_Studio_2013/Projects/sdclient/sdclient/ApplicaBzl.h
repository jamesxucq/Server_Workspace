#pragma once

#include "clientcom/clientcom.h"
#include "ShellLinkBzl.h"
#include "LocalBzl.h"

class CApplicaInit {
public:
	CApplicaInit(void);
	~CApplicaInit(void);
private:
	DWORD m_dwApplicaInitFlags;
public:
	DWORD ApplicaInitialize(CWnd* pNotifyWnd);
	DWORD ApplicaFinalize();
};

extern class CApplicaInit objApplicaInit;


#define INIT_APPLICA_FLAGS(FLAGS)				FLAGS = 0;

#define CLIENT_INIT_OKAY(FLAGS)					FLAGS |= 0x00000001
#define ANCH_INIT_OKAY(FLAGS)					FLAGS |= 0x00000002
#define RIVER_INIT_OKAY(FLAGS)					FLAGS |= 0x00000004
#define DWATCH_INIT_OKAY(FLAGS)					FLAGS |= 0x00000008
#define WORKER_INIT_OKAY(FLAGS)					FLAGS |= 0x00000100
#define SYSLOG_INIT_OKAY(FLAGS)					FLAGS |= 0x00010000
#define LINKER_INIT_OKAY(FLAGS)					FLAGS |= 0x00020000
#define PCONFIG_INIT_OKAY(FLAGS)				FLAGS |= 0x00040000
//#define TRAYICON_INIT_OKAY(FLAGS)				FLAGS |= 0x00020000

#define CLIENT_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFFE
#define ANCH_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFD
#define RIVER_FINAL_OKAY(FLAGS)					FLAGS &= 0xFFFFFFFB
#define DWATCH_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFFF7
#define WORKER_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFFFEFF
#define SYSLOG_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFEFFFF
#define LINKER_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFDFFFF
#define PCONFIG_FINAL_OKAY(FLAGS)				FLAGS &= 0xFFFBFFFF
//#define TRAYICON_FINAL_OKAY(FLAGS)			FLAGS &= 0xFFFDFFFF

#define CLIENT_CHECK_INIT(FLAGS)				(FLAGS & 0x00000001)
#define ANCH_CHECK_INIT(FLAGS)					(FLAGS & 0x00000002)
#define RIVER_CHECK_INIT(FLAGS)					(FLAGS & 0x00000004)
#define DWATCH_CHECK_INIT(FLAGS)				(FLAGS & 0x00000008)
#define WORKER_CHECK_INIT(FLAGS)				(FLAGS & 0x00000100)
#define SYSLOG_CHECK_INIT(FLAGS)				(FLAGS & 0x00010000)
#define LINKER_CHECK_INIT(FLAGS)				(FLAGS & 0x00020000)
#define PCONFIG_CHECK_INIT(FLAGS)				(FLAGS & 0x00040000)
//#define TRAYICON_CHECK_INIT(FLAGS)			(FLAGS & 0x00020000)


//
namespace NApplicaBzl {
	static DWORD SingleInstance(LPCTSTR szAppName);
	//
	inline DWORD ApplicaInitialize(CWnd* pNotifyWnd) { return objApplicaInit.ApplicaInitialize(pNotifyWnd); }
	inline DWORD ApplicaFinalize() { return objApplicaInit.ApplicaFinalize(); } 
	//
	static VOID EraseUserspace(TCHAR *szLocation, TCHAR *szAnchorInde, TCHAR *szAnchorData, TCHAR *szFileConfli);
	static DWORD ValidateUserspaceIntegrity(TCHAR *szLocation, TCHAR *szUserPool, DWORD dwPoolSize, TCHAR *szFileForbid);
	static DWORD ValidateMounted(DWORD dwPoolSize, TCHAR *szDriveLetter);
	static DWORD ValidUserSettings(struct LocalConfig *pLocalConfig);
	// system
#define EXECUTE_FAULT						-1
#define EXECUTE_SUCCESS						0x00000000
	// fatal error
#define EXECUTE_ONE_INSTANCE				0x00000001
#define EXECUTE_CONFIGURATION				0x00000002
#define EXECUTE_NOACCOUNT					0x00000004
	// exception
#define EXECUTE_USER_SPACE					0x00000008
#define EXECUTE_PASSWD_PROTECT				0x00000010
#define EXECUTE_DRIVE_MOUNTED				0x00000020
#define EXECUTE_LETTER_OCCPUY				0x00000040
#define EXECUTE_LIVE_UPDATE					0x00000080
#define EXECUTE_ACTIVE_UPDATE				0x00000100
	DWORD CheckEnviron(TCHAR *szDriveLetter);
	//
	DWORD RunLiveUpdate();
	DWORD RunLoginWizard();
	DWORD ShiftDriveLetter();
};



