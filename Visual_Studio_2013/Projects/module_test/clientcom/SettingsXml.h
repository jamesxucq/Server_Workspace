#pragma once

#include <stdio.h>
#include "expat\expat.h"

#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")

struct XmlSettings {
	char client_version[VERSION_LENGTH];
	DWORD uid;
	int pool_size;
};

struct XML_SETTINGS {
	int Depth;
	int Element;
	struct XmlSettings *pXmlSettings;
};

class CSettingsXml {
public:
	CSettingsXml(void);
	~CSettingsXml(void);
public:
	DWORD Initialize(struct XmlSettings *pXmlSettings);
	void Finalize();
private:
	struct XML_SETTINGS m_xmlSettingsXml;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseSettingsXml(const char *buffer, int length);
};

extern CSettingsXml objSettingsXml;
