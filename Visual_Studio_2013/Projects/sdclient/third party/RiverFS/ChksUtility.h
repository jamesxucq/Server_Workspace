#pragma once

#include "ChksList.h"
//

struct riv_chks {
	unsigned __int64 offset;
	unsigned char md5_chks[MD5_DIGEST_LEN];
};

//

namespace ChksUtility {
	HANDLE ChksFileHandle();
	HANDLE CreatChksFile(HANDLE hFileRiver, const TCHAR *szFileName);
	VOID FileSha1Chks(unsigned char *sha1_chks, HANDLE hRiveChks);
	VOID FileSha1Chks(unsigned char *sha1_chks, HANDLE hChksList, ULONG lListInde);
	//
	DWORD FillIdleEntry(HANDLE hChksList);
	ULONG FindIdleEntry(HANDLE hChksList);
	ULONG AddIdleEntry(HANDLE hChksList, ULONG lIdleInde);
	ULONG ReadNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListInde);
	ULONG WriteNode(HANDLE hChksList, struct ChksEntry *pChksEntry, ULONG lListInde);
	ULONG ModifyNode(HANDLE hChksList, ULONG lListInde, DWORD dwIndeType, ULONG lNextInde);
	//
	DWORD ListLength(HANDLE hChksList, ULONG lListHinde); // for test
	DWORD ListEntry(HANDLE hChksList, ULONG lListHinde); // for test
	VOID ChksIdleEntry(HANDLE hChksList); // for test
	ULONG ResetListLength(HANDLE hChksList, ULONG lListHinde, DWORD dwOldLength, DWORD dwNewLength);
	ULONG ModifyNode(HANDLE hChksList, ULONG lListInde, unsigned char *md5_chks, FILETIME *ftLastWrite);
	//
	// DWORD ValidChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD dwListPosit, FILETIME *ftLastWrite);
	DWORD FileChksRelay(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, WIN32_FIND_DATA *fileInfo);
	DWORD FileChksUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, WIN32_FIND_DATA *fileInfo);
	DWORD FileChunkRelay(HANDLE hChksList, ULONG *lIndePosit, const TCHAR *szFilePath, ULONG lListHinde, DWORD dwListPosit, WIN32_FIND_DATA *fileInfo);
	DWORD FileRiveChks(HANDLE hRiveChks, HANDLE hChksList, ULONG lListPosit);
	ULONG ChunkChksUpdate(HANDLE hChksList, const TCHAR *szFilePath, ULONG lListHinde, DWORD dwListPosit, WIN32_FIND_DATA *fileInfo);
	DWORD ChunkRiveChks(struct riv_chks *pRiveChks, HANDLE hChksList, ULONG lListPosit);
	ULONG ChunkLawiUpdate(HANDLE hChksList, ULONG lListHinde, DWORD dwListPosit, FILETIME *ftLastWrite);
	ULONG ChunkUpdate(HANDLE hChksList, ULONG lListHinde, DWORD dwListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite);
	//
	VOID FileChksCopy(HANDLE hChksList, ULONG lToHinde, ULONG lFromHinde);
};