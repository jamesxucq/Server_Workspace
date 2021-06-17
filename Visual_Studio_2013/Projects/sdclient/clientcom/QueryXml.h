#pragma once

#include <stdio.h>
#include "expat\expat.h"

#pragma comment(lib, "..\\third party\\expat\\libexpat.lib")
#include "ParseConfig.h"

struct XmlQuery {
	UINT uiUID;
	char req_valid[VALID_LENGTH];
	serv_address address;
	char worker_locked[BOOL_TEXT_LEN]; // 0:false 1:true
	DWORD cached_anchor;
	char session_id[SESSION_LENGTH];
};

struct XML_QUERY {
	int Depth;
	int Element;
	struct XmlQuery *pXmlQuery;
};

class CQueryXml {
public:
	CQueryXml(void);
	~CQueryXml(void);
public:
	DWORD Initialize(struct XmlQuery *pXmlQuery);
	VOID Finalize();
private:
	struct XML_QUERY m_tXmlQuery;
	XML_Parser m_tXmlParser;
public:
	DWORD ParseQueryXml(const char *buffer, unsigned int length);
};

extern CQueryXml objQueryXml;
