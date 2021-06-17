#include "StdAfx.h"
#include "common_macro.h"
#include "ProxyConnect.h"

#include "Encoding.h"

#define HEAD_BUFFER_LEN		1024

/*
int NProxyConnect::ConnectHttpAuth(SOCKET ProxySocket, char *szAddress, int iPort, char *usr, char *psw) {
BOOLEAN bNeedAuth = false;
if (NULL != usr)
bNeedAuth = true;
int iLen = 0;
char szBuffer[1024 + 1] = {""};
if (!bNeedAuth) {
sprintf_s(szBuffer, "CONNECT %s:%d HTTP/1.0\r\n\r\n", szAddress, iPort);
} else {
// Proxy-Authorization:   Basic   
char szAuth[1024 + 1] = {""};
char szAuthT[1024 + 1] = {""};
sprintf_s(szAuthT, "%s:%s", usr, psw);
NEncoding::Base64FromBits((unsigned char*) szAuth, (unsigned char*) szAuthT, strlen(szAuthT));
sprintf_s(szBuffer, "CONNECT %s:%d HTTP/1.0\r\nProxy-Authorization:Basic %s\r\n\r\n", szAddress, iPort, szAuth);
}

iLen = strlen(szBuffer);
send(ProxySocket, szBuffer, iLen, 0);
iLen = 1024;
recv(ProxySocket, szBuffer, iLen, 0);
if (0 == _strnicmp(szBuffer, "HTTP/1.0 200", strlen("HTTP/1.0 200")) ||
0 == _strnicmp(szBuffer, "HTTP/1.1 200", strlen("HTTP/1.1 200")))
return 0;
else
return ((DWORD)-1);

return 0;
}
*/

// return value:0=success; -1=connect failed; -2=wrong user and pword
DWORD NProxyConnect::ConnectHttpAuth(SOCKET ProxySocket, char *address, int port, char *user, char *pword) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];
//
	// Proxy-Authorization:   Basic   
	char szAuth[512] = {""};
	char szBase64Auth[512] = {""};
	sprintf_s(szAuth, "%s:%s", user, pword);
	NEncoding::Base64FromBits((unsigned char*) szBase64Auth, (unsigned char*) szAuth, strlen(szAuth));
//
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	// 发送请求 
	sprintf_s( head_buffer, "CONNECT %s:%d HTTP/1.1\r\nHost: %s\r\nProxy-Authorization: Basic %s\r\nUser-Agent: MSIE 8.0\r\nProxy-Connection: keep-alive\r\n\r\n", address, port, address, szBase64Auth); 
	// ProxySocket.Send(head_buffer, (int)strlen(head_buffer)); 
	CntSR = send( ProxySocket, head_buffer, (int)strlen(head_buffer), 0);
	if (0 == CntSR || SOCKET_ERROR == CntSR)	return ((DWORD)-1);// Graceful close
//
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	// ProxySocket.Receive(head_buffer, HEAD_BUFFER_LEN); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( 0 == CntSR || SOCKET_ERROR == CntSR || WSAECONNRESET == CntSR ) {
		// "Connection Closed.\n";
		return ((DWORD)-1);
	}
//
	if(NULL == strstr(head_buffer, "HTTP/1.0 200 Connection established") && NULL == strstr(head_buffer, "HTTP/1.1 200 Connection established")) //连接不成功 
		return ((DWORD)-1);// 通过代理连接主站不成功!
//
	return 0x00;
}

// return value:0=success; -1=connect failed;
DWORD NProxyConnect::ConnectHttp(SOCKET ProxySocket, char *address, int port) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];
//
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	// 发送请求 
	sprintf_s( head_buffer, "CONNECT %s:%d HTTP/1.1\r\nHost: %s\r\nUser-Agent: MSIE 8.0\r\nProxy-Connection: keep-alive\r\n\r\n", address, port, address); 
	// ProxySocket.Send(head_buffer, (int)strlen(head_buffer)); 	
	CntSR = send( ProxySocket, head_buffer, (int)strlen(head_buffer), 0);
	if (0 == CntSR || SOCKET_ERROR == CntSR)	return ((DWORD)-1);// Graceful close
