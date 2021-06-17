/*
 * ParseConfig.h
 *
 *  Created on: 2010-3-8
 *      Author: Administrator
 */

#ifndef PARSECONFIG_H_
#define PARSECONFIG_H_

#include "CommonMacro.h"
// #include "Logger.h"




struct ServInformation
{
	//Auth info
	DWORD			uiUID;
////////////////////////////
	wchar_t			sAuthHost[HOST_NAME_LEN];
	DWORD 			iAuthPort;
////////////////////////////
	wchar_t			sServHost[HOST_NAME_LEN];
	DWORD 			iServPort;
////////////////////////////
	wchar_t			sMagicNum[MAGIC_STRING_LEN];
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
struct NetworkConf
{
	//Network
	wchar_t			sDownloadLimit[BOOL_STRING_LEN];//0 false 1 true
	DWORD			iDownloadRate;
	wchar_t			sUploadLimit[BOOL_STRING_LEN];//0 false 1 true 3 auto
	DWORD			iUploadRate;
	//Proxies
	wchar_t			sProxyStatus[BOOL_STRING_LEN];//0 false 1 true -1 auto
	wchar_t			sType[PROXY_TYPE];//HTTP SOCKS4 SOCKS5
	wchar_t			sServer[SERV_NAME_LEN];//
	DWORD			iPort;//
	wchar_t			sProxyAuth[BOOL_STRING_LEN];//0 false 1 true 3 auto
	wchar_t			sUserName[USERNAME_LENGTH];
	wchar_t			sPassword[PASSWORD_LENGTH];
};

struct ServInfo
{
	//Auth info
	//Sync Server info
	wchar_t			sRegisHost[HOST_NAME_LEN];
	DWORD 			iRegisPort;
////////////////////////////
	wchar_t			sAuthHost[HOST_NAME_LEN];
	DWORD 			iAuthPort;
	DWORD			uiUID;
	wchar_t			sMagicNum[MAGIC_STRING_LEN];
////////////////////////////
	wchar_t			sServHost[HOST_NAME_LEN];
	DWORD 			iServPort;
////////////////////////////
};


struct ClientConf
{
	// user info
	wchar_t			sUserName[USERNAME_LENGTH];
	wchar_t			sPassword[PASSWORD_LENGTH];
	//User Data
	wchar_t			sLocation[MAX_PATH];
	wchar_t			sSDIFileName[MAX_PATH];
	DWORD			iSDIFileSize;  //xxx GB
	wchar_t			sFilesAttrib[MAX_PATH];
	wchar_t			sAnchorIndex[MAX_PATH];
	wchar_t			sAnchorData[MAX_PATH];
	wchar_t			sSyncingType[MAX_PATH];
	wchar_t			sDriveRoot[MIN_STRING_LEN];
	wchar_t			sDiskLabel[MIN_STRING_LEN];
	//System
	wchar_t			sClientName[CLIENTNAME_LENGTH];
	wchar_t			sLinkStatus[BOOL_STRING_LEN];	//0 false 1 true
	wchar_t			sAutoRun[BOOL_STRING_LEN]; //0 false 1 true
	wchar_t			sShowBalloonTip[BOOL_STRING_LEN]; //0 false 1 true
	wchar_t			sPasswordProtected[BOOL_STRING_LEN]; //0 false 1 true
	wchar_t			sClientPassword[PASSWORD_LENGTH]; //
	DWORD			nSyncingThreads;
};

/*
struct LogConfig
{
	//log
    wchar_t			sLogfile[MAX_PATH];
    log_lvl_t		iDebugLevel;
};
*/

struct DisplayStatic
{
	// SysLink
	wchar_t			sForgetPassword[MID_STRING_LEN];
	wchar_t			sRegisUser[MID_STRING_LEN];
	wchar_t			sChangePassword[MID_STRING_LEN];
	wchar_t			sAppUpdate[MID_STRING_LEN];
};

struct LocalConf
{
	struct ClientConf		stClientConf;
	struct ServInfo			stServInfo;
	struct NetworkConf		stNetworkConf;
//	struct LogConfig			stLogConf;
	//////////////////////////////////////
	struct DisplayStatic	stDisplayStatic;
};

/*
class CParseConfig
{
public:
	CParseConfig(void);
	~CParseConfig(void);
public:
	struct LocalConf m_stLocalConf;
	inline bool Final() {return true;}
private:
	wchar_t	m_XmlFilePath[MAX_PATH];
public:
	bool SaveLocalConf();
	bool LoadLocalConf(OUT struct LocalConf **pLocalConf, IN LPWSTR pXmlFilePath);
	struct LocalConf *LoadLocalConf(IN LPWSTR pXmlFilePath);
private:
	log_lvl_t StringDebugLevelConvert(wchar_t *sLevel);
	wchar_t *IntDebugLevelConvert(wchar_t *sLevel, log_lvl_t iLevel);
	bool BuildLocalConfXml(OUT CString &strXml, IN struct LocalConf *pLocalConf);
	bool ParseLocalConfXml(OUT struct LocalConf *pLocalConf, IN CString &strXml);
public:
	inline void SetLocalConf(IN struct LocalConf *pLocalConf) {m_stLocalConf = *pLocalConf;}
	inline struct LocalConf *GetLocalConf() {return &m_stLocalConf;}
	inline struct ClientConf *GetClientConf() {return &(m_stLocalConf.stClientConf);}
public:
	bool SetServInformation(struct ServInformation *pServInformation);
};

#define CLIENT_CONFIG_FNAME  _T("Configuration.xml")
extern CParseConfig OParseConfig;
*/
#endif /* PARSECONFIG_H_ */
