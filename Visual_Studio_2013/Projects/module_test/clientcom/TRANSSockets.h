#pragma once

//#include <Wininet.h>
#include <Sensapi.h>
#pragma comment(lib, "Sensapi.lib")

#include "ParseConfig.h"
#include "common_macro.h"

class CTRANSSockets {
public:
	CTRANSSockets(void);
	~CTRANSSockets(void);
public:
	virtual DWORD Create(struct SocketsArgu *pSocketsArgu) = 0;
private:
	virtual DWORD SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend) = 0;	
public:
	virtual DWORD Connect(char *szDestiServ, int iDestiPort) = 0;
	virtual DWORD Close() = 0;
public:
	virtual DWORD Send(char *szMessage, size_t length) = 0;
	virtual DWORD Receive(char *szReceive, int length) = 0;
	virtual DWORD SendReceive(char *szReceive, char *pSendMessage) = 0;
public:
	virtual DWORD SafeSend(char *szMessage, size_t length) = 0;
	virtual DWORD SafeReceive(char *szReceive, int length) = 0;
	virtual DWORD SafeSendReceive(char *szReceive, char *pSendMessage) = 0;
};

class CReplySockets {
public:
	CReplySockets(void);
	~CReplySockets(void);
public:
	virtual DWORD Create(struct ReplySocksArgu *pReplySocksArgu) = 0;
	virtual DWORD SetAddress(char *szDestiServ, int iDestiPort) = 0;
private:
	virtual DWORD Connect(char *szDestiServ, int iDestiPort) = 0;
	virtual DWORD Close() = 0;
private:
	virtual DWORD Send(char *szMessage) = 0;
	virtual DWORD Receive(char *szReceive) = 0;
public:
	virtual DWORD SendReceive(char *szReceive, char *pSendMessage) = 0;
private:
	virtual DWORD SafeSend(char *szMessage) = 0;
	virtual DWORD SafeReceive(char *szReceive) = 0;
public:
	virtual DWORD SafeSendReceive(char *szReceive, char *pSendMessage) = 0;
};

namespace NTRANSSockets {
	////////////////////////////////////////////////////////////////////////////////////
	DWORD InitialWinsock();
	DWORD FinishWinsock();
	DWORD SD_IsNetworkAlive(); //0 connected ; 1 no; //add by james 20090910
	////////////////////////////////////////////////////////////////////////////////////
	CTRANSSockets *Factory(struct SocketsArgu *pSocketsArgu);
	void DestroyObject(CTRANSSockets *pSdSockets);
	////////////////////////////////////////////////////////////////////////////////////
	CReplySockets *Factory(struct ReplySocksArgu *pReplySocksArgu);
	void DestroyObject(CReplySockets *pMessageSockets);
};


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
#define ONE_SOCKETS_THREAD	1
enum PROXY_TYPE {
	PROXY_TYPE_HTTP = 1,
	PROXY_TYPE_HTTPS,
	PROXY_TYPE_SOCKS4,
	PROXY_TYPE_SOCKS4a,
	PROXY_TYPE_SOCKS5
};

struct SocketsArgu {
	unsigned int 	dwWorkerThreads;
	////////////////////////////
	serv_address	ServAddress;
	//Network
	DWORD			dwDownloadLimit;//0 false 1 true
	DWORD			dwDownloadRate;
	DWORD			dwUploadLimit;//0 false 1 true 3 auto
	DWORD			dwUploadRate;
	//Proxies
	DWORD			dwProxyEnable;//0 false 1 true -1 auto
	enum PROXY_TYPE	dwType;//HTTP SOCKS4 SOCKS5
	serv_address	Address;
	DWORD			dwProxyAuth;//0 false 1 true 3 auto
	char			szUserName[USERNAME_LENGTH];
	char			szPassword[PASSWORD_LENGTH];
};
void CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSocketsArgu, struct NetworkConfig *pNetworkConfig, unsigned int dwWorkerThreads);
void CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSocketsArgu, struct ListInform *pListInform);

struct ReplySocksArgu {
	//Auth info //Sync Server info
	serv_address	ReplyAddress;
	////////////////////////////
	//Proxies
	DWORD			dwProxyEnable;//0 false 1 true -1 auto
	enum PROXY_TYPE	dwType;//HTTP SOCKS4 SOCKS5
	serv_address	Address;
	DWORD			dwProxyAuth;//0 false 1 true 3 auto
	char			szUserName[USERNAME_LENGTH];
	char			szPassword[PASSWORD_LENGTH];
};
void CONVERT_VALSOCKS_ARGU(struct ReplySocksArgu *pReplySocksArgu, struct LocalConfig *pLocalConfig);
void CONVERT_AUTHSOCKS_ARGU(struct ReplySocksArgu *pReplySocksArgu, struct ListInform *pListInform, struct NetworkConfig *pNetworkConfig);