#pragma once

#include "common.h"
// #include "clientcom/clientcom.h"
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <boost/tr1/unordered_map.hpp>
using std::tr1::unordered_map;
#include <vector>
using  std::vector;

#define LOCKED_ACTION_DEFAULT				_T("~\\locked_action.acd")
//
#define	LATT_FILE_ATT		0x000F0000
#define	LATT_FILE			0x00010000
#define	LATT_DIRECTORY		0x00020000

#define	LATT_LOCKED_ATT		0x00F00000
#define	LATT_EXLOCK			0x00100000
#define	LATT_UNLOCK			0x00200000

#define	LATT_FINISH_ATT		0xF0000000
#define	LATT_FINISHED		0x10000000
#define	LATT_DISCARD		0x20000000

#define ACTION_TYPE(ATYPE)	(0x0000FFFF & ATYPE)	
#define LOCK_ATTRIB(ATYPE)	(0xFFFF0000 & ATYPE)	
//
#define INVA_LANDE_VAL		((ULONG)-1)
//
struct LockAction {
	DWORD dwActionType;
	DWORD dwExceptFound;
	TCHAR szFileName[MAX_PATH];
	DWORD ulTimestamp;
};
typedef vector <struct LockAction *> ActionVec;
typedef unordered_map <wstring, ULONG> LockActHmap;

//
namespace NActionVec {
	inline VOID DeleteNode(struct LockAction *lock_action)
	{ if(lock_action) free(lock_action); }
	struct LockAction *AppendNewNode(ActionVec *pActionVec);
	VOID DeleteActionVec(ActionVec *pActionVec);
};

//
class CLockAction {
public:
	CLockAction(void);
	~CLockAction(void);
public:
	DWORD Initialize(TCHAR *szLocation);
	DWORD Finalize();
private:
	TCHAR m_szLockAction[MAX_PATH];
	HANDLE m_hLockAction;
public:
	HANDLE LockActionInitial();
	VOID FinalLockAction();
	// sync locked watch dir files modify insert to temp file
public:
	ULONG ReadNode(struct Action *pAction, ULONG lPosiInde);
	ULONG WriteNode(struct Action *pAction, ULONG lPosiInde);
	DWORD ModifyNode(ULONG lPosiInde, DWORD dwNextType);
public:
	void ValidAddition();
	void ValidRecycled();
public:
	DWORD ValidPreproFinished(); // 0:succ 0x01:err
	DWORD WildActionNode();
public:
	DWORD LoadLockAction(ActionVec *pLockAction);
};

//
extern CLockAction objLockAction;