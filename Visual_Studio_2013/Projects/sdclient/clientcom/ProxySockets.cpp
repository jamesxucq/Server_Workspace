#include "StdAfx.h"
#include "ProxySockets.h"

CProxySockets::CProxySockets(void):
    ConnectSocket(INVALID_SOCKET)
    ,m_lSendTimeslice(0)
    ,m_lReceiveTimeslice(0)
{
    memset(&m_tSockeArgu, 0, sizeof(struct SocketsArgu));
}

CProxySockets::~CProxySockets(void) {
}

DWORD CProxySockets::Create(struct SocketsArgu *pSockeArgu) {
    if(!pSockeArgu) return ((DWORD)-1);
    memcpy(&m_tSockeArgu, pSockeArgu, sizeof(struct SocketsArgu));
//
    unsigned long ulDowndRate = 0x00, ulUploadRate = 0x00;
    if(pSockeArgu->dwDowndLimit)
        ulDowndRate = pSockeArgu->dwDowndRate / pSockeArgu->dwWorkerThreads;
    else ulDowndRate = 0x00;
//
    if(0x01 == pSockeArgu->dwUploadLimit)
        ulUploadRate = pSockeArgu->dwUploadRate / pSockeArgu->dwWorkerThreads;
    else if(0x00 == pSockeArgu->dwUploadLimit || 0x03 == pSockeArgu->dwUploadLimit) ulUploadRate = 0x00;
//
    return SetSendReceiveSpeed(ulDowndRate, ulUploadRate);
}

DWORD CProxySockets::SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend) {
    if(!ulMaxSend) m_lSendTimeslice = 0;
    else if(8 > ulMaxSend) m_lSendTimeslice = 1000;
    else m_lSendTimeslice = 1000 / (ulMaxSend>>3);
//
    if(!ulMaxReceive) m_lReceiveTimeslice = 0;
    else if(8 > ulMaxReceive) m_lReceiveTimeslice = 1000;
    else m_lReceiveTimeslice = 1000 / (ulMaxReceive>>3);
//
    return 0x00;
}


DWORD CProxySockets::Connect(char *address, int port) {
_LOG_DEBUG(_T("new connection!"));
    if (-1 == (ConnectSocket=ProxyConnect(m_tSockeArgu.tAddress.sin_addr, m_tSockeArgu.tAddress.sin_port))) {
        _LOG_WARN(_T("Error At ProxyConnect()"));
        return ((DWORD)-1);
    }
//
    if (ServConnect(ConnectSocket, m_tSockeArgu.dwType, address, port, m_tSockeArgu.dwProxyEnable, m_tSockeArgu.szUserName, m_tSockeArgu.szPassword)) {
        _LOG_WARN(_T("Error At ServConnect()"));
        return ((DWORD)-1);
    }
//
    return 0x00;
}

SOCKET CProxySockets::ProxyConnect(char *proxy_serv, int proxy_port) {
    SOCKET ProxySocket;
//
    if(!proxy_serv) return ((DWORD)-1);
    // Create a SOCKET for connecting to server
    // SOCKET ConnectSocket;
    ProxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ProxySocket) {
        _LOG_WARN(_T("error at socket(): %ld"), WSAGetLastError());
        return ((DWORD)-1);
    }
    // Set colse socket timeout, for graceful close to server.
    struct linger liner;
    liner.l_onoff = TRUE;
    liner.l_linger = 8;
    setsockopt(ProxySocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    struct hostent *host = NULL;
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(proxy_serv);
    clientService.sin_port = htons((u_short)proxy_port);
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
    // Connect to server.
    if ( SOCKET_ERROR == connect( ProxySocket, (SOCKADDR*) &clientService, sizeof(clientService) )) {
		_LOG_WARN(_T("failed to connect.ecode:%d"), WSAGetLastError());
        return ((DWORD)-1); //不能连接到代理服务器!
    }
//
    return ProxySocket;
}

