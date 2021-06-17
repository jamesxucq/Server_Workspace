#pragma once

#include <stdio.h>
#include "expat\expat.h"

#include "FilesVec.h"
#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")

struct XML_FILES_ATTRIB{
	int Depth;
	int Element;
	struct file_attrib *current_fast;
	FilesVec *pFilesVec;
};


class CFilesAttribXml {
public:
	CFilesAttribXml(void);
	~CFilesAttribXml(void);
public:
	DWORD Initialize(FilesVec *pFilesVec);
	void Finalize();
private:
	struct XML_FILES_ATTRIB m_xmlFilesAttrib;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseFilesAttXml(const char *buffer, int length);
};

extern CFilesAttribXml objFilesAttribXml;