#include "StdAfx.h"

// #include "Logger.h"
#include "NormalSockets.h"

CNormalSockets::CNormalSockets(void):
m_nSendTimeslice(0),
m_nReceiveTimeslice(0)
{
}

CNormalSockets::~CNormalSockets(void)
{
}
	
	int CNormalSockets::Create(struct SocketsArgu *pSocketsArgu)
{
	int nSocksDownloadRate = 0, nSocksUploadRate = 0;

	if(!pSocketsArgu) return -1;

	if(!_tcscmp(_T("true"), pSocketsArgu->sDownloadLimit)) 
		nSocksDownloadRate = pSocketsArgu->iDownloadRate / pSocketsArgu->nSyncingThreads;
	if(!_tcscmp(_T("true"), pSocketsArgu->sUploadLimit)) 
		nSocksUploadRate = pSocketsArgu->iUploadRate / pSocketsArgu->nSyncingThreads;

	return SetSendReceiveSpeed(nSocksDownloadRate, nSocksUploadRate);
}

int CNormalSockets::SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend)
{
	if(!ulMaxSend) m_nSendTimeslice = 0;
	else if(8 > ulMaxSend) m_nSendTimeslice = 1000;
	else m_nSendTimeslice = 1000 / (ulMaxSend>>3);
	
	if(!ulMaxReceive) m_nReceiveTimeslice = 0;
	else if(8 > ulMaxReceive) m_nReceiveTimeslice = 1000;
	else m_nReceiveTimeslice = 1000 / (ulMaxReceive>>3);

	return 0;
}

int CNormalSockets::Connect(char *sDestiServ, int iDestiPort)
{
	if(!sDestiServ) return -1;

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
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		TRACE(_T("Error at socket(): %ld\n"), WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//----------------------
	// Set colse socket timeout, for graceful close to server.
	struct linger liner;
	liner.l_onoff = TRUE;
	liner.l_linger = 32;
	setsockopt(ConnectSocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(sDestiServ);
	clientService.sin_port = htons(iDestiPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(sDestiServ);
		if (host == NULL){
			TRACE(_T("Unable to resolve server: %s\n"), sDestiServ);
			WSACleanup();
			return 1;
		}
		CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
	}
	//----------------------
	// Connect to server.
	if ( connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR){
		WSACleanup();
		return -1;
	}

	return 0;
}

int CNormalSockets::Close()
{
	//----------------------
	// Graceful close to server.
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

int CNormalSockets::Send(char *szMessage, size_t len)
{
	static DWORD ulTimestamp;
	static unsigned long ulSendTimesCount;
	//----------------------
	// Declare and initialize variables.
	int	  bytesSent;

	if(!m_nSendTimeslice) {
		//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
		bytesSent = send( ConnectSocket, szMessage, (int)len, 0);
		if (bytesSent == 0)	return -1;// Graceful close
		else if (bytesSent == SOCKET_ERROR) {
//			LOG_TRACE(_T("send() failed: %d\n"), WSAGetLastError());
			return -1;
		}
	} else {	
		int IsCalculateTimestamp = POW2N_MOD(ulSendTimesCount++, 16); 
		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount(); 

		//bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
		bytesSent = send( ConnectSocket, szMessage, (int)len, 0);
		if (bytesSent == 0)	return -1;// Graceful close
		else if (bytesSent == SOCKET_ERROR) {
//			LOG_TRACE(_T("send() failed: %d\n"), WSAGetLastError());
			return -1;
		}

		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount() - ulTimestamp; 
		if(m_nSendTimeslice > ulTimestamp) Sleep(m_nSendTimeslice - ulTimestamp);
	}

	return bytesSent;
}

int CNormalSockets::Receive(char *pRecvData, int len)
{	
	static DWORD ulTimestamp;
	static unsigned long ulSendTimesCount;	
	//----------------------
	// Declare and initialize variables.
	int		bytesRecv = 0;

	if(!m_nReceiveTimeslice) {
		//----------------------
		// Send and receive data.
		bytesRecv = recv( ConnectSocket, pRecvData, len, 0 );
		if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
			//"Connection Closed.\n";
			return -1;
		}
		//printf( "HeadBuf: %s", HeadBuf);	
	} else {
		int IsCalculateTimestamp = POW2N_MOD(ulSendTimesCount++, 16);
		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount(); 
		
		//----------------------
		// Send and receive data.
		bytesRecv = recv( ConnectSocket, pRecvData, len, 0 );
		if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
			//"Connection Closed.\n";
			return -1;
		}
		//printf( "HeadBuf: %s", HeadBuf);	

		if(!IsCalculateTimestamp) ulTimestamp = GetTickCount() - ulTimestamp; 
		if(m_nReceiveTimeslice > ulTimestamp) Sleep(m_nReceiveTimeslice - ulTimestamp);
	}

	return bytesRecv;
}

int CNormalSockets::SendReceive(char *pRecvData, char *pSendMessage)
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

int CNormalSockets::SafeSend(char *szMessage, size_t len)
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

int CNormalSockets::SafeReceive(char *pRecvData, int len)
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

int CNormalSockets::SafeSendReceive(char *pRecvData, char *pSendMessage)
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


CMsgNormalSockets::CMsgNormalSockets(void)
{
}

CMsgNormalSockets::~CMsgNormalSockets(void)
{
}

int CMsgNormalSockets::Create(struct MsgSocketsArgu *pMsgSocketsArgu)
{ return 0; }

int CMsgNormalSockets::SetAddress(char *sDestiServ, int iDestiPort)
{
	if(!sDestiServ) return -1;

	strcpy_s(m_sServName, sDestiServ);
	m_iServPort = iDestiPort;

	return 0;
}

int CMsgNormalSockets::Connect(char *sDestiServ, int iDestiPort)
{
	if(!sDestiServ) return -1;

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
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		TRACE(_T("Error at socket(): %ld\n"), WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//----------------------
	// Set colse socket timeout, for graceful close to server.
	struct linger liner;
	liner.l_onoff = TRUE;
	liner.l_linger = 16;
	setsockopt(ConnectSocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));

	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(sDestiServ);
	clientService.sin_port = htons(iDestiPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(sDestiServ);
		if (host == NULL){
			TRACE(_T("Unable to resolve server: %s\n"), sDestiServ);
			WSACleanup();
			return 1;
		}
		CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
	}
	//----------------------
	// Connect to server.
	if ( connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR){
		TRACE( _T("Failed to connect.\n") );
		WSACleanup();
		return -1;
	}

	return 0;
}

int CMsgNormalSockets::Close()
{
	//----------------------
	// Graceful close to server.
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}

int CMsgNormalSockets::Send(char *szMessage)
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

int CMsgNormalSockets::Receive(char *pRecvData)
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

int CMsgNormalSockets::SendReceive(char *pRecvData, char *pSendMessage)
{
	int retValue = -1;

	//----------------------
	// Connect to server.
	if(Connect(m_sServName, m_iServPort)) return -1;

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

int CMsgNormalSockets::SafeSend(char *szMessage)
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

int CMsgNormalSockets::SafeReceive(char *pRecvData)
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

int CMsgNormalSockets::SafeSendReceive(char *pRecvData, char *pSendMessage)
{
	int retValue = -1;

	//----------------------
	// Connect to server.
	if(Connect(m_sServName, m_iServPort)) return -1;

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