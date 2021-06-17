#include "StdAfx.h"
#include "ProxySockets.h"

CProxySockets::CProxySockets(void):
m_nSendTimeslice(0),
m_nReceiveTimeslice(0)
{
}

CProxySockets::~CProxySockets(void)
{
}
	
int CProxySockets::Create(struct SocketsArgu *pSocketsArgu)
{
	if(!pSocketsArgu) return -1;
	memcpy(&m_stSocketsArgu, pSocketsArgu, sizeof(struct SocketsArgu));
	return 0;
}

int CProxySockets::SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend)
{
	if(!ulMaxSend) m_nSendTimeslice = 0;
	else if(8 > ulMaxSend) m_nSendTimeslice = 1000;
	else m_nSendTimeslice = 1000 / (ulMaxSend>>3);
	
	if(!ulMaxReceive) m_nReceiveTimeslice = 0;
	else if(8 > ulMaxReceive) m_nReceiveTimeslice = 1000;
	else m_nReceiveTimeslice = 1000 / (ulMaxReceive>>3);

	return 0;
}


int CProxySockets::Connect(char *sDestiServ, int iDestiPort) {
	enum ProxyType iProxyType;
	int iProxyAuth;

	char server[256];
	NStringCon::unicode_utf8(server, m_stSocketsArgu.sServer);
	if (ProxyConnect(&ConnectSocket, server, m_stSocketsArgu.iPort)) {
		TRACE(_T("Error at ProxyConnect()\n"));
		return -1;	
	}

	if(!_tcscmp(m_stSocketsArgu.sType, _T("HTTP"))) iProxyType = HTTP; // 通过HTTP方式代理 
	else if(!_tcscmp(m_stSocketsArgu.sType, _T("HTTPS"))) iProxyType = HTTPS;
	else if(!_tcscmp(m_stSocketsArgu.sType, _T("SOCKS4"))) iProxyType = SOCKS4; // 通过Socks4方式代理 
	else if(!_tcscmp(m_stSocketsArgu.sType, _T("SOCKS4a"))) iProxyType = SOCKS4a; // 通过Socks4方式代理 
	else if(!_tcscmp(m_stSocketsArgu.sType, _T("SOCKS5"))) iProxyType = SOCKS5; // 通过Socks5方式代理 

	if(!_tcscmp(m_stSocketsArgu.sProxyAuth, _T("true"))) iProxyAuth = 0x01; // 通过HTTP方式代理 
	else if(!_tcscmp(m_stSocketsArgu.sProxyAuth, _T("false"))) iProxyAuth = 0x00;

	char user_name[256], password[256];
	NStringCon::unicode_utf8(user_name, m_stSocketsArgu.sUserName);
	NStringCon::unicode_utf8(password, m_stSocketsArgu.sPassword);
	if (DestiConnect(ConnectSocket, iProxyType, sDestiServ, iDestiPort, iProxyAuth, user_name, password)) {
		TRACE(_T("Error at DestiConnect()\n"));
		return -1;	
	}

	return 0;
}

