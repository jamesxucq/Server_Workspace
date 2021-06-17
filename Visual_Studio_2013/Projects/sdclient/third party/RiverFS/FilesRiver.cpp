#include "StdAfx.h"
#include "CommonUtility.h"

#include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "Sha1.h"
#include "INodeUtility.h"
#include "ChksList.h"
#include "RiverFolder.h"
#include "FilesRiver.h"

#pragma comment(lib, "..\\third party\\dbz\\dbz.lib")

CFilesRiver::CFilesRiver(void):
    m_hFilesRiver(INVALID_HANDLE_VALUE)
{
    MKZEO(m_szFilesRiver);
}

CFilesRiver::~CFilesRiver(void) {
}

CFilesRiver objFilesRiver;

DWORD CFilesRiver::Initialize(const TCHAR *szLocation) {

    comutil::get_name(m_szFilesRiver, szLocation, FILES_RIVER_DEFAULT);
    m_hFilesRiver = CreateFile( m_szFilesRiver,
                                GENERIC_WRITE | GENERIC_READ,
                                /*NULL,*/ FILE_SHARE_READ,	// share test
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                NULL);
    if( INVALID_HANDLE_VALUE == m_hFilesRiver ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    // open the database
    TCHAR szFileIsonym[MAX_PATH];
    char fileName[MAX_PATH];
    comutil::get_name(szFileIsonym, szLocation, FILES_ISONYM_DEFAULT);
    comutil::ustr_ansi(fileName, szFileIsonym);
    // open the database
    if (!rivdb.open(fileName, TreeDB::OWRITER | TreeDB::OCREATE)) {
        // cerr << "open error: " << rivdb.error().name() << endl;
        _LOG_WARN( _T("open rivdb error."));
    }
    //
    return 0x00;
}

DWORD CFilesRiver::Finalize() {
    // close the database
    if (!rivdb.close()) {
        // cerr << "close error: " << rivdb.error().name() << endl;
        _LOG_WARN( _T("close rivdb error."));
    }
    //
    if(INVALID_HANDLE_VALUE != m_hFilesRiver) {
        CloseHandle( m_hFilesRiver );
        m_hFilesRiver = INVALID_HANDLE_VALUE;
    }
    //
    return 0x00;
}

// for test
static TCHAR *ChksSha1(unsigned char *sha1sum) {
    static TCHAR chksum_str[64];
    for (int inde=0; inde<SHA1_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, 41, _T("%02x"), sha1sum[inde]);
    chksum_str[40] = _T('\0');
//
	return chksum_str;
}

ULONG CFilesRiver::InsEntry(const TCHAR *szFileName, struct FileID *pFileID, ULONG lParentInde, ULONG lSibliInde) {
    ULONG lListHinde = INVA_INDE_VALU;
    DWORD dwListLength = 0x00;
    ULONG lFileInde = EntryUtility::FindIdleEntry(m_hFilesRiver, &lListHinde, &dwListLength);
    //
    struct FileEntry tFileEntry;
    INIT_FILE_ENTRY(tFileEntry)
    _tcscpy_s(tFileEntry.szFileName, szFileName);
	tFileEntry.qwFileSize = pFileID->qwFileSize;
    tFileEntry.parent = lParentInde;
    tFileEntry.sibling = lSibliInde;
    //
    DWORD key_len = _tcslen(tFileEntry.szFileName) << 1;
    ULONG value;
    //
    // retrieve a record
    if (0 < rivdb.get((const char *)tFileEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        tFileEntry.isonym_node = value;
    }
    // store records
    if (!rivdb.set((const char *)tFileEntry.szFileName, key_len, (char *)&lFileInde, sizeof(ULONG))) {
        return INVA_INDE_VALU;
    }
    //
	memcpy(&tFileEntry.ftCreatTime, &pFileID->ftCreatTime, sizeof(FILETIME));
	memcpy(&tFileEntry.ftLastWrite, &pFileID->ftLastWrite, sizeof(FILETIME));
_LOG_DEBUG( _T("tFileEntry.ftCreatTime:%s"), comutil::ftim_unis(&tFileEntry.ftCreatTime));
_LOG_DEBUG( _T("tFileEntry.ftLastWrite:%s"), comutil::ftim_unis(&tFileEntry.ftLastWrite));
    memcpy(tFileEntry.szFileChks, pFileID->szFileChks, SHA1_DIGEST_LEN);
_LOG_DEBUG(_T("++++++++ file sha1:%s"), ChksSha1(tFileEntry.szFileChks));
	DWORD dwNewLength;
	CHKS_LENGTH(dwNewLength, pFileID->qwFileSize)
//
    tFileEntry.list_hinde = objChksList.InsEntryList(pFileID->hRiveChks, lListHinde, dwListLength, dwNewLength, &pFileID->ftLastWrite);
    tFileEntry.chkslen = dwNewLength;
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

ULONG CFilesRiver::InsEntry(ULONG *lListInde, DWORD dwChkslen, const TCHAR *szFileName, FILETIME *ftCreatTime, ULONG lParentInde, ULONG lSibliInde) {
    ULONG lListHinde = INVA_INDE_VALU;
    DWORD dwListLength = 0x00;
    ULONG lFileInde = EntryUtility::FindIdleEntry(m_hFilesRiver, &lListHinde, &dwListLength);
    //
    struct FileEntry tFileEntry;
    INIT_FILE_ENTRY(tFileEntry)
    _tcscpy_s(tFileEntry.szFileName, szFileName);
    tFileEntry.parent = lParentInde;
    tFileEntry.sibling = lSibliInde;
    //
    DWORD key_len = _tcslen(tFileEntry.szFileName) << 1;
    ULONG value;
    //
    // retrieve a record
    if (0 < rivdb.get((const char *)tFileEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        tFileEntry.isonym_node = value;
    }
    // store records
    if (!rivdb.set((const char *)tFileEntry.szFileName, key_len, (char *)&lFileInde, sizeof(ULONG))) {
        return INVA_INDE_VALU;
    }
    //
	memcpy(&tFileEntry.ftCreatTime, ftCreatTime, sizeof(FILETIME));
	tFileEntry.list_hinde = objChksList.FillEntryList(lListHinde, &dwListLength, dwChkslen);
    if(lListInde) *lListInde = tFileEntry.list_hinde;
    tFileEntry.chkslen = dwListLength;
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

ULONG CFilesRiver::InsBlankEntry(const TCHAR *szFileName, FILETIME *ftCreatTime) {
    ULONG lListHinde = INVA_INDE_VALU;
    DWORD dwListLength = 0x00;
    ULONG lFileInde = EntryUtility::FindIdleEntry(m_hFilesRiver, &lListHinde, &dwListLength);
    //
    struct FileEntry tFileEntry;
    INIT_FILE_ENTRY(tFileEntry)
    _tcscpy_s(tFileEntry.szFileName, szFileName);
	memcpy(&tFileEntry.ftCreatTime, ftCreatTime, sizeof(FILETIME));
    //
	tFileEntry.list_hinde = objChksList.FillEntryList(lListHinde, &dwListLength, 0x00);
    tFileEntry.chkslen = dwListLength;
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

ULONG CFilesRiver::DeliFileEntry(ULONG *lNextInde, const TCHAR *szFileName, ULONG lSibliInde) {
    struct FileEntry tFileEntry;
    ULONG lFileInde, lIsonymInde, lPrevInde = INVA_INDE_VALU;
    //
    lFileInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSibliInde);
    while (_tcscmp(szFileName, tFileEntry.szFileName) && INVA_INDE_VALU!=lFileInde) {
        lPrevInde = lFileInde;
        lFileInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);
    }
    if(INVA_INDE_VALU != lPrevInde)
        EntryUtility::ModifyNode(m_hFilesRiver, lPrevInde, FILE_INDE_SIBLING, tFileEntry.sibling);
    *lNextInde = tFileEntry.sibling;
    //
    DWORD key_len = _tcslen(szFileName) << 1;
    ULONG value;
    // retrieve a record
    if(0 < rivdb.get((const char *)szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        lPrevInde = INVA_INDE_VALU;
        lIsonymInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, (DWORD)value);
        while (lFileInde!=lIsonymInde && INVA_INDE_VALU!=lIsonymInde) {
            lPrevInde = lIsonymInde;
            lIsonymInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.isonym_node);
        }
        if(INVA_INDE_VALU == lPrevInde) {
            // store records
            if(!rivdb.set((const char *)tFileEntry.szFileName, key_len, (char *)&tFileEntry.isonym_node, sizeof(ULONG))) {
                return ((DWORD)-1);
            }
        } else EntryUtility::ModifyNode(m_hFilesRiver, lPrevInde, FILE_INDE_ISONYM, tFileEntry.isonym_node);
    }
    //
    EntryUtility::AddIdleEntry(m_hFilesRiver, lFileInde);
    //
    return lFileInde;
}

ULONG CFilesRiver::GetExistEntry(ULONG *lNextInde, BOOL *bCTimeUpdate, const TCHAR *szExistFile, ULONG lSibliInde) {
	FILETIME ftNullTime = {0};
    struct FileEntry tFileEntry;
    ULONG lFileInde, lPrevInde = INVA_INDE_VALU;
    //
    lFileInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSibliInde);
    while ((INVA_INDE_VALU!=lFileInde) && _tcscmp(szExistFile, tFileEntry.szFileName)) {
        lPrevInde = lFileInde;
        lFileInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);
// _LOG_DEBUG(_T("exist lFileInde:%X tFileEntry.szFileName:%s"), lFileInde, tFileEntry.szFileName);
    }
    if(INVA_INDE_VALU != lPrevInde)
        EntryUtility::ModifyNode(m_hFilesRiver, lPrevInde, FILE_INDE_SIBLING, tFileEntry.sibling);
    *lNextInde = tFileEntry.sibling;
	if(memcmp(&ftNullTime, &tFileEntry.ftCreatTime, sizeof(FILETIME))) *bCTimeUpdate = FALSE;
	else *bCTimeUpdate = TRUE;
	// delete exist isonym
    ULONG lIsonymInde;
    DWORD key_len = _tcslen(szExistFile) << 1;
    ULONG value;
    // retrieve a record
    if(0 < rivdb.get((const char *)szExistFile, key_len, (char *)&value, sizeof(ULONG))) {
        lPrevInde = INVA_INDE_VALU;
        lIsonymInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, (DWORD)value);
        while (lFileInde!=lIsonymInde && INVA_INDE_VALU!=lIsonymInde) {
            lPrevInde = lIsonymInde;
            lIsonymInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.isonym_node);
        }
        if(INVA_INDE_VALU == lPrevInde) {
            // store records
            if(!rivdb.set((const char *)szExistFile, key_len, (char *)&tFileEntry.isonym_node, sizeof(ULONG))) {
                return ((DWORD)-1);
            }
        } else EntryUtility::ModifyNode(m_hFilesRiver, lPrevInde, FILE_INDE_ISONYM, tFileEntry.isonym_node);
    }
    //
    return lFileInde;
}

