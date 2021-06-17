#include "StdAfx.h"
#include <sys/stat.h>

#include "iattb_type.h"
#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "FilesVec.h"


VOID NFilesVec::DeleFilesVec(FilesVec *pFilesVec) {
	FilesVec::iterator iter;
// if(pFilesVec->empty()) return;
	for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter)
		DeleNode(*iter);
	pFilesVec->clear();
}

VOID NFilesVec::DeleFilesHmap(FilesHmap *pFilesHmap) {
    unordered_map <wstring, struct attent *>::iterator aite;
    // if(pFilesHmap->empty()) return;
    for(aite = pFilesHmap->begin(); pFilesHmap->end() != aite; ++aite)
        DeleNode(aite->second);
    pFilesHmap->clear();
}

DWORD NFilesVec::DeepCopyVec(FilesVec *pToVec, FilesVec *pFromVec) {
    FilesVec::iterator iter;
    if(!pToVec || !pFromVec) return ((DWORD)-1);
    pToVec->clear();
    for(iter = pFromVec->begin(); pFromVec->end() != iter; ++iter)
        NFilesVec::AppNewNode(pToVec, *iter);
//
    return 0x00;
}

DWORD NFilesVec::ShallowCopyVec(FilesVec *pToVec, FilesVec *pFromVec) {
    FilesVec::iterator iter;
    if(!pToVec || !pFromVec) return ((DWORD)-1);
    pToVec->clear();
    for(iter = pFromVec->begin(); pFromVec->end() != iter; ++iter)
        pToVec->push_back(*iter);
//
    return 0x00;
}

DWORD NFilesVec::MoveVec(FilesVec *pToVec, FilesVec *pFromVec) {
    FilesVec::iterator iter;
    if(!pToVec || !pFromVec) return ((DWORD)-1);
	for(iter = pFromVec->begin(); pFromVec->end() != iter;) {
        pToVec->push_back(*iter);
		iter = pFromVec->erase(iter);
	}
//
    return 0x00;
}

DWORD NFilesVec::CompleteFilesVec(FilesVec *pFilesVec, const TCHAR *szDriveLetter) {
	struct attent *exist, *atte;
    TCHAR file_name[MAX_PATH];
    FilesVec::iterator iter;
    //
	DRIVE_LETTE(file_name, szDriveLetter)
    for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
		atte = (*iter);
		if(EXIST & atte->action_type) {
			exist = atte;
			continue;
		}
//
        _tcscpy_s(NO_LETT(file_name), MAX_PATH-LETT_LENGTH, atte->file_name);
        NFileUtility::FileSizeTime(&atte->file_size, &atte->last_write, file_name);
//
		if((COPY|MOVE) & atte->action_type) {
			exist->file_size = atte->file_size;
			memcpy(&exist->last_write, &atte->last_write, sizeof(FILETIME));
		}
    }
    return 0x00;
}

struct attent *NFilesVec::AddNewNode(FilesVec *pFilesVec, TCHAR *szPathString, WIN32_FIND_DATA *pFindData) {
    struct attent *atte_new;
    // SYSTEMTIME ftLastWrite;
    atte_new = (struct attent *)malloc(sizeof(struct attent));
    if(!atte_new) return NULL;
    memset(atte_new, '\0', sizeof(struct attent));
    //
    struti::full_name(atte_new->file_name, szPathString, pFindData->cFileName);
    atte_new->file_size = pFindData->nFileSizeLow;
    atte_new->file_size += ((unsigned __int64)pFindData->nFileSizeHigh) << 32;
    atte_new->last_write = pFindData->ftLastWriteTime;
    pFilesVec->push_back(atte_new);
//
    return atte_new;
}

struct attent *NFilesVec::AppNewNode(FilesVec *pFilesVec) {
    struct attent *atte_new;
    // SYSTEMTIME ftLastWrite;
    atte_new = (struct attent *)malloc(sizeof(struct attent));
    if(!atte_new) return NULL;
    memset(atte_new, '\0', sizeof(struct attent));
    pFilesVec->push_back(atte_new);
    return atte_new;
}

struct attent *NFilesVec::AppNewNode(FilesVec *pFilesVec, struct attent *atte) {
    struct attent *atte_new;
    // SYSTEMTIME ftLastWrite;
    atte_new = (struct attent *)malloc(sizeof(struct attent));
    if(!atte_new) return NULL;
    memcpy(atte_new, atte, sizeof(struct attent));
    pFilesVec->push_back(atte_new);
    return atte_new;
}

struct attent *NFilesVec::AppNewNode(FilesVec *pFilesVec, struct attent *atte, DWORD action_type) {
    struct attent *atte_new;
    // SYSTEMTIME ftLastWrite;
    atte_new = (struct attent *)malloc(sizeof(struct attent));
    if(!atte_new) return NULL;
    memcpy(atte_new, atte, sizeof(struct attent));
	atte_new->action_type = action_type;
    pFilesVec->push_back(atte_new);
    return atte_new;
}


struct attent *NFilesVec::MoveToLastNode(FilesVec *pFilesVec, struct attent *atte) {
    FilesVec::iterator iter;
//
    for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ) {
        if (atte == *iter) {
            pFilesVec->erase(iter);
            break;
        } else ++iter;
    }
    // SYSTEMTIME ftLastWrite;
    pFilesVec->push_back(atte);
    return atte;
}

struct attent *NFilesVec::FindFromVec(FilesVec *pFindVec, struct attent *atte) {
    FilesVec::iterator iter;
    struct attent *find_result = NULL;
//
    if(!pFindVec || !atte) return NULL;
    for(iter = pFindVec->begin(); pFindVec->end() != iter; ++iter) {
        if (!_tcscmp(atte->file_name, (*iter)->file_name)) {
            find_result = *iter;
            break;
        }
    }
    return find_result;
}

