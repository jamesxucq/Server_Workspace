#pragma once

#include "tools_macro.h"
#include "SDTProtocol.h"
#include "ParseConf.h"
#include "ReplySocks.h"
#include "ProxySocks.h"

class CCommand
{
public:
	CCommand(void);
	~CCommand(void);
private:
	struct NetworkConf *m_pNetworkConf;
	struct Session *m_pSession;
public:
	bool Create(struct NetworkConf *pNetworkConf, struct Session *pSession);
	inline bool Destroy() {return true;}
private:
	CReplySocks m_ReplySockets;
private:
	int SetReplySocks(char *szDestnServ, int dwDestnPort);
	int ReplySendReceive(struct SessionBuffer *psbuffer);
	//////////////////////////////////////////////////////
public:
	DWORD OptionAdd(DWORD dwAddType, char *szAddress, struct CacheParam *pCacheParam);
	DWORD OptionClear(DWORD dwClearType, char *szAddress, struct CacheParam *pCacheParam);
	DWORD OptionList(char *szListXml, DWORD dwListType, struct CacheParam *pCacheParam);
};


extern CCommand OCommand;