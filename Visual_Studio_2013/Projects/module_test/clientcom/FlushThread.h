#pragma once

#include "AnchorCache.h"
#include "Anchor.h"

struct FLUSH_THREAD_ARGU {
	LONG bProcessStarted;
	/////////////////
	CAnchor *pAnchor;
	CAnchorCache *pAnchorCache;
	HANDLE hFlushEvent;
	DWORD *dwLastTickCount;
};

class CFlushThread {
public:
	CFlushThread(void);
	~CFlushThread(void);
public:
	DWORD Initialize(CAnchor *pAnchor, CAnchorCache *pAnchorCache);
	DWORD Finalize();
private:
	struct FLUSH_THREAD_ARGU m_tFlushArgu;
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