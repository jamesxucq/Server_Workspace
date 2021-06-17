/*
* ParseConfig.h
*
*  Created on: 2010-3-8
*      Author: Administrator
*/

#ifndef PARSECONFIG_H_
#define PARSECONFIG_H_

#include "common_macro.h"
#include "third_party.h"


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
typedef struct {
	TCHAR sin_addr[HOSTNAME_LENGTH];
	int sin_port;
} ServAddress;

typedef struct {
	char sin_addr[HOSTNAME_LENGTH];
	int sin_port;
} serv_address;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
struct NetworkConfig {
	// network
	TCHAR			szDownloadLimit[BOOL_TEXT_LEN];//0 false 1 true
	DWORD			dwDownloadRate;
	TCHAR			szUploadLimit[BOOL_TEXT_LEN];//0 false 1 true 3 auto
	DWORD			dwUploadRate;
	// proxies
	TCHAR			szProxyEnable[BOOL_TEXT_LEN];//0 false 1 true -1 auto
	TCHAR			szProxyType[PROXY_TYPE_LEN];//HTTP SOCKS4 SOCKS5
	ServAddress		Address;
	TCHAR			szProxyAuth[BOOL_TEXT_LEN];//0 false 1 true 3 auto
	TCHAR			szUserName[USERNAME_LENGTH];
	TCHAR			szSalt[MIN_TEXT_LEN];
	TCHAR			szPassword[PASSWORD_LENGTH];
};

struct ServInform {
	// sync server inform
	ServAddress		RegistAddress;
};


struct ClientConfig {
	// 
	TCHAR			szVersion[VERSION_LENGTH];
	// user inform
	TCHAR			szUserName[USERNAME_LENGTH];
	TCHAR			szSalt[MIN_TEXT_LEN];
	TCHAR			szPassword[PASSWORD_LENGTH];
	// user data
	TCHAR			szLocation[MAX_PATH];
	TCHAR			szUserPool[MAX_PATH];
	DWORD			dwPoolLength;  //xxx GB
	TCHAR			szFilesDenied[MAX_PATH];
	TCHAR			szAnchorIndex[MAX_PATH];
	TCHAR			szAnchorData[MAX_PATH];
	TCHAR			szDriveRoot[MIN_TEXT_LEN];
	TCHAR			szDiskLabel[MIN_TEXT_LEN];
	// system client inform
	TCHAR			szLocalIdenti[IDENTI_LENGTH];
	TCHAR			szLinkStatus[BOOL_TEXT_LEN];	//0 false 1 true
	TCHAR			szAutoRun[BOOL_TEXT_LEN]; //0 false 1 true
	TCHAR			szShowBalloonTip[BOOL_TEXT_LEN]; //0 false 1 true
	TCHAR			szProtected[BOOL_TEXT_LEN]; //0 false 1 true
	TCHAR			szSaltValue[MIN_TEXT_LEN];
	TCHAR			szLocalPassword[MD5_TEXT_LENGTH]; //
	DWORD			dwWorkerThreads;
};

struct LogConfig {
	//log
	TCHAR			szLogfile[MAX_PATH];
	log_lvl_t		dwDebugLevel;
};

struct DisplayLabel {
	// systemLink
	TCHAR			szForgetPassword[MID_TEXT_LEN];
	TCHAR			szRegistUser[MID_TEXT_LEN];
	TCHAR			szChangePassword[MID_TEXT_LEN];
	TCHAR			szAppUpdate[MID_TEXT_LEN];
};

struct LocalConfig {
	struct ClientConfig			tClientConfig;
	struct ServInform			tServInform;
	struct NetworkConfig		tNetworkConfig;
	struct LogConfig			tLogConfig;
	//////////////////////////////////////
	struct DisplayLabel			tDisplayLabel;
};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class CParseConfig {
public:
	CParseConfig(void);
	~CParseConfig(void);
public:
	struct LocalConfig m_tLocalConfig;
	inline DWORD Finalize() {return 0;}
private:
	TCHAR	m_szXmlFileName[MAX_PATH];
public:
	DWORD SaveConfiguration();
	DWORD LoadConfiguration(OUT struct LocalConfig **pLocalConfig, IN LPWSTR pXmlFilePath);
	struct LocalConfig *LoadConfiguration(IN LPWSTR pXmlFilePath);
private:
	log_lvl_t DebugLevelValue(TCHAR *szLevel);
	TCHAR *IntDebugLevelConv(TCHAR *szLevel, log_lvl_t nLevel);
	DWORD BuildConfigXml(OUT CString &strXml, IN struct LocalConfig *pLocalConfig);
	DWORD ParseConfigXml(OUT struct LocalConfig *pLocalConfig, IN CString &strXml);
public:
	inline void SetLocalConfig(IN struct LocalConfig *pLocalConfig) {m_tLocalConfig = *pLocalConfig;}
	inline struct LocalConfig *GetLocalConfig() {return &m_tLocalConfig;}
	inline struct ClientConfig *GetClientConfig() {return &(m_tLocalConfig.tClientConfig);}
};

#define AGENT_CONFIGURATION  _T("Configuration.xml")
extern CParseConfig objParseConfig;

#endif /* PARSECONFIG_H_ */
