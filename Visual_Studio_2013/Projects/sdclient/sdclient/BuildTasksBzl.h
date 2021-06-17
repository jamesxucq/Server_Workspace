#pragma once
#include "clientcom/clientcom.h"
#include "TRANSWorker.h"
//#include "FilesVec.h"

//
#define	LIST_VALUE_FAULT	SYNP_VALUE_FAULT		// -1
// #define	LIST_VALUE_RUNNING	SYNP_VALUE_RUNNING		// 0x00000000
// #define	LIST_VALUE_WAITING	SYNP_VALUE_WAITING		// 0x00000001
#define	LIST_VALUE_PAUSED	SYNP_VALUE_PAUSED		// 0x00000002
#define	LIST_VALUE_FORCESTOP	SYNP_VALUE_FORCESTOP	// 0x00000004
#define	LIST_VALUE_FINISH	SYNP_VALUE_FINISH		// 0x00000008
#define	LIST_VALUE_FAILED	SYNP_VALUE_FAILED		// 0x00000010
#define	LIST_VALUE_KILLED	SYNP_VALUE_KILLED		// 0x00000020
#define	LIST_VALUE_NETWORK	SYNP_VALUE_NETWORK		// 0x00000040
// #define	LIST_VALUE_LOCKED	SYNP_VALUE_LOCKED		// 0x00000080
// #define	LIST_VALUE_EXCEPTION	SYNP_VALUE_EXCEPTION	// 0x00010000

#define CONVERT_LIST_STATUS(RET_VALUE, LIST_STATUS) { \
	switch(LIST_STATUS) { \
	case PROCESS_STATUS_FORCESTOP: \
	RET_VALUE = LIST_VALUE_FORCESTOP; \
	break; \
	case PROCESS_STATUS_PAUSED: \
	RET_VALUE = LIST_VALUE_PAUSED; \
	break; \
	case PROCESS_STATUS_KILLED: \
	RET_VALUE = LIST_VALUE_KILLED; \
	break; \
} \
}

enum STATUS_METHOD {
	ES_UNUSE = 0,
	ES_GLIST,
	ES_GET,
	ES_GSYNC,
	ES_PSYNC,
	ES_PUT,
	ES_MOVE,
	ES_DELETE
};

//
struct LIST_ARGUMENT {
	struct SessionArgu	tSeionArgu;
	struct SocketsArgu	tSockeArgu;
	class CListHObj *pListHead;
	class CListVObj *pListVec;
};

namespace NBuildTasksBzl {
	DWORD Initialize(TCHAR *szLocation);
	DWORD Finalize();
	//
	DWORD BuildListEnviro(DWORD *plist_type, DWORD *pslast_anchor, LIST_ARGUMENT *pListArgu, DWORD llast_anchor);
	void EarseListEnviro(LIST_ARGUMENT *pListArgu);
	int BulidSlowListFile(HANDLE hListFile, LIST_ARGUMENT *pListArgu, TCHAR *szLocation);
	int BulidFastListVector(FilesVec *pListVec, LIST_ARGUMENT *pListArgu, TCHAR *szLocation);
	int BulidColiListVector(FileColiVec *pColiVec, LIST_ARGUMENT *pListArgu);
	int BulidExcepCache(struct TasksCache *pExcepCache);
	//
	DWORD CreateSlowTasksList(HANDLE hListFile, LIST_ARGUMENT *pListArgu);
	DWORD CreateFastTasksList(FilesVec *pListVec, DWORD llast_anchor, LIST_ARGUMENT *pListArgu);
	DWORD CreateExceptTasksList(struct TasksCache *pExcepCache);
	inline DWORD GetNodeTotal() { return objTaskList.GetNodeTatol();}
	//
	DWORD InitTListEnviro();
	void TListEnviroFinal();
	//
	int RecursiveListEnviro(LIST_ARGUMENT *pListArgu);
	int RecursiveListVector(DiresVec *pListVec, LIST_ARGUMENT *pListArgu, TCHAR *szLocation);
};

#define FIVE_LIST_THREAD	5

namespace ListDownd {
	int GetSlastAnchor(DWORD *pslast_anchor, CTRANSWorker *pListWorker);
	int GetListHead(unsigned __int64 *pleng, TCHAR *list_chks, CTRANSWorker *pListWorker, DWORD llast_anchor, DWORD slast_anchor);
	int GetFileHead(unsigned __int64 *pleng, FILETIME *last_write, CTRANSWorker *pListWorker, TCHAR *szFileName);
	//
	int RecursiveGetList(struct ListNode *list_node, CTRANSWorker *pListWorker);
	int ListPrepa(struct ListNode **pListNode, class CListHObj *pListHead);
	int FinishWrite(struct ListNode *list_node, class CListVObj *pListVec);
	DWORD BuildListXml(LIST_ARGUMENT *pListArgu);
	//
	// DWORD ConnKeepAlive(CTRANSWorker *pListWorker);
	int TasksSlowBuilder(struct TasksCache *pTasksCache, HANDLE hsatt, TCHAR *szSpaceLocation, CTRANSWorker *pListWorker);
	int TasksFastBuilder(struct TasksCache *pTasksCache, FilesVec *sattent, DWORD llast_anchor, TCHAR *szSpaceLocation);
	void TasksExceptBuilder(struct TasksCache *pTasksCache, TNodeVec *pTNodeV);
	//
	int RecursiveListHead(unsigned __int64 *pleng, TCHAR *list_chks, CTRANSWorker *pListWorker);
};