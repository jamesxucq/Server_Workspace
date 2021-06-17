#pragma once
#include "clientcom/clientcom.h"

class CTRANSWorker : public CSDTPUtility {
public:
	CTRANSWorker(void);
	~CTRANSWorker(void);
public:
	DWORD GetNewAnchor();
public:
	inline VOID CloseResource() 
	{ if(m_pTRANSSeionBzl) m_pTRANSSeionBzl->KillTRANSSession(); }
};

//
class CWorkerVector {
public:
	CWorkerVector(void);
	~CWorkerVector(void);
private:
	vector<CTRANSWorker *> vWorkerVector;
	CRITICAL_SECTION m_csWorkerVector;
public:
	DWORD AppendTransWorker(CTRANSWorker *pTransWorker);
	DWORD EraseTransWorker(CTRANSWorker *pTransWorker);
	DWORD CloseTransWorker();
};
extern class CWorkerVector objWorkerVector;

//
namespace NTRANSWorker {
	CTRANSWorker *Factory(struct SessionArgu *pSeionArgu, struct SocketsArgu *pSockeArgu);
	VOID DestroyObject(CTRANSWorker *pTransWorker);
	//
	VOID CloseWorkerResource();
};

//