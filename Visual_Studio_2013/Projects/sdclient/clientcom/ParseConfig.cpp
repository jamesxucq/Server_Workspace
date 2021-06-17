/*
* ParseConfig.cpp
*
*  Created on: 2010-3-8
*      Author: Administrator
*/
#include "stdafx.h"

#include "third_party.h"
#include "Encoding.h"
#include "ParseConfig.h"


// (TCHAR*)(LPCTSTR)csValue
CParseConfig objParseConfig;

CParseConfig::CParseConfig(void) {
    memset(&m_tLocalConfig, 0, sizeof(struct LocalConfig));
    _tcscpy_s(m_szXmlFileName, _T(""));
}

CParseConfig::~CParseConfig(void) {
}

TCHAR *CParseConfig::DebugLevelText(TCHAR *szLevel, log_lvl_t iLevel) {
    // static TCHAR szLevel[32];
    if(!szLevel) return NULL;
    switch(iLevel) {
    case log_lvl_fatal:
        _tcscpy_s(szLevel, 32, _T("FATAL"));
        break;
    case log_lvl_error:
        _tcscpy_s(szLevel, 32, _T("ERROR"));
        break;
    case log_lvl_warn:
        _tcscpy_s(szLevel, 32, _T("WARN"));
        break;
    case log_lvl_info:
        _tcscpy_s(szLevel, 32, _T("INFO"));
        break;
    case log_lvl_debug:
        _tcscpy_s(szLevel, 32, _T("DEBUG"));
        break;
    case log_lvl_trace:
        _tcscpy_s(szLevel, 32, _T("TRACE"));
        break;
    case log_lvl_max:
        _tcscpy_s(szLevel, 32, _T("MAX"));
        break;
    }
//
    return szLevel;
}

