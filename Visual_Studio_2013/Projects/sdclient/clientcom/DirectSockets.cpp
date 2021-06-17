#include "StdAfx.h"

#include "third_party.h"
#include "DirectSockets.h"

CDirectSockets::CDirectSockets(void):
    ConnectSocket(INVALID_SOCKET)
    ,m_lSendTimeslice(0)
    ,m_lReceiveTimeslice(0)
{
}

CDirectSockets::~CDirectSockets(void) {
}

DWORD CDirectSockets::Create(struct SocketsArgu *pSockeArgu) {
    unsigned long ulDowndRate = 0x00, ulUploadRate = 0x00;
    if(!pSockeArgu) return ((DWORD)-1);
//
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

DWORD CDirectSockets::SetSendReceiveSpeed(unsigned long ulMaxReceive, unsigned long ulMaxSend) {
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

DWORD CDirectSockets::Connect(char *address, int port) {
    if(!address) return ((DWORD)-1);
    // Create a SOCKET for connecting to server
    // SOCKET ConnectSocket;
_LOG_DEBUG(_T("new connection!"));
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        _LOG_WARN(_T("error at socket(): %ld"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    // Set colse socket timeout, for graceful close to server.
    struct linger liner;
    liner.l_onoff = TRUE;
    liner.l_linger = 32;
    setsockopt(ConnectSocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));
//
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
//
    // Connect to server.
    if ( SOCKET_ERROR == connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService)) ) {
		_LOG_WARN(_T("failed to connect.%d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return 0x00;
}

VOID CDirectSockets::Close() {
    // Graceful close to server.
_LOG_DEBUG(_T("close connection!"));
    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
}

DWORD CDirectSockets::Send(char *szMessage, size_t length) {
    static unsigned long ulTimestamp;
    static unsigned long ulSendTimesCount;
    // Declare and initialize variables.
    int bytesSent;
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

// #include "BinaryLogger.h"
// BinaryLogger::LogBinaryCode(szReceive, bytesRecv);
DWORD CDirectSockets::Receive(char *szReceive, int length) {
    static unsigned long ulTimestamp;
    static unsigned long ulRecvTimesCount;
    // Declare and initialize variables.
    int bytesRecv = 0;
//
    if(!m_lReceiveTimeslice) {
        // Send and receive data.
        bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
        if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
            // "Connection Closed.\n";
_LOG_DEBUG(_T("bytesRecv:%d"), bytesRecv);
            return ((DWORD)-1);
        }
        // fprintf(stderr, "szReceive: %s", szReceive);
    } else {
        int IsCalculateTimestamp = POW2N_MOD(ulRecvTimesCount++, 16);
        if(!IsCalculateTimestamp) ulTimestamp = GetTickCount();
        // Send and receive data.
        bytesRecv = recv( ConnectSocket, szReceive, length, 0 );
        if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
            // "Connection Closed.\n";
            return ((DWORD)-1);
        }
        // fprintf(stderr, "head_buffer: %s", head_buffer);
        if(!IsCalculateTimestamp) ulTimestamp = GetTickCount() - ulTimestamp;
        if(m_lReceiveTimeslice > ulTimestamp) Sleep(m_lReceiveTimeslice - ulTimestamp);
    }
//
    return bytesRecv;
}

DWORD CDirectSockets::SendReceive(char *szReceive, char *pSendMessage) {
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

DWORD CDirectSockets::SafeSend(char *szMessage, size_t length) {
    int bytesSent;
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

DWORD CDirectSockets::SafeReceive(char *szReceive, int length) {
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
//
    return bytesRecv;
}

DWORD CDirectSockets::SafeSendReceive(char *szReceive, char *pSendMessage) {
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


CReplyDirect::CReplyDirect(void):
    m_port(0)
    ,ConnectSocket(INVALID_SOCKET)
{
    memset(m_address, 0, SERVNAME_LENGTH);
}

CReplyDirect::~CReplyDirect(void) {
}

DWORD CReplyDirect::Create(struct ReplyArgu *pReplyArgu) {
    return 0x00;
}

DWORD CReplyDirect::SetAddress(char *address, int port) {
    if(!address) return ((DWORD)-1);
//
    strcpy_s(m_address, address);
    m_port = port;
//
    return 0x00;
}

DWORD CReplyDirect::Connect(char *address, int port) {
    if(!address) return ((DWORD)-1);
    // Create a SOCKET for connecting to server
    // SOCKET ConnectSocket;
_LOG_DEBUG(_T("new connection!"));
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == ConnectSocket) {
        _LOG_WARN(_T("error at socket(): %ld"), WSAGetLastError());
        return ((DWORD)-1);
    }
    // Set colse socket timeout, for graceful close to server.
    struct linger liner;
    liner.l_onoff = TRUE;
    liner.l_linger = 16;
    setsockopt(ConnectSocket, SOL_SOCKET, SO_LINGER, (char*)&liner, sizeof(liner));
//
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
    // Connect to server.
    if ( SOCKET_ERROR == connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) )) {
		_LOG_WARN(_T("failed to connect.ecode:%d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return 0x00;
}

void CReplyDirect::Close() {
    // Graceful close to server.
_LOG_DEBUG(_T("close connection!"));
    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
}

DWORD CReplyDirect::Send(char *szMessage) {
    int bytesSent;
    // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
    bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
    if (0 == bytesSent)	return 0x00;// Graceful close
    else if (SOCKET_ERROR == bytesSent) {
        _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return bytesSent;
}

DWORD CReplyDirect::Receive(char *szReceive) {
    // Declare and initialize variables.
    int bytesRecv = 0;
    char head_buffer[HEAD_BUFFER_LEN];
    // Send and receive data.
    // fprintf(stderr, "Bytes Sent: %ld\r\n", bytesSent );
    // fprintf(stderr, "bytesRecv: %ld\r\n", SOCKET_ERROR);
    bytesRecv = recv( ConnectSocket, head_buffer, HEAD_BUFFER_LEN, 0 );
    if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
        //"Connection Closed.\n";
        return ((DWORD)-1);
    }
    // fprintf(stderr, "head_buffer: %s", head_buffer);
    memcpy(szReceive, head_buffer, bytesRecv);
    szReceive[bytesRecv] = '\0';
//
    return bytesRecv;
}

DWORD CReplyDirect::SendReceive(char *szReceive, char *pSendMessage) {
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

DWORD CReplyDirect::SafeSend(char *szMessage) {
    int	bytesSent;
    // bytesSent = send( ConnectSocket, szMessage, MESSAGE_BUFFER, 0);
    bytesSent = send( ConnectSocket, szMessage, (int)strlen(szMessage), 0);
    if (0 == bytesSent)	return 0x00;// Graceful close
    else if (SOCKET_ERROR == bytesSent) {
        _LOG_WARN(_T("send() failed: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
//
    return bytesSent;
}

DWORD CReplyDirect::SafeReceive(char *szReceive) {
    // Declare and initialize variables.
    int bytesRecv = 0;
    char head_buffer[HEAD_BUFFER_LEN];
    // Send and receive data.
    // fprintf(stderr, "Bytes Sent: %ld\r\n", bytesSent );
    // fprintf(stderr, "bytesRecv: %ld\r\n", SOCKET_ERROR);
    bytesRecv = recv( ConnectSocket, head_buffer, HEAD_BUFFER_LEN, 0 );
    if ( 0 == bytesRecv || WSAECONNRESET == bytesRecv ) {
        // "Connection Closed.\n";
        return ((DWORD)-1);
    }
    // fprintf(stderr, "head_buffer: %s", head_buffer);
    memcpy(szReceive, head_buffer, bytesRecv);
    szReceive[bytesRecv] = '\0';
//
    return bytesRecv;
}

DWORD CReplyDirect::SafeSendReceive(char *szReceive, char *pSendMessage) {
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