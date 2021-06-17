#include "StdAfx.h"
#include "CommonUtility.h"

#include "../Logger.h"
#include "ChksList.h"
#include "RiverFolder.h"
#include "FilesRiver.h"
#include "CreatRiver.h"

#include "kchashdb.h"
// #include "dbz_t.h"

using namespace std;
using namespace kyotocabinet;

#pragma comment(lib, "..\\third party\\dbz\\dbz.lib")

CCreatRiver::CCreatRiver(void):
    m_hChksList(INVALID_HANDLE_VALUE)
    ,m_hFilesRiver(INVALID_HANDLE_VALUE)
    ,m_hRiverFolder(INVALID_HANDLE_VALUE)
{
    MKZEO(m_szFileIsonym);
}

CCreatRiver::~CCreatRiver(void) {
}

CCreatRiver objCreatRiver;
//

struct RiveHand {
    HANDLE	hRiverFolder;
    HANDLE	hFilesRiver;
    HANDLE	hChksList;
};

static DWORD RecursLocalPath(struct RiveHand *, const TCHAR *szPath);
//
DWORD CCreatRiver::InitializeHandle(const TCHAR *szLocation) {
    TCHAR szFileName[MAX_PATH];
    //
    comutil::get_name(szFileName, szLocation, FILES_CHKS_DEFAULT);
    m_hChksList = CreateFile( szFileName,
                              GENERIC_WRITE | GENERIC_READ,
                              NULL, /* FILE_SHARE_READ */
                              NULL,
                              OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                              NULL);
    if( INVALID_HANDLE_VALUE == m_hChksList ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    comutil::get_name(szFileName, szLocation, FILES_RIVER_DEFAULT);
    m_hFilesRiver = CreateFile( szFileName,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                NULL);
    if( INVALID_HANDLE_VALUE == m_hFilesRiver ) {
		CloseHandle( m_hChksList );
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    comutil::get_name(szFileName, szLocation, RIVER_FOLDER_DEFAULT);
    m_hRiverFolder = CreateFile( szFileName,
                                 GENERIC_WRITE | GENERIC_READ,
                                 NULL, /* FILE_SHARE_READ */
                                 NULL,
                                 OPEN_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                 NULL);
    if( INVALID_HANDLE_VALUE == m_hRiverFolder ) {
		CloseHandle( m_hChksList );
		CloseHandle( m_hFilesRiver );
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
_LOG_DEBUG(_T(" File: %s Line: %d"), _T(__FILE__), __LINE__);
        return ((DWORD)-1);
    }
    //
    comutil::get_name(m_szFileIsonym, szLocation, FILES_ISONYM_DEFAULT);
    return 0x00;
}

VOID CCreatRiver::FinalizeHandle() {
    if(INVALID_HANDLE_VALUE != m_hRiverFolder) {
        CloseHandle( m_hRiverFolder );
        m_hRiverFolder = INVALID_HANDLE_VALUE;
    }
    //
    if(INVALID_HANDLE_VALUE != m_hFilesRiver) {
        CloseHandle( m_hFilesRiver );
        m_hFilesRiver = INVALID_HANDLE_VALUE;
    }
    //
    if(INVALID_HANDLE_VALUE != m_hChksList) {
        CloseHandle( m_hChksList );
        m_hChksList = INVALID_HANDLE_VALUE;
    }
}

DWORD CCreatRiver::CreateEnviro(const TCHAR *szDriveLetter) {
    //
    ULONG lOffset = SetFilePointer(m_hChksList, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==lOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    lOffset = SetEndOfFile(m_hChksList);
    if(FALSE==lOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    lOffset = SetFilePointer(m_hFilesRiver, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==lOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    lOffset = SetEndOfFile(m_hFilesRiver);
    if(FALSE==lOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    lOffset = SetFilePointer(m_hRiverFolder, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==lOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    lOffset = SetEndOfFile(m_hRiverFolder);
    if(FALSE==lOffset && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    //
    struct RiveHand riv_hand = { m_hRiverFolder, m_hFilesRiver, m_hChksList };
    DWORD dwBuildValue = RecursLocalPath(&riv_hand, szDriveLetter);
    if(!dwBuildValue) {
        RiverUtility::FillIdleEntry(m_hChksList, m_hFilesRiver, m_hRiverFolder);
        DeleteFile(m_szFileIsonym);
        dwBuildValue = BuildFileIsonym(m_szFileIsonym, m_hFilesRiver);
    }
    //
    return dwBuildValue;
}

DWORD CCreatRiver::CheckRiverEnviro() {
    if(INVALID_HANDLE_VALUE == m_hChksList) return ((DWORD)-1);
    if(!GetFileSize(m_hChksList, NULL)) return ((DWORD)-1);
    //
    if(INVALID_HANDLE_VALUE == m_hFilesRiver) return ((DWORD)-1);
    if(!GetFileSize(m_hFilesRiver, NULL)) return ((DWORD)-1);
    //
    if(INVALID_HANDLE_VALUE == m_hRiverFolder) return ((DWORD)-1);
    if(!GetFileSize(m_hRiverFolder, NULL)) return ((DWORD)-1);
    //
    if(!comutil::FileExist(m_szFileIsonym)) return ((DWORD)-1);
    //
    return 0x00;
}

DWORD CCreatRiver::BuildRiverEnviro(const TCHAR *szLocation, const TCHAR *szDriveLetter) {
    DWORD build_result = 0;
    //
    if(InitializeHandle(szLocation)) return ((DWORD)-1);
    if(CheckRiverEnviro()) build_result = CreateEnviro(szDriveLetter);
    FinalizeHandle();
    //
    return build_result;
}

//
DWORD CCreatRiver::BuildFileIsonym(const TCHAR *szFileIsonym, HANDLE hFilesRiver) {
    char fileName[MAX_PATH];
    struct FileEntry tFileEntry;
    //
    comutil::ustr_ansi(fileName, szFileIsonym);
    // create the database object
    TreeDB rivdb;
    // Open our database;
    if (!rivdb.open(fileName, TreeDB::OWRITER | TreeDB::OCREATE)) {
        cerr << "open error: " << rivdb.error().name() << endl;
    }
    //
    ULONG value;
    ULONG lFileInde;
    //
    for(lFileInde = 0; ; lFileInde++) {
        if(INVA_INDE_VALU == EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileInde))
            break;
        //
        DWORD key_len = _tcslen(tFileEntry.szFileName) << 1;
        // retrieve a record
        if(0 < rivdb.get((const char *)tFileEntry.szFileName, key_len, (char *)&value, sizeof(ULONG))) {
            if(INVA_INDE_VALU == RiverUtility::InsFileIsonym(hFilesRiver, lFileInde, (DWORD)value)) {
                rivdb.close();
                return (EXIT_FAILURE);
            }
        }
        // Perform the insertion
        if(!rivdb.set((const char *)tFileEntry.szFileName, key_len, (char *)&lFileInde, sizeof(ULONG))) {
            fprintf(stderr, "key already exist.\n");
        }
    }
    // Auto-commit the transaction and close our handle
    if (!rivdb.close()) {
        cerr << "close error: " << rivdb.error().name() << endl;
    }
    //
    return (EXIT_SUCCESS);
}

//
/* Limits and constants. */
#define TYPE_FILE		0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIRE		0x02
#define TYPE_DOT		0x04
#define TYPE_RECY		0x08

static BOOL TraverseDirectory (struct RiveHand *, TCHAR *, TCHAR *, ULONG);
static DWORD FileType (LPWIN32_FIND_DATA);
static ULONG FileItem (struct RiveHand *, LPWIN32_FIND_DATA, ULONG);
static ULONG FolderItem (HANDLE, TCHAR *, LPWIN32_FIND_DATA, ULONG);

static DWORD RecursLocalPath(struct RiveHand *pRiveHand, const TCHAR *szPath) {
    BOOL ok = TRUE;
    TCHAR szSearchName [MAX_PATH], szCurrentPath [MAX_PATH];
    TCHAR szPathString [MAX_PATH] = _T("");
    //
    if(!szPath) return ((DWORD)-1);
    if(!GetCurrentDirectory (MAX_PATH, szCurrentPath)) return ((DWORD)-1);
    //
    _stprintf_s(szSearchName, _T("%s\\"), szPath);
    ULONG lFolderInde = RiverUtility::InitializeEntry(pRiveHand->hRiverFolder, _T("\\"));
    if(INVA_INDE_VALU == lFolderInde) return ((DWORD)-1);
    //
    if(!SetCurrentDirectory (szSearchName)) return ((DWORD)-1);
    ok = TraverseDirectory (pRiveHand, _T ("*"), szPathString, lFolderInde);
    if(!SetCurrentDirectory (szCurrentPath)) return ((DWORD)-1);
    //
    return ok ? 0: -1;
}

/*	Traverse a directory - Carrying out an implementation-specific
"action" for every name encountered. The action in this version is "Delete". */
/*	PathName: Relative or absolute pathname to traverse. */
static BOOL TraverseDirectory (struct RiveHand *pRiveHand, TCHAR *szPathName, TCHAR *szPathString, ULONG lParentInde) {
    HANDLE SearchHandle;
    WIN32_FIND_DATA FindData;
    DWORD dwFileType;
    ULONG lFolderInde;

    /*	Open up the directory search handle and get the
    first file name to satisfy the pathname.
    Make two passes. The first processes the files
    and the second processes the directories. */

    SearchHandle = FindFirstFile (szPathName, &FindData);
    /* A deleted file will not be found on the second pass */
    if (INVALID_HANDLE_VALUE == SearchHandle) return TRUE;

    /* Scan the directory and its subdirectories
    for files satisfying the pattern. */
    do {
        /* For each file located, get the type. Delete files on pass 1.
        On pass 2, recursively process the subdirectory
        contents, if the recursive option is set. */

        dwFileType = FileType (&FindData); /* Delete a file. */
        if(TYPE_FILE & dwFileType) {
            FileItem (pRiveHand, &FindData, lParentInde);
        } else if (TYPE_DIRE & dwFileType) {
            lFolderInde = FolderItem (pRiveHand->hRiverFolder, szPathString, &FindData, lParentInde);
        }

        /* Traverse the subdirectory on the second pass. */
        if (TYPE_DIRE & dwFileType) {
            // fprintf(stderr, "----------------%s\n", szPathString);
            SetCurrentDirectory (FindData.cFileName);
            comutil::appe_path(szPathString, FindData.cFileName);
            //
            TraverseDirectory (pRiveHand, _T ("*"), szPathString, lFolderInde);
            //
            comutil::dele_last_path(szPathString, FindData.cFileName);
            SetCurrentDirectory (_T (".."));
        }
        /* Get the next file or directory name. */
    } while (FindNextFile (SearchHandle, &FindData));
    FindClose (SearchHandle);

    return TRUE;
}

/* Return file type from the find data structure.
Types supported:
TYPE_FILE:	If this is a file
TYPE_DIRE:	If this is a directory other than . or ..
TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData) {
    DWORD dwFileType = TYPE_FILE;
    if (FILE_ATTRIBUTE_DIRECTORY & pFileData->dwFileAttributes)
        if (!lstrcmp(pFileData->cFileName, _T (".")) || !lstrcmp(pFileData->cFileName, _T ("..")))
            dwFileType = TYPE_DOT;
        else if(!_tcsncmp(pFileData->cFileName, _T ("RECYCLER"), 0x08)) dwFileType = TYPE_RECY;
        else dwFileType = TYPE_DIRE;
    //
    return dwFileType;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
#define RIVER_LAST_INDE(FINDE, HRIVER, STYPE, DWRVAL) { \
	FINDE = SetFilePointer(HRIVER, 0, NULL, FILE_END); \
	if(INVALID_SET_FILE_POINTER==FINDE && NO_ERROR!=GetLastError()) \
		return DWRVAL; \
	FINDE /= sizeof(STYPE); \
}

static ULONG FileItem (struct RiveHand *pRiveHand, LPWIN32_FIND_DATA pFileData, ULONG lParentInde) {
    //
    ULONG lFileInde;
    RIVER_LAST_INDE(lFileInde, pRiveHand->hFilesRiver, struct FileEntry, INVA_INDE_VALU)
    //
    unsigned __int64 qwFileSize = pFileData->nFileSizeLow;
    qwFileSize += (((unsigned __int64)pFileData->nFileSizeHigh) << 32);
	DWORD chks_tatol;
	CHKS_LENGTH(chks_tatol, qwFileSize)
    //
    ULONG lSibliInde = RiverUtility::InsFolderLeaf(pRiveHand->hRiverFolder, lParentInde, lFileInde);
    ULONG lListInde = RiverUtility::InsChksListItem(pRiveHand->hChksList, chks_tatol);
// _LOG_DEBUG(_T("cFileName:%s chks_tatol:%d lListInde:%X"), pFileData->cFileName, chks_tatol, lListInde);
    //
	if(INVA_INDE_VALU == RiverUtility::InsFileItem(pRiveHand->hFilesRiver, lFileInde, lSibliInde, lParentInde, 
		lListInde, pFileData->cFileName, qwFileSize, &pFileData->ftCreationTime, chks_tatol))
        return INVA_INDE_VALU;
    //
    return lFileInde;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static ULONG FolderItem (HANDLE hRiverFolder, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData, ULONG lParentInde) {
    ULONG lFolderInde = SetFilePointer(hRiverFolder, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==lFolderInde && NO_ERROR!=GetLastError())
        return INVA_INDE_VALU;
    lFolderInde /= sizeof(struct FolderEntry);
    //
    if(INVA_INDE_VALU == RiverUtility::InsFolderItem(hRiverFolder, lFolderInde, lParentInde, szPathString, pFileData))
        return INVA_INDE_VALU;
    //
    return lFolderInde;
}
