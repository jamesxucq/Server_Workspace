#pragma once

#include "ChksList.h"
//

struct riv_chks {
	unsigned __int64 offset;
	unsigned char md5_chks[MD5_DIGEST_LEN];
};

//

namespace ChksUtility
{
	HANDLE ChksFileHandle();
	HANDLE CreatChksFile(HANDLE hFileRiver, const TCHAR *szFileName);
	VOID FileSha1Chks(unsigned char *sha1_chks, HANDLE hRiveChks);
	VOID FileSha1Chks(unsigned char *sha1_chks, HANDLE hChksList, ULONG lListIndex);
	//
	DWORD FillIdleEntry(HANDLE hChksList);
	ULONG FindIdleEntry(HANDLE hChksList);
	ULONG AddIdleEntry(HANDLE hChksList, ULONG lIdleIndex);
	ULONG ReadNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListIndex);
	ULONG WriteNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListIndex);
	ULONG ModifyNode(HANDLE hChksList, ULONG lListIndex, DWORD dwIndexType, ULONG lNextIndex);
	//
	DWORD ListLength(HANDLE hChksList, ULONG lListHinde); // for test
	DWORD ListEntry(HANDLE hChksList, ULONG lListHinde); // for test
	VOID ChksIdleEntry(HANDLE hChksList); // for test
	ULONG ResetListLength(HANDLE hChksList, ULONG lListHinde, DWORD iOldLength, DWORD iNewLength);
	ULONG ModifyNode(HANDLE hChksList, ULONG lListIndex, unsigned char *md5_chks, FILETIME *ftLastWrite);
	//
	DWORD ValidChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD iListPosit, FILETIME *ftLastWrite);
	ULONG FileChunkUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, DWORD iListPosit, WIN32_FIND_DATA *fileInfo);
	DWORD FileRiveChks(HANDLE hRiveChks, HANDLE hChksList, ULONG lListPosit);
	ULONG ChunkChksUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, DWORD iListPosit, WIN32_FIND_DATA *fileInfo);
	DWORD ChunkRiveChks(struct riv_chks *pRiveChks, HANDLE hChksList, ULONG lListPosit);
	ULONG ChunkWriteUpdate(HANDLE hChksList, ULONG lListHinde, DWORD iListPosit, FILETIME *ftLastWrite);
	ULONG ChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD iListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite);
};