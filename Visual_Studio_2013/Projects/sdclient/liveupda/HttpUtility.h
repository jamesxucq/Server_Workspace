#pragma once

#include "clientcom/lupdatecom.h"
#include "LiveSocks.h"

namespace NHttpUtility {
	static SOCKET Factory(char *address, int port, struct NetworkConfig *pNetworkConfig);
	static void Close(SOCKET socks);
	//
	static DWORD HttpGet(HANDLE hTo, CLiveSocks *pLiveSocks, char *szHost, char *szReqURI);
	//
	DWORD HttpDownd(HANDLE hTo, char *szReqURL, ServAddress *pUpdateAddress, struct NetworkConfig *pNetworkConfig);
};