//
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	// ProxySocket.Receive(head_buffer, HEAD_BUFFER_LEN); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( 0 == CntSR || SOCKET_ERROR == CntSR || WSAECONNRESET == CntSR ) {
		// "Connection Closed.\n";
		return ((DWORD)-1);
	}
//
	if(NULL == strstr(head_buffer, "HTTP/1.0 200 Connection established") && NULL == strstr(head_buffer, "HTTP/1.1 200 Connection established")) //连接不成功 
		return ((DWORD)-1);// 通过代理连接主站不成功!
//
	return 0x00;
}


// return value:0=success; -1=connect failed;
DWORD NProxyConnect::ConnectSocks4(SOCKET ProxySocket, char *address, int port) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];
	struct Sock4Req *ProxyReq; 
//
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	ProxyReq = (struct Sock4Req *)head_buffer; 
	ProxyReq->VN = 0x04; 
	ProxyReq->CD = 0x01; 
	ProxyReq->Port = ntohs(port); 
	ProxyReq->IPAddr = inet_addr(address); 
	// ClientSock.Send(head_buffer, 9); 
	CntSR = send( ProxySocket, head_buffer, 9, 0);
	if (0 == CntSR || SOCKET_ERROR == CntSR) return ((DWORD)-1);// Graceful close
//
	struct Sock4Ans *ProxyAns; 
	ProxyAns = (struct Sock4Ans *)head_buffer;	
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	// ClientSock.Receive(head_buffer, 100); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( 0 == CntSR || SOCKET_ERROR == CntSR || WSAECONNRESET == CntSR ) {
		// "Connection Closed.\n";
		return ((DWORD)-1);
	}
	if(ProxyAns->VN != 0 || ProxyAns->CD != 90) { 
		// 通过代理连接主站不成功! 
		return ((DWORD)-1);
	}
//
	return 0x00;
}

/*
int NProxyConnect::ConnectSocks5(SOCKET ProxySocket, char *szAddress, int iPort, char *usr, char *psw) {

int iLen = 0;
char szBuffer[1024 + 1] = {""};

{
szBuffer[0] = 5;
szBuffer[1] = 2;
szBuffer[2] = 0;
szBuffer[3] = 2;
iLen = 4;
send(ProxySocket, szBuffer, iLen, 0);
iLen = 2;
recv(ProxySocket, szBuffer, iLen, 0);
char *lpszUserName = usr;
char *lpszPassword = psw;
if (szBuffer[0] == 5) // need auth
{
if (szBuffer[1] == 2) {
szBuffer[0] = 1;
iLen = strlen(lpszUserName);
szBuffer[1] = iLen;
strncpy_s(szBuffer + 2, strlen(lpszUserName)+1, lpszUserName, 1024 + 1);
iLen += 2;
szBuffer[iLen] = strlen(lpszPassword);
strcpy_s(szBuffer + iLen + 1, 1024 + 1, lpszPassword);
iLen = iLen + 1 + strlen(lpszPassword);
send(ProxySocket, szBuffer, iLen, 0);
iLen = 2;
recv(ProxySocket, szBuffer, iLen, 0);
if (szBuffer[1] != 0) {
return -2;
}
}

}

}

// translate ToAddr
long v = inet_addr(szAddress);
szBuffer[0] = 5;
szBuffer[1] = 1;
szBuffer[2] = 0;
szBuffer[3] = 1; // ipv4
szBuffer[4] = ((unsigned char *) &(v))[0]; // 10;
szBuffer[5] = ((unsigned char *) &(v))[1]; // 23;
szBuffer[6] = ((unsigned char *) &(v))[2]; // 7;
szBuffer[7] = ((unsigned char *) &(v))[3]; // 197;
unsigned short uPort = htons(iPort);
memcpy(szBuffer + 8, &uPort, 2);
iLen = 8 + 2;
send(ProxySocket, szBuffer, iLen, 0);
iLen = 10;
recv(ProxySocket, szBuffer, nLen, 0);

if (szBuffer[0] != 5 || szBuffer[1] != 0) {
return ((DWORD)-1);
} else {
// fprintf(stderr, "Connect socks proxy ok!\n");
return 0;
}

return 0;
} */

