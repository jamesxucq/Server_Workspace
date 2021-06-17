#pragma once

#include <stdio.h>
#include "expat\expat.h"

#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")


struct XML_SLOW_ATTRIB {
    int Depth;
    int Element;
    struct attent cure_atte;
    HANDLE hFilesAtt;
};


class CSlowAttXml
{
public:
    CSlowAttXml(void);
    ~CSlowAttXml(void);
public:
    DWORD Initialize(HANDLE hFilesAtt);
    VOID Finalize();
private:
    struct XML_SLOW_ATTRIB m_xmlFilesAttrib;
    XML_Parser m_tXmlParser;
public:
    DWORD ParseFilesAttXml(const char *buffer, unsigned int length);
};

extern CSlowAttXml objSlowAttXml;