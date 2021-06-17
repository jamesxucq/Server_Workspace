#include "StdAfx.h"
#include <sys/stat.h>
//#include "SdtpOpt.h"
#include "StringOpt.h"

#include "FilesVec.h"

CFilesVec::CFilesVec(void)
{
}

CFilesVec::~CFilesVec(void)
{
}

void CFilesVec::DeleteFilesVec(FilesAttribVec *vpFilesAttVec)
{
	vector <struct FileAttrib *>::iterator iter;

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) 
		DeleteFilesAtt(*iter);

	vpFilesAttVec->clear();
}

void CFilesVec::DeleteFilesHt(FilesAttHashTable *tpFilesAttrib)
{
	dense_hash_map <wstring, struct FileAttrib *>::iterator htiter;

	for(htiter = tpFilesAttrib->begin(); htiter != tpFilesAttrib->end(); htiter++) 
		DeleteFilesAtt(htiter->second);

	tpFilesAttrib->clear();
}

bool CFilesVec::CopyFilesAttribVec(FilesAttribVec *vpDestinationFilesVec, FilesAttribVec *vpSourceFilesVec)
{
	vector <struct FileAttrib *>::iterator iter;

	if(!vpDestinationFilesVec || !vpSourceFilesVec)
		return false;

	vpDestinationFilesVec->clear();
	for(iter = vpSourceFilesVec->begin(); iter != vpSourceFilesVec->end(); iter++) 
		CFilesVec::AppendNewFilesNode(vpDestinationFilesVec, *iter);

	return true;
}

struct FileAttrib * CFilesVec::AddNewFilesNode(FilesAttribVec *vpFilesAttVec, wchar_t *root, wchar_t *dirname, WIN32_FIND_DATA *pFindData)
{
	struct FileAttrib *NewFileAttrib;
	//SYSTEMTIME LastWrite;

	NewFileAttrib = (struct FileAttrib *)malloc(sizeof(struct FileAttrib));
	if(!NewFileAttrib) return NULL;
	memset(NewFileAttrib, '\0', sizeof(struct FileAttrib));

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	CStringOpt::fullname(NewFileAttrib->filename, dirname, pFindData->cFileName);

	NewFileAttrib->filesize = pFindData->nFileSizeLow;
	NewFileAttrib->LastWrite = pFindData->ftLastWriteTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	vpFilesAttVec->push_back(NewFileAttrib);

	return NewFileAttrib;
}

struct FileAttrib * CFilesVec::AppendNewFilesNode(FilesAttribVec *vpFilesAttVec)
{
	struct FileAttrib *NewFileAttrib;
	//SYSTEMTIME LastWrite;

	NewFileAttrib = (struct FileAttrib *)malloc(sizeof(struct FileAttrib));
	memset(NewFileAttrib, '\0', sizeof(struct FileAttrib));

	vpFilesAttVec->push_back(NewFileAttrib);

	return NewFileAttrib;
}

struct FileAttrib * CFilesVec::AppendNewFilesNode(FilesAttribVec *vpFilesAttVec, struct FileAttrib *fstw)
{
	struct FileAttrib *NewFileAttrib;
	//SYSTEMTIME LastWrite;

	NewFileAttrib = (struct FileAttrib *)malloc(sizeof(struct FileAttrib));
	memcpy(NewFileAttrib, fstw, sizeof(struct FileAttrib));

	vpFilesAttVec->push_back(NewFileAttrib);

	return NewFileAttrib;
}

struct FileAttrib *
	CFilesVec::FindFromFilesVec(FilesAttribVec *vpIndexFilesAtt, struct FileAttrib *fstw)
{
	vector <struct FileAttrib *>::iterator iter;
	FileAttrib *FileAttReturn = NULL;

	if(!vpIndexFilesAtt || !fstw) return NULL;
	for(iter = vpIndexFilesAtt->begin(); iter != vpIndexFilesAtt->end(); iter++) {
		if (!_tcscmp(fstw->filename, (*iter)->filename)){
			FileAttReturn = *iter;
			break;
		}
	}

	return FileAttReturn;
}

