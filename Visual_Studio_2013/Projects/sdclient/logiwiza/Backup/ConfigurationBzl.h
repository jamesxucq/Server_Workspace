#pragma once

namespace NConfigurationBzl
{
	//////////////////////////////////////////////////////////////////////////////////////////
	DWORD Initialize(TCHAR *szConfigFileName);
	DWORD Finalize();

	//////////////////////////////////////////////////////////////////////////////////////////
	//           below about system opt
	inline DWORD SaveConfiguration() {return objParseConfig.SaveConfiguration();}
	//inline struct LocalConfig *LoadConfiguration(IN LPWSTR pXmlFilePath) 
	//{return objParseConfig.LoadConfiguration(pXmlFilePath);}
	inline struct LocalConfig *GetLocalConfig() {return &objParseConfig.m_tLocalConfig;}
	inline struct ClientConfig *GetClientConfig() {return &objParseConfig.m_tLocalConfig.tClientConfig;}
	inline struct ServInform *GetServInform() {return &objParseConfig.m_tLocalConfig.tServInform;}
	inline struct NetworkConfig *GetNetworkConfig() {return &objParseConfig.m_tLocalConfig.tNetworkConfig;}
	inline struct DisplayLabel *GetDisplayLabel() {return &objParseConfig.m_tLocalConfig.tDisplayLabel;}

	//inline bool SetRegistInform(struct RegistInform *pRegistInform)
	// {return objParseConfig.SetRegistInform(pRegistInform);}
};
