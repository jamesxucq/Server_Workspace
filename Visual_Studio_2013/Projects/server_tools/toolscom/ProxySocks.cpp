#include "StdAfx.h"
#include "ProxySocks.h"


CProxyReplySocks::CProxyReplySocks(void)
{
}

CProxyReplySocks::~CProxyReplySocks(void)
{
}


bool CProxyReplySocks::Create(char *szDestnServ, int dwDestnPort, struct NetworkConf *pNetworkConf)
{
	strcpy_s(m_szDestiServ, szDestnServ);
	m_nDestiPort = dwDestnPort;
	m_pNetworkConf = pNetworkConf;

	return true;
}

int CProxyReplySocks::Connect()
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
	clientService.sin_addr.s_addr = inet_addr(striconv::cstring_char(m_pNetworkConf->szServer));
	clientService.sin_port = htons((u_short)m_pNetworkConf->nPort);

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
		return -1; //不能连接到代理服务器!
	}
	//proxy auth
	if(ProxyAuth()) {
		TRACE( _T("Failed ProxyAuth.\n") );
		WSACleanup();
		return -1;
	}

	return 0;
}

int CProxyReplySocks::ProxyAuth()
{
	int	 CntSR;
	char head_buffer[HEAD_BUFFER];

	//HTTP;HTTPS;SOCKS4;SOCKS5;
	if(m_pNetworkConf->szProxyType == _T("HTTP")) { //通过HTTP方式代理 
		memset(head_buffer, '\0', HEAD_BUFFER);
		sprintf_s( head_buffer, "%s%s:%d%s","CONNECT ",m_szDestiServ,m_nDestiPort,"HTTP/1.1\r\nUser-Agent: MSIE 8.0\r\n\r\n"); 
		//ConnectSocket.Send(head_buffer, strlen(head_buffer)); //发送请求 
		CntSR = send( ConnectSocket, head_buffer, (int)strlen(head_buffer), 0);
		if (CntSR == 0)	return -1;// Graceful close

		memset(head_buffer, '\0', HEAD_BUFFER);
		//ConnectSocket.Receive(head_buffer, HEAD_BUFFER); 
		CntSR = recv( ConnectSocket, head_buffer, HEAD_BUFFER, 0 );
		if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
			//"Connection Closed.\n";
			return -1;
		}
		if(strstr(head_buffer, "HTTP/1.0 200 Connection established") == NULL) //连接不成功 
			return -1;//通过代理连接主站不成功!
	}
	else if(m_pNetworkConf->szProxyType == _T("HTTPS")) {

	}
	else if(m_pNetworkConf->szProxyType == _T("SOCKS4")) {//通过Socks4方式代理 
		struct Sock4Req *ProxyReq; 
		memset(head_buffer, '\0', HEAD_BUFFER);
		ProxyReq = (struct Sock4Req *)head_buffer; 
		ProxyReq->VN = 0x04; 
		ProxyReq->CD = 1; 
		ProxyReq->Port = ntohs(m_nDestiPort); 
		ProxyReq->IPAddr = inet_addr(m_szDestiServ); 
		//ClientSock.Send(head_buffer, 9); 
		CntSR = send( ConnectSocket, head_buffer, 9, 0);
		if (CntSR == 0)	return -1;// Graceful close

		struct Sock4Ans *ProxyAns; 
		ProxyAns = (struct Sock4Ans *)head_buffer;	
		memset(head_buffer, '\0', HEAD_BUFFER);
		//ClientSock.Receive(head_buffer, 100); 
		CntSR = recv( ConnectSocket, head_buffer, HEAD_BUFFER, 0 );
		if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
			//"Connection Closed.\n";
			return -1;
		}
		if(ProxyAns->VN != 0 || ProxyAns->CD != 90) { //通过代理连接主站不成功! 
			return -1;
		}
	}
	else if(m_pNetworkConf->szProxyType == _T("SOCKS5")) {//通过Socks5方式代理 
		struct Sock5Req1 *ProxyReq1; 
		ProxyReq1 = (struct Sock5Req1 *)head_buffer; 
		ProxyReq1->Ver = 0x05; 
		ProxyReq1->nMethods = 0x02; 
		ProxyReq1->Method1 = 0x00; 
		ProxyReq1->Method2 = 0x02; 
		//ClientSock.Send(head_buffer, 4); 
		CntSR = send( ConnectSocket, head_buffer, 4, 0);
		if (CntSR == 0)	return -1;// Graceful close

		struct Sock5Ans1 *ProxyAns1; 
		ProxyAns1 = (struct Sock5Ans1 *)head_buffer; 
		memset(head_buffer, '\0', HEAD_BUFFER);
		//ClientSock.Receive(head_buffer,600); 
		CntSR = recv( ConnectSocket, head_buffer, HEAD_BUFFER, 0 );
		if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
			//"Connection Closed.\n";
			return -1;
		}
		if(ProxyAns1->Ver != 0x5 || (ProxyAns1->Method!=0x0 && ProxyAns1->Method!=0x2)) { //通过代理连接主站不成功!
			return -1;
		} 
		if(ProxyAns1->Method == 0x2) { 
			char szUName[USERNAME_LENGTH], szPassWD[PASSWORD_LENGTH];
			striconv::cstring_char(szUName, m_pNetworkConf->szUName);
			striconv::cstring_char(szPassWD, m_pNetworkConf->szPassWD);
			int iUserLen = strlen(szUName); 
			int iPassLen = strlen(szPassWD); 

			struct AuthReq *pAuthReq; 
			pAuthReq = (struct AuthReq *)head_buffer; 
			pAuthReq->Ver = 1; 

			pAuthReq->Ulen = (&pAuthReq->Ver + 0x01); 
			*(pAuthReq->Ulen) = iUserLen; 
			//strcpy(pAuthReq->Name, m_pNetworkConf->szUName); 
			//pAuthReq->PLen = iPassLen; 
			//strcpy(pAuthReq->Pass, m_pNetworkConf->szPassWD); 
			pAuthReq->Name = (pAuthReq->Ulen + 0x01);
			memcpy(pAuthReq->Name, szUName, iUserLen);

			pAuthReq->PLen = (pAuthReq->Name + iUserLen + 0x01);
			*pAuthReq->PLen = iPassLen;

			pAuthReq->Pass = (pAuthReq->PLen + 0x01);
			memcpy(pAuthReq->Pass, szPassWD, iPassLen); 	
			//ClientSock.Send(head_buffer, 513);
			CntSR = send( ConnectSocket, head_buffer, HEAD_BUFFER, 0);
			if (CntSR == 0)	return -1;// Graceful close

			struct AuthAns *pAuthAns; 
			pAuthAns = (struct AuthAns *)head_buffer; 
			memset(head_buffer, '\0', HEAD_BUFFER);
			//ClientSock.Receive(head_buffer,600); 
			CntSR = recv( ConnectSocket, head_buffer, HEAD_BUFFER, 0 );
			if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
				//"Connection Closed.\n";
				return -1;
			}			
			if(pAuthAns->Ver != 0x1 || pAuthAns->Status != 0x0) { //代理服务器用户验证不成功!
				return -1;
			} 
		} 
		struct Sock5Req2 *ProxyReq2; 
		ProxyReq2 = (struct Sock5Req2 *)head_buffer; 
		ProxyReq2->Ver = 0x5; 
		ProxyReq2->Cmd = 0x1; 
		ProxyReq2->Rsv = 0x0; 
		ProxyReq2->Atyp = 0x1; 
		unsigned long tmpLong = inet_addr(m_szDestiServ); 
		unsigned short port = ntohs(m_nDestiPort); 
		memcpy((void *)ProxyReq2->other, &tmpLong, 4); 
		memcpy((void *)(ProxyReq2->other+4), &port, 2); 
		//ClientSock.Send(head_buffer,sizeof(struct sock5req2)+5); 
		CntSR = send( ConnectSocket, head_buffer, sizeof(struct Sock5Req2)+5, 0);
		if (CntSR == 0)	return -1;// Graceful close	

		struct Sock5Ans2 *ProxyAns2; 
		memset(head_buffer, '\0', HEAD_BUFFER);
		ProxyAns2 = (struct Sock5Ans2 *)head_buffer; 
		//ClientSock.Receive(head_buffer,600); 
		CntSR = recv( ConnectSocket, head_buffer, HEAD_BUFFER, 0 );
		if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
			//"Connection Closed.\n";
			return -1;
		}	
		if(ProxyAns2->Ver != 0x5 || ProxyAns2->Rep != 0x0) { //通过代理连接主站不成功!
			return -1; 
		}
	}

	return 0;
}

