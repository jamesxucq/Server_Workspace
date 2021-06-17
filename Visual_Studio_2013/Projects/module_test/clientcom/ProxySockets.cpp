#include "StdAfx.h"
#include "ProxySockets.h"

CTProxySockets::CTProxySockets(void):
m_nSendTimeslice(0),
m_nReceiveTimeslice(0)
{
}

CTProxySockets::~CTProxySockets(void) {
}

DWORD CTProxySockets::Create(struct SocketsArgu *pSocketsArgu) {
	if(!pSocketsArgu) return -1;
	memcpy(&m_tSocketsArgu, pSocketsArgu, sizeof(struct SocketsArgu));

	int iSocksDownloadRate, iSocksUploadRate;
	if(pSocketsArgu->dwDownloadLimit) 
		iSocksDownloadRate = pSocketsArgu->dwDownloadRate / pSocketsArgu->dwWorkerThreads;
	else iSocksDownloadRate = 0;

	if(0x01 == pSocketsArgu->dwUploadLimit) 
		iSocksUploadRate = pSocketsArgu->dwUploadRate / pSocketsArgu->dwWorkerThreads;
	else if(0x00 == pSocketsArgu->dwUploadLimit || 0x03 == pSocketsArgu->dwUploadLimit) iSocksUploadRate = 0;

	return SetSendReceiveSpeed(iSocksDownloadRate, iSocksUploadRate);
}

DWORD CTProxySockets::SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend) {
	if(!ulMaxSend) m_nSendTimeslice = 0;
	else if(8 > ulMaxSend) m_nSendTimeslice = 1000;
	else m_nSendTimeslice = 1000 / (ulMaxSend>>3);

	if(!ulMaxReceive) m_nReceiveTimeslice = 0;
	else if(8 > ulMaxReceive) m_nReceiveTimeslice = 1000;
	else m_nReceiveTimeslice = 1000 / (ulMaxReceive>>3);

	return 0;
}


DWORD CTProxySockets::Connect(char *szDestiServ, int iDestiPort) {
	if (-1 == (ConnectSocket=ProxyConnect(m_tSocketsArgu.Address.sin_addr, m_tSocketsArgu.Address.sin_port))) {
		TRACE(_T("Error At ProxyConnect()\n"));
		return -1;	
	}

	if (DestiConnect(ConnectSocket, m_tSocketsArgu.dwType, szDestiServ, iDestiPort, m_tSocketsArgu.dwProxyEnable, m_tSocketsArgu.szUserName, m_tSocketsArgu.szPassword)) {
		TRACE(_T("Error At DestiConnect()\n"));
		return -1;	
	}

	return 0;
}