ULONG CFilesRiver::MoveEntry(ULONG lFileInde, ULONG lParentInde, ULONG lSibliInde, const TCHAR *szNewFile) {
    struct FileEntry tFileEntry;
    if(INVA_INDE_VALU == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde))
        return INVA_INDE_VALU;
    tFileEntry.parent = lParentInde;
    tFileEntry.sibling = lSibliInde;
	_tcscpy_s(tFileEntry.szFileName, MAX_PATH, szNewFile);
    // insert new isonym
    DWORD key_len = _tcslen(szNewFile) << 1;
    ULONG value;
    // ULONG value;
    // retrieve a record
    if (0 < rivdb.get((const char *)szNewFile, key_len, (char *)&value, sizeof(ULONG))) {
        tFileEntry.isonym_node = value;
    }
    // store records
    if (!rivdb.set((const char *)szNewFile, key_len, (char *)&lFileInde, sizeof(ULONG))) {
        return INVA_INDE_VALU;
    }
	// _LOG_DEBUG(_T("move entry lFileInde:%X fileName:%s"), lFileInde, szNewFile);
    //
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde))
        return INVA_INDE_VALU;
//
    return lFileInde;
}

ULONG CFilesRiver::CopyEntry(ULONG lFileInde, ULONG lParentInde, ULONG lSibliInde, const TCHAR *szNewFile) {
    struct FileEntry tFileEntry;
    if(INVA_INDE_VALU == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde))
        return INVA_INDE_VALU;
	//
    ULONG lListHinde = INVA_INDE_VALU;
    DWORD dwListLength = 0x00;
    ULONG lNewInde = EntryUtility::FindIdleEntry(m_hFilesRiver, &lListHinde, &dwListLength);
    //
    struct FileEntry tNewEntry;
    INIT_FILE_ENTRY(tNewEntry)
    _tcscpy_s(tNewEntry.szFileName, szNewFile);
	tNewEntry.qwFileSize = tFileEntry.qwFileSize;
    tNewEntry.parent = lParentInde;
    tNewEntry.sibling = lSibliInde;
    // insert new isonym
    DWORD key_len = _tcslen(szNewFile) << 1;
    ULONG value;
    // ULONG value;
    // retrieve a record
    if (0 < rivdb.get((const char *)szNewFile, key_len, (char *)&value, sizeof(ULONG))) {
        tNewEntry.isonym_node = value;
    }
    // store records
    if (!rivdb.set((const char *)szNewFile, key_len, (char *)&lNewInde, sizeof(ULONG))) {
        return INVA_INDE_VALU;
    }
	// _LOG_DEBUG(_T("move entry lFileInde:%X fileName:%s"), lFileInde, szNewFile);
    //
	memcpy(&tNewEntry.ftCreatTime, &tFileEntry.ftCreatTime, sizeof(FILETIME));
	memcpy(&tNewEntry.ftLastWrite, &tFileEntry.ftLastWrite, sizeof(FILETIME));
    memcpy(tNewEntry.szFileChks, tFileEntry.szFileChks, SHA1_DIGEST_LEN);
