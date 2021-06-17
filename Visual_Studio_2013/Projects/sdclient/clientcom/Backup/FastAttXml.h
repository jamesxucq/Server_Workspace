#pragma once

#include <stdio.h>
#include "expat\expat.h"

#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")


struct XML_FAST_ATTRIB{
	int Depth;
	int Element;
	struct attent *cure_atte;
	FilesVec *pFilesVec;
};


class CFastAttXml {
public:
	CFastAttXml(void);
	~CFastAttXml(void);
public:
	DWORD Initialize(FilesVec *pFilesVec);
	VOID Finalize();
private:
	struct XML_FAST_ATTRIB m_xmlFilesAttrib;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseFilesAttXml(const char *buffer, unsigned int length);
};

extern CFastAttXml objFastAttXml;