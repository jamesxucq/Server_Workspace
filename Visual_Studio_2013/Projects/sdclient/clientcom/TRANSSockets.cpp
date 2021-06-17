#include "StdAfx.h"

//#include <Wininet.h>
#include <Sensapi.h>
#pragma comment(lib, "Sensapi.lib")

#include "DirectSockets.h"
#include "ProxySockets.h"

#include "Session.h"
#include "TRANSSockets.h"

CTRANSSockets::CTRANSSockets(void) {
}

CTRANSSockets::~CTRANSSockets(void) {
}

CReplySockets::CReplySockets(void) {
}

CReplySockets::~CReplySockets(void) {
}

CTRANSSockets *NTRANSSockets::Factory(struct SocketsArgu *pSockeArgu) {
    CTRANSSockets *pTransSocke = NULL;
	//
    if(0x00 == pSockeArgu->dwProxyEnable) {
        pTransSocke = new CDirectSockets();
        pTransSocke->Create(pSockeArgu);
// _LOG_DEBUG(_T("no proxy.")); // disable by james 20130410
    } else if(0x01 == pSockeArgu->dwProxyEnable) {
        pTransSocke = new CProxySockets();
        pTransSocke->Create(pSockeArgu);
// _LOG_DEBUG(_T("proxy to server. server:%s port:%d"), pSockeArgu->tAddress.sin_addr, pSockeArgu->tAddress.sin_port); // disable by james 20130410
    } else if(0x03 == pSockeArgu->dwProxyEnable) {
        TCHAR proxy_serv[SERVNAME_LENGTH];
        int proxy_port;

        MKZEO(proxy_serv);
        DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(proxy_serv, &proxy_port);
        if(0x01 == dwProxyEnable) {
            pSockeArgu->dwType = PROXY_TYPE_HTTP;
            strcon::ustr_ansi(pSockeArgu->tAddress.sin_addr, proxy_serv);
            pSockeArgu->tAddress.sin_port = proxy_port;
            pSockeArgu->dwProxyEnable = 0x00;

            pTransSocke = new CProxySockets();
            pTransSocke->Create(pSockeArgu);
// _LOG_DEBUG(_T("get ie proxy ok! server:%s port:%d"), proxy_serv, proxy_port); // disable by james 20130410
        } else if(0x00 == dwProxyEnable) {
            pTransSocke = new CDirectSockets();
            pTransSocke->Create(pSockeArgu);
// _LOG_DEBUG(_T("get ie proxy ok! no proxy! server:%s port:%d"), pSockeArgu->tAddress.sin_addr, pSockeArgu->tAddress.sin_port); // disable by james 20130410
        }
    }
//
    return pTransSocke;
}

VOID NTRANSSockets::DestroyObject(CTRANSSockets *pTransSocke) {
    delete pTransSocke;
}


CReplySockets *NTRANSSockets::Factory(struct ReplyArgu *pReplyArgu) {
    CReplySockets *pReplySocke = NULL;
//
    if(0x00 == pReplyArgu->dwProxyEnable) {
        pReplySocke = new CReplyDirect();
        pReplySocke->Create(pReplyArgu);
// _LOG_DEBUG(_T("no proxy.")); // disable by james 20130410
    } else if(0x01 == pReplyArgu->dwProxyEnable) {
        pReplySocke = new CReplyProxy();
        pReplySocke->Create(pReplyArgu);
// _LOG_DEBUG(_T("proxy to server. server:%s port:%d"), pReplyArgu->tAddress.sin_addr, pReplyArgu->tAddress.sin_port); // disable by james 20130410
    } else if(0x03 == pReplyArgu->dwProxyEnable) {
        TCHAR proxy_serv[SERVNAME_LENGTH];
        int proxy_port;
//
        MKZEO(proxy_serv);
        DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(proxy_serv, &proxy_port);
        if(0x01 == dwProxyEnable) {
            pReplyArgu->dwType = PROXY_TYPE_HTTP;
            strcon::ustr_ansi(pReplyArgu->tAddress.sin_addr, proxy_serv);
            pReplyArgu->tAddress.sin_port = proxy_port;
            pReplyArgu->dwProxyEnable = 0x00;

            pReplySocke = new CReplyProxy();
            pReplySocke->Create(pReplyArgu);
// _LOG_DEBUG(_T("get ie proxy ok! server:%s port:%d"), proxy_serv, proxy_port); // disable by james 20130410
        } else if(0x00 == dwProxyEnable) {
            pReplySocke = new CReplyDirect();
            pReplySocke->Create(pReplyArgu);
// _LOG_DEBUG(_T("get ie proxy ok! no proxy! server:%s port:%d"), pReplyArgu->tAddress.sin_addr, pReplyArgu->tAddress.sin_port); // disable by james 20130410
        }
    }
//
    return pReplySocke;
}