_LOG_DEBUG(_T("++++++++ file sha1:%s"), ChksSha1(tNewEntry.szFileChks));
	tNewEntry.list_hinde = objChksList.EntryListCopy(tFileEntry.list_hinde, tFileEntry.chkslen);
	tNewEntry.chkslen = tFileEntry.chkslen;
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tNewEntry, lNewInde))
        return INVA_INDE_VALU;
//
    return lNewInde;
}

static DWORD IndexDele(HANDLE hFilesRiver, TreeDB *pdb, ULONG lSibliInde) {
    struct FileEntry tDeliEntry, tFileEntry;
//
    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD key_len;
    ULONG value;
    //
    ULONG lPrevInde, lIsonymInde, lDeliInde = lSibliInde;
    while(INVA_INDE_VALU != EntryUtility::ReadNode(hFilesRiver, &tDeliEntry, lDeliInde)) {
        // retrieve a record
        key_len = _tcslen(tDeliEntry.szFileName) << 1;
        if(0 < pdb->get((const char *)tDeliEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
            if(value == lDeliInde) { // remove records
                if(INVA_INDE_VALU == tDeliEntry.isonym_node) {
                    if (!pdb->remove((const char *)tDeliEntry.szFileName, key_len))
                        return ((DWORD)-1);
                } else {
                    if(!pdb->set((const char *)tDeliEntry.szFileName, key_len, (char *)&tDeliEntry.isonym_node, sizeof(ULONG)))
                        return ((DWORD)-1);
                }
            } else {
                lPrevInde = lIsonymInde = value;
                while(INVA_INDE_VALU != EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lIsonymInde)) {
                    if(lDeliInde == lIsonymInde) {
                        EntryUtility::ModifyNode(hFilesRiver, lPrevInde, FILE_INDE_ISONYM, tFileEntry.isonym_node);
                        break;
                    }
                    //
                    lPrevInde = lIsonymInde;
                    lIsonymInde = tFileEntry.isonym_node;
                }
            }
        }
        //
        lDeliInde = tDeliEntry.sibling;
    }
    //
    return 0x00;
}

