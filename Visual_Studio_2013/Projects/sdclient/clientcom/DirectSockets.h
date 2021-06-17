#pragma once

#include <winsock2.h>

// #include "ParseConfig.h"
#include "TRANSSockets.h"
#include "common_macro.h"


class CDirectSockets : public CTRANSSockets {
public:
    CDirectSockets(void);
    ~CDirectSockets(void);
private:
    SOCKET ConnectSocket;
    unsigned long m_lSendTimeslice;
    unsigned long m_lReceiveTimeslice;
public:
    DWORD Create(struct SocketsArgu *pSockeArgu);
private:
    DWORD SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend);
public:
    DWORD Connect(char *address, int port);
    VOID Close();
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

class CReplyDirect : public CReplySockets {
public:
    CReplyDirect(void);
    ~CReplyDirect(void);
private:
    char m_address[SERVNAME_LENGTH];
    int m_port;
private:
    SOCKET ConnectSocket;
public:
    DWORD Create(struct ReplyArgu *pReplyArgu);
    DWORD SetAddress(char *address, int port);
private:
    DWORD Connect(char *address, int port);
    VOID Close();
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