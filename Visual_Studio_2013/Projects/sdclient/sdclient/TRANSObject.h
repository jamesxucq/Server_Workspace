#pragma once
#include "clientcom/clientcom.h"

//
#define LOCAL_LOCK_ACTION				0x0001
#define LOCK_ACTION_TIMEOUT				0x0002
//
#define PROCESS_INITIAL_WAITING			3000	// 3s
#define PROCESS_NORMAL_WAITING			45000	// 45s
#define CONNECT_ERROR_WAITING			180000	// 180s
#define PROCESS_FAILED_WAITING			15000	// 15s

//
// check connect status code
#define CONNECT_STATUS_FAULT				-1
#define CONNECT_STATUS_OKAY					0x0000
#define CONNECT_STATUS_NOACCOUNT			0x0001
#define CONNECT_STATUS_OFFLINK				0x0002
#define CONNECT_STATUS_DISCONN				0x0004
// #define CONNECT_STATUS_SERVERBUSY		0x0008
// #define CONNECT_STATUS_EXCEPTION			0x0010
// #define CONNECT_STATUS_WAITING			0x0008
// #define CONNECT_STATUS_STOPPED			0x0010

//
// check sync changed code
#define CHANGE_STATUS_FAULT					-1
#define CHANGE_STATUS_CHANGED				0x0000
#define CHANGE_STATUS_LOCKED				0x0001
#define CHANGE_STATUS_DISCONNECT			0x0002
#define CHANGE_STATUS_NOCHANGE				0x0004
#define CHANGE_STATUS_NOTAUTH				0x0008

//
// syncing status code
#define PROCESS_STATUS_FAULT			-1
#define PROCESS_STATUS_RUNNING			0x0000
#define PROCESS_STATUS_WAITING			0x0001
#define PROCESS_STATUS_INITIAL			PROCESS_STATUS_WAITING
#define PROCESS_STATUS_PAUSED			0x0002
#define PROCESS_STATUS_FORCESTOP		0x0004
#define PROCESS_STATUS_KILLED			0x0008
// #define PROCESS_STATUS_SUCCESS			0x0010
// #define PROCESS_STATUS_FAILED			0x0020


class CTRANSObject {
public:
	CTRANSObject(void);
	~CTRANSObject(void);
public:
	DWORD Initialize();
	DWORD Finalize();
public:
	CWinThread *m_pTRANSThread;
	volatile DWORD m_dwLastServAnchor;
public:
	volatile DWORD m_dwCurrentStatus; // 0 stop /0x0001 running /0x0002 pause /0x0004 exit syncing ststus
	volatile DWORD m_dwNextStatus; // 0 stop /0x0001 running /0x0002 pause /0x0004 exit  syncing ststus
	volatile DWORD m_dwConnected;
public:
	volatile HANDLE m_hWaitingEvent; // ÊÂ¼þ¾ä±ú
private:
	volatile DWORD m_dwWorkerThreadsCount;
	HANDLE *m_hWorkerThreads;
	CRITICAL_SECTION m_csWorkerThreads;
public:
	DWORD SetWorkerThreads(HANDLE *hWorkerThreads, DWORD dwThreadsCount);
	HANDLE *GetWorkerThreads(DWORD *pWorkerThreadsCount);
	//
};

//
extern CTRANSObject objTRANSObject;
#define TRANS_OBJECT(member) objTRANSObject.member
#define FAULT_RETURN	return WORKER_VALUE_FAULT;
//