int CProxySockets::ProxyConnect(SOCKET *ProxySocket, char *sProxyServ, int iProxyPort)
{
	if(!sProxyServ) return -1;

	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		TRACE(_T("Error at WSAStartup()\n"));
		return -1;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	//SOCKET ConnectSocket;
	*ProxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*ProxySocket == INVALID_SOCKET) {
		TRACE(_T("Error at socket(): %ld\n"), WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//----------------------
	// Set colse socket timeout, for graceful close to server.
	struct linger liner;
	liner.l_onoff = TRUE;
	liner.l_linger = 8;
	setsockopt(*ProxySocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(sProxyServ);
	clientService.sin_port = htons((u_short)iProxyPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(sProxyServ);
		if (host == NULL){
			TRACE(_T("Unable to resolve server: %s\n"), sProxyServ);
			return -1;
		}
		CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
	}
	//----------------------
	// Connect to server.
	if ( connect( *ProxySocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR){
		TRACE( _T("Failed to connect.\n") );
		WSACleanup();
		return -1; //不能连接到代理服务器!
	}

	return 0;
}

int CProxySockets::DestiConnect(SOCKET ProxySocket, enum ProxyType iProxyType, char *sDestiServ, int iDestiPort, int sProxyAuth, char *sUserName, char *sPassword)
{
	if(!sDestiServ) return -1;

	//HTTP;HTTPS;SOCKS4;SOCKS5;
	switch( iProxyType ) {
		case HTTP:
			if(sProxyAuth) {
				if(NProxyConnect::ConnectAuthHttp(ProxySocket, sDestiServ, iDestiPort, sUserName, sPassword)) {
					TRACE( _T("Failed http proxy auth.\n") );
					WSACleanup();
					return -1;
				}
			} else {
				if(NProxyConnect::ConnectHttp(ProxySocket, sDestiServ, iDestiPort)) {
					TRACE( _T("Failed http proxy.\n") );
					WSACleanup();
					return -1;			
				}
			}
			break;
		case HTTPS:
			break;
		case SOCKS4:
			if(NProxyConnect::ConnectSocks4(ProxySocket, sDestiServ, iDestiPort)) {
				TRACE( _T("Failed socksr proxy.\n") );
				WSACleanup();
				return -1;
			}
			break;
		case SOCKS4a:
			break;
		case SOCKS5:
			if(NProxyConnect::ConnectSocks5(ProxySocket, sDestiServ, iDestiPort, sUserName, sPassword)) {
				TRACE( _T("Failed socks5 proxy auth.\n") );
				WSACleanup();
				return -1;		
			}
			break;
	}

	return 0;
}


int CProxySockets::Close()
{
	//----------------------
	// Graceful close to server.
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

int CProxySockets::Send(char *szMessage, size_t len)
{
	int	  bytesSent;

	//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
	bytesSent = send( ConnectSocket, szMessage, (int)len, 0);
	if (bytesSent == 0)	return -1;// Graceful close
	else if (bytesSent == SOCKET_ERROR)
	{
		TRACE(_T("send() failed: %d\n"), WSAGetLastError());
		return -1;
	}

	return bytesSent;
}

int CProxySockets::Receive(char *pRecvData, int len)
{	//----------------------
	// Declare and initialize variables.
	int		bytesRecv = 0;

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, pRecvData, len, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "HeadBuf: %s", HeadBuf);

	return bytesRecv;
}

int CProxySockets::SendReceive(char *pRecvData, char *pSendMessage)
{
	int	 retValue = -1;

	//----------------------
	// Send and receive data.
	retValue = Send(pSendMessage, strlen(pSendMessage));
	if(0 > retValue) return -1;

	retValue = Receive(pRecvData, RECV_BUFF_SIZE);
	if(0 > retValue) return -1;	

	return retValue;
}


int CProxySockets::SafeSend(char *szMessage, size_t len)
{
	int	  bytesSent;

	//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
	bytesSent = send( ConnectSocket, szMessage, (int)len, 0);
	if (bytesSent == 0)	return -1;// Graceful close
	else if (bytesSent == SOCKET_ERROR)
	{
		TRACE(_T("send() failed: %d\n"), WSAGetLastError());
		return -1;
	}

	return bytesSent;
}

int CProxySockets::SafeReceive(char *pRecvData, int len)
{	//----------------------
	// Declare and initialize variables.
	int		bytesRecv = 0;

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, pRecvData, len, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "HeadBuf: %s", HeadBuf);

	return bytesRecv;
}

int CProxySockets::SafeSendReceive(char *pRecvData, char *pSendMessage)
{
	int	 retValue = -1;

	//----------------------
	// Send and receive data.
	retValue = Send(pSendMessage, strlen(pSendMessage));
	if(0 > retValue) return -1;

	retValue = Receive(pRecvData, RECV_BUFF_SIZE);
	if(0 > retValue) return -1;	

	return retValue;
}

CMsgProxySockets::CMsgProxySockets(void)
{
}

CMsgProxySockets::~CMsgProxySockets(void)
{
}

int CMsgProxySockets::Create(struct MsgSocketsArgu *pMsgSocketsArgu)
{
	if(!pMsgSocketsArgu) return -1;
	memcpy(&m_stMsgSocketsArgu, pMsgSocketsArgu, sizeof(struct MsgSocketsArgu));
	return 0;
}

int CMsgProxySockets::SetAddress(char *sDestiServ, int iDestiPort)
{
	if(!sDestiServ) return -1;

	strcpy_s(m_sDestiServ, sDestiServ);
	m_iDestiPort = iDestiPort;

	return 0;
}


int CMsgProxySockets::Connect(char *sDestiServ, int iDestiPort) {
	enum ProxyType iProxyType;
	int iProxyAuth;

	if (ProxyConnect(&ConnectSocket, m_stMsgSocketsArgu.sServer, m_stMsgSocketsArgu.iPort)) {
		TRACE(_T("Error at ProxyConnect()\n"));
		return -1;	
	}

	if(!_tcscmp(m_stMsgSocketsArgu.sType, _T("HTTP"))) iProxyType = HTTP; // 通过HTTP方式代理 
	else if(!_tcscmp(m_stMsgSocketsArgu.sType, _T("HTTPS"))) iProxyType = HTTPS;
	else if(!_tcscmp(m_stMsgSocketsArgu.sType, _T("SOCKS4"))) iProxyType = SOCKS4; // 通过Socks4方式代理 
	else if(!_tcscmp(m_stMsgSocketsArgu.sType, _T("SOCKS4a"))) iProxyType = SOCKS4a; // 通过Socks4方式代理 
	else if(!_tcscmp(m_stMsgSocketsArgu.sType, _T("SOCKS5"))) iProxyType = SOCKS5; // 通过Socks5方式代理 

	if(!_tcscmp(m_stMsgSocketsArgu.sProxyAuth, _T("true"))) iProxyAuth = 0x01; // 通过HTTP方式代理 
	else if(!_tcscmp(m_stMsgSocketsArgu.sProxyAuth, _T("false"))) iProxyAuth = 0x00;

	if (DestiConnect(ConnectSocket, iProxyType, sDestiServ, iDestiPort, iProxyAuth, m_stMsgSocketsArgu.sUserName, m_stMsgSocketsArgu.sPassword)) {
		TRACE(_T("Error at DestiConnect()\n"));
		return -1;	
	}

	return 0;
}

int CMsgProxySockets::ProxyConnect(SOCKET *ProxySocket, char *sProxyServ, int iProxyPort)
{
	if(!sProxyServ) return -1;

	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		TRACE(_T("Error at WSAStartup()\n"));
		return -1;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	//SOCKET ConnectSocket;
	*ProxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*ProxySocket == INVALID_SOCKET) {
		TRACE(_T("Error at socket(): %ld\n"), WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//----------------------
	// Set colse socket timeout, for graceful close to server.
	struct linger liner;
	liner.l_onoff = TRUE;
	liner.l_linger = 8;
	setsockopt(*ProxySocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(sProxyServ);
	clientService.sin_port = htons((u_short)iProxyPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(sProxyServ);
		if (host == NULL){
			TRACE(_T("Unable to resolve server: %s\n"), sProxyServ);
			return -1;
		}
		CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
	}
	//----------------------
	// Connect to server.
	if ( connect( *ProxySocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR){
		TRACE( _T("Failed to connect.\n") );
		WSACleanup();
		return -1; //不能连接到代理服务器!
	}

	return 0;
}


int CMsgProxySockets::DestiConnect(SOCKET ProxySocket, enum ProxyType iProxyType, char *sDestiServ, int iDestiPort, int sProxyAuth, char *sUserName, char *sPassword)
{
	if(!sDestiServ) return -1;

	//HTTP;HTTPS;SOCKS4;SOCKS5;
	switch( iProxyType ) {
		case HTTP:
			if(sProxyAuth) {
				if(NProxyConnect::ConnectAuthHttp(ProxySocket, sDestiServ, iDestiPort, sUserName, sPassword)) {
					TRACE( _T("Failed http proxy auth.\n") );
					WSACleanup();
					return -1;
				}
			} else {
				if(NProxyConnect::ConnectHttp(ProxySocket, sDestiServ, iDestiPort)) {
					TRACE( _T("Failed http proxy.\n") );
					WSACleanup();
					return -1;			
				}
			}
			break;
		case HTTPS:
			break;
		case SOCKS4:
			if(NProxyConnect::ConnectSocks4(ProxySocket, sDestiServ, iDestiPort)) {
				TRACE( _T("Failed socksr proxy.\n") );
				WSACleanup();
				return -1;
			}
			break;
		case SOCKS4a:
			break;
		case SOCKS5:
			if(NProxyConnect::ConnectSocks5(ProxySocket, sDestiServ, iDestiPort, sUserName, sPassword)) {
				TRACE( _T("Failed socks5 proxy auth.\n") );
				WSACleanup();
				return -1;		
			}
			break;
	}

	return 0;
}



int CMsgProxySockets::Close()
{
	//----------------------
	// Graceful close to server.
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

int CMsgProxySockets::Send(char *szMessage)
{
	int	  bytesSent;

	//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
	bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
	if (bytesSent == 0)	return -1;// Graceful close
	else if (bytesSent == SOCKET_ERROR)
	{
		TRACE(_T("send() failed: %d\n"), WSAGetLastError());
		return -1;
	}

	return 0;
}

int CMsgProxySockets::Receive(char *pRecvData)
{
	//----------------------
	// Declare and initialize variables.
	int		bytesRecv = 0;
	char	HeadBuf[HEAD_BUFFER_LEN];

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, HeadBuf, HEAD_BUFFER_LEN, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "HeadBuf: %s", HeadBuf);
	memcpy(pRecvData, HeadBuf, bytesRecv);
	pRecvData[bytesRecv] = '\0';

	return 0;
}

int CMsgProxySockets::SendReceive(char *pRecvData, char *pSendMessage)
{
	int retValue = -1;

	//----------------------
	// Connect to server.
	if(Connect(m_sDestiServ, m_iDestiPort)) return -1;

	//----------------------
	// Send and receive data.
	do {
		if(Send(pSendMessage)) {retValue = -1; break;}
		if(Receive(pRecvData)) retValue = -1;
		else retValue = 0;	
	} while (FALSE);

	//----------------------
	// Graceful close to server.
	Close();

	return retValue;
}

int CMsgProxySockets::SafeSend(char *szMessage)
{
	int	  bytesSent;

	//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
	bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
	if (bytesSent == 0)	return -1;// Graceful close
	else if (bytesSent == SOCKET_ERROR)
	{
		TRACE(_T("send() failed: %d\n"), WSAGetLastError());
		return -1;
	}

	return 0;
}

int CMsgProxySockets::SafeReceive(char *pRecvData)
{
	//----------------------
	// Declare and initialize variables.
	int		bytesRecv = 0;
	char	HeadBuf[HEAD_BUFFER_LEN];

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, HeadBuf, HEAD_BUFFER_LEN, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "HeadBuf: %s", HeadBuf);
	memcpy(pRecvData, HeadBuf, bytesRecv);
	pRecvData[bytesRecv] = '\0';

	return 0;
}

int CMsgProxySockets::SafeSendReceive(char *pRecvData, char *pSendMessage)
{
	int retValue = -1;

	//----------------------
	// Connect to server.
	if(Connect(m_sDestiServ, m_iDestiPort)) return -1;

	//----------------------
	// Send and receive data.
	do {
		if(Send(pSendMessage)) {retValue = -1; break;}
		if(Receive(pRecvData)) retValue = -1;
		else retValue = 0;	
	} while (FALSE);

	//----------------------
	// Graceful close to server.
	Close();

	return retValue;
}
