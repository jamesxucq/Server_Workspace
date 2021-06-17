#pragma once

#include <vector>
using std::vector;

#include "TRANSSockets.h"
// #include "FilesVec.h"


class CSDTPUtility {
public:
	CSDTPUtility(void);
	~CSDTPUtility(void);
protected:
	class CTRANSSessionBzl *m_pTRANSSessionBzl;
	class CTRANSSockets *m_pTRANSSockets;
	//////////////////////////////////////////////////////////////////////
public:
	DWORD Create(struct SessionArgu *pSessionArgu, struct SocketsArgu *pSocketsArgu);
	void Destroy();
	//////////////////////////////////////////////////////////////////////
private:
	DWORD GetCsum(struct TaskNode *task_node);
	DWORD Post(struct TaskNode *task_node);
	DWORD PutCsum(OUT vector<struct local_match *> &vlocal_match, struct TaskNode *task_node, void *psums, DWORD dwCsumType);
	//////////////////////////////////////////////////////////////////////
public:
#define INIT_FAILED			0x00000000
#define INIT_SUCCESS		0x00000001
	DWORD Initial(); // -1 error; 0 succ; 1 failed
	//////////////////////////////////////////////////////////////////////
	DWORD GetAnchor(DWORD *last_anchor); // -1 error; 0 succ; 1 failed
	DWORD GetList(void *serv_point, DWORD dwListType, DWORD dwAnchor, TCHAR *szDirectory);
	DWORD GetFile(struct TaskNode *task_node);
	DWORD Getsync(struct TaskNode *task_node); // 本地是新文件
	//////////////////////////////////////////////////////////////////////
	DWORD PutFile(struct TaskNode *task_node); 
	DWORD Putsync(struct TaskNode *task_node); // 服务器是新文件
	//////////////////////////////////////////////////////////////////////
	DWORD MoveServer(struct TaskNode *exist_task_node, struct TaskNode *new_task_node);
	DWORD DeleteServer(struct TaskNode *task_node);
	//////////////////////////////////////////////////////////////////////
	DWORD MoveLocal(struct TaskNode *exist_task_node, struct TaskNode *new_task_node);
	DWORD DeleteLocal(struct TaskNode *task_node);
	//////////////////////////////////////////////////////////////////////
	DWORD Final(DWORD dwFinalType);
	//////////////////////////////////////////////////////////////////////
public:
	DWORD GetFileVerifyCsums(HANDLE hFileVerify, struct TaskNode *task_node);
	DWORD GetChunkVerifyCsums(HANDLE hFileVerify, struct TaskNode *task_node);
};