DWORD CFilesRiver::DeliFolderEntry(ULONG lSibliInde) {
    struct FileEntry tFillEntry;
    ULONG lIdleInde;
    DWORD slen, wlen;
    //
    if(INVA_INDE_VALU == lSibliInde) return ((DWORD)-1);
    //
    if(IndexDele(m_hFilesRiver, &rivdb, lSibliInde)) return ((DWORD)-1);
    //
    ULONG lIdleOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &slen, NULL) || !slen) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return ((DWORD)-1);
    }
    //
    lIdleInde = tFillEntry.recycled;
    tFillEntry.recycled = lSibliInde;

    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return ((DWORD)-1);
    //
    EntryUtility::ModifyNode(m_hFilesRiver, lSibliInde, FILE_INDE_RECYCLED, lIdleInde);
    //
    return 0x00;
}

TCHAR *get_md5sum_text(unsigned char *md5sum) {
    static TCHAR chksum_str[33];
    //
    for (int inde=0; inde<16; inde++)
        _stprintf_s(chksum_str+inde*2, 33, _T("%02x"), md5sum[inde]);
    chksum_str[32] = _T('\0');
    //
    return chksum_str;
}

#define DUPLI_FAILED		0
#define DUPLI_OKAY			0x00000001
#define DUPLI_MODIFY		0x00000002

