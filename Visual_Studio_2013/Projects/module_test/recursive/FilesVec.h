#pragma once

#include "common_macro.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <boost/tr1/unordered_map.hpp>
using std::tr1::unordered_map;      // namespace where class lives by default
#include <vector>
using std::vector;

//
struct attent {
	DWORD action_type;		/* add mod del list recu */
	TCHAR file_name[MAX_PATH];
	//
	unsigned __int64 file_size;
	FILETIME last_write;		/* When the item was last modified */
	//
	DWORD reserved;  /* reserved */
};
typedef vector <struct attent *> FilesVec;
typedef unordered_map <wstring, struct attent *> FilesHmap;

namespace NFilesVec
{
	inline VOID DeleteNode(struct attent *fatte)
	{
		if(fatte) free(fatte);
	}
	VOID DeleteFilesVec(FilesVec *pFilesVec);
	VOID DeleteFilesHmap(FilesHmap *pFilesHmap);
	//
	DWORD DeepCopyVec(FilesVec *pToVec, FilesVec *pFromVec);
	DWORD CompleteFilesVec(FilesVec *pFilesVec, const TCHAR *szDriveRoot);
	//
	struct attent *AddNewNode(FilesVec *pFilesVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData);
	struct attent *AppendNewNode(FilesVec *pFilesVec, struct attent *fatte);
	struct attent *AppendNewNode(FilesVec *pFilesVec);
	//
	struct attent *FindFromVec(FilesVec *pFindVec, struct attent *fatte);
	struct attent *FindFromVecExt(FilesVec *pFindVec, struct attent *fatte);
	struct attent *DeleteFromVec(FilesVec *pFilesVec, struct attent *fatte);
	//
	struct attent *FindActionNode(FilesVec *pFilesVec, struct attent *exis_node);
	struct attent *FindExistsNode(FilesVec *pFilesVec, struct attent *act_node);
	//
	struct attent *DeleteFromHmap(FilesHmap *pFilesHmap, struct attent *fatte);
	//
	DWORD WriteNode(HANDLE hFile, struct attent *fatte);
	DWORD ReadNode(struct attent *fatte, HANDLE hFile);
};
