#include "StdAfx.h"

#include "NormalSockets.h"
#include "ProxySockets.h"

#include "SdSocketsBzl.h"



CSdSocketsBzl::CSdSocketsBzl(void)
{
}

CSdSocketsBzl::~CSdSocketsBzl(void)
{
}

CMsgSocketsBzl::CMsgSocketsBzl(void)
{
}

CMsgSocketsBzl::~CMsgSocketsBzl(void)
{
}

CSdSocketsBzl *NSdSocketsBzl::Factory(struct SocketsArgu *pSocketsArgu)
{
	CSdSocketsBzl *pSdSockets = NULL;

	if(!_tcscmp(_T("false"), pSocketsArgu->sProxyStatus)) {
		pSdSockets = new CNormalSockets;
		pSdSockets->Create(pSocketsArgu);
// LOG_DEBUG(_T("no proxy."));
	}
	else if(!_tcscmp(__T("true"), pSocketsArgu->sProxyStatus)) {
		pSdSockets = new CProxySockets;
		pSdSockets->Create(pSocketsArgu);
// LOG_DEBUG(_T("proxy to server. server:%s port:%d"), pSocketsArgu->sServer, pSocketsArgu->iPort);
	}
	else if(!_tcscmp(__T("auto"), pSocketsArgu->sProxyStatus)) {
		wchar_t sProxyServ[SERV_NAME_LEN];
		int iProxyPort;

		memset(sProxyServ, '\0', SERV_NAME_LEN);
		DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(sProxyServ, &iProxyPort);
		if(0x01 == dwProxyEnable) {
			_tcscpy_s(pSocketsArgu->sType, PROXY_TYPE, _T("HTTP"));
			_tcscpy_s(pSocketsArgu->sServer, SERV_NAME_LEN, sProxyServ);
			pSocketsArgu->iPort = iProxyPort;
			_tcscpy_s(pSocketsArgu->sProxyAuth, BOOL_STRING_LEN, _T("false"));

			pSdSockets = new CProxySockets;
			pSdSockets->Create(pSocketsArgu);
// LOG_DEBUG(_T("get ie proxy ok! server:%s port:%d"), sProxyServ, iProxyPort);
		}
		else if(0x00 == dwProxyEnable) {
			pSdSockets = new CNormalSockets;
			pSdSockets->Create(pSocketsArgu);
// LOG_DEBUG(_T("get ie proxy ok! no proxy! server:%s port:%d"), pSocketsArgu->sServer, pSocketsArgu->iPort);
		}
	}

	return pSdSockets;
}

void NSdSocketsBzl::DestroyObject(CSdSocketsBzl *pCSdSockets)
{ delete pCSdSockets; }

void CONVERT_SOCKETS(struct SocketsArgu *pSocketsArgu, struct LocalConf *pLocalConf, unsigned int nSyncingThreads) {
	/////////////////////////////////////////////////////////////////////////////
	(pSocketsArgu)->nSyncingThreads = nSyncingThreads;
	/////////////////////////////////////////////////////////////////////////////
	_tcscpy_s((pSocketsArgu)->sServHost, (pLocalConf)->stServInfo.sServHost); 
	(pSocketsArgu)->iServPort = (pLocalConf)->stServInfo.iServPort; 
	/////////////////////////////////////////////////////////////////////////////
	_tcscpy_s((pSocketsArgu)->sDownloadLimit, (pLocalConf)->stNetworkConf.sDownloadLimit); 
	(pSocketsArgu)->iDownloadRate = (pLocalConf)->stNetworkConf.iDownloadRate; 
	_tcscpy_s((pSocketsArgu)->sUploadLimit, (pLocalConf)->stNetworkConf.sUploadLimit); 
	(pSocketsArgu)->iUploadRate = (pLocalConf)->stNetworkConf.iUploadRate; 
	_tcscpy_s((pSocketsArgu)->sProxyStatus, (pLocalConf)->stNetworkConf.sProxyStatus); 
	_tcscpy_s((pSocketsArgu)->sType, (pLocalConf)->stNetworkConf.sType); 
	_tcscpy_s((pSocketsArgu)->sServer, (pLocalConf)->stNetworkConf.sServer); 
	(pSocketsArgu)->iPort = (pLocalConf)->stNetworkConf.iPort; 
	_tcscpy_s((pSocketsArgu)->sProxyAuth, (pLocalConf)->stNetworkConf.sProxyAuth); 
	_tcscpy_s((pSocketsArgu)->sUserName, (pLocalConf)->stNetworkConf.sUserName); 
	_tcscpy_s((pSocketsArgu)->sPassword, (pLocalConf)->stNetworkConf.sPassword); 
}  


