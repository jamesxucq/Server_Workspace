#include "StdAfx.h"
#include "StringUtility.h"

#include "DiresVec.h"


void NDiresVec::DeleteDiresVec(vector <TCHAR *> &vDiresVec)
{
	vector <TCHAR *>::iterator iter;

	for(iter = vDiresVec.begin(); iter != vDiresVec.end(); iter++) {
		free(*iter);
	}
	vDiresVec.clear();
}

TCHAR *NDiresVec::AddNewDiresNode(vector <TCHAR *> &vDiresVec)
{
	TCHAR *szNewDirectory;

	szNewDirectory = (TCHAR *)malloc(MAX_PATH);
	if(!szNewDirectory) return NULL;
	////////////////////////////////////////////////
	vDiresVec.push_back(szNewDirectory);

	return szNewDirectory;
}

