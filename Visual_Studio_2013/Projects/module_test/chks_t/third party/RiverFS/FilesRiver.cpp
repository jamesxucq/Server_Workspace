#include "StdAfx.h"
#include "CommonUtility.h"

// #include "../../clientcom/utility/ulog.h"
#include "../Logger.h"
#include "Sha1.h"
#include "ChksList.h"
#include "RiverFolder.h"
#include "FilesRiver.h"

#pragma comment(lib, "..\\third party\\dbz\\dbz.lib")

CFilesRiver::CFilesRiver(void):
    m_hFilesRiver(INVALID_HANDLE_VALUE)
{
    MKZERO(m_szFilesRiver);
}

CFilesRiver::~CFilesRiver(void)
{
}

CFilesRiver objFilesRiver;

DWORD CFilesRiver::Initialize(const TCHAR *szLocation)
{
    CommonUtility::get_name(m_szFilesRiver, szLocation, FILES_RIVER_DEFAULT);
    m_hFilesRiver = CreateFile( m_szFilesRiver,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                NULL);
    if( INVALID_HANDLE_VALUE == m_hFilesRiver ) {
        TRACE( _T("create file failed: %d\n"), GetLastError() );
        TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    // open the database
    TCHAR szFileIsonym[MAX_PATH];
    char fileName[MAX_PATH];
    CommonUtility::get_name(szFileIsonym, szLocation, FILES_ISONYM_DEFAULT);
    CommonUtility::unicode_ansi(fileName, szFileIsonym);
    // open the database
    if (!rivdb.open(fileName, TreeDB::OWRITER | TreeDB::OCREATE)) {
        // cerr << "open error: " << rivdb.error().name() << endl;
        _LOG_WARN( _T("open rivdb error."));
    }
    //
    return 0;
}

DWORD CFilesRiver::Finalize()
{
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
    return 0;
}

ULONG CFilesRiver::InsEntry(const TCHAR *szFileName, struct FileID *pFileID, ULONG lParentIndex, ULONG lSiblingIndex)
{
    ULONG lListHinde = INVA_INDE_VALU;
    DWORD iListLength = 0;
    ULONG lFileIndex = EntryUtility::FindIdleEntry(&lListHinde, &iListLength, m_hFilesRiver);
    //
    struct FileEntry tFileEntry;
    INIT_FILE_ENTRY(tFileEntry)
    _tcscpy_s(tFileEntry.szFileName, szFileName);
    tFileEntry.parent = lParentIndex;
    tFileEntry.sibling = lSiblingIndex;
    //
    DWORD key_len = _tcslen(tFileEntry.szFileName) << 1;
    ULONG value;
    //
    // retrieve a record
    if (0 < rivdb.get((const char *)tFileEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        tFileEntry.isonym_node = value;
    }
    // store records
    if (!rivdb.set((const char *)tFileEntry.szFileName, key_len, (char *)&lFileIndex, sizeof(ULONG))) {
        return INVA_INDE_VALU;
    }
    //
    tFileEntry.ftLastWrite = pFileID->ftLastWrite;
    memcpy(tFileEntry.szFileChks, pFileID->szFileChks, SHA1_DIGEST_LEN);
    tFileEntry.list_hinde = objChksList.InsEntryList(pFileID->hRiveChks, lListHinde, &iListLength, &pFileID->ftLastWrite);
    tFileEntry.chkslen = iListLength;
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex))
        return INVA_INDE_VALU;

    //
    return lFileIndex;
}

ULONG CFilesRiver::InsEntry(ULONG *lListIndex, DWORD iFileLength, const TCHAR *szFileName, ULONG lParentIndex, ULONG lSiblingIndex)
{
    ULONG lListHinde = INVA_INDE_VALU;
    DWORD iListLength = 0;
    ULONG lFileIndex = EntryUtility::FindIdleEntry(&lListHinde, &iListLength, m_hFilesRiver);
    //
    struct FileEntry tFileEntry;
    INIT_FILE_ENTRY(tFileEntry)
    _tcscpy_s(tFileEntry.szFileName, szFileName);
    tFileEntry.parent = lParentIndex;
    tFileEntry.sibling = lSiblingIndex;
    //
    DWORD key_len = _tcslen(tFileEntry.szFileName) << 1;
    ULONG value;
    //
    // retrieve a record
    if (0 < rivdb.get((const char *)tFileEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        tFileEntry.isonym_node = value;
    }
    // store records
    if (!rivdb.set((const char *)tFileEntry.szFileName, key_len, (char *)&lFileIndex, sizeof(ULONG))) {
        return INVA_INDE_VALU;
    }
    //
	tFileEntry.list_hinde = objChksList.FillEntryList(lListHinde, &iListLength, iFileLength);
    if(lListIndex) *lListIndex = tFileEntry.list_hinde;
    tFileEntry.chkslen = iListLength;
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex))
        return INVA_INDE_VALU;

    //
    return lFileIndex;
}



