/*
 * ParseConfig.cpp
 *
 *  Created on: 2010-3-8
 *      Author: Administrator
 */
#include "stdafx.h"

// #include "clientcom/Markup.h"
// #include "Logger.h"
#include "ParseConfig.h"

/*
//(wchar_t*)(LPCTSTR)csData
CParseConfig OParseConfig;

CParseConfig::CParseConfig(void)
{
	_tcscpy_s(m_XmlFilePath, _T(""));
}

CParseConfig::~CParseConfig(void)
{
}

wchar_t *CParseConfig::IntDebugLevelConvert(wchar_t *sLevel, log_lvl_t iLevel) {
	// static wchar_t sLevel[32];
	if(!sLevel) return NULL;
	switch(iLevel){
	case 0:
		_tcscpy_s(sLevel, 32, _T("FATAL"));
		break;
	case 1:
		_tcscpy_s(sLevel, 32, _T("ERROR"));
		break;
	case 2:
		_tcscpy_s(sLevel, 32, _T("WARN"));
		break;
	case 3:
		_tcscpy_s(sLevel, 32, _T("INFO"));
		break;
	case 4:
		_tcscpy_s(sLevel, 32, _T("DEBUG"));
		break;
	case 5:
		_tcscpy_s(sLevel, 32, _T("TRACE"));
		break;
	case 6:
		_tcscpy_s(sLevel, 32, _T("MAX"));
		break;
	}

    return sLevel;
}

bool CParseConfig::BuildLocalConfXml(OUT CString &strXml, IN struct LocalConf *pLocalConf)
{
	CMarkup xml;

	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));
	xml.AddElem(_T("SyncClient"));
	xml.AddAttrib(_T("Version"), _T("1.2.1"));//Ìí¼ÓÊôÐÔ
	xml.IntoElem(); //SyncClient
		xml.AddElem(_T("ServInfo"));
		xml.IntoElem(); //ServInfo
			xml.AddElem(_T("RegisConf"));
			xml.IntoElem(); //RegisConf
				xml.AddElem(_T("Host"), pLocalConf->stServInfo.sRegisHost);			
				xml.AddElem(_T("Port"), pLocalConf->stServInfo.iRegisPort);
			xml.OutOfElem(); //RegisConf
			xml.AddElem(_T("AuthConf"));
			xml.IntoElem(); //AuthConf
				xml.AddElem(_T("Host"), pLocalConf->stServInfo.sAuthHost);			
				xml.AddElem(_T("Port"), pLocalConf->stServInfo.iAuthPort);
				xml.AddElem(_T("UID"), pLocalConf->stServInfo.uiUID);
				xml.AddElem(_T("MagicNum"), pLocalConf->stServInfo.sMagicNum);
			xml.OutOfElem(); //AuthConf
			xml.AddElem(_T("ServConf"));
			xml.IntoElem(); //ServConf
				xml.AddElem(_T("Host"), pLocalConf->stServInfo.sServHost);			
				xml.AddElem(_T("Port"), pLocalConf->stServInfo.iServPort);
			xml.OutOfElem(); //ServConf
		xml.OutOfElem(); //ServInfo
		xml.AddElem(_T("ClientConf"));
		xml.IntoElem(); //ClientConf
			xml.AddElem(_T("UserInfo"));
			xml.IntoElem(); //UserInfo
				xml.AddElem(_T("UserName"), pLocalConf->stClientConf.sUserName);	
				xml.AddElem(_T("Password"), pLocalConf->stClientConf.sPassword);
			xml.OutOfElem(); //UserInfo
			xml.AddElem(_T("UserData"));
			xml.IntoElem(); //UserData
				xml.AddElem(_T("Location"), pLocalConf->stClientConf.sLocation);
				xml.AddElem(_T("SDIFileName"), pLocalConf->stClientConf.sSDIFileName);
				xml.AddElem(_T("SDIFileSize"), pLocalConf->stClientConf.iSDIFileSize);
				xml.AddElem(_T("FilesAttrib"), pLocalConf->stClientConf.sFilesAttrib);
				xml.AddElem(_T("AnchorIndex"), pLocalConf->stClientConf.sAnchorIndex);
				xml.AddElem(_T("AnchorData"), pLocalConf->stClientConf.sAnchorData);
				xml.AddElem(_T("SyncingType"), pLocalConf->stClientConf.sSyncingType);
				xml.AddElem(_T("DriveRoot"), pLocalConf->stClientConf.sDriveRoot);
			xml.OutOfElem(); //UserData
			xml.AddElem(_T("System"));
			xml.IntoElem(); //System
				xml.AddElem(_T("ClientName"), pLocalConf->stClientConf.sClientName);
				xml.AddElem(_T("LinkStatus"), pLocalConf->stClientConf.sLinkStatus);
				xml.AddElem(_T("AutoRun"), pLocalConf->stClientConf.sAutoRun);
				xml.AddElem(_T("ShowBalloonTip"), pLocalConf->stClientConf.sShowBalloonTip);
				xml.AddElem(_T("PasswordProtect"), pLocalConf->stClientConf.sPasswordProtected);
				xml.AddElem(_T("ClientPassword"), pLocalConf->stClientConf.sClientPassword);
				xml.AddElem(_T("SyncingThreads"), pLocalConf->stClientConf.nSyncingThreads);
			xml.OutOfElem(); //System
		xml.OutOfElem(); //ClientConf
		xml.AddElem(_T("NetworkConf"));
		xml.IntoElem(); //NetworkConf
			xml.AddElem(_T("NetworkRate"));
			xml.IntoElem(); //NetworkRate
				xml.AddElem(_T("DownloadLimit"), pLocalConf->stNetworkConf.sDownloadLimit);			
				xml.AddElem(_T("DownloadRate"), pLocalConf->stNetworkConf.iDownloadRate);
				xml.AddElem(_T("UploadLimit"), pLocalConf->stNetworkConf.sUploadLimit);			
				xml.AddElem(_T("UploadRate"), pLocalConf->stNetworkConf.iUploadRate);
			xml.OutOfElem(); //NetworkRate
			xml.AddElem(_T("Proxies"));
			xml.IntoElem(); //Proxies
				xml.AddElem(_T("ProxyStatus"), pLocalConf->stNetworkConf.sProxyStatus);			
				xml.AddElem(_T("Type"), pLocalConf->stNetworkConf.sType);
				xml.AddElem(_T("Server"), pLocalConf->stNetworkConf.sServer);			
				xml.AddElem(_T("Port"), pLocalConf->stNetworkConf.iPort);
				xml.AddElem(_T("ProxyAuth"), pLocalConf->stNetworkConf.sProxyAuth);			
				xml.AddElem(_T("UserName"), pLocalConf->stNetworkConf.sUserName);
				xml.AddElem(_T("Password"), pLocalConf->stNetworkConf.sPassword);
			xml.OutOfElem(); //Proxies
		xml.OutOfElem(); //NetworkConf
		xml.AddElem(_T("LogConfig"));
		xml.IntoElem(); //LogConfig
			xml.AddElem(_T("Logfile"), pLocalConf->stLogConf.sLogfile);
			wchar_t sLevel[32];
			xml.AddElem(_T("DebugLevel"), IntDebugLevelConvert(sLevel, pLocalConf->stLogConf.iDebugLevel));
		xml.OutOfElem(); //LogConfig
		xml.AddElem(_T("Display"));
		xml.IntoElem(); //Display
			xml.AddElem(_T("SysLink"));
			xml.IntoElem(); //SysLink
				xml.AddElem(_T("ForgetPassword"), pLocalConf->stDisplayStatic.sForgetPassword);
				xml.AddElem(_T("RegisUser"), pLocalConf->stDisplayStatic.sRegisUser);
				xml.AddElem(_T("ChangePassword"), pLocalConf->stDisplayStatic.sChangePassword);
				xml.AddElem(_T("AppUpdate"), pLocalConf->stDisplayStatic.sAppUpdate);
			xml.OutOfElem(); //SysLink
		xml.OutOfElem(); //Display
	xml.OutOfElem(); //SyncClient

	strXml = xml.GetDoc();

	return true;
}

log_lvl_t CParseConfig::StringDebugLevelConvert(wchar_t *sLevel) {
    log_lvl_t iLevel;

    if (!_tcscmp(_T("FATAL"), sLevel)) iLevel = log_lvl_fatal;
    else if (!_tcscmp(_T("ERROR"), sLevel)) iLevel = log_lvl_error;
    else if (!_tcscmp(_T("WARN"), sLevel)) iLevel = log_lvl_warn;
    else if (!_tcscmp(_T("INFO"), sLevel)) iLevel = log_lvl_info;
    else if (!_tcscmp(_T("DEBUG"), sLevel)) iLevel = log_lvl_debug;
    else if (!_tcscmp(_T("TRACE"), sLevel)) iLevel = log_lvl_trace;
    else if (!_tcscmp(_T("MAX"), sLevel)) iLevel = log_lvl_max;
    else iLevel = log_lvl_max;

    return iLevel;
}

bool CParseConfig::ParseLocalConfXml(OUT struct LocalConf *pLocalConf, IN CString &strXml)
{
	CMarkup xml;
	CString csTagName = _T("");
	CString csData = _T(""); 

	xml.SetDoc(strXml);
	xml.ResetMainPos();  
	xml.FindElem(_T("SyncClient"));
	xml.IntoElem(); //SyncClient
		while (xml.FindElem())
		{		
			csTagName = xml.GetTagName();
			if (_T("ServInfo") == csTagName)
			{			
				xml.IntoElem(); //ServInfo			
				while (xml.FindElem())
				{	
					csTagName = xml.GetTagName();
					if (_T("RegisConf") == csTagName)
					{
						xml.IntoElem(); //RegisConf
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("Host") == csTagName)
								_tcscpy_s(pLocalConf->stServInfo.sRegisHost, csData);
							else if (_T("Port") == csTagName)
								pLocalConf->stServInfo.iRegisPort = _tstoi(csData);
						}
						xml.OutOfElem(); //RegisConf
					}
					else if (_T("AuthConf") == csTagName)
					{
						xml.IntoElem(); //AuthConf
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("Host") == csTagName)
								_tcscpy_s(pLocalConf->stServInfo.sAuthHost, csData);
							else if (_T("Port") == csTagName)
								pLocalConf->stServInfo.iAuthPort = _tstoi(csData);
							else if (_T("UID") == csTagName)
								pLocalConf->stServInfo.uiUID = _tstoi(csData);
							else if (_T("MagicNum") == csTagName)
								_tcscpy_s(pLocalConf->stServInfo.sMagicNum, csData);
						}
						xml.OutOfElem(); //AuthConf
					}
					else if (_T("ServConf") == csTagName)
					{
						xml.IntoElem(); //ServConf
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("Host") == csTagName)
								_tcscpy_s(pLocalConf->stServInfo.sServHost, csData);
							else if (_T("Port") == csTagName)
								pLocalConf->stServInfo.iServPort = _tstoi(csData);
						}
						xml.OutOfElem(); //ServConf
					}
				}
				xml.OutOfElem(); //ServInfo
			}
			else if (_T("ClientConf") == csTagName)
			{			
				xml.IntoElem(); //ClientConf			
				while (xml.FindElem())
				{	
					csTagName = xml.GetTagName();
					if (_T("UserInfo") == csTagName)
					{
						xml.IntoElem(); //UserInfo
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("UserName") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sUserName, csData);
							else if (_T("Password") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sPassword, csData);
						}
						xml.OutOfElem(); //UserInfo
					}
					else if (_T("UserData") == csTagName)
					{
						xml.IntoElem(); //UserData
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();
	
							if (_T("Location") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sLocation, csData);
							else if (_T("SDIFileName") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sSDIFileName, csData);
							else if (_T("SDIFileSize") == csTagName)
								pLocalConf->stClientConf.iSDIFileSize = _tstoi(csData);
							else if (_T("FilesAttrib") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sFilesAttrib, csData);
							else if (_T("AnchorIndex") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sAnchorIndex, csData);
							else if (_T("AnchorData") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sAnchorData, csData);
							else if (_T("SyncingType") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sSyncingType, csData);
							else if (_T("DriveRoot") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sDriveRoot, csData);
						}
						xml.OutOfElem(); //UserData
					}
					else if (_T("System") == csTagName)
					{
						xml.IntoElem(); //System
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("ClientName") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sClientName, csData);
							else if (_T("LinkStatus") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sLinkStatus, csData);
							else if (_T("AutoRun") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sAutoRun, csData);
							else if (_T("ShowBalloonTip") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sShowBalloonTip, csData);
							else if (_T("PasswordProtect") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sPasswordProtected, csData);
							else if (_T("ClientPassword") == csTagName)
								_tcscpy_s(pLocalConf->stClientConf.sClientPassword, csData);
							else if (_T("SyncingThreads") == csTagName)
								pLocalConf->stClientConf.nSyncingThreads = _tstoi(csData);
						}
						xml.OutOfElem(); //System
					}
				}
				xml.OutOfElem(); //ClientConf
			}
			else if (_T("NetworkConf") == csTagName)
			{
				xml.IntoElem(); //NetworkConf
				while(xml.FindElem())
				{
					csTagName = xml.GetTagName();
					if (_T("NetworkRate") == csTagName)
					{
						xml.IntoElem(); //NetworkRate
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("DownloadLimit") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sDownloadLimit, csData);
							else if (_T("DownloadRate") == csTagName)
								pLocalConf->stNetworkConf.iDownloadRate = _tstoi(csData);
							else if (_T("UploadLimit") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sUploadLimit, csData);
							else if (_T("UploadRate") == csTagName)
								pLocalConf->stNetworkConf.iUploadRate = _tstoi(csData);
						}
						xml.OutOfElem(); //NetworkRate
					}
					else if (_T("Proxies") == csTagName)
					{
						xml.IntoElem(); //Proxies
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("ProxyStatus") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sProxyStatus, csData);
							else if (_T("Type") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sType, csData);
							else if (_T("Server") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sServer, csData);
							else if (_T("Port") == csTagName)
								pLocalConf->stNetworkConf.iPort = _tstoi(csData);
							else if (_T("ProxyAuth") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sProxyAuth, csData);
							else if (_T("UserName") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sUserName, csData);
							else if (_T("Password") == csTagName)
								_tcscpy_s(pLocalConf->stNetworkConf.sPassword, csData);
						}
						xml.OutOfElem(); //Proxies
					}
				}
				xml.OutOfElem(); //NetworkConf
			}
			else if (_T("LogConfig") == csTagName)
			{
				xml.IntoElem(); //LogConfig
				while (xml.FindElem())
				{
					csTagName = xml.GetTagName();
					csData = xml.GetData();

					if (_T("Logfile") == csTagName)
						_tcscpy_s(pLocalConf->stLogConf.sLogfile, csData);
					else if (_T("DebugLevel") == csTagName)
						pLocalConf->stLogConf.iDebugLevel = StringDebugLevelConvert((wchar_t*)(LPCTSTR)csData);
				}
				xml.OutOfElem(); //LogConfig
			}
			else if (_T("Display") == csTagName)
			{
				xml.IntoElem(); //Display
				while (xml.FindElem())
				{
					csTagName = xml.GetTagName();
					if (_T("SysLink") == csTagName) 
					{
						xml.IntoElem(); //SysLink
						while (xml.FindElem())
						{
							csTagName = xml.GetTagName();
							csData = xml.GetData();

							if (_T("ForgetPassword") == csTagName)
								_tcscpy_s(pLocalConf->stDisplayStatic.sForgetPassword, csData);
							else if (_T("RegisUser") == csTagName)
								_tcscpy_s(pLocalConf->stDisplayStatic.sRegisUser, csData);
							else if (_T("ChangePassword") == csTagName)
								_tcscpy_s(pLocalConf->stDisplayStatic.sChangePassword, csData);
							else if (_T("AppUpdate") == csTagName)
								_tcscpy_s(pLocalConf->stDisplayStatic.sAppUpdate, csData);
						}
						xml.OutOfElem(); //SysLink
					}
				}
				xml.OutOfElem(); //Display
			}
		}
	xml.OutOfElem(); //SyncClient

	return true;
}


bool CParseConfig::SaveLocalConf()
{
	CMarkup xml;
	CString strXml;

	if (!BuildLocalConfXml(strXml, &m_stLocalConf)) return false;
	if (!xml.SetDoc(strXml)) return false;
	if (!xml.Save(m_XmlFilePath)) return false;

	return true;
}

bool CParseConfig::LoadLocalConf(OUT struct LocalConf **pLocalConf, IN LPWSTR pXmlFilePath)
{
	CMarkup xml;
	CString strXml;

	if (!xml.Load(pXmlFilePath)) return false;
	strXml = xml.GetDoc();

	if (!ParseLocalConfXml(&m_stLocalConf, strXml)) return false;
	*pLocalConf = &m_stLocalConf;
	_tcscpy_s(m_XmlFilePath, pXmlFilePath);

	return true;
}

struct LocalConf *CParseConfig::LoadLocalConf(IN LPWSTR pXmlFilePath)
{
	CMarkup xml;
	CString strXml;

	if (!xml.Load(pXmlFilePath)) return NULL;
	strXml = xml.GetDoc();

	if (!ParseLocalConfXml(&m_stLocalConf, strXml)) return NULL;
	_tcscpy_s(m_XmlFilePath, pXmlFilePath);

	return &m_stLocalConf;
}

bool CParseConfig::SetServInformation(struct ServInformation *pServInformation)
{
	if(!pServInformation)
		return false;

	struct ServInfo *pServInfo = &m_stLocalConf.stServInfo;
	pServInfo->uiUID = pServInformation->uiUID;
	_tcscpy_s(pServInfo->sAuthHost, HOST_NAME_LEN, pServInformation->sAuthHost);	
	pServInfo->iAuthPort = pServInformation->iAuthPort;
	_tcscpy_s(pServInfo->sServHost, HOST_NAME_LEN, pServInformation->sServHost);
	pServInfo->iServPort = pServInformation->iServPort;
	_tcscpy_s(pServInfo->sMagicNum, MAGIC_STRING_LEN, pServInformation->sMagicNum);

	return SaveLocalConf();
}
*/