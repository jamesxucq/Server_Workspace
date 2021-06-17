#pragma once

#include <stdio.h>
#include "expat\expat.h"

// #include "DiresVec.h"
#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")

struct XmlAnchor {
	UINT uiUID;
	DWORD last_anchor;
};

struct XML_ANCHOR{
	int Depth;
	int Element;
	struct XmlAnchor *pXmlAnchor;
};

class CAnchorXml {
public:
	CAnchorXml(void);
	~CAnchorXml(void);
public:
	DWORD Initialize(struct XmlAnchor *pXmlAnchor);
	void Finalize();
private:
	struct XML_ANCHOR m_xmlAnchor;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseAnchorXml(const char *buffer, int length);
};

extern CAnchorXml objAnchorXml;

/*
namespace NAnchorXml {
DWORD 

}
*/

