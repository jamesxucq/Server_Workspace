#pragma once

//#include "common_macro.h"

#include <vector>
using std::vector;


typedef vector <TCHAR *> DiresVec;


namespace NDiresVec
{
	void DeleteDiresVec(vector <TCHAR *> &vDiresVec);
	TCHAR * AddNewDiresNode(vector <TCHAR *> &vDiresVec);
};



