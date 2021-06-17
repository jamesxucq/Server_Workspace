/*
* Match.h
*
*  Created on: 2010-3-9
*      Author: Administrator
*/
#ifndef MATCH_FILES_H_
#define MATCH_FILES_H_

#include "common_macro.h"
#include "FileCsums.h"
#include "Session.h"

#include <vector>
using std::vector;


#define MODIFY_CONTENT  0x01
#define CHECK_SUM       0x02


#pragma	pack(1)
struct file_matcher {
	char match_type; //m: modify data. s: sum data
	off_t offset; //new offset
	DWORD index_length;
};
#pragma	pack()

struct local_match {
	struct file_matcher *inform;
	char *buffer;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
struct local_match *AppendLocalMatch(OUT vector<struct local_match *> &vlocal_match, struct file_matcher *inform);

////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD FlushMatch2File(struct SessionCtrl *psctrl, struct InternalValue *psvalue, vector<struct local_match *> *local_vector);

struct file_matcher *AddCsumMatch(vector<struct local_match *> *local_vector);
struct local_match *AddModifyData(vector<struct local_match *> *local_vector, int length);

void DeleteMatchVec(vector<struct local_match *> *local_vector);

////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD BlockMatchFileRange(struct SessionCtrl *psctrl, struct InternalValue *psvalue, TCHAR *szFileLocal);
DWORD BlockMatchChunkRange(struct SessionCtrl *psctrl, struct InternalValue *psvalue, TCHAR *szFileLocal);

#endif /* MATCH_FILES_H_ */