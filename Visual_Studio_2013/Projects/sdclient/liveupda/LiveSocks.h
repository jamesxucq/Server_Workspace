#pragma once

#include "clientcom/lupdatecom.h"

//
struct LiveArgu {
	// Proxies
	DWORD			dwProxyEnable; // 0 false 1 true -1 auto
	enum PROXY_TYPE	dwType; // HTTP SOCKS4 SOCKS5
	serv_address	tAddress;
	DWORD			dwProxyAuth; // 0 false 1 true 3 auto
	char			szUserName[USERNAME_LENGTH];
	char			szPassword[PASSWORD_LENGTH];
};
VOID CONVERT_LIVESOCKS_ARGU(struct LiveArgu *pLiveArgu, struct NetworkConfig *pNetworkConfig);

//
class CLiveSocks
{
public:
	CLiveSocks(void);
	~CLiveSocks(void);
public:
	virtual DWORD Create(struct LiveArgu *pLiveArgu) = 0;
public:
	virtual DWORD Connect(char *address, int port) = 0;
	virtual VOID Close() = 0;
public:
	virtual DWORD Send(char *szMessage, size_t length) = 0;
	virtual DWORD Receive(char *szReceive, int length) = 0;
};

//
namespace NLiveSocks {
	//
	DWORD InitialWinsock();
	VOID FinishWinsock();
	DWORD SWD_IsNetworkAlive(); //0 connected ; 1 no; // add by james 20090910
	//
	CLiveSocks *Factory(struct LiveArgu *pLiveArgu);
	VOID DestroyObject(CLiveSocks *pLiveSocks);
}

//
class CLiveProxy : public CLiveSocks {
public:
	CLiveProxy(void);
	~CLiveProxy(void);
private:
	char m_address[SERVNAME_LENGTH];
	int m_port;
	struct LiveArgu m_tLiveArgu;
private:
	SOCKET ConnectSocket;
public:
	DWORD Create(struct LiveArgu *pLiveArgu);
private:
	SOCKET ProxyConnect(char *proxy_serv, int proxy_port);
	DWORD ServConnect(SOCKET ProxySocket, enum PROXY_TYPE dwProxyType, char *address, int port, int szProxyAuth, char *user, char *pword);
public:
	DWORD Connect(char *address, int port);
	VOID Close();
public:
	DWORD Send(char *szMessage, size_t length);
	DWORD Receive(char *szReceive, int length);
};

//
class CLiveDirect : public CLiveSocks {
public:
	CLiveDirect(void);
	~CLiveDirect(void);
private:
	char m_address[SERVNAME_LENGTH];
	int m_port;
	// struct LiveArgu m_tLiveArgu;
private:
	SOCKET ConnectSocket;
public:
	DWORD Create(struct LiveArgu *pLiveArgu);
private:
	DWORD Connect(char *address, int port);
	VOID Close();
private:
	DWORD Send(char *szMessage, size_t length);
	DWORD Receive(char *szReceive, int length);
};