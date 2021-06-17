#include "StdAfx.h"
#include <sys/stat.h>

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "FilesVec.h"


void NFilesVec::DeleteFilesVec(FilesVec *pFilesVec) {
	FilesVec::iterator iter;

	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); iter++) 
		DeleteNode(*iter);

	pFilesVec->clear();
}

void NFilesVec::DeleteFilesHmap(FilesHmap *pFilesHmap) {
	unordered_map <wstring, struct file_attrib *>::iterator file_iter;

	// if(pFilesHmap->empty()) return;
	for(file_iter = pFilesHmap->begin(); file_iter != pFilesHmap->end(); ++file_iter) 
		DeleteNode(file_iter->second);

	pFilesHmap->clear();
}

DWORD NFilesVec::DeepCopyFilesVec(FilesVec *pDestiVec, FilesVec *pSourceVec) {
	FilesVec::iterator iter;

	if(!pDestiVec || !pSourceVec) return -1;

	pDestiVec->clear();
	for(iter = pSourceVec->begin(); iter != pSourceVec->end(); iter++) 
		NFilesVec::AppendNewNode(pDestiVec, *iter);

	return 0;
}

DWORD NFilesVec::CompleteFilesVec(FilesVec *pFilesVec, const TCHAR *szDriveRoot) {
	FilesVec::iterator iter;
	TCHAR file_name[MAX_PATH];

	if(!pFilesVec) return -1;

	_tcscpy_s(file_name, szDriveRoot);
	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); iter++) {
		_tcscpy_s(NO_ROOT(file_name), _tcslen((*iter)->file_name)+1, (*iter)->file_name);
		NFileUtility::FileSizeTimeExt(&(*iter)->file_size, &(*iter)->last_write, file_name);
	}

	return 0;
}

struct file_attrib *NFilesVec::AddNewNode(FilesVec *pFilesVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
	struct file_attrib *fast_new;
	//SYSTEMTIME ftLastWrite;

	fast_new = (struct file_attrib *)malloc(sizeof(struct file_attrib));
	if(!fast_new) return NULL;
	memset(fast_new, '\0', sizeof(struct file_attrib));

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	nstriutility::full_name(fast_new->file_name, szPathString, pFindData->cFileName);

	fast_new->file_size = pFindData->nFileSizeLow;
	fast_new->last_write = pFindData->ftLastWriteTime;

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	pFilesVec->push_back(fast_new);

	return fast_new;
}

struct file_attrib *NFilesVec::AppendNewNode(FilesVec *pFilesVec) {
	struct file_attrib *fast_new;
	//SYSTEMTIME ftLastWrite;

	fast_new = (struct file_attrib *)malloc(sizeof(struct file_attrib));
	memset(fast_new, '\0', sizeof(struct file_attrib));

	pFilesVec->push_back(fast_new);

	return fast_new;
}

struct file_attrib *NFilesVec::AppendNewNode(FilesVec *pFilesVec, struct file_attrib *fast) {
	struct file_attrib *fast_new;
	//SYSTEMTIME ftLastWrite;

	fast_new = (struct file_attrib *)malloc(sizeof(struct file_attrib));
	memcpy(fast_new, fast, sizeof(struct file_attrib));

	pFilesVec->push_back(fast_new);

	return fast_new;
}

struct file_attrib *NFilesVec::FindFromVec(FilesVec *pFindFiles, struct file_attrib *fast) {
	FilesVec::iterator iter;
	file_attrib *find_result = NULL;

	if(!pFindFiles || !fast) return NULL;
	for(iter = pFindFiles->begin(); iter != pFindFiles->end(); iter++) {
		if (!_tcscmp(fast->file_name, (*iter)->file_name)){
			find_result = *iter;
			break;
		}
	}

	return find_result;
}

struct file_attrib *NFilesVec::FindFromVecExt(FilesVec *pFindFiles, struct file_attrib *fast) {
	static FilesVec local_files;
	static FilesVec::iterator token;

