#pragma once

#include <stdio.h>
#include "expat\expat.h"

#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")

struct XmlSettings {
	char client_version[VERSION_LENGTH];
	DWORD uid;
	int pool_size;
	char data_bomb[BOOL_TEXT_LEN];
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
	VOID Finalize();
private:
	struct XML_SETTINGS m_tSettingsXml;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseSettingsXml(const char *buffer, unsigned int length);
};

extern CSettingsXml objSettingsXml;
