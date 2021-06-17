#pragma once

#include "clientcom/clientcom.h"

#include <vector>
using std::vector;

#define PRODU_LIST_DEFAULT			_T("~\\produ_list.xml")

//
#define LIST_INVA				0x00000000
#define LIST_RECU_FILES			0x00000100
#define LIST_LIST_DIREC			0x00000200
#define LIST_RECU_DIRES			0x00000400
#define LIST_ANCH_FILES			0x00000800

#define LNODE_LTYPE_BYTE(CTRL_CODE)			0x0000FF00 & (CTRL_CODE)
#define LNODE_RANGE_BYTE(CTRL_CODE)			0x000000FF & (CTRL_CODE)

struct ListNode {
	DWORD node_inde;
	DWORD control_code;
	TCHAR builder_cache[MIN_TEXT_LEN];
	unsigned __int64 offset;
	TCHAR directory[MAX_PATH];
	DWORD llast_anchor;
};
typedef vector <struct ListNode *>  ListVec;

//
class CListHObj {
public:
	CListHObj(void);
	~CListHObj(void);
private:
	unsigned char list_md5sum[MD5_DIGEST_LEN];
public:
	void GetListMd5sum(unsigned char *list_sum)
	{ memcpy(list_sum, list_md5sum, MD5_DIGEST_LEN); }
private:
	ListVec m_vListVector;
	DWORD m_dwHandNode;
	CRITICAL_SECTION m_csHandNumbe;
public:
	DWORD CreatListVec(TCHAR *list_sum, unsigned __int64 list_length, DWORD list_type, DWORD last_anchor, TCHAR *directory);
	VOID DeleListVec();
public:
	VOID SetHandNumbe(DWORD dwNodeInde);
	DWORD SetNode(struct ListNode *list_node, DWORD dwNodeInde);
	DWORD GetNodeAddition(struct ListNode **list_node);
};
extern CListHObj objListHead;

//
class CListVObj {
public:
	CListVObj(void);
	~CListVObj(void);
public:
	DWORD Initialize(TCHAR *szListFile, TCHAR *szLocation);
	DWORD Finalize();
private:
	TCHAR	m_szListName[MAX_PATH];
	HANDLE	m_hListFile;
	CRITICAL_SECTION m_csListHand;
public:
	DWORD WriteListFile(char *buffer, DWORD size, unsigned __int64 offset);
private:
	VOID ResetListFilePointer();
	DWORD ReadListFile(char *buffer, DWORD size);
public:
	void GetListMd5sum(unsigned char *list_sum);
	int ParseListXml(VOID *serv_list, DWORD list_type);
};
extern CListVObj objListVec;

//
namespace ListWorkBzl {
	int CheckListFile(class CListHObj *pListHead, class CListVObj *pListVec);
};