DWORD CParseConfig::BuildConfigXml(CString &csXml, struct LocalConfig *pLocalConfig) {
    struct NetworkConfig *pNetworkConfig = &pLocalConfig->tNetworkConfig;
    struct ServParam *pServParam = &pLocalConfig->tServParam;
    struct ClientConfig *pClientConfig = &pLocalConfig->tClientConfig;
    struct LogConfig *pLogConfig = &pLocalConfig->tLogConfig;
    struct DisplayLink *pDisplayLink = &pLocalConfig->tDisplayLink;
    CMarkup xml;
    TCHAR szCiphertext[PASSWORD_LENGTH];
//
    xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));
    xml.AddElem(_T("SyncAgent"));
    xml.AddAttrib(_T("Version"), pClientConfig->szVersion); // Ìí¼ÓÊôÐÔ
    xml.IntoElem(); // SyncAgent
    xml.AddElem(_T("ServParam"));
    xml.IntoElem(); // ServParam
    xml.AddElem(_T("RegistConfig"));
    xml.AddAttrib(_T("Address"), pServParam->tRegistAddress.sin_addr);
    xml.AddAttrib(_T("Port"), pServParam->tRegistAddress.sin_port);
    xml.AddElem(_T("UpdateServer"));
    xml.AddAttrib(_T("Address"), pServParam->tUpdateAddress.sin_addr);
    xml.AddAttrib(_T("Port"), pServParam->tUpdateAddress.sin_port);
    xml.OutOfElem(); // ServParam
    xml.AddElem(_T("ClientConfig"));
    xml.IntoElem(); // ClientConfig
    xml.AddElem(_T("UserParam"));
    xml.IntoElem(); // UserParam
    xml.AddElem(_T("UserName"), pClientConfig->szUserName);
    xml.AddElem(_T("Password"), NEncoding::EncodeBase64Text(szCiphertext, pClientConfig->szPassword, pClientConfig->szSalt));
    xml.AddAttrib(_T("Salt"), pClientConfig->szSalt);
    xml.OutOfElem(); // UserParam
    xml.AddElem(_T("UserSpace"));
    xml.IntoElem(); // UserSpace
    xml.AddElem(_T("Location"), pClientConfig->szLocation);
    xml.AddElem(_T("UserPool"), pClientConfig->szUserPool);
    xml.AddAttrib(_T("Length"), pClientConfig->dwPoolLength);
    xml.AddElem(_T("FileForbid"), pClientConfig->szFileForbid);
    xml.AddElem(_T("FileConflict"), pClientConfig->szFileConfli);
    xml.AddElem(_T("Anchor"));
    xml.AddAttrib(_T("Index"), pClientConfig->szAnchorInde);
    xml.AddAttrib(_T("Data"), pClientConfig->szAnchorData);
    xml.AddElem(_T("DriveLetter"), pClientConfig->szDriveLetter);
    xml.OutOfElem(); // UserSpace
    xml.AddElem(_T("System"));
    xml.IntoElem(); // System
    xml.AddElem(_T("LocalIdenti"), pClientConfig->szLocalIdenti);
    xml.AddElem(_T("AccountLinked"), pClientConfig->szAccountLinked);
    xml.AddElem(_T("AutoRun"), pClientConfig->szAutoRun);
    xml.AddElem(_T("ShowBalloonTip"), pClientConfig->szShowBalloonTip);
    xml.AddElem(_T("AutoUpdate"), pClientConfig->szAutoUpdate);
    xml.AddElem(_T("Protected"));
    xml.AddAttrib(_T("Enable"), pClientConfig->szProtected);
    xml.IntoElem(); // Protected
    xml.AddElem(_T("Password"), pClientConfig->szLocalPassword);
    xml.AddAttrib(_T("Salt"), pClientConfig->szSaltValue);
    xml.OutOfElem(); // Protected
    xml.AddElem(_T("ForceCtrl"), pClientConfig->szForceCtrl);
    xml.OutOfElem(); // System
    xml.OutOfElem(); // ClientConfig
    xml.AddElem(_T("NetworkConfig"));
    xml.IntoElem(); // NetworkConfig
    xml.AddElem(_T("NetworkRate"));
    xml.IntoElem(); // NetworkRate
    xml.AddElem(_T("Download"));
    xml.AddAttrib(_T("Limit"), pNetworkConfig->szDowndLimit);
    xml.AddAttrib(_T("Rate"), pNetworkConfig->dwDowndRate);
    xml.AddElem(_T("Upload"));
    xml.AddAttrib(_T("Limit"), pNetworkConfig->szUploadLimit);
    xml.AddAttrib(_T("Rate"), pNetworkConfig->dwUploadRate);
    xml.OutOfElem(); // NetworkRate
    xml.AddElem(_T("Proxies"));
    xml.AddAttrib(_T("Enable"), pNetworkConfig->szProxyEnable);
    xml.IntoElem(); // Proxies
    xml.AddElem(_T("Type"), pNetworkConfig->szProxyType);
    xml.AddElem(_T("Address"), pNetworkConfig->tAddress.sin_addr);
    xml.AddAttrib(_T("Port"), pNetworkConfig->tAddress.sin_port);
    xml.AddElem(_T("ProxyAuth"));
    xml.AddAttrib(_T("Enable"), pNetworkConfig->szProxyAuth);
    xml.IntoElem(); // ProxyAuth
    xml.AddElem(_T("UserName"), pNetworkConfig->szUserName);
    xml.AddElem(_T("Password"), NEncoding::EncodeBase64Text(szCiphertext, pNetworkConfig->szPassword, pNetworkConfig->szSalt));
    xml.AddAttrib(_T("Salt"), pNetworkConfig->szSalt);
    xml.OutOfElem(); // ProxyAuth
    xml.OutOfElem(); // Proxies
    xml.OutOfElem(); // NetworkConfig
    xml.AddElem(_T("LogConfig"));
    xml.AddAttrib(_T("Logfile"), pLocalConfig->tLogConfig.szLogfile);
    TCHAR szLevel[32];
    xml.AddAttrib(_T("DebugLevel"), DebugLevelText(szLevel, pLocalConfig->tLogConfig.dwDebugLevel));
    xml.AddElem(_T("Display"));
    xml.IntoElem(); // Display
    xml.AddElem(_T("SystemLink"));
    xml.IntoElem(); // SystemLink
    xml.AddElem(_T("ForgetPassword"), pDisplayLink->szForgetPassword);
    xml.AddElem(_T("RegistUser"), pDisplayLink->szRegistUser);
    xml.AddElem(_T("ChangePassword"), pDisplayLink->szChangePassword);
    xml.AddElem(_T("CompanyLink"), pDisplayLink->szCompanyLink);
    xml.OutOfElem(); // SystemLink
    xml.OutOfElem(); // Display
    xml.OutOfElem(); // SyncAgent
