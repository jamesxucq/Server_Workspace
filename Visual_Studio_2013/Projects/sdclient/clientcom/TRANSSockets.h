#pragma once

#include "ParseConfig.h"
#include "common_macro.h"

class CTRANSSockets {
public:
	CTRANSSockets(void);
	~CTRANSSockets(void);
public:
	virtual DWORD Create(struct SocketsArgu *pSockeArgu) = 0;
private:
	virtual DWORD SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend) = 0;	
public:
	virtual DWORD Connect(char *address, int port) = 0;
	virtual VOID Close() = 0;
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
	virtual DWORD Create(struct ReplyArgu *pReplyArgu) = 0;
	virtual DWORD SetAddress(char *address, int port) = 0;
private:
	virtual DWORD Connect(char *address, int port) = 0;
	virtual VOID Close() = 0;
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
	//
	DWORD InitialWinsock();
	VOID FinishWinsock();
	DWORD SWD_IsNetworkAlive(); // 0 connected ; 1 no; // add by james 20090910
	//
	CTRANSSockets *Factory(struct SocketsArgu *pSockeArgu);
	VOID DestroyObject(CTRANSSockets *pTransSocke);
	//
	CReplySockets *Factory(struct ReplyArgu *pReplyArgu);
	VOID DestroyObject(CReplySockets *pReplySocke);
};

//
#define ONE_SOCKETS_THREAD	1
#define FIVE_SOCKETS_THREAD 5

enum PROXY_TYPE {
	PROXY_TYPE_HTTP = 1,
	PROXY_TYPE_HTTPS,
	PROXY_TYPE_SOCKS4,
	PROXY_TYPE_SOCKS4a,
	PROXY_TYPE_SOCKS5
};

struct SocketsArgu {
	unsigned int 	dwWorkerThreads;
	serv_address	tServAddress;
	// network
	DWORD			dwDowndLimit;// 0 false 1 true
	DWORD			dwDowndRate;
	DWORD			dwUploadLimit;// 0 false 1 true 3 auto
	DWORD			dwUploadRate;
	// proxies
	DWORD			dwProxyEnable;// 0 false 1 true -1 auto
	enum PROXY_TYPE	dwType;// HTTP SOCKS4 SOCKS5
	serv_address	tAddress;
	DWORD			dwProxyAuth;// 0 false 1 true 3 auto
	char			szUserName[USERNAME_LENGTH];
	char			szPassword[PASSWORD_LENGTH];
};
VOID CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSockeArgu, struct NetworkConfig *pNetworkConfig);
inline VOID SET_SOCKETS_THREADS(struct SocketsArgu *pSockeArgu, DWORD dwWorkerThreads) 
{ pSockeArgu->dwWorkerThreads = dwWorkerThreads; }
// inline unsigned int GET_SOCKETS_THREADS(struct SocketsArgu *pSockeArgu) { return pSockeArgu->dwWorkerThreads; }
VOID CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSockeArgu, struct ListData *ldata);

struct ReplyArgu {
	// Auth info // Sync Server info
	serv_address	tReplyAddress;
	//
	// Proxies
	DWORD			dwProxyEnable;// 0 false 1 true -1 auto
	enum PROXY_TYPE	dwType;// HTTP SOCKS4 SOCKS5
	serv_address	tAddress;
	DWORD			dwProxyAuth;// 0 false 1 true 3 auto
	char			szUserName[USERNAME_LENGTH];
	char			szPassword[PASSWORD_LENGTH];
};
VOID CONVERT_VALSOCKS_ARGU(struct ReplyArgu *pReplyArgu, struct LocalConfig *pLocalConfig);
VOID CONVERT_AUTHSOCKS_ARGU(struct ReplyArgu *pReplyArgu, struct ListData *ldata, struct NetworkConfig *pNetworkConfig);