	file_attrib *find_result = NULL;
	if (pFindFiles && fast) {
		for(token=pFindFiles->begin(); token!=pFindFiles->end(); token++) {
			if (!_tcscmp(fast->file_name, (*token)->file_name))
				local_files.push_back(*token);				
		}
		token = local_files.begin();
		if(token != local_files.end()) find_result = *token;
	} else {
		token++;
		if(token != local_files.end()) find_result = *token;
	}
	if(!find_result) local_files.clear();

	return find_result;
}

struct file_attrib *NFilesVec::DeleteFromVec(FilesVec *pFilesVec, struct file_attrib *fast) {
	FilesVec::iterator iter;
	file_attrib *del_result = NULL;

	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); ) {
		if (fast == *iter) {
			del_result = *iter;
			pFilesVec->erase(iter);
			break;
		} else iter++;
	}

	return del_result;
}


struct file_attrib *NFilesVec::FindExistsNode(FilesVec *pFilesVec, struct file_attrib *nfast) {
	FilesVec::iterator iter;
	struct file_attrib *move_result = NULL;

	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); iter++) {
		if (nfast == *iter) {
			move_result = *(--iter);
			break;
		}
	}

	return move_result;
}

struct file_attrib *NFilesVec::FindNextNode(FilesVec *pFilesVec, struct file_attrib *exis_node) {
	FilesVec::iterator iter;
	struct file_attrib *move_result = NULL;

	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); iter++) {
		if (exis_node == *iter) {
			move_result = *(++iter);
			break;
		}
	}

	return move_result;
}


struct file_attrib *NFilesVec::InsertPrev(FilesVec *pFilesVec, struct file_attrib *ifast, struct file_attrib *cfast) {
	FilesVec::iterator iter;
	file_attrib *ins_result = NULL;

	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); iter++) {
		if (cfast == *iter) {
			iter--;
			pFilesVec->insert(iter, ifast); //just for test;
			ins_result = *iter;
			break;
		}
	}

	return ins_result;
}

struct file_attrib *NFilesVec::InsertPrevNew(FilesVec *pFilesVec, struct file_attrib *cfast) {
	FilesVec::iterator iter;
	struct file_attrib *fast_new;
	file_attrib *ins_result = NULL;

	//SYSTEMTIME ftLastWrite;
	fast_new = (struct file_attrib *)malloc(sizeof(struct file_attrib));
	memset(fast_new, '\0', sizeof(struct file_attrib));

	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); iter++) {
		if (cfast == *iter) {
			iter--;
			pFilesVec->insert(iter, fast_new); //just for test;
			ins_result = *iter;
			break;
		}
	}

	return ins_result;
}

struct file_attrib *NFilesVec::InsertPrevNew(FilesVec *pFilesVec, struct file_attrib *ifast, struct file_attrib *cfast) {
	FilesVec::iterator iter;
	struct file_attrib *fast_new;
	file_attrib *ins_result = NULL;

	//SYSTEMTIME ftLastWrite;
	fast_new = (struct file_attrib *)malloc(sizeof(struct file_attrib));
	memcpy(fast_new, ifast, sizeof(struct file_attrib));

	for(iter = pFilesVec->begin(); iter != pFilesVec->end(); iter++) {
		if (cfast == *iter) {
			iter--;
			pFilesVec->insert(iter, fast_new); //just for test;
			ins_result = *iter;
			break;
		}
	}

	return ins_result;
}

struct file_attrib *NFilesVec::DeleteFromHmap(FilesHmap *pFilesHmap, struct file_attrib *fast) {
	unordered_map <wstring, struct file_attrib *>::iterator file_iter;
	file_attrib *del_result = NULL;

	for(file_iter = pFilesHmap->begin(); file_iter != pFilesHmap->end(); ) {
		if (fast == file_iter->second) {
			del_result = file_iter->second;
			pFilesHmap->erase(file_iter++);
			break;
		} else ++file_iter;
	}

	return del_result;

}