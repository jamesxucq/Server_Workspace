#pragma once

#include "clientcom/clientcom.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default
#include <vector>
using  std::vector;

#define EXCEPT_ACTION_DEFAULT			_T("~\\except_action.acd")

#define	EXCEPT_ACTION_BEGIN				0x00000001
#define	EXCEPT_ACTION_END				0x00000002
#define	EXCENT_BEGIN_BYTE(CTRL_CODE)	0x000000FF & (CTRL_CODE)

struct excent {
	DWORD control_code;
	TCHAR szFileName1[MAX_PATH];
	TCHAR szFileName2[MAX_PATH];
	DWORD ulTimestamp;
};

//
struct ExcepActio {
	TCHAR szFileName[MAX_PATH];
	DWORD ulSetStamp;
	DWORD ulEndStamp;
	struct ExcepActio *pNext;
};
typedef unordered_map<wstring, struct ExcepActio *> ActioHmap;

namespace NExcepHmap {
	inline VOID DeleNode(struct ExcepActio *pExcepActio)
	{ if(pExcepActio) free(pExcepActio); }
	struct ExcepActio *NewActioNode(TCHAR *szFileName, DWORD ulSetStamp);
	VOID DeleActioHmap(ActioHmap *pActioHmap);
	//
	struct ExcepActio *InsActioNode(ActioHmap *pActioHmap, ExcepActio *pExcepActio);
	int SetActionDelete(ActioHmap *pActioHmap, TCHAR *szFileName, DWORD ulTimestamp);
};

//
class CExcepActio {
public:
	CExcepActio(void);
	~CExcepActio(void);
	//
public:
	DWORD Initialize(TCHAR *szLocation);
	DWORD Finalize();
private:
	TCHAR m_szExcepActio[MAX_PATH];
	HANDLE m_hExcepActio;
	//
private:
	unordered_map<wstring, struct ExcepActio *> m_mExcepAddition;
	unordered_map<wstring, struct ExcepActio *> m_mExcepModify;
	unordered_map<wstring, struct ExcepActio *> m_mExcepDelete;
private:
	VOID ActioIndeAddi(struct excent *pexent);
	VOID ActioIndeDele(struct excent *pexent);
public:
	DWORD LoadExcepActio();
	VOID DropExcepActio();
public:
	HANDLE ExcepActioInit();
	VOID FinalExcepActio();
	VOID SetExcepActio(struct TaskNode *task_node); // no letter
	VOID ExcepActioErase(struct TaskNode *task_node); // no letter
public:
	// 检查文件是否被监控和需要同步
	DWORD ExcepAddition(const TCHAR *szFileName, DWORD ulTimestamp);
	DWORD ExcepDelete(const TCHAR *szFileName, DWORD ulTimestamp);
	DWORD ExcepModify(const TCHAR *szFileName, DWORD ulTimestamp);
};
extern CExcepActio objExcepActio;

//
#define EXCEPT_ACTION_INITIAL	objExcepActio.ExcepActioInit();
#define FINAL_EXCEPT_ACTION		objExcepActio.FinalExcepActio();

#define LINKER_EXCEPT_START(TASK_NODE)		objExcepActio.SetExcepActio(TASK_NODE);
#define LINKER_EXCEPT_SUCCESS(TASK_NODE)	objExcepActio.ExcepActioErase(TASK_NODE);
#define LINKER_EXCEPT_FAILED(TASK_NODE)		objExcepActio.ExcepActioErase(TASK_NODE);