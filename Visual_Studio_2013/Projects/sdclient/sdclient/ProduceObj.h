#pragma once

//
class CVerifyObj
{
public:
	CVerifyObj(void);
	~CVerifyObj(void);
private:
	volatile DWORD m_dwVerifyThreadsCount;
	HANDLE *m_hVerifyThreads;
	CRITICAL_SECTION m_csVerifyThreads;
public:
	DWORD SetVerifyThreads(HANDLE *hVerifyThreads, DWORD dwThreadsCount);
	HANDLE *GetVerifyThreads(DWORD *pVerifyThreadsCount);
};

extern CVerifyObj objVerify;
#define VERIFY_OBJECT(member) objVerify.member

//
class CProduceObj
{
public:
	CProduceObj(void);
	~CProduceObj(void);
private:
	volatile DWORD m_dwListThreadsCount;
	HANDLE *m_hListThreads;
	CRITICAL_SECTION m_csListThreads;
public:
	DWORD SetListThreads(HANDLE *hListThreads, DWORD dwThreadsCount);
	HANDLE *GetListThreads(DWORD *pListThreadsCount);
};

extern CProduceObj objProduce;
#define PRODUCE_OBJECT(member) objProduce.member

//