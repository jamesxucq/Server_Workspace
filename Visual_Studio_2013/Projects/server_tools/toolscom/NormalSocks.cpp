#include "StdAfx.h"
#include "NormalSocks.h"


CNormalReplySocks::CNormalReplySocks(void)
{
}

CNormalReplySocks::~CNormalReplySocks(void)
{
}

bool CNormalReplySocks::Create(char *szDestnServ, int dwDestnPort)
{
	strcpy_s(m_szDestiServ, szDestnServ);
	m_nDestiPort = dwDestnPort;

	return true;
}

int CNormalReplySocks::Connect()
{
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
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(m_szDestiServ);
	clientService.sin_port = htons(m_nDestiPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(m_szDestiServ);
		if (host == NULL){
			TRACE(_T("Unable to resolve server: %s\n"), m_szDestiServ);
			return -1;
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

int CNormalReplySocks::Connect(char *szDestnServ, int dwDestnPort)
{
	strcpy_s(m_szDestiServ, szDestnServ);
	m_nDestiPort = dwDestnPort;

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
	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	struct hostent    *host = NULL;
	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(m_szDestiServ);
	clientService.sin_port = htons(m_nDestiPort);

	//
	// If the supplied server address wasn't in the form
	// "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
	//
	if (clientService.sin_addr.s_addr == INADDR_NONE){
		host = gethostbyname(m_szDestiServ);
		if (host == NULL){
			TRACE(_T("Unable to resolve server: %s\n"), m_szDestiServ);
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

int CNormalReplySocks::SendReceive(char *pRecvData, char *pSendMessage)
{
	int	 result = -1;

	//----------------------
	// Connect to server.
	if(Connect()) return -1;

	//----------------------
	// Send and receive data
	result =Send(pSendMessage);
	if(0 > result) return -1;
	
	result = Receive(pRecvData);
	if(0 > result) return -1;

	Close();

	return result;
}

int CNormalReplySocks::Send(char *szMessage)
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

	return bytesSent;
}

int CNormalReplySocks::Receive(char *pRecvData)
{
	//----------------------
	// Declare and initialize variables.
	int		bytesRecv = 0;
	char	HeadBuff[HEAD_BUFFER];

	//----------------------
	// Send and receive data.
	//printf( "Bytes Sent: %ld\r\n", bytesSent );
	//printf( "bytesRecv: %ld\r\n", SOCKET_ERROR);
	bytesRecv = recv( ConnectSocket, HeadBuff, HEAD_BUFFER, 0 );
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ){
		//"Connection Closed.\n";
		return -1;
	}
	//printf( "HeadBuff: %s", HeadBuff);
	memcpy(pRecvData, HeadBuff, bytesRecv);
	pRecvData[bytesRecv] = '\0';

	return bytesRecv;
}

int CNormalReplySocks::Close()
{
	//----------------------
	// Graceful close to server.
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
