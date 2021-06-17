#pragma once

#include "clientcom/clientcom.h"

class CResVector {
public:
	CResVector(void);
	~CResVector(void);
private:
	vector<VOID *> m_vResVector;
	CRITICAL_SECTION m_csResVector;
public:
	DWORD AppendResource(VOID *handle);
	DWORD EraseResource(VOID *handle);
	DWORD CloseResource();
};
extern class CResVector objVerifyVector;
extern class CResVector objBuilderVector;
//