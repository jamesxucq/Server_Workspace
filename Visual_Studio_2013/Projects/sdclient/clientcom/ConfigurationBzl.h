#pragma once

#include "ParseConfig.h"

namespace NConfigBzl {
	DWORD Initialize(TCHAR *szConfigName);
	DWORD Finalize();
	//           below about system opt
	inline DWORD SaveConfiguration() {return objParseConfig.SaveConfiguration(); }
	// inline struct LocalConfig *LoadConfiguration(LPCTSTR pXmlFilePath) 
	// {return objParseConfig.LoadConfiguration(pXmlFilePath); }
	inline struct LocalConfig *GetLocalConfig() {return &objParseConfig.m_tLocalConfig; }
	inline struct ClientConfig *GetClientConfig() {return &objParseConfig.m_tLocalConfig.tClientConfig; }
	inline struct ServParam *GetServParam() {return &objParseConfig.m_tLocalConfig.tServParam; }
	inline struct NetworkConfig *GetNetworkConfig() {return &objParseConfig.m_tLocalConfig.tNetworkConfig; }
	inline struct LogConfig *GetLogConfig() {return &objParseConfig.m_tLocalConfig.tLogConfig; }
	inline struct DisplayLink *GetDisplayLink() {return &objParseConfig.m_tLocalConfig.tDisplayLink; }
};
