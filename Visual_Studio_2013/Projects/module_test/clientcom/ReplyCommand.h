#pragma once
#include "ParseConfig.h"
#include "ProxySockets.h"

#define VALIDATE_BUFFER					512

#define SERV_STATUS_FAULT				-1
#define SERV_STATUS_OK					0x0000
#define SERV_STATUS_LOCKED				0x0001
#define SERV_STATUS_NOTAUTH				0x0003

class CReplyValue
{
public:
	CReplyValue(void);
	~CReplyValue(void);
private:
	struct ReplySocksArgu m_tReplySocksArgu;
public:
	DWORD InitReplyValue(struct ReplySocksArgu *pReplySocksArgu);
	DWORD FinishReplyValue();
private:
	CReplySockets *m_pReplySockets;
private:
	DWORD SetSocketsAddress(char *szServAddress, int iServPort);
public:
	int ReplySendReceive(struct SessionBuffer *psbuffer);
};
extern CReplyValue objValidate;
extern CReplyValue objCommand;

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
namespace NValidateBzl
{
	//////////////////////////////////////////////////////////////////////
	inline DWORD InitValidate(struct ReplySocksArgu *pReplySocksArgu)
	{ return objValidate.InitReplyValue(pReplySocksArgu);}
	inline DWORD FinishValidate() { return objValidate.FinishReplyValue();}
	//////////////////////////////////////////////////////////////////////
	DWORD Register(struct ValidateSession *pValSession);
	DWORD Settings(TCHAR *client_version, int *pool_size, struct ValidateSession *pValSession);
	DWORD Link(struct ValidateSession *pValSession);
	DWORD Unlink(struct ValidateSession *pValSession);
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
namespace NCommandBzl
{
	//////////////////////////////////////////////////////////////////////
	inline DWORD InitCommand(struct ReplySocksArgu *pReplySocksArgu)
	{ return objCommand.InitReplyValue(pReplySocksArgu);}
	inline DWORD FinishCommand() { return objCommand.FinishReplyValue();}
	//////////////////////////////////////////////////////////////////////
	DWORD QueryCached(UINT *cached_anchor, struct CommandSession *pComSession);
};