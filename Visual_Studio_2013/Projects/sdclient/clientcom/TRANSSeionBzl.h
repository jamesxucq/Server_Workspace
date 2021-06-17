#pragma once

#include "common_macro.h"
#include "client_macro.h"
#include "StringUtility.h"
#include "ParseConfig.h"
#include "TaskListObj.h"
#include "Session.h"

//
struct SessionArgu {
	// user info
	TCHAR			szUserName[USERNAME_LENGTH];
	TCHAR			szPassword[PASSWORD_LENGTH];
	// system
	TCHAR			szLocalIdenti[IDENTI_LENGTH];
	//
	TCHAR			szDriveLetter[MIN_TEXT_LEN];
};

VOID CONVERT_SESSION_ARGU(struct SessionArgu *pSeionArgu, struct ClientConfig *pClientConfig);
//
// CTRANSSessionBzl
class CTRANSSessionBzl {
public:
	CTRANSSessionBzl(void);
	~CTRANSSessionBzl(void);
private:
	struct TRANSSession *m_pTRANSSeion;
public:
	DWORD Initialize();
	VOID Finalize();
public:
	struct TRANSSession *Create(struct SessionArgu *pSeionArgu);
	DWORD Destroy();
//
#define OPEN_FAULT			((DWORD)-1)
#define OPEN_SUCCESS		0x00
#define OPEN_NOT_FOUND		0x01
#define OPEN_VIOLATION		0x02
	//
public:
	DWORD OpenHeadListSe(DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory);
	VOID CloseHeadListSe();
public:
	DWORD OpenHeadFileSe(TCHAR *szFileName);
	VOID CloseHeadFileSe();
public:
	DWORD OpenGetAnchorSe(struct XmlAnchor *pXmlAnchor);
	VOID CloseGetAnchorSe();
public:
	DWORD OpenGetListSe(struct ListNode *list_nod);
	VOID CloseGetListSe();
public:
	DWORD OpenGetFileSe(struct TaskNode *task_nod);
	VOID CloseGetFileSe();
public:
	DWORD OpenPutFileSe(struct TaskNode *task_nod);
	VOID ClosePutFileSe(struct TaskNode *task_nod);
public:
	DWORD OpenPutDireSe(struct TaskNode *task_nod);
	VOID ClosePutDireSe(struct TaskNode *task_nod);
public:
	DWORD OpenGetChksSe(struct TaskNode *task_nod);
	VOID CloseGetChksSe();
	DWORD OpenPostSe(struct TaskNode *task_nod);
	VOID ClosePostSe();
public:
	DWORD OpenPutSimpChksSendSe(struct TaskNode *task_nod, struct simple_head *phead_simple);
	DWORD OpenPutCompChksSendSe(struct TaskNode *task_nod, struct complex_head *phead_complex);
	VOID ClosePutChksSendSe();
	DWORD OpenPutChksRecvSe(struct TaskNode *task_nod);
	VOID ClosePutChksRecvSe(vector<struct local_match *> &vlocal_match);
	//
	DWORD OpenMoveServSe(struct TaskNode *task_nod);
	DWORD OpenCopyServSe(struct TaskNode *task_nod);
	DWORD OpenDeleServSe(struct TaskNode *task_nod);
	//
	DWORD OpenSha1VerifySe(struct TaskNode *task_nod, HANDLE hFileValid, DWORD dwCacheVerify);
	VOID CloseSha1VerifySe();
	DWORD OpenFileVerifySe(struct TaskNode *task_nod, HANDLE hFileValid, DWORD dwCacheVerify);
	VOID CloseFileVerifySe();
	DWORD OpenChunkVerifySe(struct TaskNode *task_nod, HANDLE hFileValid, DWORD dwCacheVerify);
	VOID CloseChunkVerifySe();
	//
	VOID KillTRANSSession();
	struct TRANSSession *GetTRANSSession();
	struct ListData *GetListData();
	DWORD GetNewAncher();
};

//
namespace NTRANSSessionBzl {
	CTRANSSessionBzl *Factory(struct SessionArgu *pSeionArgu);
	VOID DestroyObject(CTRANSSessionBzl *pSeionBzl);
}

/*
get:		lock,	share delete write read
put: 		lock,  	share delete write read
putsync:	lock,	share delete write read
getsync:	lock,	share delete write read
// server delete:	
// server move:
local delete:	no operation
local move:	no operation
*/