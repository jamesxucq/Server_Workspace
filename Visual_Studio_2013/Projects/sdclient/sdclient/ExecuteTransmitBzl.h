#pragma once
#include "clientcom/clientcom.h"
#include "TRANSThread.h"
#include "TRANSObject.h"
#include "HandleTasksBzl.h"
#include "TRANSWorker.h"

#include "kchashdb.h"
// #include "dbz_t.h"

using namespace std;
using namespace kyotocabinet;

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default

namespace NExecTransBzl {
	static int LockedActionInitial(const TCHAR *szDriveLetter, const TCHAR *szLocation, DWORD dwActionCache);
	// return 0; succ/ return -1 error
	DWORD Initialize(struct LocalConfig *pLocalConfig);
	DWORD Finalize();
	//
	static VOID ValidUserSettings(struct LocalConfig *pLocalConfig);
	CWinThread *GetTRANSThread();
	DWORD SetTRANSThread(CWinThread *pTRANSThread);
	//
	inline DWORD GetServAnchor()
	{ DWORD dwLastServAnchor; InterlockedExchange((LONG*)&dwLastServAnchor, TRANS_OBJECT(m_dwLastServAnchor)); return dwLastServAnchor; }
	//
	inline DWORD GetTransmitProcessStatus()
	{ DWORD dwProcessStatus; InterlockedExchange((LONG*)&dwProcessStatus, TRANS_OBJECT(m_dwCurrentStatus)); return dwProcessStatus; }
	inline VOID SetTransmitProcessStatus(DWORD dwProcessStatus)
	{ InterlockedExchange((LONG*)&TRANS_OBJECT(m_dwCurrentStatus), dwProcessStatus); }
//
	inline DWORD GetTransmitNextStatus()
	{ DWORD dwNextStatus; InterlockedExchange((LONG*)&dwNextStatus, TRANS_OBJECT(m_dwNextStatus)); return dwNextStatus; }
	inline VOID SetTransmitNextStatus(DWORD dwNextStatus)
	{ InterlockedExchange((LONG*)&TRANS_OBJECT(m_dwNextStatus), dwNextStatus); }
	//
	inline DWORD GetConnectedStatus()
	{ DWORD dwConnected; InterlockedExchange((LONG*)&dwConnected, TRANS_OBJECT(m_dwConnected)); return dwConnected; }
	inline VOID SetConnectedStatus(DWORD dwConnected)
	{ InterlockedExchange((LONG*)&TRANS_OBJECT(m_dwConnected), dwConnected); }
	//
#define REGIST_RETRY_DELAY	3000
#define REGIST_RETRY_TIMES	3
	VOID RegisterRetry(struct LocalConfig *pLocalConfig);
	//
	DWORD PerfromSynchronTransport(struct LocalConfig *pLocalConfig);
	DWORD ValidConnectStatus(struct LocalConfig *pLocalConfig, TCHAR *szAccountLinked);
	DWORD CheckUserPoolChanged(struct LocalConfig *pLocalConfig);
	DWORD GetSlowFilesDbBzl(TreeDB *lattdb, TCHAR *szDriveLetter); // 0:ok 0x01:error 0x02:processing 0x03:not_found
	DWORD GetFastFilesVecBzl(FilesVec *lattent, UINT dwLocalAnchor, const TCHAR *szDriveLetter);
	//
	VOID StopTRANSSession();
	VOID KillTRANSSession();
	//
	inline HANDLE GetWaitingEvent() { return TRANS_OBJECT(m_hWaitingEvent); }
	inline VOID SetWaitingEvent() { SetEvent(TRANS_OBJECT(m_hWaitingEvent)); }
	//
	int PrepareLockedAction(const TCHAR *szDriveLetter, const TCHAR *szLocation);
};

