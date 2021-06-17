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
    FILETIME ftLastWrite;
    unsigned char szFileChks[SHA1_DIGEST_LEN];
    HANDLE hRiveChks;
};

class CFilesRiver
{
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
    ULONG InsEntry(const TCHAR *szFileName, struct FileID *pFileID, ULONG lParentIndex, ULONG lSiblingIndex);
    ULONG InsEntry(ULONG *lListIndex, DWORD iFileLength, const TCHAR *szFileName, ULONG lParentIndex, ULONG lSiblingIndex);
    ULONG DelFileEntry(ULONG *lNextIndex, const TCHAR *szFileName, ULONG lSiblingIndex); // step 1;
    ULONG EntryRestore(const TCHAR *szFileName, ULONG lParentIndex, ULONG lSiblingIndex, const TCHAR *szRestoreName);
    ULONG EntryRestore(const TCHAR *szFileName, ULONG lParentIndex, ULONG lSiblingIndex, struct FileID *pFileID);
    ULONG ExistsEntry(ULONG *lNextIndex, const TCHAR *szFileName, ULONG lSiblingIndex); // step 1;
    DWORD MoveEntry(ULONG lFileIndex, ULONG lParentIndex, ULONG lSiblingIndex, struct FileID *pFileID);
    DWORD MoveEntry(ULONG lFileIndex, ULONG lParentIndex, ULONG lSiblingIndex, const TCHAR *szMoveFile);
public:
    ULONG EntryRestore(ULONG lSiblingIndex);
    DWORD DelFolderEntry(ULONG lSiblingIndex);
public:
    ULONG FindIsonym(const TCHAR *szFileName, struct FileID *pFileID);
    ULONG FindFileEntry(const TCHAR *szFileName, ULONG lSiblingIndex);
    ULONG FindFileEntry(const TCHAR *szFileName, ULONG lSiblingIndex, unsigned char *szFileChks);
    ULONG FindFileEntry(ULONG *lListIndex, DWORD *iListLength, const TCHAR *szFileName, ULONG lSiblingIndex);
    ULONG FindRecycled(BOOL *bPassValid, const TCHAR *szFileName, unsigned char *szFileChks);
    DWORD VoidRecycled();
public:
    ULONG FileChklenUpdate(ULONG lFileIndex, DWORD iListLength);
    DWORD ValidFileSha1(unsigned char *pSha1Chks, FILETIME *ftLastWrite, ULONG lFileIndex, TCHAR *szFileName);
    ULONG FileSha1Update(ULONG lFileIndex, unsigned char *pSha1Chks, FILETIME *ftLastWrite, DWORD iListLength);
};

extern CFilesRiver objFilesRiver;