//
    csXml = xml.GetDoc();
//
    return 0x00;
}

log_lvl_t CParseConfig::DebugLevelValue(TCHAR *szLevel) {
    log_lvl_t iLevel;
//
    if (!_tcscmp(_T("FATAL"), szLevel)) iLevel = log_lvl_fatal;
    else if (!_tcscmp(_T("ERROR"), szLevel)) iLevel = log_lvl_error;
    else if (!_tcscmp(_T("WARN"), szLevel)) iLevel = log_lvl_warn;
    else if (!_tcscmp(_T("INFO"), szLevel)) iLevel = log_lvl_info;
    else if (!_tcscmp(_T("DEBUG"), szLevel)) iLevel = log_lvl_debug;
    else if (!_tcscmp(_T("TRACE"), szLevel)) iLevel = log_lvl_trace;
    else if (!_tcscmp(_T("MAX"), szLevel)) iLevel = log_lvl_max;
    else iLevel = log_lvl_max;
//
    return iLevel;
}

DWORD CParseConfig::ParseConfigXml(struct LocalConfig *pLocalConfig, CString &csXml) {
    struct NetworkConfig *pNetworkConfig = &pLocalConfig->tNetworkConfig;
    struct ServParam *pServParam = &pLocalConfig->tServParam;
    struct ClientConfig *pClientConfig = &pLocalConfig->tClientConfig;
    struct LogConfig *pLogConfig = &pLocalConfig->tLogConfig;
    struct DisplayLink *pDisplayLink = &pLocalConfig->tDisplayLink;
    CMarkup xml;
    CString csLable = _T("");
    CString csAttrib = _T("");
    CString csValue = _T("");
    TCHAR szCiphertext[PASSWORD_LENGTH];
//
    xml.SetDoc(csXml);
    xml.ResetMainPos();
    if(xml.FindElem(_T("SyncAgent"))) {
        csAttrib = xml.GetAttrib(_T("Version"));
        _tcscpy_s(pClientConfig->szVersion, csAttrib);
        xml.IntoElem(); // SyncAgent
        while (xml.FindElem()) {
            csLable = xml.GetTagName();
            if (_T("ServParam") == csLable) {
                xml.IntoElem(); // ServParam
                while (xml.FindElem()) {
                    csLable = xml.GetTagName();
                    if (_T("RegistConfig") == csLable) {
                        csAttrib = xml.GetAttrib(_T("Address"));
                        _tcscpy_s(pServParam->tRegistAddress.sin_addr, csAttrib);
                        csAttrib = xml.GetAttrib(_T("Port"));
                        pServParam->tRegistAddress.sin_port = _tstoi(csAttrib);
                    } else if (_T("UpdateServer") == csLable) {
                        csAttrib = xml.GetAttrib(_T("Address"));
                        _tcscpy_s(pServParam->tUpdateAddress.sin_addr, csAttrib);
                        csAttrib = xml.GetAttrib(_T("Port"));
                        pServParam->tUpdateAddress.sin_port = _tstoi(csAttrib);
                    }
                }
                xml.OutOfElem(); // ServParam
            } else if (_T("ClientConfig") == csLable) {
                xml.IntoElem(); // ClientConfig
                while (xml.FindElem()) {
                    csLable = xml.GetTagName();
                    if (_T("UserParam") == csLable) {
                        xml.IntoElem(); // UserParam
                        while (xml.FindElem()) {
                            csLable = xml.GetTagName();
                            csValue = xml.GetData();

                            if (_T("UserName") == csLable)
                                _tcscpy_s(pClientConfig->szUserName, csValue);
                            else if (_T("Password") == csLable) {
                                _tcscpy_s(szCiphertext, csValue);
                                csAttrib = xml.GetAttrib(_T("Salt"));
                                _tcscpy_s(pClientConfig->szSalt, csAttrib);
                                NEncoding::DecodeBase64Text(pClientConfig->szPassword, szCiphertext, pClientConfig->szSalt);
                            }
                        }
                        xml.OutOfElem(); // UserParam
                    } else if (_T("UserSpace") == csLable) {
                        xml.IntoElem(); // UserSpace
                        while (xml.FindElem()) {
                            csLable = xml.GetTagName();
                            csValue = xml.GetData();

                            if (_T("Location") == csLable)
                                _tcscpy_s(pClientConfig->szLocation, csValue);
                            else if (_T("UserPool") == csLable) {
                                _tcscpy_s(pClientConfig->szUserPool, csValue);
                                csAttrib = xml.GetAttrib(_T("Length"));
                                pClientConfig->dwPoolLength = _tstoi(csAttrib);
                            } else if (_T("FileForbid") == csLable)
                                _tcscpy_s(pClientConfig->szFileForbid, csValue);
							else if (_T("FileConflict") == csLable)
                                _tcscpy_s(pClientConfig->szFileConfli, csValue);
                            else if (_T("Anchor") == csLable) {
                                csAttrib = xml.GetAttrib(_T("Index"));
                                _tcscpy_s(pClientConfig->szAnchorInde, csAttrib);
                                csAttrib = xml.GetAttrib(_T("Data"));
                                _tcscpy_s(pClientConfig->szAnchorData, csAttrib);
                            } else if (_T("DriveLetter") == csLable)
                                _tcscpy_s(pClientConfig->szDriveLetter, csValue);
                        }
                        xml.OutOfElem(); // UserSpace
                    } else if (_T("System") == csLable) {
                        xml.IntoElem(); // System
                        while (xml.FindElem()) {
                            csLable = xml.GetTagName();
                            csValue = xml.GetData();

                            if (_T("LocalIdenti") == csLable)
                                _tcscpy_s(pClientConfig->szLocalIdenti, csValue);
                            else if (_T("AccountLinked") == csLable)
                                _tcscpy_s(pClientConfig->szAccountLinked, csValue);
                            else if (_T("AutoRun") == csLable)
                                _tcscpy_s(pClientConfig->szAutoRun, csValue);
                            else if (_T("ShowBalloonTip") == csLable)
                                _tcscpy_s(pClientConfig->szShowBalloonTip, csValue);
                            else if (_T("AutoUpdate") == csLable)
                                _tcscpy_s(pClientConfig->szAutoUpdate, csValue);
                            else if (_T("Protected") == csLable) {
                                csAttrib = xml.GetAttrib(_T("Enable"));
                                _tcscpy_s(pClientConfig->szProtected, csAttrib);
                                xml.IntoElem(); // Protected
                                while (xml.FindElem()) {
                                    csLable = xml.GetTagName();
                                    csValue = xml.GetData();

                                    if (_T("Password") == csLable) {
                                        _tcscpy_s(pClientConfig->szLocalPassword, csValue);
                                        csAttrib = xml.GetAttrib(_T("Salt"));
                                        _tcscpy_s(pClientConfig->szSaltValue, csAttrib);
                                    }
                                }
                                xml.OutOfElem(); // Protected
                            } else if (_T("ForceCtrl") == csLable)
                                _tcscpy_s(pClientConfig->szForceCtrl, csValue);
                        }
                        xml.OutOfElem(); // System
                    }
                }
                xml.OutOfElem(); // ClientConfig
            } else if (_T("NetworkConfig") == csLable) {
                xml.IntoElem(); // NetworkConfig
                while(xml.FindElem()) {
                    csLable = xml.GetTagName();
                    if (_T("NetworkRate") == csLable) {
                        xml.IntoElem(); // NetworkRate
                        while (xml.FindElem()) {
                            csLable = xml.GetTagName();
                            csValue = xml.GetData();

                            if (_T("Download") == csLable) {
                                csAttrib = xml.GetAttrib(_T("Limit"));
                                _tcscpy_s(pNetworkConfig->szDowndLimit, csAttrib);
                                csAttrib = xml.GetAttrib(_T("Rate"));
                                pNetworkConfig->dwDowndRate = _tstoi(csAttrib);
                            } else if (_T("Upload") == csLable) {
                                csAttrib = xml.GetAttrib(_T("Limit"));
                                _tcscpy_s(pNetworkConfig->szUploadLimit, csAttrib);
                                csAttrib = xml.GetAttrib(_T("Rate"));
                                pNetworkConfig->dwUploadRate = _tstoi(csAttrib);
                            }
                        }
                        xml.OutOfElem(); // NetworkRate
                    } else if (_T("Proxies") == csLable) {
                        csAttrib = xml.GetAttrib(_T("Enable"));
                        _tcscpy_s(pNetworkConfig->szProxyEnable, csAttrib);
                        xml.IntoElem(); // Proxies
                        while (xml.FindElem()) {
                            csLable = xml.GetTagName();
                            csValue = xml.GetData();

                            if (_T("Type") == csLable)
                                _tcscpy_s(pNetworkConfig->szProxyType, csValue);
                            else if (_T("Address") == csLable) {
                                _tcscpy_s(pNetworkConfig->tAddress.sin_addr, csValue);
                                csAttrib = xml.GetAttrib(_T("Port"));
                                pNetworkConfig->tAddress.sin_port = _tstoi(csValue);
                            } else if (_T("ProxyAuth") == csLable) {
                                csAttrib = xml.GetAttrib(_T("Enable"));
                                _tcscpy_s(pNetworkConfig->szProxyAuth, csAttrib);
                                xml.IntoElem(); // szProxyAuth
                                while (xml.FindElem()) {
                                    csLable = xml.GetTagName();
                                    csValue = xml.GetData();

                                    if (_T("UserName") == csLable)
                                        _tcscpy_s(pNetworkConfig->szUserName, csValue);
                                    else if (_T("Password") == csLable) {
                                        _tcscpy_s(szCiphertext, csValue);
                                        csAttrib = xml.GetAttrib(_T("Salt"));
                                        _tcscpy_s(pNetworkConfig->szSalt, csAttrib);
                                        NEncoding::DecodeBase64Text(pNetworkConfig->szPassword, szCiphertext, pNetworkConfig->szSalt);
                                    }
                                }
                                xml.OutOfElem(); // szProxyAuth
                            }
                        }
                        xml.OutOfElem(); // Proxies
                    }
                }
                xml.OutOfElem(); // NetworkConfig
            } else if (_T("LogConfig") == csLable) {
                csAttrib = xml.GetAttrib(_T("Logfile"));
                _tcscpy_s(pLocalConfig->tLogConfig.szLogfile, csAttrib);
                csAttrib = xml.GetAttrib(_T("DebugLevel"));
                pLocalConfig->tLogConfig.dwDebugLevel = DebugLevelValue((TCHAR*)(LPCTSTR)csAttrib);
            } else if (_T("Display") == csLable) {
                xml.IntoElem(); // Display
                while (xml.FindElem()) {
                    csLable = xml.GetTagName();
                    if (_T("SystemLink") == csLable) {
                        xml.IntoElem(); // SystemLink
                        while (xml.FindElem()) {
                            csLable = xml.GetTagName();
                            csValue = xml.GetData();

                            if (_T("ForgetPassword") == csLable)
                                _tcscpy_s(pDisplayLink->szForgetPassword, csValue);
                            else if (_T("RegistUser") == csLable)
                                _tcscpy_s(pDisplayLink->szRegistUser, csValue);
                            else if (_T("ChangePassword") == csLable)
                                _tcscpy_s(pDisplayLink->szChangePassword, csValue);
                            else if (_T("CompanyLink") == csLable)
                                _tcscpy_s(pDisplayLink->szCompanyLink, csValue);
                        }
                        xml.OutOfElem(); // SystemLink
                    }
                }
                xml.OutOfElem(); // Display
            }
        }
        xml.OutOfElem(); // SyncAgent
    }
//
    return 0x00;
}

DWORD CParseConfig::SaveConfiguration() {
    CMarkup xml;
    CString csXml;
//
    if (BuildConfigXml(csXml, &m_tLocalConfig)) return ((DWORD)-1);
    if (!xml.SetDoc(csXml)) return ((DWORD)-1);
    if (!xml.Save(m_szXmlFileName)) return ((DWORD)-1);
//
    return 0x00;
}

struct LocalConfig *CParseConfig::LoadConfiguration(LPCTSTR pXmlFilePath) {
    CMarkup xml;
    CString csXml;
//
    if (!xml.Load(pXmlFilePath)) return NULL;
    csXml = xml.GetDoc();
//
    if (ParseConfigXml(&m_tLocalConfig, csXml)) return NULL;
    _tcscpy_s(m_szXmlFileName, pXmlFilePath);
//
    return &m_tLocalConfig;
}
