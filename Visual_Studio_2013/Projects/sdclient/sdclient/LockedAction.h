#pragma once

#include "clientcom/clientcom.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;
#include <vector>
using  std::vector;

#define LOCKED_ACTION_DEFAULT		_T("~\\locked_action.tmp")
//
#define	LATT_FILE_ATT		0x000F0000
#define	LATT_FILE			0x00010000
#define	LATT_DIRECTORY		0x00020000

#define	LATT_LOCKED_ATT		0x0F000000
#define	LATT_LOCKED			0x01000000
#define	LATT_UNLOCK			0x02000000

#define	LATT_FINISH_ATT		0xF0000000
#define	LATT_FINISHED		0x10000000
#define	LATT_DISCARD		0x20000000

#define ACTI_ATTRIB_BYTE(ATYPE)	(0x000FFFFF & ATYPE)
#define LOCKED_ATTRIB(ATYPE)	(0xFFFF0000 & ATYPE)
#define FILE_ACTION_BYTE(ATYPE)	(0x0000FFFF & ATYPE)
//
#define INVA_LANDE_VALU		((ULONG)-1)
//
struct LockActio {
    DWORD dwActioType;
    TCHAR szFileName[MAX_PATH];
    DWORD dwExcepFound;
    DWORD ulTimestamp;
	//
	DWORD coupleID; //
	ULONG ulFileInde;
};
typedef vector <struct LockActio *> LkAiVec;
typedef unordered_map <wstring, ULONG> LkAiHmap;

//
namespace NLkAiVec {
	inline VOID DeleNode(struct LockActio *lock_acti)
	{ if(lock_acti) free(lock_acti); }
	struct LockActio *AppNewNode(LkAiVec *pLkAiVec);
	struct LockActio *AppNewNode(LkAiVec *pLkAiVec, TCHAR *szFileName, DWORD dwActioType, DWORD coupleID);
	struct LockActio *InsPrevNewNode(LkAiVec *pLkAiVec, struct LockActio *lock_acti);
	VOID DeleLkAiVec(LkAiVec *pLkAiVec);
	VOID MoveLkAiVec(LkAiVec *pToVec, LkAiVec *pExiVec, LkAiVec *pFromVec);
};

//
class CLockActio {
public:
    CLockActio(void);
    ~CLockActio(void);
public:
    DWORD Initialize(TCHAR *szLocation);
    DWORD Finalize();
private:
    TCHAR m_szLockActio[MAX_PATH];
    HANDLE m_hLockActio;
public:
    HANDLE LockActioIniti();
    VOID FinalLockActio(BOOL bOk);
	HANDLE ResetLockActio();
    // sync locked watch dir files modify insert to temp file
public:
    ULONG ReadNode(struct Action *pActioNode, ULONG lPosiInde);
    ULONG WriteNode(struct Action *pActioNode, ULONG lPosiInde);
    DWORD ModifyNode(ULONG lPosiInde, DWORD dwNextType);
public:
    void ValidAddition();
    void ValidRecycled();
public:
    DWORD ValidPreproFinished(); // 0:succ >0:undone
public:
    DWORD LoadLockActio(LkAiVec *pLkAiVec);
	inline BOOL IsLockActioExist() // 文件存在就有数据
	{ return NFileUtility::FileExist(m_szLockActio); }
};

//
extern CLockActio objLockActio;


