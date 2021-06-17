#pragma once
#include <winsock2.h>

// #include "ParseConfig.h"
#include "TRANSSockets.h"
#include "common_macro.h"
#include "ProxyConnect.h"

#include "ParseConfig.h"
#include "StringUtility.h"


class CTProxySockets : public CTRANSSockets {
public:
	CTProxySockets(void);
	~CTProxySockets(void);
private:
	struct SocketsArgu m_tSocketsArgu;
private:
	SOCKET ConnectSocket;
	unsigned long m_nSendTimeslice;
	unsigned long m_nReceiveTimeslice;
public:
	DWORD Create(struct SocketsArgu *pSocketsArgu);
private:
	DWORD SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend);
private:
	SOCKET ProxyConnect(char *szProxyServ, int iProxyPort);
	DWORD DestiConnect(SOCKET ProxySocket, enum PROXY_TYPE nProxyType, char *szDestiServ, int iDestiPort, int szProxyAuth, char *szUserName, char *szPassword);
public:
	DWORD Connect(char *szDestiServ, int iDestiPort);
	DWORD Close();
public:
	DWORD Send(char *szMessage, size_t length);
	DWORD Receive(char *szReceive, int length);
public:
	DWORD SendReceive(char *szReceive, char *pSendMessage);
public:
	DWORD SafeSend(char *szMessage, size_t length);
	DWORD SafeReceive(char *szReceive, int length);
public:
	DWORD SafeSendReceive(char *szReceive, char *pSendMessage);
};


class CReplyProxySockets : public CReplySockets {
public:
	CReplyProxySockets(void);
	~CReplyProxySockets(void);
private:
	char m_szDestiServ[SERVNAME_LENGTH];
	int m_nDestiPort;
	struct ReplySocksArgu m_tReplySocksArgu;
private:
	SOCKET ConnectSocket;
public:
	DWORD Create(struct ReplySocksArgu *pReplySocksArgu);
	DWORD SetAddress(char *szDestiServ, int iDestiPort);
private:
	SOCKET ProxyConnect(char *szProxyServ, int iProxyPort);
	DWORD DestiConnect(SOCKET ProxySocket, enum PROXY_TYPE nProxyType, char *szDestiServ, int iDestiPort, int szProxyAuth, char *szUserName, char *szPassword);
private:
	DWORD Connect(char *szDestiServ, int iDestiPort);
	DWORD Close();
private:
	DWORD Send(char *szMessage);
	DWORD Receive(char *szReceive);
public:
	DWORD SendReceive(char *szReceive, char *pSendMessage);
private:
	DWORD SafeSend(char *szMessage);
	DWORD SafeReceive(char *szReceive);
public:
	DWORD SafeSendReceive(char *szReceive, char *pSendMessage);
};


#define MESSAGE_BUFFER_LEN					1024
#define HEAD_BUFFER_LEN						1024