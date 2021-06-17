#pragma once

#include "clientcom/clientcom.h"
#include "BuildTasksBzl.h"
#include "LockedAction.h"

namespace ndp {
	VOID FilesVector(FilesVec *pFilesVec);  // for debug 20110222
	VOID TasksCache(struct TasksCache *pTasksCache);
	VOID Matcher(struct file_matcher *matcher);
	VOID TaskNode(struct TaskNode *task_node);
	VOID TaskNodeI(struct TaskNode *task_node);
	VOID ChksMD5(TCHAR *label, unsigned char *md5sum);
	VOID ChksMD5_ex(TCHAR *label, unsigned char *md5sum, TCHAR *szFileName);
	VOID ChksSha1(TCHAR *label, unsigned char *sha1sum);
	VOID LastTaskNode();
	//
	VOID ListVector(ListVec *pListVec);
	VOID ActionPrinter(LkAiVec *pLkAiVec);
	VOID DiresVector(DiresVec *pListVec);
};
