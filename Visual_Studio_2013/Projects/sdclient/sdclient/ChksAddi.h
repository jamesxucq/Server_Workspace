#pragma once

#include "clientcom/clientcom.h"
#include "./RiverFS/RiverFS.h"

//
struct ChksEnti {
	unsigned __int64 offset;
	unsigned char md5_chks[MD5_DIGEST_LEN];
};

struct FileEnti {
    TCHAR szFileName[MAX_PATH];
    unsigned __int64 qwFileSize;
	FILETIME ftCreatTime;
    FILETIME ftLastWrite;
    unsigned char szFileChks[SHA1_DIGEST_LEN];
	ULONG list_hinde;
	DWORD chkslen; /* chunk length */
};
//
namespace NAddiUtili {
	ULONG AppChksList(HANDLE hChksList, HANDLE hRiveChks, DWORD dwChksLen);
	ULONG AppFileChks(HANDLE hFileChks, struct FileID *pFileID, ULONG ulListHinde, DWORD dwChksLen);
	DWORD GetChksList(HANDLE hChksList, HANDLE hRiveChks, ULONG ulListHinde, DWORD dwChksLen);
	DWORD GetFileChks(HANDLE hFileChks, struct FileID *pFileID, ULONG *ulListHinde, DWORD *dwChksLen, ULONG ulFileInde);
};

//
class CChksAddi {
public:
	CChksAddi(void);
	~CChksAddi(void);
public:
	DWORD Initialize(const TCHAR *szLocation);
	VOID Finalize();
private:
    HANDLE m_hFileChks;
    HANDLE m_hChksList;
public:
	ULONG AppFileAttri(struct FileID *pFileID);
	DWORD GetFileAttri(struct FileID *pFileID, ULONG ulFileInde);
	DWORD GetFileAttri(unsigned __int64 *qwFileSize, FILETIME *ftCreatTime, unsigned char *szSha1Chks, ULONG ulFileInde);
	DWORD GetFileChks(char *szSha1Chks, ULONG ulFileInde);
	ULONG AppFolderAttri(struct FileID *pFolderID);
	DWORD GetFolderAttri(struct FileID *pFolderID, ULONG ulFolderInde);
};

extern class CChksAddi objChksAddi;