ULONG CFilesRiver::DelFileEntry(ULONG *lNextIndex, const TCHAR *szFileName, ULONG lSiblingIndex)
{
    struct FileEntry tFileEntry;
    ULONG lFileIndex, lIsonymIndex, lPrevIndex = INVA_INDE_VALU;
    //
    lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSiblingIndex);
    while (_tcscmp(tFileEntry.szFileName, szFileName) && INVA_INDE_VALU!=lFileIndex) {
        lPrevIndex = lFileIndex;
        lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);
    }
    if(INVA_INDE_VALU != lPrevIndex)
        EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_SIBLING, tFileEntry.sibling);
    //
    *lNextIndex = tFileEntry.sibling;
    //
    DWORD key_len = _tcslen(szFileName) << 1;
    ULONG value;
    // retrieve a record
    if(0 < rivdb.get((const char *)szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        lPrevIndex = INVA_INDE_VALU;
        lIsonymIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, (DWORD)value);
        while (lFileIndex!=lIsonymIndex && INVA_INDE_VALU!=lIsonymIndex) {
            lPrevIndex = lIsonymIndex;
            lIsonymIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.isonym_node);
        }
        if(INVA_INDE_VALU == lPrevIndex) {
            // store records
            if(!rivdb.set((const char *)tFileEntry.szFileName, key_len, (char *)&tFileEntry.isonym_node, sizeof(ULONG))) {
                return ((DWORD)-1);
            }
        } else EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_ISONYM, tFileEntry.isonym_node);
    }
    //
    EntryUtility::AddIdleEntry(m_hFilesRiver, lFileIndex);
    //
    return lFileIndex;
}

static ULONG RebuildEntry(const TCHAR *szFileName, TreeDB *pdb, ULONG lFileIndex)
{
    ULONG lIsonymIndex = INVA_INDE_VALU;
    //
    DWORD key_len = _tcslen(szFileName) << 1;
    ULONG value;
    //
    // retrieve a record
    if(0 < pdb->get((const char *)szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        lIsonymIndex = value;
    }
    // store records
    if(!pdb->set((const char *)szFileName, key_len, (char *)&lFileIndex, sizeof(ULONG))) {
        return INVA_INDE_VALU;
    }
    //
    return lIsonymIndex;
}

ULONG CFilesRiver::EntryRestore(const TCHAR *szFileName, ULONG lParentIndex, ULONG lSiblingIndex, const TCHAR *szRestoreName)
{
    struct FileEntry tFillEntry, tFileEntry;
    ULONG lIdleIndex;
    DWORD rlen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    lIdleIndex = tFillEntry.recycled;
    if(INVA_INDE_VALU == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lIdleIndex))
        return INVA_INDE_VALU;
    //
    if(!_tcscmp(tFileEntry.szFileName, szFileName)) {
        //
        tFillEntry.recycled = tFileEntry.recycled;
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        OverLapped.Offset = lIdleOffset;
        if(!WriteFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
            return INVA_INDE_VALU;
        //
        tFileEntry.isonym_node = RebuildEntry(szFileName, &rivdb, lIdleIndex);
        tFileEntry.parent = lParentIndex;
        tFileEntry.sibling = lSiblingIndex;
        CommonUtility::FileLastWrite(&tFileEntry.ftLastWrite, szRestoreName);

        if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lIdleIndex))
            return INVA_INDE_VALU;
        //
    } else lIdleIndex = INVA_INDE_VALU;
    //
    return lIdleIndex;
}