struct attent *NFilesVec::FindFromVecEx(FilesVec *pFindVec, struct attent *atte) {
    static FilesVec lattent;
    static FilesVec::iterator toke;
//
    struct attent *find_result = NULL;
    if (pFindVec && atte) {
        for(toke=pFindVec->begin(); pFindVec->end()!=toke; toke++) {
            if (!_tcscmp(atte->file_name, (*toke)->file_name))
                lattent.push_back(*toke);
        }
        toke = lattent.begin();
        if(lattent.end() != toke) find_result = *toke;
    } else {
        toke++;
        if(lattent.end() != toke) find_result = *toke;
    }
    if(!find_result) lattent.clear();
    return find_result;
}

struct attent *NFilesVec::DeleFromVec(FilesVec *pFilesVec, struct attent *atte) {
    FilesVec::iterator iter;
//
    for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ) {
        if (atte == *iter) {
            pFilesVec->erase(iter);
            break;
        } else ++iter;
    }
    return atte;
}


struct attent *NFilesVec::FindExistNode(FilesVec *pFilesVec, struct attent *acti_nod) {
    FilesVec::iterator iter;
    struct attent *exis_node = NULL;
//
    for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
        if (acti_nod == *iter) {
            exis_node = *(--iter);
            break;
        }
    }
    return exis_node;
}

struct attent *NFilesVec::FindActioNode(FilesVec *pFilesVec, struct attent *exis_node) {
    FilesVec::iterator iter;
    struct attent *acti_nod = NULL;
//
    for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
        if (exis_node == *iter) {
            acti_nod = *(++iter);
            break;
        }
    }
    return acti_nod;
}

DWORD NFilesVec::CompActioNode(FilesVec *pFilesVec, struct attent *atte) {
    FilesVec::iterator iter;
    struct attent *acti_nod;
	DWORD comp_value = ((DWORD)-1);
//
    for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
		acti_nod = (*iter);
		if((atte->action_type==acti_nod->action_type) && !_tcscmp(atte->file_name, acti_nod->file_name)) {
			comp_value = 0x00;
			break;
		}
    }
    return comp_value;
}

DWORD NFilesVec::CompActioNode(FilesVec *pFilesVec, struct attent *exis, struct attent *neis) {
    FilesVec::iterator iter;
    struct attent *exis_nod, *neis_nod;
	DWORD comp_value = ((DWORD)-1);
//
    for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
		exis_nod = (*iter);
		if(EXIST & exis_nod->action_type) {
			neis_nod = *(++iter);
			if((exis->action_type==exis_nod->action_type) && (neis->action_type==neis_nod->action_type)
				&& !_tcscmp(exis->file_name, exis_nod->file_name) && !_tcscmp(neis->file_name, neis_nod->file_name)) {
				comp_value = 0x00;
				break;
			}
		}
    }
    return comp_value;
}


/*
struct attent *NFilesVec::InsertPrev(FilesVec *pFilesVec, struct attent *iatte, struct attent *catte) {
	FilesVec::iterator iter;
	struct attent *ins_result = NULL;

	for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
		if (catte == *iter) {
			iter--;
			pFilesVec->insert(iter, iatte); // just for test;
			ins_result = *iter;
			break;
		}
	}

	return ins_result;
}

struct attent *NFilesVec::InsertPrevNew(FilesVec *pFilesVec, struct attent *catte) {
	FilesVec::iterator iter;
	struct attent *atte_new;
	struct attent *ins_result = NULL;

	// SYSTEMTIME ftLastWrite;
	atte_new = (struct attent *)malloc(sizeof(struct attent));
    if(!atte_new) return NULL;
	memset(atte_new, '\0', sizeof(struct attent));

	for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
		if (catte == *iter) {
			iter--;
			pFilesVec->insert(iter, atte_new); // just for test;
			ins_result = *iter;
			break;
		}
	}

	return ins_result;
}

struct attent *NFilesVec::InsertPrevNew(FilesVec *pFilesVec, struct attent *iatte, struct attent *catte) {
	FilesVec::iterator iter;
	struct attent *atte_new;
	struct attent *ins_result = NULL;

	// SYSTEMTIME ftLastWrite;
	atte_new = (struct attent *)malloc(sizeof(struct attent));
    if(!atte_new) return NULL;
	memcpy(atte_new, iatte, sizeof(struct attent));

	for(iter = pFilesVec->begin(); pFilesVec->end() != iter; ++iter) {
		if (catte == *iter) {
			iter--;
			pFilesVec->insert(iter, atte_new); // just for test;
			ins_result = *iter;
			break;
		}
	}

	return ins_result;
}
*/
struct attent *NFilesVec::DeleFromHmap(FilesHmap *pFilesHmap, struct attent *atte) {
    unordered_map <wstring, struct attent *>::iterator aite;
//
    for(aite = pFilesHmap->begin(); pFilesHmap->end() != aite; ) {
        if (atte == aite->second) {
            pFilesHmap->erase(aite);
            break;
        } else ++aite;
    }
    return atte;

}

DWORD NFilesVec::WriteNode(HANDLE hFile, struct attent *atte) {
    DWORD wlen;
    if(!WriteFile(hFile, atte, sizeof(struct attent), &wlen, NULL))
        return ((DWORD)-1);
    return 0x00;
}

DWORD NFilesVec::ReadNode(struct attent *atte, HANDLE hFile) {
    DWORD slen;
    if(!ReadFile(hFile, atte, sizeof(struct attent), &slen, NULL) || !slen)
        return ((DWORD)-1);
    return 0x00;
}