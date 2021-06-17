#pragma once

#include "kchashdb.h"
//#include "dbz_t.h"
#include "RiverUtility.h"

#include <vector>
using  std::vector;

using namespace std;
using namespace kyotocabinet;

#define FILES_RIVER_DEFAULT				_T("~\\files_river.sdo")
#define FILES_ISONYM_DEFAULT			_T("~\\files_isonym.kct")

//
struct FileID {
    TCHAR szFileName[MAX_PATH];
    unsigned __int64 qwFileSize;
	FILETIME ftCreatTime;
    FILETIME ftLastWrite;
    unsigned char szFileChks[SHA1_DIGEST_LEN];
    HANDLE hRiveChks;
};

class CFilesRiver {
public:
    CFilesRiver(void);
    ~CFilesRiver(void);
public:
    DWORD Initialize(const TCHAR *szLocation);
    DWORD Finalize();
private:
    TreeDB rivdb;
    //
    TCHAR m_szFilesRiver[MAX_PATH];
    HANDLE m_hFilesRiver;
public:
    ULONG InsEntry(const TCHAR *szFileName, struct FileID *pFileID, ULONG lParentInde, ULONG lSibliInde);
    ULONG InsEntry(ULONG *lListInde, DWORD dwChkslen, const TCHAR *szFileName, FILETIME *ftCreatTime, ULONG lParentInde, ULONG lSibliInde);
    ULONG InsBlankEntry(const TCHAR *szFileName, FILETIME *ftCreatTime);
    ULONG DeliFileEntry(ULONG *lNextInde, const TCHAR *szFileName, ULONG lSibliInde); // step 1;
    ULONG GetExistEntry(ULONG *lNextInde, BOOL *bCTimeUpdate,const TCHAR *szExistFile, ULONG lSibliInde); // step 1;
    ULONG MoveEntry(ULONG lFileInde, ULONG lParentInde, ULONG lSibliInde, const TCHAR *szNewFile);
    ULONG CopyEntry(ULONG lFileInde, ULONG lParentInde, ULONG lSibliInde, const TCHAR *szNewFile);
public:
    DWORD DeliFolderEntry(ULONG lSibliInde);
public:
    ULONG FindIsonym(const TCHAR *szFileName, const TCHAR *szDriveLetter, unsigned __int64 qwFileSize, unsigned char *szSha1Chks);
    ULONG FindFileEntry(const TCHAR *szFileName, ULONG lSibliInde);
    ULONG FindFileEntry(ULONG *lListInde, const TCHAR *szFileName, ULONG lSibliInde);
    ULONG FindFileEntry(ULONG *lListInde, DWORD *dwListLength, const TCHAR *szFileName, ULONG lSibliInde);
public:
    ULONG FileLawiClenUpdate(ULONG lFileInde, unsigned __int64 qwFileSize, FILETIME *ftLastWrite, ULONG lListHinde, DWORD dwListLength);
    ULONG FileChklenUpdate(ULONG lFileInde, ULONG lListHinde, DWORD dwListLength);
    ULONG FileTimeChksUpdate(ULONG lFileInde, unsigned __int64 qwFileSize, FILETIME *ftLastWrite, unsigned char *szSha1Chks, FILETIME *ftCreatTime);
    ULONG FileLawiUpdate(ULONG lFileInde, FILETIME *ftLastWrite);
	ULONG CTimeUpdate(ULONG lFileInde, FILETIME *ftCreatTime);
    DWORD ValidFileSha1(unsigned char *szSha1Chks, unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, ULONG lFileInde, TCHAR *szFileName);
    DWORD ValidFileSha1(ULONG lFileInde, FILETIME *ftLastWrite);
    ULONG FileUpdate(ULONG lFileInde, unsigned __int64 qwFileSize, unsigned char *szSha1Chks, FILETIME *ftLastWrite, ULONG lListHinde, DWORD dwListLength);
public:
	DWORD FindSibliAppA(HANDLE hINodeA, ULONG lSibliInde, const TCHAR *szAbsolutePath);
	DWORD FindSibliAppV(HANDLE hINodeV, ULONG lSibliInde, const TCHAR *szRelativePath);
	DWORD IsRecycledEntry(ULONG lSibliInde, ULONG lFileInde); // 0x00:yes !0:no
	DWORD FileAttrib(unsigned __int64 *qwFileSize, unsigned char *szSha1Chks, FILETIME *ftCreatTime, ULONG lFileInde);
};

extern CFilesRiver objFilesRiver;
