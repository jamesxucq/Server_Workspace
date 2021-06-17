#pragma once

#include <winsock2.h>
//#include "DefineMacro.h"
#include "tools_macro.h"

class CNormalReplySocks
{
public:
	CNormalReplySocks(void);
	~CNormalReplySocks(void);
private:
	char m_szDestiServ[SERV_NAME];
	int m_nDestiPort;
	SOCKET ConnectSocket;
public:
	bool Create(char *szDestnServ, int dwDestnPort);
	int Connect();
	int Connect(char *szDestnServ, int dwDestnPort);
	int Close();
private:
	int Send(char *szMessage);
	int Receive(char *pRecvData);
public:
	int SendReceive(char *pRecvData, char *pSendMessage);
};


#define MESSAGE_BUFFER					512
#define HEAD_BUFFER						512

