#pragma once

#define FILES_CHKS_DEFAULT				_T("~\\files_chks.sdo")

struct ChksEntry {
    ULONG sibling, recycled;
    FILETIME ftLastWrite;
    unsigned char szFileChks[MD5_DIGEST_LEN];
};

#define INIT_CHKS_ENTRY(entry) \
	memset(&entry, '\0', sizeof(struct ChksEntry)); \
	entry.sibling = INVA_INDE_VALU; \
	entry.recycled = INVA_INDE_VALU;

class CChksList
{
public:
    CChksList(void);
    ~CChksList(void);
public:
    DWORD Initialize(const TCHAR *szLocation);
    DWORD Finalize();
private:
    TCHAR m_szChksList[MAX_PATH];
    HANDLE m_hChksList;
public:
	ULONG FillEntryList(ULONG lListHinde, DWORD *iListLength, DWORD iNewLength);
	ULONG InsEntryList(HANDLE hRiveChks, ULONG lListHinde, DWORD *iListLength, FILETIME *ftLastWrite);
public:
	DWORD FileSha1(unsigned char *pSha1Chks, TCHAR *szFileName, ULONG lListHinde, DWORD *iListLength);
	DWORD ValidFileChks(TCHAR *szFileName, ULONG lListHinde, DWORD iListLength, DWORD iListPosit);
	DWORD FileChks(HANDLE hRiveChks, TCHAR *szFileName, ULONG lListHinde, DWORD *iListLength, DWORD iListPosit);
	DWORD ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, ULONG lListHinde, DWORD *iListLength, DWORD iListPosit);
	ULONG ChunkLastWrite(ULONG lListHinde, DWORD *iListLength, DWORD iListPosit, FILETIME *ftLastWrite);
	ULONG ChunkUpdate(ULONG lListHinde, DWORD *iListLength, DWORD iListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite);
};

extern CChksList objChksList;