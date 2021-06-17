#include "StdAfx.h"

#include "NormalSockets.h"
#include "ProxySockets.h"

#include "Session.h"
#include "TRANSSockets.h"



CTRANSSockets::CTRANSSockets(void)
{
}

CTRANSSockets::~CTRANSSockets(void)
{
}

CReplySockets::CReplySockets(void)
{
}

CReplySockets::~CReplySockets(void)
{
}

CTRANSSockets *NTRANSSockets::Factory(struct SocketsArgu *pSocketsArgu)
{
	CTRANSSockets *pSdSockets = NULL;

	if(0x00 == pSocketsArgu->dwProxyEnable) {
		pSdSockets = new CTNormalSockets;
		pSdSockets->Create(pSocketsArgu);
		LOG_DEBUG(_T("no proxy."));
	} else if(0x01 == pSocketsArgu->dwProxyEnable) {
		pSdSockets = new CTProxySockets;
		pSdSockets->Create(pSocketsArgu);
		LOG_DEBUG(_T("proxy to server. server:%s port:%d"), pSocketsArgu->Address.sin_addr, pSocketsArgu->Address.sin_port);
	} else if(0x03 == pSocketsArgu->dwProxyEnable) {
		TCHAR szProxyServ[SERVNAME_LENGTH];
		int iProxyPort;

		memset(szProxyServ, '\0', SERVNAME_LENGTH);
		DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(szProxyServ, &iProxyPort);
		if(0x01 == dwProxyEnable) {
			pSocketsArgu->dwType = PROXY_TYPE_HTTP;
			nstriconv::unicode_ansi(pSocketsArgu->Address.sin_addr, szProxyServ);
			pSocketsArgu->Address.sin_port = iProxyPort;
			pSocketsArgu->dwProxyEnable = 0x00;

			pSdSockets = new CTProxySockets;
			pSdSockets->Create(pSocketsArgu);
			LOG_DEBUG(_T("get ie proxy ok! server:%s port:%d"), szProxyServ, iProxyPort);
		} else if(0x00 == dwProxyEnable) {
			pSdSockets = new CTNormalSockets;
			pSdSockets->Create(pSocketsArgu);
			LOG_DEBUG(_T("get ie proxy ok! no proxy! server:%s port:%d"), pSocketsArgu->Address.sin_addr, pSocketsArgu->Address.sin_port);
		}
	}

	return pSdSockets;
}

void NTRANSSockets::DestroyObject(CTRANSSockets *pSdSockets)
{ delete pSdSockets; }


CReplySockets *NTRANSSockets::Factory(struct ReplySocksArgu *pReplySocksArgu)
{
	CReplySockets *pMessageSockets = NULL;

	if(0x00 == pReplySocksArgu->dwProxyEnable) {
		pMessageSockets = new CReplyNormalSockets;
		pMessageSockets->Create(pReplySocksArgu);
		LOG_DEBUG(_T("no proxy."));
	} else if(0x01 == pReplySocksArgu->dwProxyEnable) {
		pMessageSockets = new CReplyProxySockets;
		pMessageSockets->Create(pReplySocksArgu);
		LOG_DEBUG(_T("proxy to server. server:%s port:%d"), pReplySocksArgu->Address.sin_addr, pReplySocksArgu->Address.sin_port);
	} else if(0x03 == pReplySocksArgu->dwProxyEnable) {
		TCHAR szProxyServ[SERVNAME_LENGTH];
		int iProxyPort;

		memset(szProxyServ, '\0', SERVNAME_LENGTH);
		DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(szProxyServ, &iProxyPort);
		if(0x01 == dwProxyEnable) {
			pReplySocksArgu->dwType = PROXY_TYPE_HTTP;
			nstriconv::unicode_ansi(pReplySocksArgu->Address.sin_addr, szProxyServ);
			pReplySocksArgu->Address.sin_port = iProxyPort;
			pReplySocksArgu->dwProxyEnable = 0x00;

			pMessageSockets = new CReplyProxySockets;
			pMessageSockets->Create(pReplySocksArgu);
			LOG_DEBUG(_T("get ie proxy ok! server:%s port:%d"), szProxyServ, iProxyPort);
		} else if(0x00 == dwProxyEnable) {
			pMessageSockets = new CReplyNormalSockets;
			pMessageSockets->Create(pReplySocksArgu);
			LOG_DEBUG(_T("get ie proxy ok! no proxy! server:%s port:%d"), pReplySocksArgu->Address.sin_addr, pReplySocksArgu->Address.sin_port);
		}
	}

	return pMessageSockets;
}

void NTRANSSockets::DestroyObject(CReplySockets *pMessageSockets)
{ delete pMessageSockets; }