ULONG CFilesRiver::EntryRestore(const TCHAR *szFileName, ULONG lParentIndex, ULONG lSiblingIndex, struct FileID *pFileID)
{
    struct FileEntry tFillEntry, tFileEntry;
    ULONG lIdleIndex;
    DWORD rlen, wlen;
    //
    ULONG lIdleOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    lIdleIndex = tFillEntry.recycled;
    if(INVA_INDE_VALU == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lIdleIndex))
        return INVA_INDE_VALU;
    //
    if(!_tcscmp(tFileEntry.szFileName, szFileName)) {
        //
        tFillEntry.recycled = tFileEntry.recycled;
        OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
        OverLapped.Offset = lIdleOffset;
        if(!WriteFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
            return INVA_INDE_VALU;
        //
        tFileEntry.isonym_node = RebuildEntry(szFileName, &rivdb, lIdleIndex);
        tFileEntry.parent = lParentIndex;
        tFileEntry.sibling = lSiblingIndex;
        CommonUtility::FileLastWrite(&tFileEntry.ftLastWrite, pFileID->szFileName);
        memcpy(&tFileEntry.szFileChks, pFileID->szFileChks, SHA1_DIGEST_LEN);

        if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lIdleIndex))
            return INVA_INDE_VALU;
        //
    } else lIdleIndex = INVA_INDE_VALU;
    //
    return lIdleIndex;
}

ULONG CFilesRiver::ExistsEntry(ULONG *lNextIndex, const TCHAR *szFileName, ULONG lSiblingIndex)
{
    struct FileEntry tFileEntry;
    ULONG lFileIndex, lPrevIndex = INVA_INDE_VALU;
    //
    lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSiblingIndex);
    while (_tcscmp(tFileEntry.szFileName, szFileName)) {
        lPrevIndex = lFileIndex;
        lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);
    }
    if(INVA_INDE_VALU != lPrevIndex)
        EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_SIBLING, tFileEntry.sibling);
    *lNextIndex = tFileEntry.sibling;
    //
    return lFileIndex;
}

DWORD CFilesRiver::MoveEntry(ULONG lFileIndex, ULONG lParentIndex, ULONG lSiblingIndex, struct FileID *pFileID)
{
    struct FileEntry tFileEntry;
    //
    if(INVA_INDE_VALU == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileIndex))
        return ((DWORD)-1);
    tFileEntry.parent = lParentIndex;
    tFileEntry.sibling = lSiblingIndex;

    CommonUtility::FileLastWrite(&tFileEntry.ftLastWrite, pFileID->szFileName);
    memcpy(tFileEntry.szFileChks, pFileID->szFileChks, SHA1_DIGEST_LEN);
    //
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex))
        return ((DWORD)-1);
    //
    return 0;
}

DWORD CFilesRiver::MoveEntry(ULONG lFileIndex, ULONG lParentIndex, ULONG lSiblingIndex, const TCHAR *szMoveFile)
{
    struct FileEntry tFileEntry;
    //
    if(INVA_INDE_VALU == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileIndex))
        return ((DWORD)-1);
    tFileEntry.parent = lParentIndex;
    tFileEntry.sibling = lSiblingIndex;
    //
    CommonUtility::FileLastWrite(&tFileEntry.ftLastWrite, szMoveFile);
    //
    if(INVA_INDE_VALU == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex))
        return ((DWORD)-1);

    return 0;
}

static ULONG RebuildEntry(HANDLE hFilesRiver, TreeDB *pdb, ULONG lSiblingIndex)
{
    struct FileEntry tFileEntry;
    DWORD key_len;
    ULONG value;
    //
    ULONG lIdleIndex = lSiblingIndex;
    while(INVA_INDE_VALU != EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lIdleIndex)) {
        //
        // retrieve a record
        key_len = _tcslen(tFileEntry.szFileName) << 1;
        if(0 < pdb->get((const char *)tFileEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
            tFileEntry.isonym_node = value;
        }
        // store records
        if(!pdb->set((const char *)tFileEntry.szFileName, key_len, (char *)&lIdleIndex, sizeof(ULONG))) {
            return ((DWORD)-1);
        }
        //
        if(INVA_INDE_VALU == EntryUtility::WriteNode(hFilesRiver, &tFileEntry, lIdleIndex))
            return INVA_INDE_VALU;
        //
        lIdleIndex = tFileEntry.sibling;
    }

    return 0;
}

