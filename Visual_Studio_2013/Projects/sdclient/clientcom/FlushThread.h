#pragma once

#include "AnchorCache.h"
#include "Anchor.h"
/*
struct FLUSH_ARGUMENT {
	volatile LONG bProcessStarted;
	/////////////////
	CAnchor *pAnchor;
	CAnchorCache *pAnchorCache;
	HANDLE hFlushEvent;
};

class CFlushThread {
public:
	CFlushThread(void);
	~CFlushThread(void);
public:
	DWORD Initialize(CAnchor *pAnchor, CAnchorCache *pAnchorCache);
	DWORD Finalize();
private:
	struct FLUSH_ARGUMENT m_tFlushArgu;
private:
	CWinThread *m_pFlushThread;
	volatile HANDLE m_hFlushEvent; // ÊÂ¼þ¾ä±ú
public:
	inline BOOL PulseFlushEvent() 
	{ return SetEvent(m_hFlushEvent); }
public:
	DWORD BeginFlushThread();
};

extern class CFlushThread objFlushThread;

UINT FlushCacheProcess(LPVOID lpParam);
*/