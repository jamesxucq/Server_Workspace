#pragma once

#include <vector>
using std::vector;

#include "TRANSSockets.h"


class CSDTPUtility {
public:
	CSDTPUtility(void);
	~CSDTPUtility(void);
protected:
	class CTRANSSessionBzl *m_pTRANSSeionBzl;
	class CTRANSSockets *m_pTRANSSocke;
	//
public:
	DWORD Create(struct SessionArgu *pSeionArgu, struct SocketsArgu *pSockeArgu);
	VOID Destroy();
	//
private:
	int GetChks(struct TaskNode *task_node);
	int Post(struct TaskNode *task_node);
	int PutChks(vector<struct local_match *> &vlocal_match, struct TaskNode *task_node, VOID *chk_sums, DWORD dwChksType);
	//
public:
#define PROTO_FAULT -1
#define PROTO_OKAY 0
// env error
#define PROTO_NETWORK 0x0001
// internal error
#define PROTO_LOCKED 0x0010
#define PROTO_CREATE 0x0020
// protocal error
#define PROTO_PROCESSING 0x0100
#define PROTO_NO_CONTENT 0x0200
#define PROTO_NOT_FOUND 0x0400
	int Initial(); // -1 fault 0 success 1 network
	int KeepAlive();
	//
public:
	int HeadList(unsigned __int64 *cleng, TCHAR *list_chks, DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory); // -1 fault 0 success 1 network 2 processing 4 not found
	int HeadFile(unsigned __int64 *cleng, FILETIME *last_write, TCHAR *szFileName); // -1 fault 0 success 1 network
	int GetAnchor(DWORD *slast_anchor); // -1 fault 0 success 1 network
	int GetList(struct ListNode *list_node); // -1 fault 0 success 1 network
	//
public:
	int GetFile(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int GetSynchron(struct TaskNode *task_node);  // -1 fault 0 success 1 network // 本地是新文件
	int PutFile(struct TaskNode *task_node);  // -1 fault 0 success 1 network
	int PutSynchron(struct TaskNode *task_node);  // -1 fault 0 success 1 network // 服务器是新文件
	//
	int MoveServer(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int CopyServer(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int DeleServFile(struct TaskNode *task_node); // -1 fault 0 success 1 network
	//
	int MoveLocal(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int CopyLocal(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int DeleLocaFile(struct TaskNode *task_node); // -1 fault 0 success 1 network
	//
public:
	int GetDire(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int PutDire(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int DeleLocaDire(struct TaskNode *task_node); // -1 fault 0 success 1 network
	int DeleServDire(struct TaskNode *task_node); // -1 fault 0 success 1 network
	//
public:
	int Final(DWORD dwFinalType); // -1 fault 0 success 1 network
	//
public:
	int GetFileVerifySha1(HANDLE hFileValid, struct TaskNode *task_node, DWORD dwCacheVerify);
	int GetFileVerifyChks(HANDLE hFileValid, struct TaskNode *task_node, DWORD dwCacheVerify);
	int GetChunkVerifyChks(HANDLE hFileValid, struct TaskNode *task_node, DWORD dwCacheVerify);
};


