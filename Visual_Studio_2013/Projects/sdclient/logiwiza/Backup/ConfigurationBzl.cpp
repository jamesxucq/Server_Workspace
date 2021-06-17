#include "StdAfx.h"
#include "ConfigurationBzl.h"

DWORD NConfigurationBzl::Initialize(TCHAR *szConfigFileName) 
{
	// Load location config
	TCHAR szXmlFilePath[MAX_PATH];

	MKZERO(szXmlFilePath);
	if(!NFileUtility::ModuleBaseName(szXmlFilePath)) return -1;
	_tcscat_s(szXmlFilePath, _T("\\"));
	_tcscat_s(szXmlFilePath, szConfigFileName);
	if(!objParseConfig.LoadConfiguration(szXmlFilePath)) return -1;

	return 0;
}

DWORD NConfigurationBzl::Finalize()
{
	if(objParseConfig.SaveConfiguration())
		return -1;

	return 0;
}