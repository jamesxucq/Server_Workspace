#include "StdAfx.h"
#include "ReplySocks.h"


CReplySocks::CReplySocks(void):
NPSocksType(NONE)
{
}

CReplySocks::~CReplySocks(void)
{
}

bool CReplySocks::Create(char *szDestnServ, int dwDestnPort, struct NetworkConf *pNetworkConf)
{
	if(!_tcscmp(__T("false"), pNetworkConf->szProxyStatus)) {
		NPSocksType = NSOCKS;
		ONormalReplySocks.Create(szDestnServ, dwDestnPort);
	}
	else if(!_tcscmp(__T("true"), pNetworkConf->szProxyStatus)) {
		NPSocksType = PSOCKS;
		OProxyReplySocks.Create(szDestnServ, dwDestnPort, pNetworkConf);
	}
	else if(!_tcscmp(__T("auto"), pNetworkConf->szProxyStatus)) {
		wchar_t ProxyServ[SERV_NAME];
		int ProxyPort;

		memset(ProxyServ, '\0', SERV_NAME);
		DWORD dwProxyEnable = NDetectProxy::GetIEPorxyServ(ProxyServ, &ProxyPort);
		if(0x01 == dwProxyEnable) {
			NPSocksType = PSOCKS;
			_tcscpy_s(pNetworkConf->szProxyType, PROXY_TYPE, _T("HTTP"));
			_tcscpy_s(pNetworkConf->szServer, SERV_NAME, ProxyServ);
			pNetworkConf->nPort = ProxyPort;
			_tcscpy_s(pNetworkConf->sProxyAuth, BOOL_STRING_LEN, _T("false"));
			OProxyReplySocks.Create(szDestnServ, dwDestnPort, pNetworkConf);
		}
		else if(0x00 == dwProxyEnable) {
			NPSocksType = NSOCKS;
			ONormalReplySocks.Create(szDestnServ, dwDestnPort);
		}
	}

	return true;
}