static DWORD IsFileDupli(struct FileEntry *pFileEntry, const TCHAR *szDriveLetter, unsigned __int64 qwFileSize, unsigned char *szSha1Chks) {
    wchar_t szParentName[MAX_PATH], szDupName[MAX_PATH];
    DWORD dwIsDupli = DUPLI_FAILED;
    //
    objRiverFolder.FolderNameIndex(szParentName, pFileEntry->parent);
    DRIVE_LETTE(szDupName, szDriveLetter)
    _tcscpy_s(NO_LETT(szDupName), MAX_PATH-LETT_LENGTH, szParentName);
    if(_T('\0') != *(szDupName+0x03)) _tcscat_s(szDupName, MAX_PATH, _T("\\"));
    _tcscat_s(szDupName, MAX_PATH, pFileEntry->szFileName);
    //
	FILETIME ftLastWrite = {0};
    unsigned __int64 qwNewSize;
    comutil::FileSizeTime(&qwNewSize, &ftLastWrite, szDupName);
// logger(_T("c:\\river.log"), _T("qwNewSize: %lld pFileID->qwNewSize: %lld\r\n"), qwNewSize, pFileID->qwNewSize);
_LOG_DEBUG(_T("++++++++ qwFileSize:%I64u qwNewSize:%I64u"), qwFileSize, qwNewSize);
    if(qwFileSize == qwNewSize) {
        if(memcmp(&pFileEntry->ftLastWrite, &ftLastWrite, sizeof(FILETIME))) {
_LOG_DEBUG(_T("++++++++ pFileEntry->ftLastWrite:%I64u ftLastWrite:%I64u"), pFileEntry->ftLastWrite, ftLastWrite);
            // comutil::file_chks(pFileEntry->szFileChks, szDupName);
            objChksList.FileSha1Relay(pFileEntry->szFileChks, szDupName, &pFileEntry->list_hinde, &pFileEntry->chkslen);
			memcpy(&pFileEntry->ftLastWrite, &ftLastWrite, sizeof(FILETIME));
            dwIsDupli |= DUPLI_MODIFY;
        }
        //
_LOG_DEBUG(_T("++++++++ pFileEntry->szFileChks:%s"), get_md5sum_text(pFileEntry->szFileChks));
_LOG_DEBUG(_T("++++++++ szSha1Chks:%s"), get_md5sum_text(szSha1Chks));
        if(!memcmp(pFileEntry->szFileChks, szSha1Chks, SHA1_DIGEST_LEN)) dwIsDupli |= DUPLI_OKAY;
    }
    //
    return dwIsDupli;
}

ULONG CFilesRiver::FindIsonym(const TCHAR *szFileName, const TCHAR *szDriveLetter, unsigned __int64 qwFileSize, unsigned char *szSha1Chks) {
    struct FileEntry tFileEntry;
    ULONG value;
    ULONG lParentInde = INVA_INDE_VALU;
// logger(_T("c:\\river.log"), _T("file valid:%s %s\r\n"), szFileName, pFileID->szFileName);
_LOG_DEBUG(_T("++++++++ file valid:%s szDriveLetter:%s"), szFileName, szDriveLetter);
    //
    DWORD key_len = _tcslen(szFileName) << 1;
    if(0 < rivdb.get((const char *)szFileName, key_len, (char *)&value, sizeof(ULONG))) {
// logger(_T("c:\\river.log"), _T("rivdb get key: %s value: %d\r\n"), szFileName, value);
_LOG_DEBUG(_T("++++++++ rivdb get key: %s value: %d"), szFileName, value);
        while (INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, value)) {
            DWORD dwIsDupli = IsFileDupli(&tFileEntry, szDriveLetter, qwFileSize, szSha1Chks);
// logger(_T("c:\\river.log"), _T("dwIsDupli: %d\r\n"), dwIsDupli);
_LOG_DEBUG(_T("++++++++ dwIsDupli:%d"), dwIsDupli);
            if(DUPLI_MODIFY & dwIsDupli) EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, value);
            if(DUPLI_OKAY & dwIsDupli) {
                lParentInde = tFileEntry.parent;
// logger(_T("c:\\river.log"), _T("lParentInde: %d\r\n"), lParentInde);
_LOG_DEBUG(_T("++++++++ lParentInde: %d"), lParentInde);
                break;
            }
            value = tFileEntry.isonym_node;
        }
    }
    //
    return lParentInde;
}

