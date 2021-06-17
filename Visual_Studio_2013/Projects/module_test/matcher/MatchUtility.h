#ifndef MATCH_FILES_H_
#define MATCH_FILES_H_

#include "common_macro.h"
#include "FileChks.h"
#include "BuildHTable.h"
// #include "Session.h"

#include <vector>
using std::vector;

		
#define CONEN_MODI		0x01
#define CHECK_SUMS      0x02
#define CONEN_VARI		0x04

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
#define MATCH_FAULT			((DWORD)-1)
#define MATCH_SUCCESS		0x00
#define MATCH_NOT_FOUND		0x01
#define MATCH_VIOLATION		0x02
	DWORD BlockMatchFile(HANDLE hFileFlush, struct complex_head *phead_complex, unsigned __int64 offset, TCHAR *szFileLocal);
	DWORD BlockMatchChunk(HANDLE hFileFlush, struct complex_head *phead_complex, unsigned __int64 offset, TCHAR *szFileLocal);
};

#endif /* MATCH_FILES_H_ */