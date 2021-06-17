#pragma once

#include "DiresVec.h"

class CDiresXml {
public:
	CDiresXml(void);
	~CDiresXml(void);
private:
	DiresVec *m_pDiresVec;
private:
	char *parse_buffe;
	char *parse_posi;
	unsigned int initi_parse;
public:
	DWORD Initialize(DiresVec *pListVec, unsigned int max_len);
	VOID Finalize();
public:
	DWORD ParseDiresPathXml(const char *buffe, unsigned int length);
};

extern CDiresXml objDiresXml;