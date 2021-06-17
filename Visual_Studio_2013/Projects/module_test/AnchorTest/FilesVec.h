#pragma once

//#include "CommonMacro.h"
#include  <windows.h>

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <google/dense_hash_map>
using google::dense_hash_map;      // namespace where class lives by default

#include <vector>
using std::vector;

#define UNKNOWN				'U'
// file real status
#define ADD					'A'
#define MODIFY				'M'
#define DEL					'D'
#define EXIST				'E'
#define NEW					'N'
// list status
#define RECU				'R'
#define LIST				'L'
#define CFT					'C'
// dir virtual status
#define FORBID				'F'
#define READONLY			'R'
#define LOCKED				'L'
 //extern virtual code
#define ERASE				'S'

struct FileAttrib {
	char mod_type;		/* add mod del list recu */
	wchar_t filename[MAX_PATH];
	DWORD filesize;
	FILETIME LastWrite;		/* When the item was last modified */
	DWORD Reserved;  /* filehigh */
};
typedef vector <struct FileAttrib *> FilesAttribVec;
typedef dense_hash_map <wstring, struct FileAttrib *> FilesAttHashTable;


class CFilesVec
{
public:
	CFilesVec(void);
	~CFilesVec(void);
public:
	static inline void DeleteFilesAtt(struct FileAttrib *fstw)
		{if(!fstw) return; free(fstw);}
	static void DeleteFilesVec(FilesAttribVec *vpFilesAttVec);
	static void DeleteFilesHt(FilesAttHashTable *tpFilesAttrib);
public:
	static bool CopyFilesAttribVec(FilesAttribVec *vpDestinationFilesVec, FilesAttribVec *vpSourceFilesVec);
public:
	static struct FileAttrib * 
	AddNewFilesNode(FilesAttribVec *vpFilesAttVec, 
				wchar_t *root,
				wchar_t *dirname, 
				WIN32_FIND_DATA *pFindData);
	static struct FileAttrib * 
	AppendNewFilesNode(FilesAttribVec *vpFilesAttVec,
				struct FileAttrib *fstw);
	static struct FileAttrib * 
	AppendNewFilesNode(FilesAttribVec *vpFilesAttVec);
public:
	static struct FileAttrib *
		FindFromFilesVec(FilesAttribVec *vpIndexFilesAtt, struct FileAttrib *fstw);
	static struct FileAttrib *
		FindFromFilesVecEx(FilesAttribVec *vpIndexFilesAtt, struct FileAttrib *fstw);
	static struct FileAttrib * 
		DeleteFromFilesVec(FilesAttribVec *vpFilesAttVec, struct FileAttrib *fstw);
public:
	static struct FileAttrib *
		FilesRenameNew(FilesAttribVec *vpFilesAttVec, struct FileAttrib *efst);
	static struct FileAttrib *
		FilesRenameExist(FilesAttribVec *vpFilesAttVec, struct FileAttrib *nfst);
public:
	static struct FileAttrib *
		PrevFilesInsert(FilesAttribVec *vpFilesAttVec, struct FileAttrib *ifst, struct FileAttrib *cfst);
	static struct FileAttrib *
		PrevFilesInsertNew(FilesAttribVec *vpFilesAttVec, struct FileAttrib *cfst);
	static struct FileAttrib *
		PrevFilesInsertNew(FilesAttribVec *vpFilesAttVec, struct FileAttrib *ifst, struct FileAttrib *cfst);
};