ULONG CFilesRiver::EntryRestore(ULONG lSiblingIndex)
{
    ULONG lIdleOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    //
    struct FileEntry tFileEntry;
    ULONG lPrevIndex, lIdleIndex;
    lPrevIndex = lIdleIndex = lIdleOffset / sizeof(struct FileEntry);
    while(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lIdleIndex)) {
        if(lSiblingIndex == lIdleIndex) {
            EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_RECYCLED, tFileEntry.recycled);
            RebuildEntry(m_hFilesRiver, &rivdb, lSiblingIndex);
            break;
        }
        lPrevIndex = lIdleIndex;
        lIdleIndex = tFileEntry.recycled;
    }
    //
    return 0;
}

static DWORD IndexDelete(HANDLE hFilesRiver, TreeDB *pdb, ULONG lSiblingIndex)
{
    struct FileEntry tDelEntry, tFileEntry;

    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    DWORD key_len;
    ULONG value;
    //
    ULONG lPrevIndex, lIsonymIndex, lDelIndex = lSiblingIndex;
    while(INVA_INDE_VALU != EntryUtility::ReadNode(hFilesRiver, &tDelEntry, lDelIndex)) {
        //
        // retrieve a record
        key_len = _tcslen(tDelEntry.szFileName) << 1;
        if(0 < pdb->get((const char *)tDelEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
            if(value == lDelIndex) { // remove records
                if(INVA_INDE_VALU == tDelEntry.isonym_node) {
                    if (!pdb->remove((const char *)tDelEntry.szFileName, key_len))
                        return ((DWORD)-1);
                } else {
                    if(!pdb->set((const char *)tDelEntry.szFileName, key_len, (char *)&tDelEntry.isonym_node, sizeof(ULONG)))
                        return ((DWORD)-1);
                }
            } else {
                lPrevIndex = lIsonymIndex = value;
                while(INVA_INDE_VALU != EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lIsonymIndex)) {
                    if(lDelIndex == lIsonymIndex) {
                        EntryUtility::ModifyNode(hFilesRiver, lPrevIndex, FILE_INDEX_ISONYM, tFileEntry.isonym_node);
                        break;
                    }
                    //
                    lPrevIndex = lIsonymIndex;
                    lIsonymIndex = tFileEntry.isonym_node;
                }
            }
        }
        //
        lDelIndex = tDelEntry.sibling;
    }
    //
    return 0;
}

DWORD CFilesRiver::DelFolderEntry(ULONG lSiblingIndex)
{
    struct FileEntry tFillEntry;
    ULONG lIdleIndex;
    DWORD rlen, wlen;
    //
    if(INVA_INDE_VALU == lSiblingIndex) return ((DWORD)-1);
    //
    if(IndexDelete(m_hFilesRiver, &rivdb, lSiblingIndex)) return ((DWORD)-1);
    //
    ULONG lIdleOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return ((DWORD)-1);
    }
    //
    lIdleIndex = tFillEntry.recycled;
    tFillEntry.recycled = lSiblingIndex;

    OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
    OverLapped.Offset = lIdleOffset;
    if(!WriteFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
        return ((DWORD)-1);
    //
    EntryUtility::ModifyNode(m_hFilesRiver, lSiblingIndex, FILE_INDEX_RECYCLED, lIdleIndex);
    //
    return 0;
}

#define DUPLI_FAILED		0
#define DUPLI_OKAY			0x00000001
#define DUPLI_MODIFY		0x00000002

