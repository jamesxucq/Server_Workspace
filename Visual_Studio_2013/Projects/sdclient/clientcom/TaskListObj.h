#pragma once

#include "common_macro.h"
#include "TasksVec.h"

#include <vector>
using std::vector;

#define TLIST_DATA_DEFAULT			_T("~\\tasks_list_data.tmp")

//
// control code
#define		LOCATION_CLIENT				0x01000000
#define		LOCATION_SERVER				0x02000000
#define		FILE_TASK_BEGIN				0x00010000
#define		FILE_TASK_END				0x00020000

#define		STYPE_GET					0x00000100
#define		STYPE_PUT					0x00000200
#define		STYPE_PSYNC					0x00000400
#define		STYPE_GSYNC					0x00000800
#define		STYPE_MOVE					0x00001000
#define		STYPE_COPY					0x00002000
#define		STYPE_DELETE				0x00004000

#define RANGE_TYPE_INVA					0x00000000 // invalid
#define RANGE_TYPE_FILE                 0x00000001 // file
#define RANGE_TYPE_CHUNK                0x00000002 // chunk
#define RANGE_TYPE_BLOCK                0x00000004 // block 

#define TNODE_LOCATION_BYTE(CTRL_CODE)		(0xFF000000&(CTRL_CODE))
#define TNODE_BEGIN_BYTE(CTRL_CODE)			(0x00FF0000&(CTRL_CODE))
#define TNODE_STYPE_BYTE(CTRL_CODE)			(0x0000FF00&(CTRL_CODE))
#define TNODE_RANGE_BYTE(CTRL_CODE)			(0x000000FF&(CTRL_CODE))
// #define TNODE_LOCATION_STYPE(CTRL_CODE)		(0xFF00FF00&(CTRL_CODE))

//
// success flags
// #define	TRANSMIT_DONE			0x00000000
#define	TRANSMIT_UNDONE			0x00000001
#define	TRANSMIT_EXCEPTION		0x00000002
// #define	VERIFY_DONE				0x00000000
#define	VERIFY_UNDONE			0x00000010
#define	VERIFY_EXCEPTION		0x00000020
// #define	OWRTMRVE_DONE			0x00000000
#define	OWRTMRVE_UNDONE			0x00000100
#define	OWRTMRVE_EXCEPTION		0x00000200
 // virtual
#define	BREAK_ALL_TASK			0xF0000000
#define	WAIVE_ALL_TASK			0x0F000000
//
#define ONLY_TASK_UNDONE(SUCCESS_FLAGS)						(!((BREAK_ALL_TASK|WAIVE_ALL_TASK)&SUCCESS_FLAGS) && (0x00FFFFFF & SUCCESS_FLAGS))
#define ONLY_TASK_OKAY(SUCCESS_FLAGS)						(((BREAK_ALL_TASK|WAIVE_ALL_TASK)&SUCCESS_FLAGS) || !(0x00FFFFFF&SUCCESS_FLAGS))
//
#define	SET_TRANSMIT_DONE(SUCCESS_FLAGS)					(SUCCESS_FLAGS) &= 0xFFFFFFF0
#define	SET_TRANSMIT_EXCEPTION(SUCCESS_FLAGS)				(SUCCESS_FLAGS) = (0xFFFFFFF0&SUCCESS_FLAGS) | TRANSMIT_EXCEPTION
#define ONLY_TRANSMIT_OKAY(SUCCESS_FLAGS)					(0x0000000F & SUCCESS_FLAGS)
//
#define	SET_VERIFY_DONE(SUCCESS_FLAGS)						(SUCCESS_FLAGS) &= 0xFFFFFF0F
#define	SET_VERIFY_EXCEPTION(SUCCESS_FLAGS)					(SUCCESS_FLAGS) = (0xFFFFFF0F&SUCCESS_FLAGS) | VERIFY_EXCEPTION
#define	ONLY_VERIFY_OKAY(SUCCESS_FLAGS)						(0x000000F0 & SUCCESS_FLAGS)
#define	ONLY_VERIFY_FAIL(SUCCESS_FLAGS)						(0xFFFFFF0F & SUCCESS_FLAGS)
#define	ONLY_VERIFY_UNDONE(SUCCESS_FLAGS)					(VERIFY_UNDONE & SUCCESS_FLAGS)
//
#define	SET_TRANSMIT_VERIFY_EXCEPTION(SUCCESS_FLAGS)		(SUCCESS_FLAGS) = (0xFFFFFF00&SUCCESS_FLAGS) | (VERIFY_EXCEPTION|TRANSMIT_EXCEPTION)
#define	ONLY_TRANSMIT_VERIFY_OKAY(SUCCESS_FLAGS)			(0x000000FF & SUCCESS_FLAGS)
//
#define	SET_OWRTMRVE_DONE(SUCCESS_FLAGS)					(SUCCESS_FLAGS) &= 0xFFFFF0FF
#define	SET_OWRTMRVE_EXCEPTION(SUCCESS_FLAGS)				(SUCCESS_FLAGS) = (0xFFFFF0FF&SUCCESS_FLAGS) | OWRTMRVE_EXCEPTION
#define ONLY_OWRTMRVE_OKAY(SUCCESS_FLAGS)					(0x00000F00 & SUCCESS_FLAGS)
//
#define	SET_TASK_BREAK(SUCCESS_FLAGS)						(SUCCESS_FLAGS) |= BREAK_ALL_TASK
#define	SET_TASK_WAIVE(SUCCESS_FLAGS)						(SUCCESS_FLAGS) |= WAIVE_ALL_TASK
#define ONLY_BREAK_WAIVE_OKAY(SUCCESS_FLAGS)				((BREAK_ALL_TASK|WAIVE_ALL_TASK) & SUCCESS_FLAGS)
//