struct FileAttrib *
	CFilesVec::FindFromFilesVecEx(FilesAttribVec *vpIndexFilesAtt, struct FileAttrib *fstw)
{
	static wchar_t sFileName[MAX_PATH];
	static vector <struct FileAttrib *>::iterator tmp_iter;
	FileAttrib *FileAttReturn = NULL;

	if (!vpIndexFilesAtt || !fstw) tmp_iter++;
	else{
		tmp_iter = vpIndexFilesAtt->begin();
		_tcscpy_s(sFileName, MAX_PATH, fstw->filename);
	}

	for( ; tmp_iter != vpIndexFilesAtt->end(); tmp_iter++) {
		if (!_tcscmp(sFileName, (*tmp_iter)->filename)){
			FileAttReturn = *tmp_iter;
			break;
		}
	}

	return FileAttReturn;
}

struct FileAttrib * 
	CFilesVec::DeleteFromFilesVec(FilesAttribVec *vpFilesAttVec, struct FileAttrib *fstw)
{
	vector <struct FileAttrib *>::iterator iter;
	FileAttrib *FileAttReturn = NULL;

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		if (fstw == *iter) {
			FileAttReturn = *iter;
			vpFilesAttVec->erase(iter);
			break;
		}
	}
	return FileAttReturn;
}


struct FileAttrib *
	CFilesVec::FilesRenameExist(FilesAttribVec *vpFilesAttVec, struct FileAttrib *nfst)
{
	vector <struct FileAttrib *>::iterator iter;
	struct FileAttrib *FileAttReturn = NULL;

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		if (nfst == *iter) {
			FileAttReturn = *(--iter);
			break;
		}
	}
	return FileAttReturn;
}

struct FileAttrib *
	CFilesVec::FilesRenameNew(FilesAttribVec *vpFilesAttVec, struct FileAttrib *efst)
{
	vector <struct FileAttrib *>::iterator iter;
	struct FileAttrib *FileAttReturn = NULL;

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		if (efst == *iter) {
			FileAttReturn = *(++iter);
			break;
		}
	}
	return FileAttReturn;
}


struct FileAttrib *
	CFilesVec::PrevFilesInsert(FilesAttribVec *vpFilesAttVec, struct FileAttrib *ifst, struct FileAttrib *cfst)
{
	vector <struct FileAttrib *>::iterator iter;
	FileAttrib *FileAttReturn = NULL;

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		if (cfst == *iter) {
			iter--;
			vpFilesAttVec->insert(iter, ifst); //just for test;
			FileAttReturn = *iter;
			break;
		}
	}
	return FileAttReturn;
}

struct FileAttrib *
	CFilesVec::PrevFilesInsertNew(FilesAttribVec *vpFilesAttVec, struct FileAttrib *cfst)
{
	vector <struct FileAttrib *>::iterator iter;
	struct FileAttrib *NewFileAttrib;
	FileAttrib *FileAttReturn = NULL;


	//SYSTEMTIME LastWrite;
	NewFileAttrib = (struct FileAttrib *)malloc(sizeof(struct FileAttrib));
	memset(NewFileAttrib, '\0', sizeof(struct FileAttrib));

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		if (cfst == *iter) {
			iter--;
			vpFilesAttVec->insert(iter, NewFileAttrib); //just for test;
			FileAttReturn = *iter;
			break;
		}
	}
	return FileAttReturn;
}

struct FileAttrib *
	CFilesVec::PrevFilesInsertNew(FilesAttribVec *vpFilesAttVec, struct FileAttrib *ifst, struct FileAttrib *cfst)
{
	vector <struct FileAttrib *>::iterator iter;
	struct FileAttrib *NewFileAttrib;
	FileAttrib *FileAttReturn = NULL;


	//SYSTEMTIME LastWrite;
	NewFileAttrib = (struct FileAttrib *)malloc(sizeof(struct FileAttrib));
	memcpy(NewFileAttrib, ifst, sizeof(struct FileAttrib));

	for(iter = vpFilesAttVec->begin(); iter != vpFilesAttVec->end(); iter++) {
		if (cfst == *iter) {
			iter--;
			vpFilesAttVec->insert(iter, NewFileAttrib); //just for test;
			FileAttReturn = *iter;
			break;
		}
	}
	return FileAttReturn;
}