// return value:0=success; -1=connect failed; -2=wrong user and pword
DWORD NProxyConnect::ConnectSocks5(SOCKET ProxySocket, char *address, int port, char *user, char *pword) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];
	struct Sock5Req1 *ProxyReq1; 
//
	ProxyReq1 = (struct Sock5Req1 *)head_buffer; 
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	ProxyReq1->Ver = 0x05; 
	ProxyReq1->nMethods = 0x02; 
	ProxyReq1->Methods[0] = 0x00; 
	ProxyReq1->Methods[1] = 0x02; 
	// ClientSock.Send(head_buffer, 4); 
	CntSR = send( ProxySocket, head_buffer, 4, 0);
	if (0 == CntSR || SOCKET_ERROR == CntSR) return ((DWORD)-1);// Graceful close
//
	struct Sock5Ans1 *ProxyAns1; 
	ProxyAns1 = (struct Sock5Ans1 *)head_buffer; 
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	// ClientSock.Receive(head_buffer,600); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( 0 == CntSR || SOCKET_ERROR == CntSR || WSAECONNRESET == CntSR ) {
		// "Connection Closed.\n";
		return ((DWORD)-1);
	}
	if(ProxyAns1->Ver != 0x5 || (ProxyAns1->Method!=0x0 && ProxyAns1->Method!=0x2)) { //通过代理连接主站不成功!
		return ((DWORD)-1);
	} 
	if(ProxyAns1->Method == 0x2) { 
		char szUserName[260], szPassword[260];
		strcpy_s(szUserName, user);
		strcpy_s(szPassword, pword);
		int user_length = strlen(szUserName); 
		int pword_Length = strlen(szPassword); 

		struct AuthReq *pAuthReq = NULL, AuthRequest; 
		// pAuthReq = (struct AuthReq *)head_buffer; 
		pAuthReq = &AuthRequest;
		memset(head_buffer, '\0', HEAD_BUFFER_LEN);
		pAuthReq->Ver = (char *)head_buffer;
		*pAuthReq->Ver = 0x01; 
//
		pAuthReq->Ulen = (pAuthReq->Ver + 0x01); 
		*(pAuthReq->Ulen) = user_length; 
		// strcpy(pAuthReq->Name, szUserName); 
		// pAuthReq->PLen = pword_Length; 
		// strcpy(pAuthReq->Pass, szPassword); 
		pAuthReq->Name = (pAuthReq->Ulen + 0x01);
		memcpy(pAuthReq->Name, szUserName, user_length);
//
		pAuthReq->PLen = (pAuthReq->Name + user_length);
		*pAuthReq->PLen = pword_Length;
//
		pAuthReq->Pass = (pAuthReq->PLen + 0x01);
		memcpy(pAuthReq->Pass, szPassword, pword_Length); 
//
		int iLeng = 0x03 + user_length + pword_Length;
		// ClientSock.Send(head_buffer, 513);
		CntSR = send( ProxySocket, head_buffer, iLeng, 0);
		if (0 == CntSR || SOCKET_ERROR == CntSR) return ((DWORD)-1);// Graceful close
//
		struct AuthAns *pAuthAns; 
		pAuthAns = (struct AuthAns *)head_buffer; 
		memset(head_buffer, '\0', HEAD_BUFFER_LEN);
		// ClientSock.Receive(head_buffer,600); 
		CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
		if ( 0 == CntSR || SOCKET_ERROR == CntSR || WSAECONNRESET == CntSR ) {
			// "Connection Closed.\n";
			return ((DWORD)-1);
		}			
		if(pAuthAns->Ver != 0x1 || pAuthAns->Status != 0x0) { //代理服务器用户验证不成功!
			return ((DWORD)-1);
		} 
	} 

	struct Sock5Req2 *ProxyReq2; 
	ProxyReq2 = (struct Sock5Req2 *)head_buffer; 
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	ProxyReq2->Ver = 0x05; 
	ProxyReq2->Cmd = 0x01; 
	ProxyReq2->Rsv = 0x00; 
	ProxyReq2->Atyp = 0x01; 
	unsigned long tmpLong = inet_addr(address); 
	unsigned short iport = ntohs(port); 
	memcpy((void *)ProxyReq2->other, &tmpLong, 4); 
	memcpy((void *)(ProxyReq2->other+4), &iport, 2); 
	// ClientSock.Send(head_buffer,sizeof(struct sock5req2)+5); 
	CntSR = send( ProxySocket, head_buffer, sizeof(struct Sock5Req2)+5, 0);
	if (0 == CntSR || SOCKET_ERROR == CntSR) return ((DWORD)-1);// Graceful close	