DWORD CProxySockets::ServConnect(SOCKET ProxySocket, enum PROXY_TYPE dwProxyType, char *address, int port, int szProxyAuth, char *user, char *pword) {
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


VOID CProxySockets::Close() {
    // Graceful close to server.
_LOG_DEBUG(_T("close connection!"));
    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
}

DWORD CProxySockets::Send(char *szMessage, size_t length) {
    static unsigned long ulTimestamp;
    static unsigned long ulSendTimesCount;
    // Declare and initialize variables.
    int	  bytesSent;
//
    if(!m_lSendTimeslice) {
        // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
        bytesSent = send( ConnectSocket, szMessage, (int)length, 0);
        if (0 == bytesSent)	return 0x00;// Graceful close
        else if (SOCKET_ERROR == bytesSent) {
            _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
            return ((DWORD)-1);
        }
    } else {
        int IsCalculateTimestamp = POW2N_MOD(ulSendTimesCount++, 16);
        if(!IsCalculateTimestamp) ulTimestamp = GetTickCount();

        // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
        bytesSent = send( ConnectSocket, szMessage, (int)length, 0);
        if (0 == bytesSent)	return 0x00;// Graceful close
        else if (SOCKET_ERROR == bytesSent) {
            _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
            return ((DWORD)-1);
        }
//
        if(!IsCalculateTimestamp) ulTimestamp = GetTickCount() - ulTimestamp;
        if(m_lSendTimeslice > ulTimestamp) Sleep(m_lSendTimeslice - ulTimestamp);
    }
//
    return bytesSent;
}

DWORD CProxySockets::Receive(char *szReceive, int length) {
    static unsigned long ulTimestamp;
    static unsigned long ulRecvTimesCount;
    // Declare and initialize variables.
    int bytesRecv = 0;
//
    if(!m_lReceiveTimeslice) {
        // Send and receive data.
        bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
        if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
            //"Connection Closed.\n";
            return ((DWORD)-1);
        }
        // fprintf(stderr, "head_buffer: %s", head_buffer);
    } else {
        int IsCalculateTimestamp = POW2N_MOD(ulRecvTimesCount++, 16);
        if(!IsCalculateTimestamp) ulTimestamp = GetTickCount();

        // Send and receive data.
        bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
        if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
            //"Connection Closed.\n";
            return ((DWORD)-1);
        }
        // fprintf(stderr, "head_buffer: %s", head_buffer);
//
        if(!IsCalculateTimestamp) ulTimestamp = GetTickCount() - ulTimestamp;
        if(m_lReceiveTimeslice > ulTimestamp) Sleep(m_lReceiveTimeslice - ulTimestamp);
    }
//
    return bytesRecv;
}

DWORD CProxySockets::SendReceive(char *szReceive, char *pSendMessage) {
    DWORD result = ((DWORD)-1);
    // Send and receive data.
    result = Send(pSendMessage, strlen(pSendMessage));
    if(0 > result) return ((DWORD)-1);
//
    result = Receive(szReceive, RECV_BUFF_SIZE);
    if(0 > result) return ((DWORD)-1);
//
    return result;
}


DWORD CProxySockets::SafeSend(char *szMessage, size_t length) {
    int	bytesSent;
    // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
    bytesSent = send( ConnectSocket, szMessage, (int)length, 0);
    if (0 == bytesSent)	return 0x00;// Graceful close
    else if (SOCKET_ERROR == bytesSent) {
        _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return bytesSent;
}

DWORD CProxySockets::SafeReceive(char *szReceive, int length) {
    // Declare and initialize variables.
    int bytesRecv = 0;
    // Send and receive data.
    // fprintf(stderr, "Bytes Sent: %ld\r\n", bytesSent );
    // fprintf(stderr, "bytesRecv: %ld\r\n", SOCKET_ERROR);
    bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
    if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
        //"Connection Closed.\n";
        return ((DWORD)-1);
    }
    // fprintf(stderr, "head_buffer: %s", head_buffer);
    return bytesRecv;
}

DWORD CProxySockets::SafeSendReceive(char *szReceive, char *pSendMessage) {
    DWORD result = ((DWORD)-1);
    // Send and receive data.
    result = Send(pSendMessage, strlen(pSendMessage));
    if(0 > result) return ((DWORD)-1);
//
    result = Receive(szReceive, RECV_BUFF_SIZE);
    if(0 > result) return ((DWORD)-1);
//
    return result;
}

CReplyProxy::CReplyProxy(void):
    m_port(0),
    ConnectSocket(INVALID_SOCKET)
{
    memset(m_address, 0, SERVNAME_LENGTH);
    memset(&m_tReplyArgu, 0, sizeof(struct ReplyArgu));
}

CReplyProxy::~CReplyProxy(void) {
}

DWORD CReplyProxy::Create(struct ReplyArgu *pReplyArgu) {
    if(!pReplyArgu) return ((DWORD)-1);
    memcpy(&m_tReplyArgu, pReplyArgu, sizeof(struct ReplyArgu));
    return 0x00;
}

DWORD CReplyProxy::SetAddress(char *address, int port) {
    if(!address) return ((DWORD)-1);
//
    strcpy_s(m_address, address);
    m_port = port;
//
    return 0x00;
}


DWORD CReplyProxy::Connect(char *address, int port) {
_LOG_DEBUG(_T("new connection!"));
    if (-1 == (ConnectSocket=ProxyConnect(m_tReplyArgu.tAddress.sin_addr, m_tReplyArgu.tAddress.sin_port))) {
        _LOG_WARN(_T("Error At ProxyConnect()"));
        return ((DWORD)-1);
    }
//
    if (ServConnect(ConnectSocket, m_tReplyArgu.dwType, address, port, m_tReplyArgu.dwProxyEnable, m_tReplyArgu.szUserName, m_tReplyArgu.szPassword)) {
        _LOG_WARN(_T("Error At ServConnect()"));
        return ((DWORD)-1);
    }
//
    return 0x00;
}

