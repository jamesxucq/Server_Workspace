/*
* BuildUtility.h
*
*  Created on: 2010-3-17
*      Author: Administrator
*/

#ifndef _BUILDFILES_H_
#define _BUILDFILES_H_

#include "Checksum.h"
#include "MatchUtility.h"
// #include "ResVector.h"

#define RANGE_TYPE_INVA					0x00000000 // invalid
#define RANGE_TYPE_FILE                 0x00000001 // file
#define RANGE_TYPE_CHUNK                0x00000002 // chunk
#define RANGE_TYPE_BLOCK                0x00000004 // block 

struct FileBuilder {
	DWORD build_type;
	TCHAR file_name[MAX_PATH];
	unsigned __int64 offset;
	size_t length;
	TCHAR builder_cache[MIN_TEXT_LEN];
};

namespace NTRANSCache {
	//
	TCHAR *CreatBuilderName(TCHAR *szBuilderName, const TCHAR *szFileName, size_t offset);
	HANDLE OpenBuilder(const TCHAR *szBuilderName);
	VOID CloseBuilder(HANDLE hBuilderCache);
	VOID DestroyBuilder(HANDLE hBuilderCache, const TCHAR *szBuilderName);
};

namespace NBuildUtility {
#define BUILD_FAULT			((DWORD)-1)
#define BUILD_SUCCESS		0x00
#define BUILD_NOT_FOUND		0x01
#define BUILD_VIOLATION		0x02
	//
	DWORD ContentBuilder(struct FileBuilder *builder, vector<struct local_match *> *local_vector);
	void CloseBuilderHandle();
};

#endif /* _BUILDFILES_H_ */
