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

class CChksList {
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
	ULONG FillEntryList(ULONG lListHinde, DWORD *dwListLength, DWORD dwNewLength);
	ULONG InsEntryList(HANDLE hRiveChks, ULONG lListHinde, DWORD dwListLength, DWORD dwNewLength, FILETIME *ftLastWrite);
	ULONG EntryListCopy(ULONG lListHinde, DWORD dwNewLength); // for copy
public:
	DWORD FileSha1Relay(unsigned char *szSha1Chks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength);
	DWORD FileSha1Update(unsigned char *szSha1Chks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength);
	DWORD FileSha1Chks(unsigned char *szSha1Chks, ULONG lListHinde);
	DWORD FileChksRelay(HANDLE hRiveChks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit);
	DWORD ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit);
	ULONG ChunkLawiUpdate(ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit, DWORD chunk_length, FILETIME *ftLastWrite);
	ULONG ChunkUpdate(ULONG *lListHinde, DWORD *dwListLength, DWORD dwListPosit, DWORD chunk_length, unsigned char *md5_chks, FILETIME *ftLastWrite);
};

extern CChksList objChksList;