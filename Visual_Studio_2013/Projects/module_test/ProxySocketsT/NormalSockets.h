#pragma once

#include <winsock2.h>

// #include "ParseConfig.h"
#include "SdSocketsBzl.h"
#include "CommonMacro.h"

#ifdef SD_APPLICATION
#include "ClientMacro.h"
#endif
#ifdef FORMAT_WIZARD
#include "WizardMacro.h"
#endif


class CNormalSockets : public CSdSocketsBzl
{
public:
	CNormalSockets(void);
	~CNormalSockets(void);
private:
	SOCKET ConnectSocket;
	unsigned long m_nSendTimeslice;
	unsigned long m_nReceiveTimeslice;
public:
	int Create(struct SocketsArgu *pSocketsArgu);
	int SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend);
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

class CMsgNormalSockets : public CMsgSocketsBzl
{
public:
	CMsgNormalSockets(void);
	~CMsgNormalSockets(void);
private:
	char m_sServName[SERV_NAME_LEN];
	int m_iServPort;
private:
	SOCKET ConnectSocket;
public:
	int Create(struct MsgSocketsArgu *pMsgSocketsArguuu);
	int SetAddress(char *sDestiServ, int iDestiPort);
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

#define MESSAGE_BUFFER					512
#define HEAD_BUFFER_LEN					512