VOID NTRANSSockets::DestroyObject(CReplySockets *pReplySocke) {
    delete pReplySocke;
}


DWORD NTRANSSockets::SWD_IsNetworkAlive() {
    DWORD   flags;
    if(!IsNetworkAlive(&flags))	{ //在线
        // error or no connection
        if(ERROR_SUCCESS == GetLastError()) {
            _LOG_WARN(_T("No connection!"));
        } else {
            _LOG_WARN(_T("Connection error!"));
        }
        return 0x01;
    } else {
        if (NETWORK_ALIVE_LAN & flags) {
            //在线 NETWORK_ALIVE_LAN
        }
        if (NETWORK_ALIVE_WAN & flags) {
            //在线 NETWORK_ALIVE_WAN
        }
        if (NETWORK_ALIVE_AOL & flags) {
            //在线 NETWORK_ALIVE_AOL
        }
    }
    return 0x00;
}

DWORD NTRANSSockets::InitialWinsock() {
    // Initialize Winsock
    WSADATA wsaData;
    if (NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData)) {
        _LOG_WARN(_T("Error At WSAStartup(), Error No.: %d"), WSAGetLastError());
        return ((DWORD)-1);
    }
    _LOG_INFO(_T("WSAStartup() OK.")); // add by james 20130416
    return 0x00;
}

VOID NTRANSSockets::FinishWinsock() {
    // Sleep(1024); // wait for graceful close
    // Finish Winsock
    do {
		if (NO_ERROR != WSACleanup()) {
            _LOG_WARN(_T("Last Error Is OK, Error At WSACleanup(), Error No.: %d"), WSAGetLastError());
		}
    } while(WSANOTINITIALISED != WSAGetLastError());
    _LOG_INFO(_T("WSACleanup() OK.")); // add by james 20130416
}

VOID CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSockeArgu, struct NetworkConfig *pNetworkConfig) {
    pSockeArgu->dwWorkerThreads = ONE_SOCKETS_THREAD;
    // _tcscpy_s(pSockeArgu->szDowndLimit, pNetworkConfig->szDowndLimit);
    if(!_tcscmp(_T("true"), pNetworkConfig->szDowndLimit)) pSockeArgu->dwDowndLimit = 0x01;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szDowndLimit)) pSockeArgu->dwDowndLimit = 0x00;
    pSockeArgu->dwDowndRate = pNetworkConfig->dwDowndRate;
    // _tcscpy_s(pSockeArgu->dwUploadLimit, pNetworkConfig->szUploadLimit);
    if(!_tcscmp(_T("true"), pNetworkConfig->szUploadLimit)) pSockeArgu->dwUploadLimit = 0x01;
    else if(!_tcscmp(_T("auto"), pNetworkConfig->szUploadLimit)) pSockeArgu->dwUploadLimit = 0x03;
    else if (!_tcscmp(_T("false"), pNetworkConfig->szUploadLimit)) pSockeArgu->dwUploadLimit = 0x00;
    pSockeArgu->dwUploadRate = pNetworkConfig->dwUploadRate;
    //
    // _tcscpy_s(pSockeArgu->szProxyEnable, pNetworkConfig->szProxyEnable);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyEnable)) pSockeArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("auto"), pNetworkConfig->szProxyEnable)) pSockeArgu->dwProxyEnable = 0x03;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyEnable)) pSockeArgu->dwProxyEnable = 0x00;
    // _tcscpy_s(pSockeArgu->dwType, pNetworkConfig->szProxyType);
    if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTP"))) pSockeArgu->dwType = PROXY_TYPE_HTTP; // 通过HTTP方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTPS"))) pSockeArgu->dwType = PROXY_TYPE_HTTPS;
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4"))) pSockeArgu->dwType = PROXY_TYPE_SOCKS4; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4a"))) pSockeArgu->dwType = PROXY_TYPE_SOCKS4a; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS5"))) pSockeArgu->dwType = PROXY_TYPE_SOCKS5; // 通过Socks5方式代理
	//
    strcon::ustr_ansi(pSockeArgu->tAddress.sin_addr, pNetworkConfig->tAddress.sin_addr);
    pSockeArgu->tAddress.sin_port = pNetworkConfig->tAddress.sin_port;
    // _tcscpy_s(pSockeArgu->szProxyAuth, pNetworkConfig->szProxyAuth);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyAuth)) pSockeArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyAuth)) pSockeArgu->dwProxyEnable = 0x00;
    strcon::ustr_ansi(pSockeArgu->szUserName, pNetworkConfig->szUserName);
    strcon::ustr_ansi(pSockeArgu->szPassword, pNetworkConfig->szPassword);
}

