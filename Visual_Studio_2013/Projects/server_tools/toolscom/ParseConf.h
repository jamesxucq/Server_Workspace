/*
* ParseConf.h
*
*  Created on: 2010-3-8
*      Author: Administrator
*/

#ifndef PARSECONF_H_
#define PARSECONF_H_

#include "tools_macro.h"
#include "StringUtility.h"

struct NetworkConf
{
	//Proxies
	wchar_t			szProxyStatus[BOOL_STRING_LEN];//0 false 1 true -1 auto
	wchar_t			szProxyType[PROXY_TYPE];//HTTP SOCKS4 SOCKS5
	wchar_t			szServer[SERV_NAME];//
	DWORD			nPort;//
	wchar_t			sProxyAuth[BOOL_STRING_LEN];//0 false 1 true 3 auto
	wchar_t			szUName[USERNAME_LENGTH];
	wchar_t			szPassWD[PASSWORD_LENGTH];
};
extern struct NetworkConf ONetworkConf;

struct CacheParam
{
	//Auth info
	////////////////////////////
	serv_address		Address;
	// DWORD			uiUID;
	wchar_t			szAdmin[USERNAME_LENGTH];
	wchar_t			szPassword[PASSWORD_LENGTH];
};


#endif /* PARSECONF_H_ */
