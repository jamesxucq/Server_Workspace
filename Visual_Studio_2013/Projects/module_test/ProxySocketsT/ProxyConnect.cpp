#include "StdAfx.h"
#include "ProxyConnect.h"

#include "Encoding.h"

#define HEAD_BUFFER_LEN		512


/*
int NProxyConnect::ConnectAuthHttp(SOCKET ProxySocket, char *sDestiServ, int iDestiPort, char *usr, char *psw) {
    BOOLEAN bNeedAuth = false;
    if (usr != NULL)
        bNeedAuth = true;
    int nLen = 0;
    char szBuffer[1024 + 1] = {""};
    if (!bNeedAuth) {
        sprintf_s(szBuffer, "CONNECT %s:%d HTTP/1.0\r\n\r\n", sDestiServ, iDestiPort);
    } else {
        //Proxy-Authorization:   Basic   
        char szAuth[1024 + 1] = {""};
        char szAuthT[1024 + 1] = {""};
        sprintf_s(szAuthT, "%s:%s", usr, psw);
		NEncoding::Base64FromBits((unsigned char*) szAuth, (unsigned char*) szAuthT, strlen(szAuthT));
        sprintf_s(szBuffer, "CONNECT %s:%d HTTP/1.0\r\nProxy-Authorization:Basic %s\r\n\r\n", sDestiServ, iDestiPort, szAuth);
    }

    nLen = strlen(szBuffer);
    send(ProxySocket, szBuffer, nLen, 0);
    nLen = 1024;
    recv(ProxySocket, szBuffer, nLen, 0);
    if (_strnicmp(szBuffer, "HTTP/1.0 200", strlen("HTTP/1.0 200")) == 0 ||
            _strnicmp(szBuffer, "HTTP/1.1 200", strlen("HTTP/1.1 200")) == 0)
        return 0;
    else
        return -1;

    return 0;
}
*/

//return value:0=success; -1=connect failed; -2=wrong usr and psw
int NProxyConnect::ConnectAuthHttp(SOCKET ProxySocket, char *sDestiServ, int iDestiPort, char *usr, char *psw) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];

    //Proxy-Authorization:   Basic   
    char szAuth[512] = {""};
    char szBase64Auth[512] = {""};
    sprintf_s(szAuth, "%s:%s", usr, psw);
	NEncoding::Base64FromBits((unsigned char*) szBase64Auth, (unsigned char*) szAuth, strlen(szAuth));

	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	//发送请求 
	sprintf_s( head_buffer, "CONNECT %s:%d HTTP/1.1\r\nHost: %s\r\nProxy-Authorization: Basic %s\r\nUser-Agent: MSIE 8.0\r\nProxy-Connection: keep-alive\r\n\r\n", sDestiServ, iDestiPort, sDestiServ, szBase64Auth); 
	//ProxySocket.Send(head_buffer, (int)strlen(head_buffer)); 
	CntSR = send( ProxySocket, head_buffer, (int)strlen(head_buffer), 0);
	if (CntSR == 0)	return -1;// Graceful close

	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	//ProxySocket.Receive(head_buffer, HEAD_BUFFER_LEN); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
		//"Connection Closed.\n";
		return -1;
	}
	
	if(strstr(head_buffer, "HTTP/1.0 200 Connection established") == NULL && strstr(head_buffer, "HTTP/1.1 200 Connection established") == NULL) //连接不成功 
		return -1;//通过代理连接主站不成功!

	return 0;
}

//return value:0=success; -1=connect failed;
int NProxyConnect::ConnectHttp(SOCKET ProxySocket, char *sDestiServ, int iDestiPort) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];

	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	//发送请求 
	sprintf_s( head_buffer, "CONNECT %s:%d HTTP/1.1\r\nHost: %s\r\nUser-Agent: MSIE 8.0\r\nProxy-Connection: keep-alive\r\n\r\n", sDestiServ, iDestiPort, sDestiServ); 
	// sprintf_s( head_buffer, "CONNECT %s:%d HTTP/1.1\r\nUser-Agent: MSIE 8.0\r\nProxy-Connection: keep-alive\r\n\r\n", sDestiServ, iDestiPort); 
	//ProxySocket.Send(head_buffer, (int)strlen(head_buffer));
	CntSR = send( ProxySocket, head_buffer, (int)strlen(head_buffer), 0);
	if (CntSR == 0)	return -1;// Graceful close

	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	//ProxySocket.Receive(head_buffer, HEAD_BUFFER_LEN); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
		//"Connection Closed.\n";
		return -1;
	}
	
	if(strstr(head_buffer, "HTTP/1.0 200 Connection established") == NULL && strstr(head_buffer, "HTTP/1.1 200 Connection established") == NULL) //连接不成功 
		return -1;//通过代理连接主站不成功!

	return 0;
}


