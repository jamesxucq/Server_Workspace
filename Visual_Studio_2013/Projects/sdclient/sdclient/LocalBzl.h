#pragma once
#include "Resource.h"

#include "clientcom/clientcom.h"
#include "BuildTasksBzl.h"
#include "ExecuteTransmitBzl.h"
//#include "AnchorBzl.h"
#include "WatcherBzl.h"

#include "LocalObj.h"
#include "OperaExclus.h"
#include "sdTrayIcon.h"
#include "TRANSThread.h"

//
#define NLOCA_PASS_EXCLUS	objOperaExclus.PassValid()
#define NLOCA_RELEASE_EXCLUS(LAST_EXCLUS)	objOperaExclus.ReleaseExclusive(LAST_EXCLUS)
// #define GET_EXCLUSIVE	objOperaExclus.GetExclus()
//
namespace NLocalBzl {
	// return 0; succ/ return -1 error
	DWORD Initialize(CWnd* pNotifyWnd, ClientConfig *pClientConfig);
	DWORD Finalize();
	DWORD AddIconToTaskbar(CWnd* pNotifyWnd);
	//
	// client启动停止操纵
	inline BOOL GetShutdownStatus()
	{ return LOCAL_OBJECT(m_bShutdown); }
	inline VOID SetShutdownStatus(BOOL bStatus)
	{ LOCAL_OBJECT(m_bShutdown) = bStatus; }
	DWORD ShutdownApplica();
	//
	// below about system opt
	DWORD SwitchToLoginWizard();
	//
	static DWORD CheckOptionInput();
	static DWORD ConfigChangedAction();
	DWORD SaveOptionConfig();
	DWORD AccountUnlink();
	//
	VOID OpenRootDirectorise(HWND hParentHWnd);
	//
	// 连接服务器界面显示和状态设置, 同步线程界面显示和状态设置
#define RUN_INVALID_CHANGE_STEP			0x0001
#define RUN_WAITING_CHANGE_STEP			0x0003
#define RUN_OPERATE_DELAY_STEP			0x0005
#define RUN_FILE_LOCKED_STEP			0x0007 // add by james 201121010
#define RUN_TIMEOUT_CHECK_STEP			0x0009
#define RUN_OPERATI_CHECK_STEP			0x000B
#define RUN_PERFROM_SYNC_STEP			0x000D
	VOID TrayiconTransmit(DWORD dwRunStep);
#define CONNECT_ACTIVE_STEP				0x0001
#define CONNECT_NOACCOUNT_STEP			0x0003
#define CONNECT_OFFLINK_STEP			0x0005
#define CONNECT_DISCONNECT_STEP			0x0007
#define CONNECT_SERVERBUSY_STEP			0x0009
#define CONNECT_EXCEPTION_STEP			0x000B
#define CONNECT_NOTAUTH_STEP			0x000D
	VOID TrayiconConnecting(DWORD dwConnectStep);
	VOID TrayiconConnected();
	DWORD ExclusOptionsEnable();
	VOID ExclusOptionsDisable(DWORD dwPassExclus);
	//
	//	inline VOID TrayTip(LPCTSTR szMessage) { objTrayIcon.TarySetTip(szMessage); }
#define BALLOON_TIP_TRACE				0x0001
#define BALLOON_TIP_INFORM				0x0003
#define BALLOON_TIP_EXCEPTION			0x0005
#define BALLOON_TIP_WARNING				0x0007
	VOID BalloonTip(DWORD dwDisplayLevel, LPCTSTR szMessage);
	//
	VOID PausedResumeTransmitProcess();
	DWORD ResumeTransmitProcess(DWORD dwProcessStatus);
	//
	VOID StartTransmitProcess();
	VOID PerformTransmitProcess(DWORD dwNodeTotal, DWORD dwIncreValue);
	VOID TransmitLockedProcess();
	VOID FailedTransmitProcess();
	VOID ErrorTransmitProcess();
	//
	VOID PauseTransmitProcess();
	VOID PausedTransmitProcess(); // not used
	//
#define NOCHANGE_SUCCESS		0x0001
#define CHANGED_SUCCESS			0x0003
	VOID TransmitSuccessfully(DWORD dwSuccessType);
	//
	VOID KillTransmitProcess(); // not used
	VOID KilledTransmitProcess();
	//
	VOID StopTransmitProcess();
	VOID StopedTransmitProcess();
	//
	DWORD ForbidTransmitValid(TCHAR *szFileName);
	//
	// 检查本地用户空间
#define VALID_POOL_DESTROY			0x00000001
#define VALID_POOL_ADJUST			0x00000002
#define VALID_ADJUST_FAULT			0x00000004
#define VALID_LIVE_UPDATE			0x00000008
#define VALID_ACTIVE_UPDATE			0x00000010
	DWORD ValidateSettings(struct ClientConfig *pClientConfig, TCHAR *client_version, int pool_size, int data_bomb);
};

//
#define LINKER_DISPLAY_START(FILE_NAME, IS_DIRE) \
	NShellLinkBzl::DisplayFileSyncing(FILE_NAME, IS_DIRE);

#define LINKER_DISPLAY_FAILED(FILE_NAME, IS_DIRE) \
	NShellLinkBzl::DisplayFilePrevious(FILE_NAME, IS_DIRE);

#define REFRESH_EXPLORER_WINDOWS(DRIVE_LETTER) \
	ShellUtility::RefreshExplorerWindows(DRIVE_LETTER);

// it modify the file_name.
#define LINKER_DISPLAY_SUCCESS(FILE_NAME, IS_DIRE) \
	NShellLinkBzl::DisplayFileNormal(FILE_NAME, IS_DIRE);

//