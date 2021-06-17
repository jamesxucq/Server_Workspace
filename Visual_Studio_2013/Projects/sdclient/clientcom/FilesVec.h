#pragma once

#include "common_macro.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default

#include <vector>
using std::vector;

//
struct attent {
	DWORD action_type;		/* add mod del list recu */
	TCHAR file_name[MAX_PATH];
	unsigned __int64 file_size;
	FILETIME last_write;		/* When the item was last modified */
	DWORD reserved;  /* reserved */
};
typedef vector <struct attent *> FilesVec;
typedef unordered_map <wstring, struct attent *> FilesHmap;

namespace NFilesVec {
	inline VOID DeleNode(struct attent *pNode)
	{ if(pNode) free(pNode); }
	VOID DeleFilesVec(FilesVec *pFilesVec);
	VOID DeleFilesHmap(FilesHmap *pFilesHmap);
	//
	DWORD DeepCopyVec(FilesVec *pToVec, FilesVec *pFromVec);
	DWORD ShallowCopyVec(FilesVec *pToVec, FilesVec *pFromVec);
	DWORD MoveVec(FilesVec *pToVec, FilesVec *pFromVec);
	DWORD CompleteFilesVec(FilesVec *pFilesVec, const TCHAR *szDriveLetter);
	//
	struct attent *AddNewNode(FilesVec *pFilesVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData);
	struct attent *AppNewNode(FilesVec *pFilesVec, struct attent *atte);
	struct attent *AppNewNode(FilesVec *pFilesVec, struct attent *atte, DWORD action_type);
	struct attent *AppNewNode(FilesVec *pFilesVec);
	struct attent *MoveToLastNode(FilesVec *pFilesVec, struct attent *atte);
	//
	struct attent *FindFromVec(FilesVec *pFindVec, struct attent *atte);
	struct attent *FindFromVecEx(FilesVec *pFindVec, struct attent *atte);
	struct attent *DeleFromVec(FilesVec *pFilesVec, struct attent *atte);
	//
	struct attent *FindActioNode(FilesVec *pFilesVec, struct attent *exis_node);
	struct attent *FindExistNode(FilesVec *pFilesVec, struct attent *acti_nod);
	DWORD CompActioNode(FilesVec *pFilesVec, struct attent *atte);
	DWORD CompActioNode(FilesVec *pFilesVec, struct attent *exis, struct attent *neis);
	struct attent *DeleFromHmap(FilesHmap *pFilesHmap, struct attent *atte);
	//
	DWORD WriteNode(HANDLE hFile, struct attent *atte);
	DWORD ReadNode(struct attent *atte, HANDLE hFile);
};
