#pragma once

#include <stdio.h>
#include "expat\expat.h"
#include "common_macro.h"

#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")
#include "ParseConfig.h"

struct XmlRegister {
	DWORD uid;
	serv_address cache_address;;
	char access_key[MIN_TEXT_LEN];
};

struct XML_REGIST{
	int Depth;
	int Element;
	struct XmlRegister *pXmlRegister;
};

class CRegistXml {
public:
	CRegistXml(void);
	~CRegistXml(void);
public:
	DWORD Initialize(struct XmlRegister *pXmlRegister);
	void Finalize();
private:
	struct XML_REGIST m_xmlRegister;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseRegistXml(const char *buffer, int length);
};

extern CRegistXml objRegistXml;