VOID CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSockeArgu, struct ListData *ldata) {
    strcpy_s(pSockeArgu->tServAddress.sin_addr, ldata->tServAddress.sin_addr);
    pSockeArgu->tServAddress.sin_port = ldata->tServAddress.sin_port;
}


VOID CONVERT_VALSOCKS_ARGU(struct ReplyArgu *pReplyArgu, struct LocalConfig *pLocalConfig) {
    struct ServParam *pServParam = &pLocalConfig->tServParam;
    strcon::ustr_ansi(pReplyArgu->tReplyAddress.sin_addr, pServParam->tRegistAddress.sin_addr);
    pReplyArgu->tReplyAddress.sin_port = pServParam->tRegistAddress.sin_port;
    //
    struct NetworkConfig *pNetworkConfig = &pLocalConfig->tNetworkConfig;
    // _tcscpy_s(pSockeArgu->szProxyEnable, pNetworkConfig->szProxyEnable);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyEnable)) pReplyArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("auto"), pNetworkConfig->szProxyEnable)) pReplyArgu->dwProxyEnable = 0x03;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyEnable)) pReplyArgu->dwProxyEnable = 0x00;
    // _tcscpy_s(pSockeArgu->dwType, pNetworkConfig->szProxyType);
    if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTP"))) pReplyArgu->dwType = PROXY_TYPE_HTTP; // 通过HTTP方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTPS"))) pReplyArgu->dwType = PROXY_TYPE_HTTPS;
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4"))) pReplyArgu->dwType = PROXY_TYPE_SOCKS4; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4a"))) pReplyArgu->dwType = PROXY_TYPE_SOCKS4a; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS5"))) pReplyArgu->dwType = PROXY_TYPE_SOCKS5; // 通过Socks5方式代理
	//
    strcon::ustr_ansi(pReplyArgu->tAddress.sin_addr, pNetworkConfig->tAddress.sin_addr);
    pReplyArgu->tAddress.sin_port = pNetworkConfig->tAddress.sin_port;
    // _tcscpy_s(pSockeArgu->szProxyAuth, pNetworkConfig->szProxyAuth);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyAuth)) pReplyArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyAuth)) pReplyArgu->dwProxyEnable = 0x00;
    strcon::ustr_ansi(pReplyArgu->szUserName, pNetworkConfig->szUserName);
    strcon::ustr_ansi(pReplyArgu->szPassword, pNetworkConfig->szPassword);
}

VOID CONVERT_AUTHSOCKS_ARGU(struct ReplyArgu *pReplyArgu, struct ListData *ldata, struct NetworkConfig *pNetworkConfig) {
    strcpy_s(pReplyArgu->tReplyAddress.sin_addr, ldata->tAuthAddress.sin_addr);
    pReplyArgu->tReplyAddress.sin_port = ldata->tAuthAddress.sin_port;
    //
    // _tcscpy_s(pSockeArgu->szProxyEnable, pNetworkConfig->szProxyEnable);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyEnable)) pReplyArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("auto"), pNetworkConfig->szProxyEnable)) pReplyArgu->dwProxyEnable = 0x03;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyEnable)) pReplyArgu->dwProxyEnable = 0x00;
    // _tcscpy_s(pSockeArgu->dwType, pNetworkConfig->szProxyType);
    if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTP"))) pReplyArgu->dwType = PROXY_TYPE_HTTP; // 通过HTTP方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTPS"))) pReplyArgu->dwType = PROXY_TYPE_HTTPS;
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4"))) pReplyArgu->dwType = PROXY_TYPE_SOCKS4; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4a"))) pReplyArgu->dwType = PROXY_TYPE_SOCKS4a; // 通过Socks4方式代理
    else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS5"))) pReplyArgu->dwType = PROXY_TYPE_SOCKS5; // 通过Socks5方式代理
	//
    strcon::ustr_ansi(pReplyArgu->tAddress.sin_addr, pNetworkConfig->tAddress.sin_addr);
    pReplyArgu->tAddress.sin_port = pNetworkConfig->tAddress.sin_port;
    // _tcscpy_s(pSockeArgu->szProxyAuth, pNetworkConfig->szProxyAuth);
    if(!_tcscmp(_T("true"), pNetworkConfig->szProxyAuth)) pReplyArgu->dwProxyEnable = 0x01;
    else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyAuth)) pReplyArgu->dwProxyEnable = 0x00;
    strcon::ustr_ansi(pReplyArgu->szUserName, pNetworkConfig->szUserName);
    strcon::ustr_ansi(pReplyArgu->szPassword, pNetworkConfig->szPassword);
}