#include "StdAfx.h"

#include <Sensapi.h>
#pragma comment(lib, "Sensapi.lib")

#include "LiveSocks.h"

enum PROXY_TYPE {
    PROXY_TYPE_HTTP = 1,
    PROXY_TYPE_HTTPS,
    PROXY_TYPE_SOCKS4,
    PROXY_TYPE_SOCKS4a,
    PROXY_TYPE_SOCKS5
};

CLiveSocks::CLiveSocks(void) {
}

CLiveSocks::~CLiveSocks(void) {
}

//

VOID CONVERT_LIVESOCKS_ARGU(struct LiveArgu *pLiveArgu, struct NetworkConfig *pNetworkConfig) {
    // _tcscpy_s(pSockeArgu->szProxyEnable, pNetworkConfig->szProxyEnable);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyEnable)) pLiveArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("auto"), pNetworkConfig->szProxyEnable)) pLiveArgu->dwProxyEnable = 0x03;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyEnable)) pLiveArgu->dwProxyEnable = 0x00;

    // _tcscpy_s(pSockeArgu->dwType, pNetworkConfig->szProxyType);
    if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTP"))) pLiveArgu->dwType = PROXY_TYPE_HTTP; // 通过HTTP方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTPS"))) pLiveArgu->dwType = PROXY_TYPE_HTTPS;
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4"))) pLiveArgu->dwType = PROXY_TYPE_SOCKS4; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4a"))) pLiveArgu->dwType = PROXY_TYPE_SOCKS4a; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS5"))) pLiveArgu->dwType = PROXY_TYPE_SOCKS5; // 通过Socks5方式代理

    strcon::ustr_ansi(pLiveArgu->tAddress.sin_addr, pNetworkConfig->tAddress.sin_addr);
    pLiveArgu->tAddress.sin_port = pNetworkConfig->tAddress.sin_port;

    // _tcscpy_s(pSockeArgu->szProxyAuth, pNetworkConfig->szProxyAuth);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyAuth)) pLiveArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyAuth)) pLiveArgu->dwProxyEnable = 0x00;
    strcon::ustr_ansi(pLiveArgu->szUserName, pNetworkConfig->szUserName);
    strcon::ustr_ansi(pLiveArgu->szPassword, pNetworkConfig->szPassword);
}

//
DWORD NLiveSocks::SWD_IsNetworkAlive() {
    DWORD   flags;
    if(!IsNetworkAlive(&flags)) { //在线
        // error or no connection
        if(ERROR_SUCCESS == GetLastError()) {
            _LOG_WARN(_T("No connection!"));
        } else {
            _LOG_WARN(_T("Connection error!"));
        }
        return 0x01;
    } else {
        if (NETWORK_ALIVE_LAN & flags) {
            // 在线 NETWORK_ALIVE_LAN
        }
        if (NETWORK_ALIVE_WAN & flags) {
            // 在线 NETWORK_ALIVE_WAN
        }
        if (NETWORK_ALIVE_AOL & flags) {
            //在线 NETWORK_ALIVE_AOL
        }
    }
    return 0x00;
}

