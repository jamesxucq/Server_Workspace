#pragma once
#include <winsock2.h>

// #include "ParseConfig.h"
#include "SdSocketsBzl.h"
#include "CommonMacro.h"
#include "ProxyConnect.h"

#ifdef SD_APPLICATION
#include "ClientMacro.h"
#endif
#ifdef FORMAT_WIZARD
#include "WizardMacro.h"
#endif

#include "ParseConfig.h"
#include "StringOpt.h"


enum ProxyType {
  HTTP = 1,
  HTTPS,
  SOCKS4,
  SOCKS4a,
  SOCKS5
};


class CProxySockets : public CSdSocketsBzl
{
public:
	CProxySockets(void);
	~CProxySockets(void);
private:
	struct SocketsArgu m_stSocketsArgu;
private:
	SOCKET ConnectSocket;
	unsigned long m_nSendTimeslice;
	unsigned long m_nReceiveTimeslice;
public:
	int Create(struct SocketsArgu *pSocketsArgu);
	int SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend);
private:
	int ProxyConnect(SOCKET *ProxySocket, char *sProxyServ, int ProxyPort);
	int DestiConnect(SOCKET ProxySocket, enum ProxyType iProxyType, char *sDestiServ, int iDestiPort, int sProxyAuth, char *sUserName, char *sPassword);
public:
	int Connect(char *sDestiServ, int iDestiPort);
	int Close();
public:
	int Send(char *szMessage, size_t len);
	int Receive(char *pRecvData, int len);
public:
	int SendReceive(char *pRecvData, char *pSendMessage);
public:
	int SafeSend(char *szMessage, size_t len);
	int SafeReceive(char *pRecvData, int len);
public:
	int SafeSendReceive(char *pRecvData, char *pSendMessage);
};


class CMsgProxySockets : public CMsgSocketsBzl
{
public:
	CMsgProxySockets(void);
	~CMsgProxySockets(void);
private:
	char m_sDestiServ[SERV_NAME_LEN];
	int m_iDestiPort;
	struct MsgSocketsArgu m_stMsgSocketsArgu;
private:
	SOCKET ConnectSocket;
public:
	int Create(struct MsgSocketsArgu *pMsgSocketsArgu);
	int SetAddress(char *sDestiServ, int iDestiPort);
private:
	int ProxyConnect(SOCKET *ProxySocket, char *sProxyServ, int ProxyPort);
	int DestiConnect(SOCKET ProxySocket, enum ProxyType iProxyType, char *sDestiServ, int iDestiPort, int sProxyAuth, char *sUserName, char *sPassword);
private:
	int Connect(char *sDestiServ, int iDestiPort);
	int Close();
private:
	int Send(char *szMessage);
	int Receive(char *pRecvData);
public:
	int SendReceive(char *pRecvData, char *pSendMessage);
private:
	int SafeSend(char *szMessage);
	int SafeReceive(char *pRecvData);
public:
	int SafeSendReceive(char *pRecvData, char *pSendMessage);
};


#define MESSAGE_BUFFER_LEN					512
#define HEAD_BUFFER_LEN						512