static DWORD IsFileDupli(struct FileEntry *pFileEntry, struct FileID *pFileID)
{
    wchar_t szParentName[MAX_PATH], szDupName[MAX_PATH];
    DWORD dwIsDupli = DUPLI_FAILED;
    //
    objRiverFolder.FolderNameIndex(szParentName, pFileEntry->parent);
    PATH_ROOT(szDupName, pFileID->szFileName)
    _tcscpy_s(NO_ROOT(szDupName), MAX_PATH-ROOT_LENGTH, szParentName);
    if(_T('\0') != *(szDupName+0x03)) _tcscat_s(szDupName, MAX_PATH, _T("\\"));
    _tcscat_s(szDupName, MAX_PATH, pFileEntry->szFileName);
    //
    FILETIME ftLastWrite;
    unsigned __int64 qwFileSize;
    CommonUtility::FileSizeTimeExt(&qwFileSize, &ftLastWrite, szDupName);
    // logger(_T("c:\\river.log"), _T("dwFileSize: %d pFileID->lFileSize: %d\r\n"), dwFileSize, pFileID->lFileSize);
    if(pFileID->qwFileSize == qwFileSize) {
        if(memcmp(&pFileEntry->ftLastWrite, &ftLastWrite, sizeof(FILETIME))) {
            // CommonUtility::file_chks(pFileEntry->szFileChks, szDupName);
            objChksList.FileSha1(pFileEntry->szFileChks, szDupName, pFileEntry->list_hinde, &pFileEntry->chkslen);
            pFileEntry->ftLastWrite = ftLastWrite;
            dwIsDupli |= DUPLI_MODIFY;
        }
        //
        if(!memcmp(pFileEntry->szFileChks, pFileID->szFileChks, SHA1_DIGEST_LEN)) dwIsDupli |= DUPLI_OKAY;
    }
    //
    return dwIsDupli;
}

TCHAR *get_md5sum_text(unsigned char *md5sum)
{
    static TCHAR chksum_str[33];
    //
    for (int i=0; i<16; i++)
        _stprintf_s(chksum_str+i*2, 33, _T("%02x"), md5sum[i]);
    chksum_str[32] = _T('\0');
    //
    return chksum_str;
}

ULONG CFilesRiver::FindIsonym(const TCHAR *szFileName, struct FileID *pFileID)
{
    struct FileEntry tFileEntry;
    ULONG value;
    ULONG lParentIndex = INVA_INDE_VALU;
    // logger(_T("c:\\river.log"), _T("file valid:%s %s\r\n"), szFileName, pFileID->szFileName);
    //
    DWORD key_len = _tcslen(szFileName) << 1;
    if(0 < rivdb.get((const char *)szFileName, key_len, (char *)&value, sizeof(ULONG))) {
        // logger(_T("c:\\river.log"), _T("rivdb get key: %s value: %d\r\n"), szFileName, value);
        while (INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, value)) {
            DWORD dwIsDupli = IsFileDupli(&tFileEntry, pFileID);
            if(DUPLI_MODIFY & dwIsDupli) EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, value);
            if(DUPLI_OKAY & dwIsDupli) {
                lParentIndex = tFileEntry.parent;
                // logger(_T("c:\\river.log"), _T("lParentIndex: %d\r\n"), lParentIndex);
                break;
            }
            value = tFileEntry.isonym_node;
            // logger(_T("c:\\river.log"), _T("file md5_chks :%s valid md5_chks: %s\r\n"), get_md5sum_text(tFileEntry.szFileChks), get_md5sum_text(pFileID->szFileChks));
        }
    }
    //
    return lParentIndex;
}

ULONG CFilesRiver::FindFileEntry(const TCHAR *szFileName, ULONG lSiblingIndex)
{
    struct FileEntry tFileEntry;
    ULONG lFileIndex;
    //
    lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSiblingIndex);
    while (INVA_INDE_VALU != lFileIndex) {
        // logger(_T("c:\\river.log"), _T("find file entry md5_chks :%s %s\r\n"), get_md5sum_text(tFileEntry.szFileChks), get_md5sum_text(szFileChks));
        if(!_tcscmp(tFileEntry.szFileName, szFileName)) {
            break;
        }
        lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);
    }
    //
    return lFileIndex;
}

ULONG CFilesRiver::FindFileEntry(const TCHAR *szFileName, ULONG lSiblingIndex, unsigned char *szFileChks)
{
    struct FileEntry tFileEntry;
    ULONG lFileIndex;
    //
    lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSiblingIndex);
    while (INVA_INDE_VALU != lFileIndex) {
        // logger(_T("c:\\river.log"), _T("find file entry md5_chks :%s %s\r\n"), get_md5sum_text(tFileEntry.szFileChks), get_md5sum_text(szFileChks));
        if(!_tcscmp(tFileEntry.szFileName, szFileName)) {
            if(memcmp(&tFileEntry.szFileChks, szFileChks, SHA1_DIGEST_LEN)) lFileIndex = INVA_INDE_VALU;
            break;
        }
        lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);
    }
    //
    return lFileIndex;
}

