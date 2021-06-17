#pragma once


#define WORKER_COUNT		5 // eq FIVE_TRANS_THREAD
#define KEEP_ALIVE_INTE		48000 // interval 48s

class CKeepAlive {
public:
	CKeepAlive(void);
	~CKeepAlive(void);
private:
	DWORD ulTimeStamp[WORKER_COUNT];
public:
	DWORD Continue(DWORD dwThreadInde);
};

extern CKeepAlive objKeepAlive;