//return value:0=success; -1=connect failed;
int NProxyConnect::ConnectSocks4(SOCKET ProxySocket, char *sDestiServ, int iDestiPort) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];
	struct Sock4Req *ProxyReq; 

	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	ProxyReq = (struct Sock4Req *)head_buffer; 
	ProxyReq->VN = 0x04; 
	ProxyReq->CD = 0x01; 
	ProxyReq->Port = ntohs(iDestiPort); 
	ProxyReq->IPAddr = inet_addr(sDestiServ); 
	//ClientSock.Send(head_buffer, 9); 
	CntSR = send( ProxySocket, head_buffer, 9, 0);
	if (CntSR == 0)	return -1;// Graceful close

	struct Sock4Ans *ProxyAns; 
	ProxyAns = (struct Sock4Ans *)head_buffer;	
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	//ClientSock.Receive(head_buffer, 100); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
		//"Connection Closed.\n";
		return -1;
	}
	if(ProxyAns->VN != 0 || ProxyAns->CD != 90) { 
		//通过代理连接主站不成功! 
		return -1;
	}

    return 0;
}


/*
int NProxyConnect::ConnectSocks5(SOCKET ProxySocket, char *sDestiServ, int iDestiPort, char *usr, char *psw) {


    int nLen = 0;
    char szBuffer[1024 + 1] = {""};

    {
        szBuffer[0] = 5;
        szBuffer[1] = 2;
        szBuffer[2] = 0;
        szBuffer[3] = 2;
        nLen = 4;
        send(ProxySocket, szBuffer, nLen, 0);
        nLen = 2;
        recv(ProxySocket, szBuffer, nLen, 0);
        char *lpszUserName = usr;
        char *lpszPassword = psw;
        if (szBuffer[0] == 5)//need auth
        {
            if (szBuffer[1] == 2) {
                szBuffer[0] = 1;
                nLen = strlen(lpszUserName);
                szBuffer[1] = nLen;
                strncpy_s(szBuffer + 2, strlen(lpszUserName)+1, lpszUserName, 1024 + 1);
                nLen += 2;
                szBuffer[nLen] = strlen(lpszPassword);
                strcpy_s(szBuffer + nLen + 1, 1024 + 1, lpszPassword);
                nLen = nLen + 1 + strlen(lpszPassword);
                send(ProxySocket, szBuffer, nLen, 0);
                nLen = 2;
                recv(ProxySocket, szBuffer, nLen, 0);
                if (szBuffer[1] != 0) {
                    return -2;
                }
            }

        }

    }

    //translate DestiAddr
    long v = inet_addr(sDestiServ);
    szBuffer[0] = 5;
    szBuffer[1] = 1;
    szBuffer[2] = 0;
    szBuffer[3] = 1; //ipv4
    szBuffer[4] = ((unsigned char *) &(v))[0]; //10;
    szBuffer[5] = ((unsigned char *) &(v))[1]; //23;
    szBuffer[6] = ((unsigned char *) &(v))[2]; //7;
    szBuffer[7] = ((unsigned char *) &(v))[3]; //197;
    unsigned short uPort = htons(iDestiPort);
    memcpy(szBuffer + 8, &uPort, 2);
    nLen = 8 + 2;
    send(ProxySocket, szBuffer, nLen, 0);
    nLen = 10;
    recv(ProxySocket, szBuffer, nLen, 0);

    if (szBuffer[0] != 5 || szBuffer[1] != 0) {
        return -1;
    } else {
        //printf("Connect socks proxy ok!\n");
        return 0;
    }

    return 0;
} */

