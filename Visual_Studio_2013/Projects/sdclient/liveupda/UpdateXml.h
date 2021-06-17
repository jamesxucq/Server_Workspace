#pragma once

#include <stdio.h>
#include "expat\expat.h"

#include "LiveVec.h"
#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")

struct XML_LDATA_NAME{
	int Depth;
	int Element;
	struct LiveData *ldata;
	TCHAR *szVersion;
	LiveVec *pLiveVec;
};

class CUpdateXml {
public:
	CUpdateXml(void);
	~CUpdateXml(void);
public:
	DWORD Initialize(TCHAR *szVersion, LiveVec *pLiveVec);
	VOID Finalize();
private:
	struct XML_LDATA_NAME m_tLdataName;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseUpdateXml(const char *buffer, int length);
};

extern CUpdateXml objUpdateXml;
