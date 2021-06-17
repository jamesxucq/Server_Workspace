#pragma once

#include "NormalSocks.h"
#include "ProxySocks.h"

//#include <Wininet.h>
#include <Sensapi.h>
#pragma comment(lib, "Sensapi.lib")


#define NONE		0x00
#define NSOCKS		0x02
#define PSOCKS		0x04


class CReplySocks
{
public:
	CReplySocks(void);
	~CReplySocks(void);
private:
	CNormalReplySocks ONormalReplySocks;
	CProxyReplySocks OProxyReplySocks; 
private:
	int NPSocksType; //0 un create, 2 normal socks, 4 proxy socks
public:
	bool Create(char *szDestnServ, int dwDestnPort, struct NetworkConf *pNetworkConf);
public:
	inline int SendReceive(char *pRecvData, char *pSendMessage)
	{
		if(NSOCKS == NPSocksType) return ONormalReplySocks.SendReceive(pRecvData, pSendMessage);
		else if(PSOCKS == NPSocksType) return OProxyReplySocks.SendReceive(pRecvData, pSendMessage);
		return -1;
	}
};
