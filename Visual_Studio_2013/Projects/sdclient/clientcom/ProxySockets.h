#pragma once
#include <winsock2.h>

// #include "ParseConfig.h"
#include "TRANSSockets.h"
#include "common_macro.h"
#include "ProxyConnect.h"

#include "ParseConfig.h"
#include "StringUtility.h"


class CProxySockets : public CTRANSSockets {
public:
    CProxySockets(void);
    ~CProxySockets(void);
private:
    struct SocketsArgu m_tSockeArgu;
private:
    SOCKET ConnectSocket;
    unsigned long m_lSendTimeslice;
    unsigned long m_lReceiveTimeslice;
public:
    DWORD Create(struct SocketsArgu *pSockeArgu);
private:
    DWORD SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend);
private:
    SOCKET ProxyConnect(char *proxy_serv, int proxy_port);
    DWORD ServConnect(SOCKET ProxySocket, enum PROXY_TYPE dwProxyType, char *address, int port, int szProxyAuth, char *user, char *pword);
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


class CReplyProxy : public CReplySockets {
public:
    CReplyProxy(void);
    ~CReplyProxy(void);
private:
    char m_address[SERVNAME_LENGTH];
    int m_port;
    struct ReplyArgu m_tReplyArgu;
private:
    SOCKET ConnectSocket;
public:
    DWORD Create(struct ReplyArgu *pReplyArgu);
    DWORD SetAddress(char *address, int port);
private:
    SOCKET ProxyConnect(char *proxy_serv, int proxy_port);
    DWORD ServConnect(SOCKET ProxySocket, enum PROXY_TYPE dwProxyType, char *address, int port, int szProxyAuth, char *user, char *pword);
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


#define MESSAGE_BUFFER_LEN					1024
#define HEAD_BUFFER_LEN						1024