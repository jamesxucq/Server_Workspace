#pragma once

//#include <Wininet.h>
#include <Sensapi.h>
#pragma comment(lib, "Sensapi.lib")

#include "ParseConfig.h"
#include "CommonMacro.h"

#define ONE_SOCKETS_THREAD	1
struct SocketsArgu {
	unsigned int 	nSyncingThreads;
////////////////////////////
	wchar_t			sServHost[HOST_NAME_LEN];
	DWORD 			iServPort;
	//Network
	wchar_t			sDownloadLimit[BOOL_STRING_LEN];//0 false 1 true
	DWORD			iDownloadRate;
	wchar_t			sUploadLimit[BOOL_STRING_LEN];//0 false 1 true 3 auto
	DWORD			iUploadRate;
	//Proxies
	wchar_t			sProxyStatus[BOOL_STRING_LEN];//0 false 1 true -1 auto
	wchar_t			sType[PROXY_TYPE];//HTTP SOCKS4 SOCKS5
	wchar_t			sServer[SERV_NAME_LEN];//
	DWORD			iPort;//
	wchar_t			sProxyAuth[BOOL_STRING_LEN];//0 false 1 true 3 auto
	wchar_t			sUserName[USERNAME_LENGTH];
	wchar_t			sPassword[PASSWORD_LENGTH];
};

void CONVERT_SOCKETS(struct SocketsArgu *pSocketsArgu, struct LocalConf *pLocalConf, unsigned int nSyncingThreads);

class CSdSocketsBzl
{
public:
	CSdSocketsBzl(void);
	~CSdSocketsBzl(void);
public:
	virtual int Create(struct SocketsArgu *pSocketsArgu) = 0;
	virtual int SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend) = 0;	
public:
	virtual int Connect(char *sServName, int iPort) = 0;
	virtual int Close() = 0;
public:
	virtual int Send(char *szMessage, size_t ilen) = 0;
	virtual int Receive(char *pRecvData, int ilen) = 0;
	virtual int SendReceive(char *pRecvData, char *pSendMessage) = 0;
public:
	virtual int SafeSend(char *szMessage, size_t ilen) = 0;
	virtual int SafeReceive(char *pRecvData, int ilen) = 0;
	virtual int SafeSendReceive(char *pRecvData, char *pSendMessage) = 0;
};


struct MsgSocketsArgu {
	//Auth info //Sync Server info
	char			sRegisHost[HOST_NAME_LEN];
	DWORD 			iRegisPort;
////////////////////////////
	char			sAuthHost[HOST_NAME_LEN];
	DWORD 			iAuthPort;
	//Network
	wchar_t			sDownloadLimit[BOOL_STRING_LEN];//0 false 1 true
	DWORD			iDownloadRate;
	wchar_t			sUploadLimit[BOOL_STRING_LEN];//0 false 1 true 3 auto
	DWORD			iUploadRate;
	//Proxies
	wchar_t			sProxyStatus[BOOL_STRING_LEN];//0 false 1 true -1 auto
	wchar_t			sType[PROXY_TYPE];//HTTP SOCKS4 SOCKS5
	char			sServer[SERV_NAME_LEN];//
	DWORD			iPort;//
	wchar_t			sProxyAuth[BOOL_STRING_LEN];//0 false 1 true 3 auto
	char			sUserName[USERNAME_LENGTH];
	char			sPassword[PASSWORD_LENGTH];
};

void CONVERT_MSG_SOCKETS(struct MsgSocketsArgu *pMsgSocketsArgu, struct LocalConf *pLocalConf);

class CMsgSocketsBzl
{
public:
	CMsgSocketsBzl(void);
	~CMsgSocketsBzl(void);
public:
	virtual int Create(struct MsgSocketsArgu *pMsgSocketsArgu) = 0;
	virtual int SetAddress(char *sServName, int iPort) = 0;
private:
	virtual int Connect(char *sServName, int iPort) = 0;
	virtual int Close() = 0;
private:
	virtual int Send(char *szMessage) = 0;
	virtual int Receive(char *pRecvData) = 0;
public:
	virtual int SendReceive(char *pRecvData, char *pSendMessage) = 0;
private:
	virtual int SafeSend(char *szMessage) = 0;
	virtual int SafeReceive(char *pRecvData) = 0;
public:
	virtual int SafeSendReceive(char *pRecvData, char *pSendMessage) = 0;
};

namespace NSdSocketsBzl
{
////////////////////////////////////////////////////////////////////////////////////
	int InitialWinsock();
	int FinishWinsock();
	DWORD SD_IsNetworkAlive(); //0 connected ; 1 no; //add by james 20090910
////////////////////////////////////////////////////////////////////////////////////
	CSdSocketsBzl *Factory(struct SocketsArgu *pSocketsArgu);
	void DestroyObject(CSdSocketsBzl *pCSdSockets);
////////////////////////////////////////////////////////////////////////////////////
	CMsgSocketsBzl *Factory(struct MsgSocketsArgu *pMsgSocketsArgu);
	void DestroyObject(CMsgSocketsBzl *pMessageSockets);
};