int CProxyReplySocks::SendReceive(char *pRecvData, char *pSendMessage)
{
	int	 result = -1;

	//----------------------
	// Connect to server.
	if(Connect()) return -1;

	//----------------------
	// Send and receive data
	result = Send(pSendMessage);
	if(0 > result) return -1;
	
	result = Receive(pRecvData);	
	if(0 > result) return -1;

	Close();

	return 0;
}

int CProxyReplySocks::Send(char *szMessage)
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

int CProxyReplySocks::Receive(char *pRecvData)
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

int CProxyReplySocks::Close()
{
	//----------------------
	// Close to server.
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}


#define PROXY_HOST_LEN	260
DWORD NDetectProxy::GetIEPorxyServ(TCHAR *szProxyServ, int *nProxyPort)
{
	HKEY hKey = NULL;
	long lRst = 0;

	lRst = RegOpenKeyEx(HKEY_CURRENT_USER,   
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),   
		0,   
		KEY_CREATE_LINK | KEY_WRITE | KEY_READ | KEY_NOTIFY,   
		&hKey);
	if(ERROR_SUCCESS != lRst) return -1; 

	DWORD dwType = REG_DWORD; 
	DWORD dwCount = sizeof(DWORD);
	DWORD dwProxyEnable;   
	lRst = RegQueryValueEx(hKey, _T("ProxyEnable"), NULL, &dwType, (LPBYTE)&dwProxyEnable, &dwCount);
	if(ERROR_SUCCESS != lRst) return -1;
	if(0x01 != dwProxyEnable) return 0;

	dwType = REG_SZ; 
	wchar_t szProxyServer[PROXY_HOST_LEN];
	memset(szProxyServer, '\0', PROXY_HOST_LEN);
	dwCount = PROXY_HOST_LEN;
	lRst = RegQueryValueEx(hKey, _T("ProxyServer"), NULL, &dwType, (LPBYTE)szProxyServer, &dwCount);
	if(ERROR_SUCCESS != lRst) return -1;	

	RegCloseKey(hKey);
	hKey = NULL;

	wchar_t *splittoken;
	wchar_t *pValue = wcstok_s(szProxyServer, _T(":"), &splittoken);
	_tcscpy_s(szProxyServ, _tcslen(pValue)+1, pValue);
	pValue = wcstok_s(NULL, _T(":"), &splittoken);
	*nProxyPort =_tstoi(pValue);

	return 1;
}