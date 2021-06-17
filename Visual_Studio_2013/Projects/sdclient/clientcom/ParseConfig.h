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


//
typedef struct {
	TCHAR sin_addr[HOSTNAME_LENGTH];
	int sin_port;
} ServAddress;

typedef struct {
	char sin_addr[HOSTNAME_LENGTH];
	int sin_port;
} serv_address;

//
struct NetworkConfig {
	// network
	TCHAR			szDowndLimit[BOOL_TEXT_LEN];// 0 false 1 true
	DWORD			dwDowndRate;
	TCHAR			szUploadLimit[BOOL_TEXT_LEN];// 0 false 1 true 3 auto
	DWORD			dwUploadRate;
	// proxies
	TCHAR			szProxyEnable[BOOL_TEXT_LEN];// 0 false 1 true -1 auto
	TCHAR			szProxyType[PROXY_TYPE_LEN];// HTTP SOCKS4 SOCKS5
	ServAddress		tAddress;
	TCHAR			szProxyAuth[BOOL_TEXT_LEN];// 0 false 1 true 3 auto
	TCHAR			szUserName[USERNAME_LENGTH];
	TCHAR			szSalt[SALT_TEXT_LEN];
	TCHAR			szPassword[PASSWORD_LENGTH];
};

struct ServParam { // sync server inform
	ServAddress		tRegistAddress;
	ServAddress		tUpdateAddress;
};


struct ClientConfig { 
	TCHAR			szVersion[VERSION_LENGTH];
	// user inform
	TCHAR			szUserName[USERNAME_LENGTH];
	TCHAR			szSalt[MIN_TEXT_LEN];
	TCHAR			szPassword[PASSWORD_LENGTH];
	// user data
	TCHAR			szLocation[MAX_PATH];
	TCHAR			szUserPool[MAX_PATH];
	DWORD			dwPoolLength;  // xxx GB
	TCHAR			szFileForbid[MAX_PATH];
	TCHAR			szFileConfli[MAX_PATH];
	TCHAR			szAnchorInde[MAX_PATH];
	TCHAR			szAnchorData[MAX_PATH];
	TCHAR			szDriveLetter[MIN_TEXT_LEN];
	TCHAR			szDiskLabel[MIN_TEXT_LEN];
	// system client inform
	TCHAR			szLocalIdenti[IDENTI_LENGTH];
	TCHAR			szAccountLinked[BOOL_TEXT_LEN];	// 0 false 1 true
	TCHAR			szAutoRun[BOOL_TEXT_LEN]; // 0 false 1 true
	TCHAR			szShowBalloonTip[BOOL_TEXT_LEN]; // 0 false 1 true
	TCHAR			szAutoUpdate[BOOL_TEXT_LEN]; // true false-query
	TCHAR			szProtected[BOOL_TEXT_LEN]; // 0 false 1 true
	TCHAR			szSaltValue[MIN_TEXT_LEN];
	TCHAR			szLocalPassword[MD5_TEXT_LENGTH]; //
	TCHAR			szForceCtrl[MIN_TEXT_LEN];
};

struct LogConfig { // log
	TCHAR			szLogfile[MAX_PATH];
	log_lvl_t		dwDebugLevel;
};

struct DisplayLink { // systemLink
	TCHAR			szForgetPassword[URI_LENGTH];
	TCHAR			szRegistUser[URI_LENGTH];
	TCHAR			szChangePassword[URI_LENGTH];
	TCHAR			szCompanyLink[URI_LENGTH];
};

struct LocalConfig {
	struct ClientConfig			tClientConfig;
	struct ServParam			tServParam;
	struct NetworkConfig		tNetworkConfig;
	struct LogConfig			tLogConfig;
	//
	struct DisplayLink			tDisplayLink;
};

//
class CParseConfig {
public:
	CParseConfig(void);
	~CParseConfig(void);
public:
	struct LocalConfig m_tLocalConfig;
	inline DWORD Finalize() { return 0x00; }
private:
	TCHAR	m_szXmlFileName[MAX_PATH];
public:
	DWORD SaveConfiguration();
	//	DWORD LoadConfiguration(struct LocalConfig **pLocalConfig, LPCTSTR pXmlFilePath);
	struct LocalConfig *LoadConfiguration(LPCTSTR pXmlFilePath);
private:
	log_lvl_t DebugLevelValue(TCHAR *szLevel);
	TCHAR *DebugLevelText(TCHAR *szLevel, log_lvl_t iLevel);
	DWORD BuildConfigXml(CString &csXml, struct LocalConfig *pLocalConfig);
	DWORD ParseConfigXml(struct LocalConfig *pLocalConfig, CString &csXml);
public:
	inline VOID SetLocalConfig(struct LocalConfig *pLocalConfig) { m_tLocalConfig = *pLocalConfig; }
	inline struct LocalConfig *GetLocalConfig() { return &m_tLocalConfig; }
	inline struct ClientConfig *GetClientConfig() { return &(m_tLocalConfig.tClientConfig); }
};

#define CLIENT_CONFIGURATION  _T("Configuration.XML")
extern CParseConfig objParseConfig;

#endif /* PARSECONFIG_H_ */