//
	struct Sock5Ans2 *ProxyAns2;
	ProxyAns2 = (struct Sock5Ans2 *)head_buffer;  
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	// ClientSock.Receive(head_buffer,600); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( 0 == CntSR || SOCKET_ERROR == CntSR || WSAECONNRESET == CntSR ) {
		// "Connection Closed.\n";
		return ((DWORD)-1);
	}	
	if(ProxyAns2->Ver != 0x5 || ProxyAns2->Rep != 0x0) { // 通过代理连接主站不成功!
		return ((DWORD)-1); 
	}
//
	return 0x00;
}

#define PROXY_HOST_LEN	260
DWORD NDetectProxy::GetIEPorxyServ(TCHAR *proxy_serv, int *proxy_port) {
	HKEY hKey = NULL;
	long lResult = 0;
//
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER,   
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),   
		0,   
		KEY_CREATE_LINK | KEY_WRITE | KEY_READ | KEY_NOTIFY,   
		&hKey);
	if(ERROR_SUCCESS != lResult) return ((DWORD)-1); 
//
	DWORD dwType = REG_DWORD; 
	DWORD dwCount = sizeof(DWORD);
	DWORD dwProxyEnable;   
	lResult = RegQueryValueEx(hKey, _T("ProxyEnable"), NULL, &dwType, (LPBYTE)&dwProxyEnable, &dwCount);
	if(ERROR_SUCCESS != lResult) {
		RegCloseKey(hKey);
		return ((DWORD)-1);
	}
	RegCloseKey(hKey);
	if(0x01 != dwProxyEnable) return 0x00;
//
	dwType = REG_SZ; 
	TCHAR szProxyServer[PROXY_HOST_LEN];
	MKZEO(szProxyServer);
	dwCount = PROXY_HOST_LEN;
	lResult = RegQueryValueEx(hKey, _T("ProxyServer"), NULL, &dwType, (LPBYTE)szProxyServer, &dwCount);
	if(ERROR_SUCCESS != lResult) {
		RegCloseKey(hKey);
		return ((DWORD)-1);
	}
	RegCloseKey(hKey);
	hKey = NULL;
//
	TCHAR *splittok;
	TCHAR *pValue = wcstok_s(szProxyServer, _T(":"), &splittok);
	_tcscpy_s(proxy_serv, SERVNAME_LENGTH, pValue);
	pValue = wcstok_s(NULL, _T(":"), &splittok);
	*proxy_port =_tstoi(pValue);
//
	return 0x01;
}