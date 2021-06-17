#pragma once

#include "common_macro.h"

#include <vector>
using std::vector;

#define TASKS_LISTDATA_DEFAULT			_T("~\\tasks_list.tmp")

////////////////////////////////////////////
// success flags
#define		TASK_NODE_DONE				0x00000000
//#define		TRANSMIT_DONE				0x00000000
#define		TRANSMIT_UNDONE				0x01000000
#define		TRANSMIT_EXCEPTION			0x02000000
//#define		VERIFY_DONE					0x00000000
#define		VERIFY_UNDONE				0x00010000
#define		VERIFY_EXCEPTION			0x00020000
//#define		OVERWRITE_DONE				0x00000000
#define		OVERWRITE_UNDONE			0x00000100
#define		OVERWRITE_EXCEPTION			0x00000200
// #define		COMMAND_DONE				0x00000000
// #define		COMMAND_UNDONE				0x00000001
// #define		COMMAND_EXCEPTION			0x00000002

#define	SET_TRANSMIT_DONE(SUCCESS_FLAGS)			(SUCCESS_FLAGS) &= 0x00FFFFFF
#define	SET_TRANSMIT_EXCEPTION(SUCCESS_FLAGS)		(SUCCESS_FLAGS) = (SUCCESS_FLAGS & 0x00FFFFFF) | TRANSMIT_EXCEPTION
#define	SET_VERIFY_DONE(SUCCESS_FLAGS)				(SUCCESS_FLAGS) &= 0xFF00FFFF
#define	SET_VERIFY_EXCEPTION(SUCCESS_FLAGS)			(SUCCESS_FLAGS) = (SUCCESS_FLAGS & 0xFF00FFFF) | TRANSMIT_EXCEPTION
#define	ONLY_VERIFY_FAIL(SUCCESS_FLAGS)				(SUCCESS_FLAGS & 0xFF00FFFF)
#define	SET_OVERWRITE_DONE(SUCCESS_FLAGS)			(SUCCESS_FLAGS) &= 0xFFFF00FF
#define	SET_OVERWRITE_EXCEPTION(SUCCESS_FLAGS)		(SUCCESS_FLAGS) = (SUCCESS_FLAGS & 0xFFFF00FF) | OVERWRITE_EXCEPTION
// #define	SET_COMMAND_DONE(SUCCESS_FLAGS)				(SUCCESS_FLAGS) &= 0xFFFFFF00
// #define	SET_COMMAND_EXCEPTION(SUCCESS_FLAGS)		(SUCCESS_FLAGS) = (SUCCESS_FLAGS & 0xFFFFFF00) | OVERWRITE_EXCEPTION

////////////////////////////////////////////
// syncing task ctrl code
#define		LOCATION_LOCAL				0x01000000
#define		LOCATION_SERVER				0x02000000
#define		FILE_TASK_BEGIN				0x00010000
#define		FILE_TASK_END				0x00020000

#define		STYPE_GET					0x00000100
#define		STYPE_PUT					0x00000200
#define		STYPE_PUTSYNC				0x00000400
#define		STYPE_GETSYNC				0x00000800
#define		STYPE_MOVE					0x00001000
#define		STYPE_DELETE				0x00002000

#define TNODE_LOCATION_CODE(CONTROL_CODE)		(CONTROL_CODE) & 0xFF000000
#define TNODE_BEGIN_CODE(CONTROL_CODE)			(CONTROL_CODE) & 0x00FF0000
#define TNODE_STYPE_CODE(CONTROL_CODE)			(CONTROL_CODE) & 0x0000FF00
#define TNODE_RANGE_CODE(CONTROL_CODE)			(CONTROL_CODE) & 0x000000FF

struct TaskNode {
	//////////////////////////////////////////////////////////////////
	DWORD node_index;
	DWORD success_flags;	// 0x00:success 0x01:undone 0x02:exception
	DWORD control_code;		/* add mod del list recu */
	TCHAR build_cache[MIN_TEXT_LEN];
	//////////////////////////////////////////////////////////////////
	TCHAR szFileName[MAX_PATH];
	off_t offset;
	DWORD build_length;
	FILETIME ftLastWrite;		/* When the item was last modified */
	unsigned char correct_csum[SUM_LENGTH]; // Todo:
	DWORD dwReserved;  /* filehigh */
};
typedef vector <struct TaskNode *>  TasksNodeList;

struct TasksVector {
	//client modify
	vector <struct file_attrib *> local_copy;
	vector <struct file_attrib *> local_move;
	vector <struct file_attrib *> local_delete;
	vector <struct file_attrib *> local_get;
	vector <struct file_attrib *> local_putsync;
	//server modify
	vector <struct file_attrib *> serv_copy;
	vector <struct file_attrib *> serv_move;
	vector <struct file_attrib *> serv_delete;
	vector <struct file_attrib *> serv_put;
	vector <struct file_attrib *> serv_getsync;
};

class CTasksListObject {
public:
	CTasksListObject(void);
	~CTasksListObject(void);
public:
	DWORD Initialize(TCHAR *szTasksListFile, TCHAR *szLocation);
	DWORD Finalize();
	// anchor data file operate;
private:
	TCHAR	m_szTasksListName[MAX_PATH];
	HANDLE m_hTasksListFile;
public:
	DWORD OpenTasksFile();
	DWORD CloseTasksFile();
public:
	DWORD SetNodePoint(int iNodeCount);
	DWORD WriteNode(struct TaskNode *task_node);
	DWORD ReadNode(TaskNode *task_node);
private:
	struct TasksVector m_tTasksVector;
public:
	struct TasksVector *GetTasksVector() { return &m_tTasksVector; }
private:
	DWORD m_dwHandleNode;
public:
	DWORD GetHandleNumber();
	DWORD GetHandleNodeAddition();
	void SetHandleNumber(int dwHandleNumber);
	DWORD WriteNodeExt(struct TaskNode *task_node, int iNodeCount);
	DWORD ReadNodeExt(TaskNode *task_node, int iNodeCount);
};

extern CTasksListObject objTasksList;