//return value:0=success; -1=connect failed; -2=wrong usr and psw
int NProxyConnect::ConnectSocks5(SOCKET ProxySocket, char *sDestiServ, int iDestiPort, char *usr, char *psw) {
	int	 CntSR;
	char head_buffer[HEAD_BUFFER_LEN];
	struct Sock5Req1 *ProxyReq1; 

	ProxyReq1 = (struct Sock5Req1 *)head_buffer; 
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	ProxyReq1->Ver = 0x05; 
	ProxyReq1->nMethods = 0x02; 
	ProxyReq1->Methods[0] = 0x00; 
	ProxyReq1->Methods[1] = 0x02; 
	//ClientSock.Send(head_buffer, 4); 
	CntSR = send( ProxySocket, head_buffer, 4, 0);
	if (CntSR == 0)	return -1;// Graceful close
		
	struct Sock5Ans1 *ProxyAns1; 
	ProxyAns1 = (struct Sock5Ans1 *)head_buffer; 
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	//ClientSock.Receive(head_buffer,600); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
		//"Connection Closed.\n";
		return -1;
	}
	if(ProxyAns1->Ver != 0x5 || (ProxyAns1->Method!=0x0 && ProxyAns1->Method!=0x2)) { //通过代理连接主站不成功!
		return -1;
	} 
	if(ProxyAns1->Method == 0x2) { 
		char sUserName[256], sPassword[256];
		strcpy_s(sUserName, usr);
		strcpy_s(sPassword, psw);
		int nUserLen = strlen(sUserName); 
		int nPassLen = strlen(sPassword); 

		struct AuthReq *pAuthReq = NULL; 
		// pAuthReq = (struct AuthReq *)head_buffer; 
		pAuthReq = (struct AuthReq *) malloc (sizeof (struct AuthReq));
		memset(head_buffer, '\0', HEAD_BUFFER_LEN);
		pAuthReq->Ver = (char *)head_buffer;
		*pAuthReq->Ver = 0x01; 

		pAuthReq->Ulen = (pAuthReq->Ver + 0x01); 
		*(pAuthReq->Ulen) = nUserLen; 
		//strcpy(pAuthReq->Name, sUserName); 
		//pAuthReq->PLen = nPassLen; 
		//strcpy(pAuthReq->Pass, sPassword); 
		pAuthReq->Name = (pAuthReq->Ulen + 0x01);
		memcpy(pAuthReq->Name, sUserName, nUserLen);

		pAuthReq->PLen = (pAuthReq->Name + nUserLen);
		*pAuthReq->PLen = nPassLen;
	
		pAuthReq->Pass = (pAuthReq->PLen + 0x01);
		memcpy(pAuthReq->Pass, sPassword, nPassLen); 

		if(pAuthReq) free(pAuthReq);
		int nLength = 0x03 + nUserLen + nPassLen;
		//ClientSock.Send(head_buffer, 513);
		CntSR = send( ProxySocket, head_buffer, nLength, 0);
		if (CntSR == 0)	return -1;// Graceful close

		struct AuthAns *pAuthAns; 
		pAuthAns = (struct AuthAns *)head_buffer; 
		memset(head_buffer, '\0', HEAD_BUFFER_LEN);
		//ClientSock.Receive(head_buffer,600); 
		CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
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
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	ProxyReq2->Ver = 0x05; 
	ProxyReq2->Cmd = 0x01; 
	ProxyReq2->Rsv = 0x00; 
	ProxyReq2->Atyp = 0x01; 
	unsigned long tmpLong = inet_addr(sDestiServ); 
	unsigned short port = ntohs(iDestiPort); 
	memcpy((void *)ProxyReq2->other, &tmpLong, 4); 
	memcpy((void *)(ProxyReq2->other+4), &port, 2); 
	//ClientSock.Send(head_buffer,sizeof(struct sock5req2)+5); 
	CntSR = send( ProxySocket, head_buffer, sizeof(struct Sock5Req2)+5, 0);
	if (CntSR == 0)	return -1;// Graceful close	

	struct Sock5Ans2 *ProxyAns2;
	ProxyAns2 = (struct Sock5Ans2 *)head_buffer;  
	memset(head_buffer, '\0', HEAD_BUFFER_LEN);
	//ClientSock.Receive(head_buffer,600); 
	CntSR = recv( ProxySocket, head_buffer, HEAD_BUFFER_LEN, 0);
	if ( CntSR == 0 || CntSR == WSAECONNRESET ) {
		//"Connection Closed.\n";
		return -1;
	}	
	if(ProxyAns2->Ver != 0x5 || ProxyAns2->Rep != 0x0) { //通过代理连接主站不成功!
		return -1; 
	}

	return 0;
}


#define PROXY_HOST_LEN	260
int NDetectProxy::GetIEPorxyServ(TCHAR *sProxyServ, int *iProxyPort)
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
	wchar_t szProxyServ[PROXY_HOST_LEN];
	memset(szProxyServ, '\0', PROXY_HOST_LEN);
	dwCount = PROXY_HOST_LEN;
	lRst = RegQueryValueEx(hKey, _T("ProxyServer"), NULL, &dwType, (LPBYTE)szProxyServ, &dwCount);
	if(ERROR_SUCCESS != lRst) return -1;	

    RegCloseKey(hKey);
    hKey = NULL;

	wchar_t *splittoken;
	wchar_t *pValue = wcstok_s(szProxyServ, _T(":"), &splittoken);
	_tcscpy_s(sProxyServ, _tcslen(pValue)+1, pValue);
	pValue = wcstok_s(NULL, _T(":"), &splittoken);
	*iProxyPort =_tstoi(pValue);

	return 1;
}