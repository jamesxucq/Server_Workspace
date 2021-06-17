#pragma once

class COperaExclusi
{
public:
	COperaExclusi(void);
	~COperaExclusi(void);
private:
	DWORD m_dwOperaExclus;
	CRITICAL_SECTION m_gcsOperaExclus;
public:
	DWORD PassValid();
	VOID ReleaseExclusive(DWORD dwPassExclus);
	DWORD GetExclus();
};

extern COperaExclusi objOperaExclus;
