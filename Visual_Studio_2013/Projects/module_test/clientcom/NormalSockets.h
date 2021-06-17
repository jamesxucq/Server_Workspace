#pragma once

#include <winsock2.h>

// #include "ParseConfig.h"
#include "TRANSSockets.h"
#include "common_macro.h"


class CTNormalSockets : public CTRANSSockets {
public:
	CTNormalSockets(void);
	~CTNormalSockets(void);
private:
	SOCKET ConnectSocket;
	unsigned long m_nSendTimeslice;
	unsigned long m_nReceiveTimeslice;
public:
	DWORD Create(struct SocketsArgu *pSocketsArgu);
private:
	DWORD SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend);
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

class CReplyNormalSockets : public CReplySockets {
public:
	CReplyNormalSockets(void);
	~CReplyNormalSockets(void);
private:
	char m_szDestiServ[SERVNAME_LENGTH];
	int m_nDestiPort;
private:
	SOCKET ConnectSocket;
public:
	DWORD Create(struct ReplySocksArgu *pReplySocksArgu);
	DWORD SetAddress(char *szDestiServ, int iDestiPort);
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

#define MESSAGE_BUFFER_LEN				1024
#define HEAD_BUFFER_LEN					1024