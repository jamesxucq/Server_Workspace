#pragma once

#include "common_macro.h"
#include "client_macro.h"
#include "StringUtility.h"
#include "ParseConfig.h"
#include "TasksListObject.h"
#include "Session.h"

struct SessionArgu {
	////////////////////////////////////////////////
	// user info
	TCHAR			szUserName[USERNAME_LENGTH];
	TCHAR			szPassword[PASSWORD_LENGTH];
	////////////////////////////////////////////////
	//System
	TCHAR			szLocalIdenti[IDENTI_LENGTH];
	////////////////////////////////////////////////
	TCHAR			szDriveRoot[MIN_TEXT_LEN];
};

void CONVERT_SESSION_ARGU(struct SessionArgu *pSessionArgu, struct ClientConfig *pClientConfig);

/////////////////////////////////////////////////////////////////////////////////////
// CTRANSSessionBzl
class CTRANSSessionBzl {
public:
	CTRANSSessionBzl(void);
	~CTRANSSessionBzl(void);
private:
	struct TRANSSession *m_pTRANSSession;
public:
	DWORD Initialize();
	void Finalize();
public:
	struct TRANSSession *Create(struct SessionArgu *pSessionArgu);
	DWORD Destroy();
	/////////////////////////////////////////////////////////////////////////////////////
	// public:
	//	DWORD OpenInitialSe(struct XmlAnchor *pXmlAnchor);
	//	void CloseInitialSe();
public:
	DWORD OpenGetAnchorSe(struct XmlAnchor *pXmlAnchor);
	void CloseGetAnchorSe();
public:
	DWORD OpenGetListSe(DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory, void *serv_point);
	void CloseGetListSe();
public:
	DWORD OpenGetFileSe(struct TaskNode *task_node);
	void CloseGetFileSe(struct TaskNode *task_node);
public:
	DWORD OpenPutFileSe(struct TaskNode *task_node);
	void ClosePutFileSe(struct TaskNode *task_node);
public:
	DWORD OpenGetCsumSe(struct TaskNode *task_node);
	void CloseGetCsumSe();
	DWORD OpenPostSe(struct TaskNode *task_node);
	void ClosePostSe();
public:
	DWORD OpenPutSimpleCsumsSendSe(struct TaskNode *task_node, struct simple_sums *psums_simple);
	DWORD OpenPutComplexCsumsSendSe(struct TaskNode *task_node, struct complex_sums *psums_complex);
	void ClosePutCsumSendSe();
	DWORD OpenPutCsumRecvSe(struct TaskNode *task_node);
	void ClosePutCsumRecvSe(OUT vector<struct local_match *> &vlocal_match);
	//////////////////////////////////
	DWORD OpenMoveServerSe(struct TaskNode *exist_task_node, struct TaskNode *new_task_node);
	DWORD OpenDeleteServerSe(struct TaskNode *task_node);
	DWORD OpenFileVerifySe(struct TaskNode *task_node, HANDLE hFileVerify);
	DWORD OpenChunkVerifySe(struct TaskNode *task_node, HANDLE hFileVerify);
	/////////////////////////////////////////////////////////////////////////////////////
	void KillTRANSSession();
	struct TRANSSession *GetTRANSSession();
	struct ListInform *GetListInform();
	DWORD GetServerNewAncher();
};

/////////////////////////////////////////////////////////////////////////////////////
namespace NTRANSSessionBzl {
	CTRANSSessionBzl *Factory(struct SessionArgu *pSessionArgu);
	void DestroyObject(CTRANSSessionBzl *pSessionBzl);
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