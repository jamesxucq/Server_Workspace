#pragma once

#include <stdio.h>
#include "expat\expat.h"

#include "DiresVec.h"
#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")

struct XML_DIRIES_NAME{
	int Depth;
	int Element;
	TCHAR *cure_directory;
	DiresVec *pDiresVec;
};


class CDiresXml {
public:
	CDiresXml(void);
	~CDiresXml(void);
public:
	DWORD Initialize(DiresVec *pDiresVec);
	VOID Finalize();
private:
	struct XML_DIRIES_NAME m_xmlDiresPath;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseDiresPathXml(const char *buffer, unsigned int length);
};

extern CDiresXml objDiresXml;