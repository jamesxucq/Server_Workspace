#pragma once
#include "ParseConfig.h"
#include "ProxySockets.h"

#define VALIDATE_BUFFER					512

#define WORKER_STATUS_FAULT					-1
#define WORKER_STATUS_OKAY					0x0000
#define WORKER_STATUS_LOCKED				0x0001
#define WORKER_STATUS_NOTAUTH				0x0003
#define WORKER_STATUS_NOTCONN				0x0005

class CReplyValue {
public:
	CReplyValue(void);
	~CReplyValue(void);
private:
	struct ReplyArgu m_tReplyArgu;
public:
	DWORD InitReplyValue(struct ReplyArgu *pReplyArgu);
	DWORD FinishReplyValue();
private:
	CReplySockets *m_pReplySocke;
public:
	int ReplySendReceive(struct SeionBuffer *sbuffer);
};
extern CReplyValue objValid;
extern CReplyValue objComand;

//
namespace NValidBzl {
	DWORD InitValid(struct ReplyArgu *pReplyArgu);
	DWORD FinishValid();
	//
	DWORD Register(struct ValidSeion *pValSeion);
	DWORD Settings(TCHAR *client_version, int *pool_size, int *data_bomb, struct ValidSeion *pValSeion);
	DWORD Link(struct ValidSeion *pValSeion);
	DWORD Unlink(struct ValidSeion *pValSeion);
};

//
namespace NComandBzl {
	DWORD InitComand(struct ReplyArgu *pReplyArgu);
	DWORD FinishComand();
	//
	DWORD QueryCached(UINT *cached_anchor, struct ComandSeion *pCmdSeion);
};