//
// exception code
#define INTERNAL_EXP			0x00000001
#define SET_INTERNAL_EXP(EXCEPT)		(EXCEPT) |= INTERNAL_EXP
#define NETWORK_EXP				0x00000010
#define SET_NETWORK_EXP(EXCEPT)			(EXCEPT) |= NETWORK_EXP
#define FILE_CREATE_EXP			0x00000100
#define SET_CREATE_EXP(EXCEPT)			(EXCEPT) |= FILE_CREATE_EXP
#define FILE_LOCKED_EXP			0x00001000
#define SET_LOCKED_EXP(EXCEPT)			(EXCEPT) |= FILE_LOCKED_EXP
#define DOWN_DATA_EXP			0x00010000
#define SET_DOWND_EXP(EXCEPT)			(EXCEPT) |= DOWN_DATA_EXP
//
#define IS_INTERNAL_EXP(EXCEPT) (INTERNAL_EXP & EXCEPT)

//
class CTaskListObj {
public:
    CTaskListObj(void);
    ~CTaskListObj(void);
public:
    DWORD Initialize(TCHAR *szTListFile, TCHAR *szLocation);
    DWORD Finalize();
    // anchor data file operate;
private:
    TCHAR	m_szTListName[MAX_PATH];
    HANDLE	m_hTListFile;
    CRITICAL_SECTION m_csListHand;
public:
    DWORD OpenTasksFile();
    DWORD CloseTasksFile();
public:
    int SetNodePoint(DWORD dwNodeInde);
    int WriteNode(struct TaskNode *task_node);
    int ReadNode(TaskNode *task_node);
//
private:
    DWORD m_dwHandNode;
    CRITICAL_SECTION m_csHandNumbe;
public:
    DWORD GetHandNumbe();
    DWORD GetHandNodeAdd();
    VOID SetHandNumbe(DWORD dwHandNumbe);
    int WriteNodeEx(struct TaskNode *task_node, DWORD dwNodeInde);
    int ReadNodeEx(TaskNode *task_node, DWORD dwNodeInde);
private:
	DWORD m_dwIndeTatol;
    CRITICAL_SECTION m_csArrayID;
public:
	VOID InitArray(DWORD dwArrayID, DWORD dwNodeInde);
    DWORD ArrayCapacity();
	inline DWORD GetNodeTatol() { return m_dwHandNode;}
};

extern CTaskListObj objTaskList;

//
class CTasksCache {
private:
    struct TasksCache m_tTasksCache;
public:
    inline struct TasksCache *GetTasksCache() 
	{ return &m_tTasksCache; }
};

extern CTasksCache objTasksCache;
//