SOCKET CReplyProxy::ProxyConnect(char *proxy_serv, int proxy_port) {
    SOCKET ProxySocket;
    if(!proxy_serv) return ((DWORD)-1);
//
    // Create a SOCKET for connecting to server
    // SOCKET ConnectSocket;
    ProxySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ProxySocket) {
        _LOG_WARN(_T("error at socket(): %ld"), WSAGetLastError());
        return ((DWORD)-1);
    }
    // Set colse socket timeout, for graceful close to server.
    struct linger liner;
    liner.l_onoff = TRUE;
    liner.l_linger = 8;
    setsockopt(ProxySocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));
//
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    struct hostent *host = NULL;
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(proxy_serv);
    clientService.sin_port = htons((u_short)proxy_port);
    // If the supplied server address wasn't in the form
    // "aaa.bbb.ccc.ddd" it's a hostname, so try to resolve it
    //
    if (clientService.sin_addr.s_addr == INADDR_NONE) {
        host = gethostbyname(proxy_serv);
        if (NULL == host) {
            _LOG_WARN(_T("unable to resolve server: %s"), proxy_serv);
            return ((DWORD)-1);
        }
        CopyMemory(&clientService.sin_addr, host->h_addr_list[0], host->h_length);
    }
    // Connect to server.
    if (SOCKET_ERROR == connect( ProxySocket, (SOCKADDR*) &clientService, sizeof(clientService)) ) {
		_LOG_WARN(_T("failed to connect.ecode:%d"), WSAGetLastError());
        return ((DWORD)-1); //不能连接到代理服务器!
    }
//
    return ProxySocket;
}


DWORD CReplyProxy::ServConnect(SOCKET ProxySocket, enum PROXY_TYPE dwProxyType, char *address, int port, int szProxyAuth, char *user, char *pword) {
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

VOID CReplyProxy::Close() {
    // Graceful close to server.
_LOG_DEBUG(_T("close connection!"));
    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
}

DWORD CReplyProxy::Send(char *szMessage) {
    int	bytesSent;
    // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
    bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
    if (0 == bytesSent)	return 0x00; // Graceful close
    else if (SOCKET_ERROR == bytesSent) {
        _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return bytesSent;
}

DWORD CReplyProxy::Receive(char *szReceive) {
    // Declare and initialize variables.
    int bytesRecv = 0;
    // Send and receive data.
    // fprintf(stderr, "Bytes Sent: %ld\r\n", bytesSent );
    // fprintf(stderr, "bytesRecv: %ld\r\n", SOCKET_ERROR);
    bytesRecv = recv( ConnectSocket, szReceive, HEAD_BUFFER_LEN, 0 );
    if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
        //"Connection Closed.\n";
        return ((DWORD)-1);
    }
    // fprintf(stderr, "head_buffer: %s", head_buffer);
    szReceive[bytesRecv] = '\0';
//
    return bytesRecv;
}

DWORD CReplyProxy::SendReceive(char *szReceive, char *pSendMessage) {
    DWORD result = ((DWORD)-1);
    // Connect to server.
    if(Connect(m_address, m_port)) return ((DWORD)-1);
    // Send and receive data.
    do {
        result = Send(pSendMessage);
        if(0 > result) break;

        result = Receive(szReceive);
        if(0 > result) break;
    } while (FALSE);
    // Graceful close to server.
    Close();
//
    return result;
}

DWORD CReplyProxy::SafeSend(char *szMessage) {
    int	  bytesSent;
    // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
    bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
    if (0 == bytesSent)	return 0x00; // Graceful close
    else if (SOCKET_ERROR == bytesSent) {
        _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return bytesSent;
}

DWORD CReplyProxy::SafeReceive(char *szReceive) {
    // Declare and initialize variables.
    int bytesRecv = 0;
    // Send and receive data.
    // fprintf(stderr, "Bytes Sent: %ld\r\n", bytesSent );
    // fprintf(stderr, "bytesRecv: %ld\r\n", SOCKET_ERROR);
    bytesRecv = recv( ConnectSocket, szReceive, HEAD_BUFFER_LEN, 0 );
    if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
        //"Connection Closed.\n";
        return ((DWORD)-1);
    }
    // fprintf(stderr, "head_buffer: %s", head_buffer);
    szReceive[bytesRecv] = '\0';
//
    return bytesRecv;
}

DWORD CReplyProxy::SafeSendReceive(char *szReceive, char *pSendMessage) {
    DWORD result = ((DWORD)-1);
    // Connect to server.
    if(Connect(m_address, m_port)) return ((DWORD)-1);
    // Send and receive data.
    do {
        result = SafeSend(pSendMessage);
        if(0 > result) break;

        result = SafeReceive(szReceive);
        if(0 > result) break;
    } while (FALSE);
    // Graceful close to server.
    Close();
//
    return result;
}