DWORD NTRANSSockets::SD_IsNetworkAlive()
{
	DWORD   flags; 

	if(!IsNetworkAlive(&flags))//在线 
	{
		// error or no connection
		if(ERROR_SUCCESS == GetLastError()) {
			TRACE(_T("No connection!\n"));
		} else {
			TRACE(_T("Connection error!\n"));
		}
		return 1;
	} else {
		if (flags & NETWORK_ALIVE_LAN) {
			//在线 NETWORK_ALIVE_LAN
		}
		if (flags & NETWORK_ALIVE_WAN) {
			//在线 NETWORK_ALIVE_WAN
		}
		if (flags & NETWORK_ALIVE_AOL) {
			//在线 NETWORK_ALIVE_AOL
		}
	}

	return 0;
}

DWORD NTRANSSockets::InitialWinsock()
{
	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		TRACE(_T("Error At WSAStartup()\n"));
		return -1;
	}

	return 0;
}


DWORD NTRANSSockets::FinishWinsock()
{
	// Sleep(1024); // wait for graceful close
	//----------------------
	// Finish Winsock
	int iResult = WSACleanup();
	if (iResult != NO_ERROR){
		TRACE(_T("Error At WSACleanup()\n"));
		return -1;
	}

	return 0;
}

void CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSocketsArgu, struct NetworkConfig *pNetworkConfig, unsigned int dwWorkerThreads) {
	/////////////////////////////////////////////////////////////////////////////
	pSocketsArgu->dwWorkerThreads = dwWorkerThreads;

	/////////////////////////////////////////////////////////////////////////////
	// _tcscpy_s(pSocketsArgu->szDownloadLimit, pNetworkConfig->szDownloadLimit); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szDownloadLimit)) pSocketsArgu->dwDownloadLimit = 0x01;
	else if(!_tcscmp(_T("false"), pNetworkConfig->szDownloadLimit)) pSocketsArgu->dwDownloadLimit = 0x00;
	pSocketsArgu->dwDownloadRate = pNetworkConfig->dwDownloadRate; 

	// _tcscpy_s(pSocketsArgu->dwUploadLimit, pNetworkConfig->szUploadLimit); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szUploadLimit)) pSocketsArgu->dwUploadLimit = 0x01;
	else if(!_tcscmp(_T("auto"), pNetworkConfig->szUploadLimit)) pSocketsArgu->dwUploadLimit = 0x03;
	else if (!_tcscmp(_T("false"), pNetworkConfig->szUploadLimit)) pSocketsArgu->dwUploadLimit = 0x00;		
	pSocketsArgu->dwUploadRate = pNetworkConfig->dwUploadRate; 

	/////////////////////////////////////////////////////////////////////////////
	// _tcscpy_s(pSocketsArgu->szProxyEnable, pNetworkConfig->szProxyEnable); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szProxyEnable)) pSocketsArgu->dwProxyEnable = 0x01;
	else if(!_tcscmp(_T("auto"), pNetworkConfig->szProxyEnable)) pSocketsArgu->dwProxyEnable = 0x03;
	else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyEnable)) pSocketsArgu->dwProxyEnable = 0x00;

	// _tcscpy_s(pSocketsArgu->dwType, pNetworkConfig->szProxyType); 
	if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTP"))) pSocketsArgu->dwType = PROXY_TYPE_HTTP; // 通过HTTP方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTPS"))) pSocketsArgu->dwType = PROXY_TYPE_HTTPS;
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4"))) pSocketsArgu->dwType = PROXY_TYPE_SOCKS4; // 通过Socks4方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4a"))) pSocketsArgu->dwType = PROXY_TYPE_SOCKS4a; // 通过Socks4方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS5"))) pSocketsArgu->dwType = PROXY_TYPE_SOCKS5; // 通过Socks5方式代理 

	nstriconv::unicode_ansi(pSocketsArgu->Address.sin_addr, pNetworkConfig->Address.sin_addr);
	pSocketsArgu->Address.sin_port = pNetworkConfig->Address.sin_port; 

	// _tcscpy_s(pSocketsArgu->szProxyAuth, pNetworkConfig->szProxyAuth); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szProxyAuth)) pSocketsArgu->dwProxyEnable = 0x01;
	else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyAuth)) pSocketsArgu->dwProxyEnable = 0x00;	
	nstriconv::unicode_ansi(pSocketsArgu->szUserName, pNetworkConfig->szUserName);
	nstriconv::unicode_ansi(pSocketsArgu->szPassword, pNetworkConfig->szPassword);
}  

void CONVERT_SOCKETS_ARGU(struct SocketsArgu *pSocketsArgu, struct ListInform *pListInform) {
	/////////////////////////////////////////////////////////////////////////////
	strcpy_s(pSocketsArgu->ServAddress.sin_addr, pListInform->ServAddress.sin_addr);
	pSocketsArgu->ServAddress.sin_port = pListInform->ServAddress.sin_port; 
}  


