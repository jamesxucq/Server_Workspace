/*
* Match.h
*
*  Created on: 2010-3-9
*      Author: Administrator
*/
#ifndef MATCH_FILES_H_
#define MATCH_FILES_H_

#include "common_macro.h"
#include "FileChks.h"
#include "BuildHTable.h"
#include "Session.h"

#include <vector>
using std::vector;

		
#define CONEN_MODI		0x01
#define CHECK_SUMS      0x02

#pragma	pack(1)
struct file_matcher {
	unsigned char match_type; // 0x01: modify data. 0x02: sum data
	unsigned int offset; // new offset
	DWORD inde_len;
};
#pragma	pack()

struct local_match {
	struct file_matcher *matcher;
	unsigned char *buffer;
};
//
namespace MUtil {
	static DWORD MatchFileChks(vector<struct local_match *> &vlocal_match, hash_table *htab, TCHAR *szFileLocal, unsigned __int64 foset);
	static DWORD MatchChunkChks(vector<struct local_match *> &vlocal_match, hash_table *htab, TCHAR *szFileLocal, unsigned __int64 coset);
	//
	static DWORD FlushZeroFile(HANDLE hFileZero, struct tiny_map64 *buffer, unsigned __int64 offset, unsigned int posit, unsigned int molen);
	static DWORD FlushMatchFile(HANDLE hFileFlush, vector<struct local_match *> *local_vector, int file_chunk);
	static struct file_matcher *AddChksMatch(vector<struct local_match *> *local_vector);
	static struct local_match *AddModifyData(vector<struct local_match *> *local_vector, int length);
	//
	struct local_match *AppendLocalMatch(vector<struct local_match *> &vlocal_match, struct file_matcher *matcher);
	VOID DeleMatchVec(vector<struct local_match *> *local_vector);
	//
#define MATCH_FAULT			((DWORD)-1)
#define MATCH_SUCCESS		0x00
#define MATCH_NOT_FOUND		0x01
#define MATCH_VIOLATION		0x02
	DWORD MatchZeroFile(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal);
	DWORD MatchZeroChunk(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal);
	DWORD BlockMatchFile(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal);
	DWORD BlockMatchChunk(struct SeionCtrl *sctrl, struct InteValue *ival, TCHAR *szFileLocal);
	VOID CloseBlockMatch(struct SeionCtrl *sctrl);
};

#endif /* MATCH_FILES_H_ */