DWORD NLiveSocks::InitialWinsock() {
    // ----------------------
    // Initialize Winsock
    WSADATA wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData)) {
        _LOG_WARN(_T("Error At WSAStartup(), Error No.: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
    _LOG_INFO(_T("WSAStartup() OK.")); // add by james 20130416
    return 0x00;
}

VOID NLiveSocks::FinishWinsock() {
    // Sleep(1024); // wait for graceful close
    // Finish Winsock
    do {
		if (NO_ERROR != WSACleanup()) {
            _LOG_WARN(_T("Last Error Is OK, Error At WSACleanup(), Error No.: %d"), WSAGetLastError());
		}
    } while(WSANOTINITIALISED != WSAGetLastError());
    _LOG_INFO(_T("WSACleanup() OK.")); // add by james 20130416
}

CLiveSocks *NLiveSocks::Factory(struct LiveArgu *pLiveArgu) {
    CLiveSocks *pLiveSocks = NULL;
//
    if(0x00 == pLiveArgu->dwProxyEnable) {
        pLiveSocks = new CLiveDirect();
        pLiveSocks->Create(pLiveArgu);
// _LOG_DEBUG(_T("no proxy.")); // disable by james 20130410
    } else if(0x01 == pLiveArgu->dwProxyEnable) {
        pLiveSocks = new CLiveProxy();
        pLiveSocks->Create(pLiveArgu);
// _LOG_DEBUG(_T("proxy to server. server:%s port:%d"), pLiveArgu->tAddress.sin_addr, pLiveArgu->tAddress.sin_port); // disable by james 20130410
    } else if(0x03 == pLiveArgu->dwProxyEnable) {
        TCHAR proxy_serv[SERVNAME_LENGTH];
        int proxy_port;
//
        MKZEO(proxy_serv);
        DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(proxy_serv, &proxy_port);
        if(0x01 == dwProxyEnable) {
            pLiveArgu->dwType = PROXY_TYPE_HTTP;
            strcon::ustr_ansi(pLiveArgu->tAddress.sin_addr, proxy_serv);
            pLiveArgu->tAddress.sin_port = proxy_port;
            pLiveArgu->dwProxyEnable = 0x00;

            pLiveSocks = new CLiveProxy();
            pLiveSocks->Create(pLiveArgu);
// _LOG_DEBUG(_T("get ie proxy ok! server:%s port:%d"), proxy_serv, proxy_port); // disable by james 20130410
        } else if(0x00 == dwProxyEnable) {
            pLiveSocks = new CLiveDirect();
            pLiveSocks->Create(pLiveArgu);
// _LOG_DEBUG(_T("get ie proxy ok! no proxy! server:%s port:%d"), pLiveArgu->tAddress.sin_addr, pLiveArgu->tAddress.sin_port); // disable by james 20130410
        }
    }
//
    return pLiveSocks;
}

VOID NLiveSocks::DestroyObject(CLiveSocks *pLiveSocks) {
    delete pLiveSocks;
}

//
CLiveProxy::CLiveProxy(void):
    m_port(0)
    ,ConnectSocket()
{
    memset(m_address, '\0', SERVNAME_LENGTH);
    memset(&m_tLiveArgu, 0, sizeof(struct LiveArgu));
}

CLiveProxy::~CLiveProxy(void) {
}


DWORD CLiveProxy::Create(struct LiveArgu *pLiveArgu) {
    if(!pLiveArgu) return ((DWORD)-1);
    memcpy(&m_tLiveArgu, pLiveArgu, sizeof(struct LiveArgu));
    return 0x00;
}

DWORD CLiveProxy::Connect(char *address, int port) {
_LOG_DEBUG(_T("new connection!"));
    if (-1 == (ConnectSocket=ProxyConnect(m_tLiveArgu.tAddress.sin_addr, m_tLiveArgu.tAddress.sin_port))) {
        _LOG_WARN(_T("Error At ProxyConnect()"));
        return ((DWORD)-1);
    }
//
    if (ServConnect(ConnectSocket, m_tLiveArgu.dwType, address, port, m_tLiveArgu.dwProxyEnable, m_tLiveArgu.szUserName, m_tLiveArgu.szPassword)) {
        _LOG_WARN(_T("Error At ServConnect()"));
        return ((DWORD)-1);
    }
//
    return 0x00;
}

SOCKET CLiveProxy::ProxyConnect(char *proxy_serv, int proxy_port) {
    SOCKET ProxySocket;
    if(!proxy_serv) return ((DWORD)-1);
    // ----------------------
    // Create a SOCKET for connecting to server
    // SOCKET ConnectSocket;
    ProxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ProxySocket) {
        _LOG_WARN(_T("error at socket(): %ld"), WSAGetLastError());
        return ((DWORD)-1);
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
    struct hostent *host = NULL;
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(proxy_serv);
    clientService.sin_port = htons((u_short)proxy_port);
    //
    // If the supplied server address wasn't in the form
    // "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
    if (clientService.sin_addr.s_addr == INADDR_NONE) {
        host = gethostbyname(proxy_serv);
        if (NULL == host) {
            _LOG_WARN(_T("unable to resolve server: %s"), proxy_serv);
            return ((DWORD)-1);
        }
        CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
    }
    // ----------------------
    // Connect to server.
    if ( SOCKET_ERROR == connect( ProxySocket, (SOCKADDR*) &clientService, sizeof(clientService) )) {
		_LOG_WARN(_T("failed to connect.ecode:%d"), WSAGetLastError());
        return ((DWORD)-1); //不能连接到代理服务器!
    }
//
    return ProxySocket;
}

DWORD CLiveProxy::ServConnect(SOCKET ProxySocket, enum PROXY_TYPE dwProxyType, char *address, int port, int szProxyAuth, char *user, char *pword) {
    if(!address) return ((DWORD)-1);
    // HTTP;HTTPS;SOCKS4;SOCKS5;
    switch( dwProxyType ) {
    case PROXY_TYPE_HTTP:
        if(szProxyAuth) {
            if(NProxyConnect::ConnectHttpAuth(ProxySocket, address, port, user, pword)) {
                _LOG_WARN( _T("Failed http proxy auth.") );
                return ((DWORD)-1);
            }
        } else {
            if(NProxyConnect::ConnectHttp(ProxySocket, address, port)) {
                _LOG_WARN( _T("Failed http proxy.") );
                return ((DWORD)-1);
            }
        }
        break;
    case PROXY_TYPE_HTTPS:
        break;
    case PROXY_TYPE_SOCKS4:
        if(NProxyConnect::ConnectSocks4(ProxySocket, address, port)) {
            _LOG_WARN( _T("Failed socksr proxy.") );
            return ((DWORD)-1);
        }
        break;
    case PROXY_TYPE_SOCKS4a:
        break;
    case PROXY_TYPE_SOCKS5:
        if(NProxyConnect::ConnectSocks5(ProxySocket, address, port, user, pword)) {
            _LOG_WARN( _T("Failed socks5 proxy auth.") );
            return ((DWORD)-1);
        }
        break;
    }
//
    return 0x00;
}

VOID CLiveProxy::Close() {
    // ----------------------
    // Graceful close to server.
_LOG_DEBUG(_T("close connection!"));
    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
}

DWORD CLiveProxy::Send(char *szMessage, size_t length) {
    int	  bytesSent;
    // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
    bytesSent = send( ConnectSocket, szMessage, length, 0);
    if (0 == bytesSent)	return 0x00;// Graceful close
    else if (SOCKET_ERROR == bytesSent) {
        _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return bytesSent;
}

DWORD CLiveProxy::Receive(char *szReceive, int length) {
    // ----------------------
    // Declare and initialize variables.
    int bytesRecv = 0;
    // ----------------------
    // Send and receive data.
    // fprintf(stderr, "Bytes Sent: %ld\r\n", bytesSent );
    // fprintf(stderr, "bytesRecv: %ld\r\n", SOCKET_ERROR);
    bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
    if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
        // "Connection Closed.\n";
        return ((DWORD)-1);
    }
    // fprintf(stderr, "head_buffer: %s", head_buffer);
//
    return bytesRecv;
}

//
CLiveDirect::CLiveDirect(void):
    m_port(0)
    ,ConnectSocket()
{
    memset(m_address, '\0', SERVNAME_LENGTH);
}

CLiveDirect::~CLiveDirect(void) {
}

DWORD CLiveDirect::Create(struct LiveArgu *pLiveArgu) {
    return 0x00;
}

DWORD CLiveDirect::Connect(char *address, int port) {
    if(!address) return ((DWORD)-1);
    // ----------------------
    // Create a SOCKET for connecting to server
    // SOCKET ConnectSocket;
_LOG_DEBUG(_T("new connection!"));
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ConnectSocket) {
        _LOG_WARN(_T("error at socket(): %ld"), WSAGetLastError());
        return ((DWORD)-1);
    }
    // ----------------------
    // Set colse socket timeout, for graceful close to server.
    struct linger liner;
    liner.l_onoff = TRUE;
    liner.l_linger = 16;
    setsockopt(ConnectSocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));
    // ----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    struct hostent *host = NULL;
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(address);
    clientService.sin_port = htons(port);
    //
    // If the supplied server address wasn't in the form
    // "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
    //
    if (clientService.sin_addr.s_addr == INADDR_NONE) {
        host = gethostbyname(address);
        if (NULL == host) {
            _LOG_WARN(_T("unable to resolve server: %s"), address);
            return 0x01;
        }
        CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
    }
    // ----------------------
    // Connect to server.
    if ( SOCKET_ERROR == connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) )) {
		_LOG_WARN(_T("failed to connect.ecode:%d"), WSAGetLastError());
        return ((DWORD)-1);
    }

    return 0x00;
}

VOID CLiveDirect::Close() {
    // ----------------------
    // Graceful close to server.
_LOG_DEBUG(_T("close connection!"));
    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
}

DWORD CLiveDirect::Send(char *szMessage, size_t length) {
    int	  bytesSent;
    // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
    bytesSent = send( ConnectSocket, szMessage, length, 0);
    if (0 == bytesSent)	return 0x00;// Graceful close
    else if (SOCKET_ERROR == bytesSent) {
        _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return bytesSent;
}

DWORD CLiveDirect::Receive(char *szReceive, int length) {
    //----------------------
    // Declare and initialize variables.
    int bytesRecv = 0;
    //----------------------
    // Send and receive data.
    // fprintf(stderr, "Bytes Sent: %ld\r\n", bytesSent );
    // fprintf(stderr, "bytesRecv: %ld\r\n", SOCKET_ERROR);
    bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
    if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
        // "Connection Closed.\n";
        return ((DWORD)-1);
    }
    // fprintf(stderr, "head_buffer: %s", head_buffer);
//
    return bytesRecv;
}