ULONG CFilesRiver::FindFileEntry(const TCHAR *szFileName, ULONG lSibliInde) {
    struct FileEntry tFileEntry;
    ULONG lFileInde;
    //
    lFileInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSibliInde);
    while (INVA_INDE_VALU != lFileInde) {
        // logger(_T("c:\\river.log"), _T("find file entry md5_chks :%s %s\r\n"), get_md5sum_text(tFileEntry.szFileChks), get_md5sum_text(szFileChks));
        if(!_tcscmp(szFileName, tFileEntry.szFileName)) {
            break;
        }
        lFileInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);
    }
    //
    return lFileInde;
}

ULONG CFilesRiver::FindFileEntry(ULONG *lListInde, const TCHAR *szFileName, ULONG lSibliInde) {
    struct FileEntry tFileEntry;
    ULONG lFindInde;
    //
// _LOG_DEBUG(_T("lSibliInde:%lu szFileName:%s"), lSibliInde, szFileName);
    for(lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSibliInde);
            _tcscmp(szFileName, tFileEntry.szFileName) && INVA_INDE_VALU!=lFindInde;
			lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling)) {
// _LOG_DEBUG(_T("lFindInde:%lu tFileEntry.sibling:%lu szFileName:%s tFileEntry.szFileName:%s"), lFindInde, tFileEntry.sibling, szFileName, tFileEntry.szFileName);
	}
    if(INVA_INDE_VALU != lFindInde) *lListInde = tFileEntry.list_hinde;
	else *lListInde = INVA_INDE_VALU;
    //
    return lFindInde;
}

ULONG CFilesRiver::FindFileEntry(ULONG *lListInde, DWORD *dwListLength, const TCHAR *szFileName, ULONG lSibliInde) {
    struct FileEntry tFileEntry;
    ULONG lFindInde;
    //
_LOG_DEBUG(_T("lSibliInde:%lu szFileName:%s"), lSibliInde, szFileName);
    for(lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSibliInde);
            _tcscmp(szFileName, tFileEntry.szFileName) && INVA_INDE_VALU!=lFindInde;
			lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling)) {
_LOG_DEBUG(_T("lFindInde:%lu tFileEntry.sibling:%lu szFileName:%s tFileEntry.szFileName:%s"), lFindInde, tFileEntry.sibling, szFileName, tFileEntry.szFileName);
	}
    if(INVA_INDE_VALU != lFindInde) {
        *lListInde = tFileEntry.list_hinde;
        *dwListLength = tFileEntry.chkslen;
_LOG_DEBUG(_T("find file entry. INVA_INDE_VALU != lFindInde"));
	} else {
        *lListInde = INVA_INDE_VALU;
        *dwListLength = 0x00;
_LOG_DEBUG(_T("find file entry. INVA_INDE_VALU == lFindInde"));
	}
    //
    return lFindInde;
}

ULONG CFilesRiver::FileLawiClenUpdate(ULONG lFileInde, unsigned __int64 qwFileSize, FILETIME *ftLastWrite, ULONG lListHinde, DWORD dwListLength) {
    struct FileEntry tFileEntry;
_LOG_DEBUG(_T("update chks length! lListHinde:%lu, dwListLength:%lu"), lListHinde, dwListLength);
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
		tFileEntry.qwFileSize = qwFileSize;
		memcpy(&tFileEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
		if(lListHinde != tFileEntry.list_hinde) tFileEntry.list_hinde = lListHinde;
        tFileEntry.chkslen = dwListLength;
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde);
_LOG_DEBUG(_T("file lawi clen update! lFileInde:%lu"), lFileInde);
    }
    return lFileInde;
}

