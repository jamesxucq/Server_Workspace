#pragma once

//#include "common_macro.h"

#include  <xstring>
using  std::wstring;
#include  <string>
using  std::string;

#include <unordered_map>
using std::unordered_map;      // namespace where class lives by default

#include <vector>
using std::vector;

//
struct DiINode {
	TCHAR szINodeName[MAX_PATH];
	DWORD selected; // 0x00:checked 0x01:part uncheck 0x02:all uncheck
	struct DiINode *sibling;
	struct DiINode *child;
};

//
typedef vector <TCHAR *> DiresVec;
typedef unordered_map <wstring, VOID *> DiresHmap;

namespace NDiresVec {
	VOID DeleDiresVec(vector <TCHAR *> *pListVec);
	TCHAR *AddNewDiresNode(vector <TCHAR *> *pListVec);
//
	inline VOID DeleNode(VOID *node)
	{ if(node) free(node); }
	VOID DeleDiresHmap(DiresHmap *pDiresHmap);
//
	struct DiINode *NewDiINode(TCHAR *szDireName, struct DiINode *sibling);
};



