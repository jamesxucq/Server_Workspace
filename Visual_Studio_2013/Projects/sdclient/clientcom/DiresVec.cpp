#include "StdAfx.h"
#include "StringUtility.h"

#include "DiresVec.h"


VOID NDiresVec::DeleDiresVec(vector <TCHAR *> *pListVec) {
	vector <TCHAR *>::iterator iter;
	for(iter = pListVec->begin(); pListVec->end() != iter; ++iter) {
		free(*iter);
	}
	pListVec->clear();
}

TCHAR *NDiresVec::AddNewDiresNode(vector <TCHAR *> *pListVec) {
	TCHAR *szNewDirectory;
	szNewDirectory = (TCHAR *)malloc(MAX_PATH * sizeof(TCHAR));
	if(!szNewDirectory) return NULL;
	//
	pListVec->push_back(szNewDirectory);
	return szNewDirectory;
}

VOID NDiresVec::DeleDiresHmap(DiresHmap *pDiresHmap)
{
    unordered_map <wstring, VOID *>::iterator aite;
    // if(pDiresHmap->empty()) return;
	for(aite = pDiresHmap->begin(); pDiresHmap->end() != aite; ++aite) {
        if(aite->second) DeleNode(aite->second);
	}
    pDiresHmap->clear();
}

struct DiINode *NDiresVec::NewDiINode(TCHAR *szDireName, struct DiINode *sibling) {
    struct DiINode *dnod_new;
//
    dnod_new = (struct DiINode *)malloc(sizeof(struct DiINode));
    if(!dnod_new) return NULL;
    memset(dnod_new, '\0', sizeof(struct DiINode));
//
	_tcscpy_s(dnod_new->szINodeName, MAX_PATH, szDireName);
	dnod_new->sibling = sibling;
//
    return dnod_new;
}

