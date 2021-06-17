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

#include <vector>
using std::vector;

struct FileBuilder {
	DWORD build_type;
	TCHAR file_name[MAX_PATH];
	off_t offset;
	off_t length;
	////////////////////////////
	TCHAR build_cache[MIN_TEXT_LEN];
};
namespace NBuildUtility {
	DWORD ContentBuilder(struct FileBuilder *builder, vector<struct local_match *> *local_vector);
	void CloseBuildHandle();
};

#endif /* _BUILDFILES_H_ */