ULONG CFilesRiver::FileChklenUpdate(ULONG lFileInde, ULONG lListHinde, DWORD dwListLength) {
    struct FileEntry tFileEntry;
_LOG_DEBUG(_T("update chks length! lFileInde:%lu dwListLength:%d"), lFileInde, dwListLength);
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
		if (lListHinde != tFileEntry.list_hinde) tFileEntry.list_hinde = lListHinde;
        tFileEntry.chkslen = dwListLength;
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde);
    }
    return lFileInde;
}

ULONG CFilesRiver::FileTimeChksUpdate(ULONG lFileInde, unsigned __int64 qwFileSize, FILETIME *ftLastWrite, unsigned char *szSha1Chks, FILETIME *ftCreatTime) {
    struct FileEntry tFileEntry;
// _LOG_DEBUG(_T("update chks length!"));
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
		tFileEntry.qwFileSize = qwFileSize;
		memcpy(&tFileEntry.ftCreatTime, ftCreatTime, sizeof(FILETIME));
		memcpy(&tFileEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
		memcpy(tFileEntry.szFileChks, szSha1Chks, SHA1_DIGEST_LEN);
_LOG_DEBUG(_T("++++++++ file sha1:%s"), ChksSha1(tFileEntry.szFileChks));
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde);
_LOG_DEBUG(_T("update file last write:%s"), comutil::ftim_unis(ftLastWrite));
    }
    return lFileInde;
}

ULONG CFilesRiver::FileLawiUpdate(ULONG lFileInde, FILETIME *ftLastWrite) {
    struct FileEntry tFileEntry;
// _LOG_DEBUG(_T("update file lastwrite!"));
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
_LOG_DEBUG(_T("update file lastwrite:%s"), comutil::ftim_unis(ftLastWrite));
		memcpy(&tFileEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde);
    }
    return lFileInde;
}

ULONG CFilesRiver::CTimeUpdate(ULONG lFileInde, FILETIME *ftCreatTime) {
    struct FileEntry tFileEntry;
// _LOG_DEBUG(_T("update file lastwrite!"));
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
_LOG_DEBUG(_T("update file ftCreatTime:%s"), comutil::ftim_unis(ftCreatTime));
		memcpy(&tFileEntry.ftCreatTime, ftCreatTime, sizeof(FILETIME));
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde);
    }
    return lFileInde;
}

// 0:pass 0x01:no context 0x02: recheck
DWORD CFilesRiver::ValidFileSha1(unsigned char *szSha1Chks, unsigned __int64 *qwFileSize, FILETIME *ftLastWrite, ULONG lFileInde, TCHAR *szFileName) {
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    hFind = FindFirstFile(szFileName, &fileInfo);
	if(INVALID_HANDLE_VALUE != hFind) {
		*qwFileSize = fileInfo.nFileSizeLow;
        *qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32;
        memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
	} else { return ((DWORD)-1); }
    FindClose(hFind);
_LOG_WARN( _T("qwFileSize:%I64u"), *qwFileSize);
    //
    DWORD dwRiveValue = 0x02;
    struct FileEntry tFileEntry;
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
		if(!memcmp(&fileInfo.ftLastWriteTime, &tFileEntry.ftLastWrite, sizeof(FILETIME))) {
			if(*qwFileSize) {
				memcpy(szSha1Chks, tFileEntry.szFileChks, SHA1_DIGEST_LEN);
				dwRiveValue = 0x00;
			} else dwRiveValue = 0x01;
		}
    }
    //
_LOG_DEBUG(_T("valid file sha1:%u"), dwRiveValue);
    return dwRiveValue;
}

DWORD CFilesRiver::ValidFileSha1(ULONG lFileInde, FILETIME *ftLastWrite) {
    DWORD dwRiveValue = 0x01;
    struct FileEntry tFileEntry;
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
		if(!memcmp(ftLastWrite, &tFileEntry.ftLastWrite, sizeof(FILETIME))) {
			dwRiveValue = 0x00;
		}
    }
    //
_LOG_DEBUG(_T("valid file sha1:%u"), dwRiveValue);
    return dwRiveValue;
}

ULONG CFilesRiver::FileUpdate(ULONG lFileInde, unsigned __int64 qwFileSize, unsigned char *szSha1Chks, FILETIME *ftLastWrite, ULONG lListHinde, DWORD dwListLength) {
    struct FileEntry tFileEntry;
    //
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
		tFileEntry.qwFileSize = qwFileSize;
        memcpy(tFileEntry.szFileChks, szSha1Chks, SHA1_DIGEST_LEN);
_LOG_DEBUG(_T("++++++++ file sha1:%s"), ChksSha1(tFileEntry.szFileChks));
        memcpy(&tFileEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
		if(lListHinde != tFileEntry.list_hinde) tFileEntry.list_hinde = lListHinde;
        if(dwListLength) tFileEntry.chkslen = dwListLength;
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileInde);
    }
    //
    return lFileInde;
}