ULONG CFilesRiver::FindRecycled(BOOL *bPassValid, const TCHAR *szFileName, unsigned char *szFileChks)
{
    struct FileEntry tFillEntry;
    DWORD rlen;
    ULONG lParentIndex = INVA_INDE_VALU;

    ULONG lIdleOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return INVA_INDE_VALU;
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled) {
        if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFillEntry, tFillEntry.recycled)) {
            if(!_tcscmp(tFillEntry.szFileName, szFileName)) {
                lParentIndex = tFillEntry.parent;
                // logger(_T("c:\\river.log"), _T("find recycled md5_chks :%s %s\r\n"), get_md5sum_text(tFillEntry.szFileChks), get_md5sum_text(szFileChks));
                if(memcmp(&tFillEntry.szFileChks, szFileChks, SHA1_DIGEST_LEN)) *bPassValid = FALSE;
                else *bPassValid = TRUE;
            }
        }
    }
    //
    return lParentIndex;
}

DWORD CFilesRiver::VoidRecycled()
{
    struct FileEntry tFillEntry;
    DWORD rlen;
    //
    ULONG lIdleOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lIdleOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
        if(ERROR_HANDLE_EOF != GetLastError()) return ((DWORD)-1);
    }
    //
    if(INVA_INDE_VALU != tFillEntry.recycled)
        EntryUtility::VoidNode(m_hFilesRiver, tFillEntry.recycled);
    //
    return 0;
}

ULONG CFilesRiver::FindFileEntry(ULONG *lListIndex, DWORD *iListLength, const TCHAR *szFileName, ULONG lSiblingIndex)
{
    struct FileEntry tFileEntry;
    ULONG lFindIndex;
    //
    for(lFindIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSiblingIndex);
            _tcscmp(tFileEntry.szFileName, szFileName) && INVA_INDE_VALU!=lFindIndex;
            lFindIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling));
    if(INVA_INDE_VALU != lFindIndex) {
        *lListIndex = tFileEntry.list_hinde;
        *iListLength = tFileEntry.chkslen;
    }
    //
    return lFindIndex;
}

ULONG CFilesRiver::FileChklenUpdate(ULONG lFileIndex, DWORD iListLength)
{
    struct FileEntry tFileEntry;
    ULONG lChklenIndex = INVA_INDE_VALU;
    //
    TRACE(_T("update chks length!\n"));
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileIndex)) {
        tFileEntry.chkslen = iListLength;
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex);
    }
    //
    return lFileIndex;
}

DWORD CFilesRiver::ValidFileSha1(unsigned char *pSha1Chks, FILETIME *ftLastWrite, ULONG lFileIndex, TCHAR *szFileName)
{
    WIN32_FIND_DATA fileInfo;
    HANDLE hFind;
    hFind = FindFirstFile(szFileName, &fileInfo);
    if(INVALID_HANDLE_VALUE != hFind)
        memcpy(ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME));
    FindClose(hFind);
    //
    DWORD dwRiveValue = 0x01;
    struct FileEntry tFileEntry;
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileIndex)) {
        if(!memcmp(&fileInfo.ftLastWriteTime, &tFileEntry.ftLastWrite, sizeof(FILETIME))) {
            memcpy(pSha1Chks, tFileEntry.szFileChks, SHA1_DIGEST_LEN);
            dwRiveValue = 0x00;
        }
    }
    //
    TRACE(_T("valid file sha1:%u\n"), dwRiveValue);
    return dwRiveValue;
}

ULONG CFilesRiver::FileSha1Update(ULONG lFileIndex, unsigned char *pSha1Chks, FILETIME *ftLastWrite, DWORD iListLength)
{
    struct FileEntry tFileEntry;
    //
    if(INVA_INDE_VALU != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileIndex)) {
        memcpy(tFileEntry.szFileChks, pSha1Chks, SHA1_DIGEST_LEN);
        memcpy(&tFileEntry.ftLastWrite, ftLastWrite, sizeof(FILETIME));
        if(iListLength) tFileEntry.chkslen = iListLength;
        EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex);
    }
    //
    return lFileIndex;
}