CMsgSocketsBzl *NSdSocketsBzl::Factory(struct MsgSocketsArgu *pMsgSocketsArgu)
{
	CMsgSocketsBzl *pMessageSockets = NULL;

	if(!_tcscmp(__T("false"), pMsgSocketsArgu->sProxyStatus)) {
		pMessageSockets = new CMsgNormalSockets;
		pMessageSockets->Create(pMsgSocketsArgu);
// LOG_DEBUG(_T("no proxy."));
	}
	else if(!_tcscmp(__T("true"), pMsgSocketsArgu->sProxyStatus)) {
		pMessageSockets = new CMsgProxySockets;
		pMessageSockets->Create(pMsgSocketsArgu);
// LOG_DEBUG(_T("proxy to server. server:%s port:%d"), pMsgSocketsArgu->sServer, pMsgSocketsArgu->iPort);
	}
	else if(!_tcscmp(__T("auto"), pMsgSocketsArgu->sProxyStatus)) {
		wchar_t sProxyServ[SERV_NAME_LEN];
		int iProxyPort;

		memset(sProxyServ, '\0', SERV_NAME_LEN);
		DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(sProxyServ, &iProxyPort);
		if(0x01 == dwProxyEnable) {
			_tcscpy_s(pMsgSocketsArgu->sType, PROXY_TYPE, _T("HTTP"));
			NStringCon::unicode_ansi(pMsgSocketsArgu->sServer, sProxyServ);
			pMsgSocketsArgu->iPort = iProxyPort;
			_tcscpy_s(pMsgSocketsArgu->sProxyAuth, BOOL_STRING_LEN, _T("false"));

			pMessageSockets = new CMsgProxySockets;
			pMessageSockets->Create(pMsgSocketsArgu);
// LOG_DEBUG(_T("get ie proxy ok! server:%s port:%d"), sProxyServ, iProxyPort);
		}
		else if(0x00 == dwProxyEnable) {
			pMessageSockets = new CMsgNormalSockets;
			pMessageSockets->Create(pMsgSocketsArgu);
// LOG_DEBUG(_T("get ie proxy ok! no proxy! server:%s port:%d"), pMsgSocketsArgu->sServer, pMsgSocketsArgu->iPort);
		}
	}

	return pMessageSockets;
}

void NSdSocketsBzl::DestroyObject(CMsgSocketsBzl *pMessageSockets)
{ delete pMessageSockets; }


void CONVERT_MSG_SOCKETS(struct MsgSocketsArgu *pMsgSocketsArgu, struct LocalConf *pLocalConf) { 
	/////////////////////////////////////////////////////////////////////////////
	NStringCon::unicode_ansi((pMsgSocketsArgu)->sRegisHost, (pLocalConf)->stServInfo.sRegisHost); 
	(pMsgSocketsArgu)->iRegisPort = (pLocalConf)->stServInfo.iRegisPort; 
	NStringCon::unicode_ansi((pMsgSocketsArgu)->sAuthHost, (pLocalConf)->stServInfo.sAuthHost); 
	(pMsgSocketsArgu)->iAuthPort = (pLocalConf)->stServInfo.iAuthPort; 
	/////////////////////////////////////////////////////////////////////////////
	_tcscpy_s((pMsgSocketsArgu)->sDownloadLimit, (pLocalConf)->stNetworkConf.sDownloadLimit); 
	(pMsgSocketsArgu)->iDownloadRate = (pLocalConf)->stNetworkConf.iDownloadRate; 
	_tcscpy_s((pMsgSocketsArgu)->sUploadLimit, (pLocalConf)->stNetworkConf.sUploadLimit); 
	(pMsgSocketsArgu)->iUploadRate = (pLocalConf)->stNetworkConf.iUploadRate; 
	_tcscpy_s((pMsgSocketsArgu)->sProxyStatus, (pLocalConf)->stNetworkConf.sProxyStatus); 
	_tcscpy_s((pMsgSocketsArgu)->sType, (pLocalConf)->stNetworkConf.sType); 
	NStringCon::unicode_ansi((pMsgSocketsArgu)->sServer, (pLocalConf)->stNetworkConf.sServer); 
	(pMsgSocketsArgu)->iPort = (pLocalConf)->stNetworkConf.iPort; 
	_tcscpy_s((pMsgSocketsArgu)->sProxyAuth, (pLocalConf)->stNetworkConf.sProxyAuth); 
	NStringCon::unicode_ansi((pMsgSocketsArgu)->sUserName, (pLocalConf)->stNetworkConf.sUserName); 
	NStringCon::unicode_ansi((pMsgSocketsArgu)->sPassword, (pLocalConf)->stNetworkConf.sPassword); 
}  

DWORD NSdSocketsBzl::SD_IsNetworkAlive()
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

int NSdSocketsBzl::InitialWinsock()
{
	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		TRACE(_T("Error at WSAStartup()\n"));
		return -1;
	}

	return 0;
}


int NSdSocketsBzl::FinishWinsock()
{
	// Sleep(1024); // wait for graceful close
	//----------------------
	// Finish Winsock
	int iResult = WSACleanup();
	if (iResult != NO_ERROR){
		TRACE(_T("Error at WSACleanup()\n"));
		return -1;
	}

	return 0;
}
