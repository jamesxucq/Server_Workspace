#pragma once
#include <winsock2.h>
//#include "DefineMacro.h"
#include "tools_macro.h"
#include "ParseConf.h"
#include "StringUtility.h"


class CProxyReplySocks
{
public:
	CProxyReplySocks(void);
	~CProxyReplySocks(void);
private:
	char m_szDestiServ[SERV_NAME];
	int m_nDestiPort;
	struct NetworkConf *m_pNetworkConf;
	SOCKET ConnectSocket;
public:
	bool Create(char *szDestnServ, int dwDestnPort, struct NetworkConf *pNetworkConf);
	int Connect();
	int Close();
private:
	int ProxyAuth();
private:
	int Send(char *szMessage);
	int Receive(char *pRecvData);
public:
	int SendReceive(char *pRecvData, char *pSendMessage);
};

namespace NDetectProxy
{
	DWORD GetIEPorxyServ(TCHAR *szProxyServ, int *nProxyPort); //return 0 succ; -1 err;
};
#define MESSAGE_BUFFER					512
#define HEAD_BUFFER						512


//使用到的结构 
struct Sock4Req
{ 
	char VN; 
	char CD; 
	unsigned short Port; 
	unsigned long IPAddr; 
	char other; 
}; 

struct Sock4Ans
{ 
	char VN; 
	char CD; 
}; 

struct Sock5Req1
{ 
	char Ver; 
	char nMethods; 
	char Method1; 
	char Method2; 
}; 

struct Sock5Ans1
{ 
	char Ver; 
	char Method; 
}; 

struct Sock5Req2
{ 
	char Ver; 
	char Cmd; 
	char Rsv; 
	char Atyp; 
	char other; 
}; 

struct Sock5Ans2
{ 
	char Ver; 
	char Rep; 
	char Rsv; 
	char Atyp; 
	char other; 
}; 

struct AuthReq 
{ 
	char Ver; 
	char *Ulen; 
	char *Name; 
	char *PLen; 
	char *Pass; 
}; 

struct AuthAns 
{ 
	char Ver; 
	char Status; 
}; 