DWORD CFilesRiver::FindSibliAppA(HANDLE hINodeA, ULONG lSibliInde, const TCHAR *szAbsolutePath) {
    struct FileEntry tFileEntry;
    ULONG lFindInde;
	TCHAR szFilePath[MAX_PATH];
	//
    for(lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSibliInde);
            INVA_INDE_VALU != lFindInde;
			lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling)) {
// _LOG_DEBUG(_T("lFindInde:%lu tFileEntry.sibling:%lu tFileEntry.szFileName:%s"), lFindInde, tFileEntry.sibling, tFileEntry.szFileName);
		// _tcscpy_s(szFilePath, MAX_PATH, tFileEntry.szFileName+dwPathLength);
		_stprintf_s(szFilePath, MAX_PATH, _T("%s\\%s"), szAbsolutePath, tFileEntry.szFileName);
 _LOG_DEBUG(_T("---- add file:%s"), szFilePath);
		INodeUtili::INodeAApp(hINodeA, szFilePath, INTYPE_FILE);
	}
	//
	return 0x00;
}

DWORD CFilesRiver::FindSibliAppV(HANDLE hINodeV, ULONG lSibliInde, const TCHAR *szRelativePath) {
    struct FileEntry tFileEntry;
    ULONG lFindInde;
	TCHAR szFilePath[MAX_PATH];
	//
    for(lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSibliInde);
			INVA_INDE_VALU != lFindInde;
			lFindInde = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling)) {
// _LOG_DEBUG(_T("lFindInde:%lu tFileEntry.sibling:%lu tFileEntry.szFileName:%s"), lFindInde, tFileEntry.sibling, tFileEntry.szFileName);
// _LOG_DEBUG(_T("szRelativePath:%s tFileEntry.szFileName:%s"), szRelativePath, tFileEntry.szFileName);
		// _tcscpy_s(szFilePath, MAX_PATH, tFileEntry.szFileName+dwPathLength);
		if(_T('\0') == szRelativePath[ROOT_LENGTH]) { // root
			_tcscpy_s(szFilePath, MAX_PATH, szRelativePath);
			_tcscat_s(szFilePath, MAX_PATH, tFileEntry.szFileName);
		} else _stprintf_s(szFilePath, MAX_PATH, _T("%s\\%s"), szRelativePath, tFileEntry.szFileName);
_LOG_DEBUG( _T("tFileEntry.ftLastWrite:%s"), comutil::ftim_unis(&tFileEntry.ftLastWrite));
		INodeUtili::FileAdd(hINodeV, szFilePath, &tFileEntry.ftLastWrite);
_LOG_DEBUG( _T("file add:%s"), szFilePath);
	}
	//
	return 0x00;
}

DWORD CFilesRiver::IsRecycledEntry(ULONG lSibliInde, ULONG lFileInde) { // 0x00:yes !0:no
	struct FileEntry tFileEntry;
	if(INVA_INDE_VALU == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde))
		return ((DWORD)-1);
	if(ISZEO(tFileEntry.szFileName)) return ((DWORD)-1);
	//
	if(INVA_INDE_VALU == EntryUtility::FindRecyEntry(m_hFilesRiver, lSibliInde, lFileInde))
		return ((DWORD)-1);
	//
	return 0x00;
}

DWORD CFilesRiver::FileAttrib(unsigned __int64 *qwFileSize, unsigned char *szSha1Chks, FILETIME *ftCreatTime, ULONG lFileInde) {
    struct FileEntry tFileEntry;
// _LOG_DEBUG(_T("update chks length!"));
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileInde)) {
		*qwFileSize = tFileEntry.qwFileSize;
		memcpy(szSha1Chks, tFileEntry.szFileChks, SHA1_DIGEST_LEN);
		memcpy(ftCreatTime, &tFileEntry.ftCreatTime, sizeof(FILETIME));
	} else { return ((DWORD)-1); }
	//
    return 0x00;
}