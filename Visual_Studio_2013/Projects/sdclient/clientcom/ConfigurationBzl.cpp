#include "StdAfx.h"

#include "FileUtility.h"
#include "ConfigurationBzl.h"


DWORD NConfigBzl::Initialize(TCHAR *szConfigName) {
	// Load location config
	TCHAR szXmlFilePath[MAX_PATH];
//
	MKZEO(szXmlFilePath);
	if(!NFileUtility::ApplicaDataPath(szXmlFilePath, szConfigName)) 
		return ((DWORD)-1);
//
	if(!objParseConfig.LoadConfiguration(szXmlFilePath))
		return ((DWORD)-1);
//
	return 0x00;
}

DWORD NConfigBzl::Finalize() {
	if(objParseConfig.SaveConfiguration()) 
		return ((DWORD)-1);
//
	return 0x00;
}