void CONVERT_VALSOCKS_ARGU(struct ReplySocksArgu *pReplySocksArgu, struct LocalConfig *pLocalConfig) { 
	/////////////////////////////////////////////////////////////////////////////
	struct ServInform *pServInform = &pLocalConfig->tServInform;
	nstriconv::unicode_ansi(pReplySocksArgu->ReplyAddress.sin_addr, pServInform->RegistAddress.sin_addr); 
	pReplySocksArgu->ReplyAddress.sin_port = pServInform->RegistAddress.sin_port; 

	/////////////////////////////////////////////////////////////////////////////
	struct NetworkConfig *pNetworkConfig = &pLocalConfig->tNetworkConfig;
	// _tcscpy_s(pSocketsArgu->szProxyEnable, pNetworkConfig->szProxyEnable); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szProxyEnable)) pReplySocksArgu->dwProxyEnable = 0x01;
	else if(!_tcscmp(_T("auto"), pNetworkConfig->szProxyEnable)) pReplySocksArgu->dwProxyEnable = 0x03;
	else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyEnable)) pReplySocksArgu->dwProxyEnable = 0x00;

	// _tcscpy_s(pSocketsArgu->dwType, pNetworkConfig->szProxyType); 
	if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTP"))) pReplySocksArgu->dwType = PROXY_TYPE_HTTP; // 通过HTTP方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTPS"))) pReplySocksArgu->dwType = PROXY_TYPE_HTTPS;
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4"))) pReplySocksArgu->dwType = PROXY_TYPE_SOCKS4; // 通过Socks4方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4a"))) pReplySocksArgu->dwType = PROXY_TYPE_SOCKS4a; // 通过Socks4方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS5"))) pReplySocksArgu->dwType = PROXY_TYPE_SOCKS5; // 通过Socks5方式代理 

	nstriconv::unicode_ansi(pReplySocksArgu->Address.sin_addr, pNetworkConfig->Address.sin_addr);
	pReplySocksArgu->Address.sin_port = pNetworkConfig->Address.sin_port; 

	// _tcscpy_s(pSocketsArgu->szProxyAuth, pNetworkConfig->szProxyAuth); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szProxyAuth)) pReplySocksArgu->dwProxyEnable = 0x01;
	else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyAuth)) pReplySocksArgu->dwProxyEnable = 0x00;	
	nstriconv::unicode_ansi(pReplySocksArgu->szUserName, pNetworkConfig->szUserName);
	nstriconv::unicode_ansi(pReplySocksArgu->szPassword, pNetworkConfig->szPassword);
}  

void CONVERT_AUTHSOCKS_ARGU(struct ReplySocksArgu *pReplySocksArgu, struct ListInform *pListInform, struct NetworkConfig *pNetworkConfig) { 
	/////////////////////////////////////////////////////////////////////////////
	strcpy_s(pReplySocksArgu->ReplyAddress.sin_addr, pListInform->AuthAddress.sin_addr); 
	pReplySocksArgu->ReplyAddress.sin_port = pListInform->AuthAddress.sin_port; 

	/////////////////////////////////////////////////////////////////////////////
	// _tcscpy_s(pSocketsArgu->szProxyEnable, pNetworkConfig->szProxyEnable); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szProxyEnable)) pReplySocksArgu->dwProxyEnable = 0x01;
	else if(!_tcscmp(_T("auto"), pNetworkConfig->szProxyEnable)) pReplySocksArgu->dwProxyEnable = 0x03;
	else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyEnable)) pReplySocksArgu->dwProxyEnable = 0x00;

	// _tcscpy_s(pSocketsArgu->dwType, pNetworkConfig->szProxyType); 
	if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTP"))) pReplySocksArgu->dwType = PROXY_TYPE_HTTP; // 通过HTTP方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("HTTPS"))) pReplySocksArgu->dwType = PROXY_TYPE_HTTPS;
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4"))) pReplySocksArgu->dwType = PROXY_TYPE_SOCKS4; // 通过Socks4方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS4a"))) pReplySocksArgu->dwType = PROXY_TYPE_SOCKS4a; // 通过Socks4方式代理 
	else if(!_tcscmp(pNetworkConfig->szProxyType, _T("SOCKS5"))) pReplySocksArgu->dwType = PROXY_TYPE_SOCKS5; // 通过Socks5方式代理 

	nstriconv::unicode_ansi(pReplySocksArgu->Address.sin_addr, pNetworkConfig->Address.sin_addr);
	pReplySocksArgu->Address.sin_port = pNetworkConfig->Address.sin_port; 

	// _tcscpy_s(pSocketsArgu->szProxyAuth, pNetworkConfig->szProxyAuth); 
	if(!_tcscmp(_T("true"), pNetworkConfig->szProxyAuth)) pReplySocksArgu->dwProxyEnable = 0x01;
	else if(!_tcscmp(_T("false"), pNetworkConfig->szProxyAuth)) pReplySocksArgu->dwProxyEnable = 0x00;	
	nstriconv::unicode_ansi(pReplySocksArgu->szUserName, pNetworkConfig->szUserName);
	nstriconv::unicode_ansi(pReplySocksArgu->szPassword, pNetworkConfig->szPassword);
}  