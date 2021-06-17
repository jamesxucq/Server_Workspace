#pragma once

#include "common_macro.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::tr1::unordered_map;      // namespace where class lives by default
#include <vector>
using std::vector;

///////////////////////////////////////////////////////
#define NONE					0x00000000
///////////////////////////////////////////////////////
// file real status
#define ADD                     0x00000001 // addition
#define MODIFY                  0x00000002 // modify
#define DEL                     0x00000004 // delete
#define EXIST                  0x00000008 // exists
#define COPY                    0x00000010 // copy
#define MOVE                    0x00000020 // move
///////////////////////////////////////////////////////
// list status
#define RECURSIVE               0x00000040 // recursive
#define LIST                    0x00000080 // list
///////////////////////////////////////////////////////
// dir virtual status
#define DIRECTORY	            0x00001000 // directory
//////////////////////////////////////////////////////////////
#define PROPERTY                0x00010000 // property
////////////////////////////////
#define LOCK					PROPERTY | 0x0001 // lock 
#define UNLOCK					PROPERTY | 0x0002 // unlink
#define READONLY				PROPERTY | 0x0004 // read only
#define WRITEABLE				PROPERTY | 0x0008 // writeable
#define SHARED					PROPERTY | 0x0010 // shared
#define EXCLUSIVE				PROPERTY | 0x0020 // exclusive
///////////////////////////////////////////////////////
// #define ERASE	                0x00020000 // erase
///////////////////////////////////////////////////////

struct file_attrib {
	///////////////////////////////////////////////////////////////////
	DWORD modify_type;		/* add mod del list recu */
	TCHAR file_name[MAX_PATH];
	///////////////////////////////////////////////////////////////////
	DWORD file_size;
	FILETIME last_write;		/* When the item was last modified */
	///////////////////////////////////////////////////////////////////
	DWORD reserved;  /* filehigh */
};
typedef vector <struct file_attrib *> FilesVec;
typedef unordered_map <wstring, struct file_attrib *> FilesHmap;

namespace NFilesVec {
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	inline void DeleteNode(struct file_attrib *fast)
	{ if(fast) free(fast); }
	void DeleteFilesVec(FilesVec *pFilesVec);
	void DeleteFilesHmap(FilesHmap *pFilesHmap);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	DWORD DeepCopyFilesVec(FilesVec *pDestiVec, FilesVec *pSourceVec);
	DWORD CompleteFilesVec(FilesVec *pFilesVec, const TCHAR *szDriveRoot);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	struct file_attrib *AddNewNode(FilesVec *pFilesVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData);
	struct file_attrib *AppendNewNode(FilesVec *pFilesVec, struct file_attrib *fast);
	struct file_attrib *AppendNewNode(FilesVec *pFilesVec);

	struct file_attrib *FindFromVec(FilesVec *pFindFiles, struct file_attrib *fast);
	struct file_attrib *FindFromVecExt(FilesVec *pFindFiles, struct file_attrib *fast);
	struct file_attrib *DeleteFromVec(FilesVec *pFilesVec, struct file_attrib *fast);

	struct file_attrib *FindNextNode(FilesVec *pFilesVec, struct file_attrib *exis_node);
	struct file_attrib *FindExistsNode(FilesVec *pFilesVec, struct file_attrib *nfast);

	struct file_attrib *InsertPrev(FilesVec *pFilesVec, struct file_attrib *ifast, struct file_attrib *cfast);
	struct file_attrib *InsertPrevNew(FilesVec *pFilesVec, struct file_attrib *cfast);
	struct file_attrib *InsertPrevNew(FilesVec *pFilesVec, struct file_attrib *ifast, struct file_attrib *cfast);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	struct file_attrib *DeleteFromHmap(FilesHmap *pFilesHmap, struct file_attrib *fast);
};