SOCKET CTProxySockets::ProxyConnect(char *szProxyServ, int iProxyPort) {
	SOCKET ProxySocket;

	if(!szProxyServ) return -1;

	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		TRACE(_T("Error At WSAStartup()\n"));
		return -1;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	//SOCKET ConnectSocket;
	ProxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ProxySocket == INVALID_SOCKET) {
		TRACE(_T("error at socket(): %ld\n"), WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//----------------------
	// Set colse socket timeout, for graceful close to server.
	struct linger liner;
	liner.l_onoff = TRUE;
	liner.l_linger = 8;
	setsockopt(ProxySocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(szProxyServ);
	clientService.sin_port = htons((u_short)iProxyPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(szProxyServ);
		if (host == NULL){
			TRACE(_T("unable to resolve server: %s\n"), szProxyServ);
			return -1;
		}
		CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
	}
	//----------------------
	// Connect to server.
	if ( connect( ProxySocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR){
		TRACE( _T("failed to connect.\n") );
		WSACleanup();
		return -1; //不能连接到代理服务器!
	}

	return ProxySocket;
}

DWORD CTProxySockets::DestiConnect(SOCKET ProxySocket, enum PROXY_TYPE nProxyType, char *szDestiServ, int iDestiPort, int szProxyAuth, char *szUserName, char *szPassword) {
	if(!szDestiServ) return -1;

	//HTTP;HTTPS;SOCKS4;SOCKS5;
	switch( nProxyType ) {
		case PROXY_TYPE_HTTP:
			if(szProxyAuth) {
				if(NProxyConnect::ConnectHttpAuth(ProxySocket, szDestiServ, iDestiPort, szUserName, szPassword)) {
					TRACE( _T("Failed http proxy auth.\n") );
					WSACleanup();
					return -1;
				}
			} else {
				if(NProxyConnect::ConnectHttp(ProxySocket, szDestiServ, iDestiPort)) {
					TRACE( _T("Failed http proxy.\n") );
					WSACleanup();
					return -1;			
				}
			}
			break;
		case PROXY_TYPE_HTTPS:
			break;
		case PROXY_TYPE_SOCKS4:
			if(NProxyConnect::ConnectSocks4(ProxySocket, szDestiServ, iDestiPort)) {
				TRACE( _T("Failed socksr proxy.\n") );
				WSACleanup();
				return -1;
			}
			break;
		case PROXY_TYPE_SOCKS4a:
			break;
		case PROXY_TYPE_SOCKS5:
			if(NProxyConnect::ConnectSocks5(ProxySocket, szDestiServ, iDestiPort, szUserName, szPassword)) {
				TRACE( _T("Failed socks5 proxy auth.\n") );
				WSACleanup();
				return -1;		
			}
			break;
	}

	return 0;
}


DWORD CTProxySockets::Close() {
	//----------------------
	// Graceful close to server.
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

DWORD CTProxySockets::Send(char *szMessage, size_t length) {
	static DWORD ulTimestamp;
	static unsigned long ulSendTimesCount;
	//----------------------
	// Declare and initialize variables.
	int	  bytesSent;

	if(!m_nSendTimeslice) {
		//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
		bytesSent = send( ConnectSocket, szMessage, (int)length, 0);
		if (bytesSent == 0)	return 0;// Graceful close
		else if (bytesSent == SOCKET_ERROR) {
			LOG_TRACE(_T("send() failed: %d\n"), WSAGetLastError());
			return -1;
		}
	} else {	
		int IsCalculateTimestamp = POW2N_MOD(ulSendTimesCount++, 16); 
		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount(); 

		//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
		bytesSent = send( ConnectSocket, szMessage, (int)length, 0);
		if (bytesSent == 0)	return 0;// Graceful close
		else if (bytesSent == SOCKET_ERROR) {
			LOG_TRACE(_T("send() failed: %d\n"), WSAGetLastError());
			return -1;
		}

		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount() - ulTimestamp; 
		if(m_nSendTimeslice > ulTimestamp) Sleep(m_nSendTimeslice - ulTimestamp);
	}

	return bytesSent;
}

DWORD CTProxySockets::Receive(char *szReceive, int length) {
	static DWORD ulTimestamp;
	static unsigned long ulSendTimesCount;	
	//----------------------
	// Declare and initialize variables.
	int bytesRecv = 0;

	if(!m_nReceiveTimeslice) {
		//----------------------
		// Send and receive data.
		bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
		if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
			//"Connection Closed.\n";
			return -1;
		}
		//printf( "head_buffer: %s", head_buffer);	
	} else {
		int IsCalculateTimestamp = POW2N_MOD(ulSendTimesCount++, 16);
		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount(); 

		//----------------------
		// Send and receive data.
		bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
		if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
			//"Connection Closed.\n";
			return -1;
		}
		//printf( "head_buffer: %s", head_buffer);	

		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount() - ulTimestamp; 
		if(m_nReceiveTimeslice > ulTimestamp) Sleep(m_nReceiveTimeslice - ulTimestamp);
	}

	return bytesRecv;
}

DWORD CTProxySockets::SendReceive(char *szReceive, char *pSendMessage) {
	int	 result = -1;

	//----------------------
	// Send and receive data.
	result = Send(pSendMessage, strlen(pSendMessage));
	if(0 > result) return -1;

	result = Receive(szReceive, RECV_BUFF_SIZE);
	if(0 > result) return -1;	

	return result;
}


DWORD CTProxySockets::SafeSend(char *szMessage, size_t length) {
	int	  bytesSent;

	//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
	bytesSent = send( ConnectSocket, szMessage, (int)length, 0);
	if (bytesSent == 0)	return 0;// Graceful close
	else if (bytesSent == SOCKET_ERROR) {
		TRACE(_T("send() failed: %d\n"), WSAGetLastError());
		return -1;
	}

	return bytesSent;
}

DWORD CTProxySockets::SafeReceive(char *szReceive, int length) {	
	//----------------------
	// Declare and initialize variables.
	int bytesRecv = 0;

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "head_buffer: %s", head_buffer);

	return bytesRecv;
}

DWORD CTProxySockets::SafeSendReceive(char *szReceive, char *pSendMessage) {
	int	 result = -1;

	//----------------------
	// Send and receive data.
	result = Send(pSendMessage, strlen(pSendMessage));
	if(0 > result) return -1;

	result = Receive(szReceive, RECV_BUFF_SIZE);
	if(0 > result) return -1;	

	return result;
}

CReplyProxySockets::CReplyProxySockets(void) {
}

CReplyProxySockets::~CReplyProxySockets(void) {
}

DWORD CReplyProxySockets::Create(struct ReplySocksArgu *pReplySocksArgu) {
	if(!pReplySocksArgu) return -1;
	memcpy(&m_tReplySocksArgu, pReplySocksArgu, sizeof(struct ReplySocksArgu));
	return 0;
}

DWORD CReplyProxySockets::SetAddress(char *szDestiServ, int iDestiPort) {
	if(!szDestiServ) return -1;

	strcpy_s(m_szDestiServ, szDestiServ);
	m_nDestiPort = iDestiPort;

	return 0;
}


DWORD CReplyProxySockets::Connect(char *szDestiServ, int iDestiPort) {
	if (-1 == (ConnectSocket=ProxyConnect(m_tReplySocksArgu.Address.sin_addr, m_tReplySocksArgu.Address.sin_port))) {
		TRACE(_T("Error At ProxyConnect()\n"));
		return -1;	
	}

	if (DestiConnect(ConnectSocket, m_tReplySocksArgu.dwType, szDestiServ, iDestiPort, m_tReplySocksArgu.dwProxyEnable, m_tReplySocksArgu.szUserName, m_tReplySocksArgu.szPassword)) {
		TRACE(_T("Error At DestiConnect()\n"));
		return -1;	
	}

	return 0;
}

SOCKET CReplyProxySockets::ProxyConnect(char *szProxyServ, int iProxyPort) {
	SOCKET ProxySocket;

	if(!szProxyServ) return -1;

	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		TRACE(_T("Error At WSAStartup()\n"));
		return -1;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	//SOCKET ConnectSocket;
	ProxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ProxySocket == INVALID_SOCKET) {
		TRACE(_T("error at socket(): %ld\n"), WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//----------------------
	// Set colse socket timeout, for graceful close to server.
	struct linger liner;
	liner.l_onoff = TRUE;
	liner.l_linger = 8;
	setsockopt(ProxySocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(szProxyServ);
	clientService.sin_port = htons((u_short)iProxyPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(szProxyServ);
		if (host == NULL){
			TRACE(_T("unable to resolve server: %s\n"), szProxyServ);
			return -1;
		}
		CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
	}
	//----------------------
	// Connect to server.
	if ( connect( ProxySocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR){
		TRACE( _T("failed to connect.\n") );
		WSACleanup();
		return -1; //不能连接到代理服务器!
	}

	return ProxySocket;
}


DWORD CReplyProxySockets::DestiConnect(SOCKET ProxySocket, enum PROXY_TYPE nProxyType, char *szDestiServ, int iDestiPort, int szProxyAuth, char *szUserName, char *szPassword) {
	if(!szDestiServ) return -1;

	//HTTP;HTTPS;SOCKS4;SOCKS5;
	switch( nProxyType ) {
		case PROXY_TYPE_HTTP:
			if(szProxyAuth) {
				if(NProxyConnect::ConnectHttpAuth(ProxySocket, szDestiServ, iDestiPort, szUserName, szPassword)) {
					TRACE( _T("Failed http proxy auth.\n") );
					WSACleanup();
					return -1;
				}
			} else {
				if(NProxyConnect::ConnectHttp(ProxySocket, szDestiServ, iDestiPort)) {
					TRACE( _T("Failed http proxy.\n") );
					WSACleanup();
					return -1;			
				}
			}
			break;
		case PROXY_TYPE_HTTPS:
			break;
		case PROXY_TYPE_SOCKS4:
			if(NProxyConnect::ConnectSocks4(ProxySocket, szDestiServ, iDestiPort)) {
				TRACE( _T("Failed socksr proxy.\n") );
				WSACleanup();
				return -1;
			}
			break;
		case PROXY_TYPE_SOCKS4a:
			break;
		case PROXY_TYPE_SOCKS5:
			if(NProxyConnect::ConnectSocks5(ProxySocket, szDestiServ, iDestiPort, szUserName, szPassword)) {
				TRACE( _T("Failed socks5 proxy auth.\n") );
				WSACleanup();
				return -1;		
			}
			break;
	}

	return 0;
}



DWORD CReplyProxySockets::Close() {
	//----------------------
	// Graceful close to server.
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

DWORD CReplyProxySockets::Send(char *szMessage) {
	int	  bytesSent;

	//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
	bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
	if (bytesSent == 0)	return 0;// Graceful close
	else if (bytesSent == SOCKET_ERROR) {
		TRACE(_T("send() failed: %d\n"), WSAGetLastError());
		return -1;
	}

	return bytesSent;
}

DWORD CReplyProxySockets::Receive(char *szReceive) {
	//----------------------
	// Declare and initialize variables.
	int bytesRecv = 0;
	char head_buffer[HEAD_BUFFER_LEN];

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, head_buffer, HEAD_BUFFER_LEN, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "head_buffer: %s", head_buffer);
	memcpy(szReceive, head_buffer, bytesRecv);
	szReceive[bytesRecv] = '\0';

	return bytesRecv;
}

DWORD CReplyProxySockets::SendReceive(char *szReceive, char *pSendMessage) {
	int result = -1;

	//----------------------
	// Connect to server.
	if(Connect(m_szDestiServ, m_nDestiPort)) return -1;

	//----------------------
	// Send and receive data.
	do {
		result = Send(pSendMessage);
		if(0 > result) break;

		result = Receive(szReceive);
		if(0 > result) break;
	} while (FALSE);

	//----------------------
	// Graceful close to server.
	Close();

	return result;
}

DWORD CReplyProxySockets::SafeSend(char *szMessage) {
	int	  bytesSent;

	//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
	bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
	if (bytesSent == 0)	return 0;// Graceful close
	else if (bytesSent == SOCKET_ERROR) {
		TRACE(_T("send() failed: %d\n"), WSAGetLastError());
		return -1;
	}

	return bytesSent;
}

DWORD CReplyProxySockets::SafeReceive(char *szReceive) {
	//----------------------
	// Declare and initialize variables.
	int bytesRecv = 0;
	char head_buffer[HEAD_BUFFER_LEN];

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, head_buffer, HEAD_BUFFER_LEN, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "head_buffer: %s", head_buffer);
	memcpy(szReceive, head_buffer, bytesRecv);
	szReceive[bytesRecv] = '\0';

	return bytesRecv;
}

DWORD CReplyProxySockets::SafeSendReceive(char *szReceive, char *pSendMessage) {
	int result = -1;

	//----------------------
	// Connect to server.
	if(Connect(m_szDestiServ, m_nDestiPort)) return -1;

	//----------------------
	// Send and receive data.
	do {
		result = SafeSend(pSendMessage);
		if(0 > result) break;

		result = SafeReceive(szReceive);
		if(0 > result) break;
	} while (FALSE);

	//----------------------
	// Graceful close to server.
	Close();

	return result;
}
