#include "StdAfx.h"
//
#include "./RiverFS/RiverFS.h"
#include "clientcom/clientcom.h"
//
#include "LockedAction.h"
#include "PrepaCache.h"
#include "DebugPrinter.h"

//
//
#define COMPARE_NFOUND			0x00000000
#define COMPARE_FOUND			0x00010000
#define INODE_TYPE(ITYPE)		(0x0000FFFF&ITYPE)
#define COMPARE_VALUE(ITYPE)	(0xFFFF0000&ITYPE)
//
#define REALLY_INODEV_DEFAULT		_T("~\\really_inode.tmp")
#define REALDB_INODEV_DEFAULT		_T("~\\really_inode.kch")
#define RIVER_INODEV_DEFAULT		_T("~\\river_inode.tmp")
static DWORD RecursReallyPath(HANDLE, const TCHAR *);

//
HANDLE NPrepaCache::RecursInitial(HashDB *pReallyDB, HANDLE *hRiverINodeV, const TCHAR *szLocation) {
	TCHAR szINodeV[MAX_PATH];
    struti::get_name(szINodeV, szLocation, REALLY_INODEV_DEFAULT);
    HANDLE hReallyINodeV = CreateFile( szINodeV,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                NULL);
    if( INVALID_HANDLE_VALUE == hReallyINodeV ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    DWORD dwResult = SetFilePointer(hReallyINodeV, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return INVALID_HANDLE_VALUE;
	// create the database object
	TCHAR szReallyINode[MAX_PATH];
    char fileName[MAX_PATH];
    comutil::get_name(szReallyINode, szLocation, REALDB_INODEV_DEFAULT);
    comutil::ustr_ansi(fileName, szReallyINode);
	// open the database
	if (!pReallyDB->open(fileName, HashDB::OWRITER | HashDB::OCREATE)) {
		// cerr << "open error: " << db.error().name() << endl;
        _LOG_WARN( _T("open really db error."));
	}
	//
    struti::get_name(szINodeV, szLocation, RIVER_INODEV_DEFAULT);
    *hRiverINodeV = CreateFile( szINodeV,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                                NULL);
    if( INVALID_HANDLE_VALUE == *hRiverINodeV ) {
_LOG_DEBUG( _T("create file failed: %d"), GetLastError() );
        return INVALID_HANDLE_VALUE;
    }
    dwResult = SetFilePointer(*hRiverINodeV, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return INVALID_HANDLE_VALUE;
	//
    return hReallyINodeV;
}

VOID NPrepaCache::FinalRecurs(HANDLE hReallyINodeV, HashDB *pReallyDB, const TCHAR *szLocation, HANDLE hRiverINodeV) {
    if(INVALID_HANDLE_VALUE != hReallyINodeV) {
        CloseHandle( hReallyINodeV );
    }
	// close the database
	if (!pReallyDB->close()) {
		// cerr << "close error: " << db.error().name() << endl;
		_LOG_WARN( _T("close really db error."));
	}
	TCHAR szReallyINode[MAX_PATH];
    comutil::get_name(szReallyINode, szLocation, REALDB_INODEV_DEFAULT);
	DeleteFile(szReallyINode);
	//
    if(INVALID_HANDLE_VALUE != hRiverINodeV) {
        CloseHandle( hRiverINodeV );
    }
}

//
DWORD NPrepaCache::BuildReallyDB(HashDB *pReallyDB, HANDLE hReallyINodeV) {
	struct INodeUtili::INodeV tINodeV;
	//
	ULONG lINodeVInde;
	for(lINodeVInde = 0; INVA_INDE_VALU != INodeUtili::ReadNodeV(hReallyINodeV, lINodeVInde, &tINodeV); lINodeVInde++) {
		DWORD key_len = _tcslen(tINodeV.szINodeName) << 1;
	    // store records
		if (!pReallyDB->set((const char *)tINodeV.szINodeName, key_len, (char *)&lINodeVInde, sizeof(ULONG))) {
			return ((DWORD)-1);
		}
	}
	//
	return 0x00;
}

DWORD NPrepaCache::BuildLockActi(HANDLE hLockActio, HANDLE hReallyINodeV, HashDB *pReallyDB, HANDLE hRiverINodeV) {
	struct INodeUtili::INodeV tRealINodeV;
	struct INodeUtili::INodeV tRiveINodeV;
	DWORD dwActioTotal = 0x00;
	// app modify
	ULONG lINodeVInde;
	for(lINodeVInde=0; INVA_INDE_VALU != INodeUtili::ReadNodeV(hRiverINodeV, lINodeVInde, &tRiveINodeV); lINodeVInde++) {
		DWORD key_len = _tcslen(tRiveINodeV.szINodeName) << 1;
		ULONG value;
		// retrieve a record
		if(0 < pReallyDB->get((const char *)tRiveINodeV.szINodeName, key_len, (char *)&value, sizeof(ULONG))) {
			INodeUtili::ReadNodeV(hReallyINodeV, value, &tRealINodeV);
			if(!INODE_TYPE(tRiveINodeV.dwINodeType) && !INODE_TYPE(tRealINodeV.dwINodeType)) {
TCHAR timestr[64];
_LOG_DEBUG( _T("rive:%s"), timcon::ftim_unis(timestr, &tRiveINodeV.ftLastWrite));
_LOG_DEBUG( _T("real:%s"), timcon::ftim_unis(timestr, &tRealINodeV.ftLastWrite));
				if(memcmp(&tRiveINodeV.ftLastWrite, &tRealINodeV.ftLastWrite, sizeof(FILETIME))) { // app modify
					dwActioTotal++;
					ActionUtility::LockActioApp(hLockActio, tRiveINodeV.szINodeName, MODIFY|LATT_FILE|LATT_FINISHED);
// _LOG_DEBUG( _T("modi:%s"), tRiveINodeV.szINodeName);
				}
			}
			INodeUtili::ModifyNodeV(hRiverINodeV, lINodeVInde, tRiveINodeV.dwINodeType|COMPARE_FOUND);
			INodeUtili::ModifyNodeV(hReallyINodeV, value, tRealINodeV.dwINodeType|COMPARE_FOUND);
		}
	}		
	// app del
	for(lINodeVInde=0; INVA_INDE_VALU != INodeUtili::ReadNodeV(hRiverINodeV, lINodeVInde, &tRiveINodeV); lINodeVInde++) {
		if(!COMPARE_VALUE(tRiveINodeV.dwINodeType)) {
			dwActioTotal++;
			if(INODE_TYPE(tRiveINodeV.dwINodeType))
				ActionUtility::LockActioApp(hLockActio, tRiveINodeV.szINodeName, DELE|LATT_DIRECTORY|LATT_FINISHED);
			else ActionUtility::LockActioApp(hLockActio, tRiveINodeV.szINodeName, DELE|LATT_FILE|LATT_FINISHED);
// _LOG_DEBUG( _T("dele:%s"), tRiveINodeV.szINodeName);
		}
	}
	// app add
	for(lINodeVInde=0; INVA_INDE_VALU != INodeUtili::ReadNodeV(hReallyINodeV, lINodeVInde, &tRealINodeV); lINodeVInde++) {
		if(!COMPARE_VALUE(tRealINodeV.dwINodeType)) {
			dwActioTotal++;
			if(INODE_TYPE(tRealINodeV.dwINodeType))
				ActionUtility::LockActioApp(hLockActio, tRealINodeV.szINodeName, ADDI|LATT_DIRECTORY|LATT_FINISHED);
			else ActionUtility::LockActioApp(hLockActio, tRealINodeV.szINodeName, ADDI|LATT_FILE|LATT_FINISHED);
// _LOG_DEBUG( _T("addi:%s"), tRealINodeV.szINodeName);
		}
	}
	//
	return dwActioTotal;
}

//
DWORD NPrepaCache::PrepaActionCache(HANDLE hLockActio, const TCHAR *szDriveLetter, const TCHAR *szLocation) { // >0:finish 0:nothing <0:exception
	// create the database object
	HashDB ReallyDB;
	HANDLE hRiverINodeV;
	HANDLE hReallyINodeV = RecursInitial(&ReallyDB, &hRiverINodeV, szLocation);
	// 生成本地数据库
	RecursReallyPath(hReallyINodeV, szDriveLetter);
	BuildReallyDB(&ReallyDB, hReallyINodeV);
	// 得到River文件
	RiverFS::RiverRecursINodeV(hRiverINodeV);
	// 生成locked action
	DWORD dwPrepaValue;
	dwPrepaValue = NPrepaCache::BuildLockActi(hLockActio, hReallyINodeV, &ReallyDB, hRiverINodeV);
	//
	NPrepaCache::FinalRecurs(hReallyINodeV, &ReallyDB, szLocation, hRiverINodeV);
	return dwPrepaValue;
}

//
//
/* Limits and constants. */
#define TYPE_FILE		0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIRE		0x02
#define TYPE_DOT		0x04
#define TYPE_RECY		0x08

static BOOL TraverseDirectory (HANDLE, TCHAR *, TCHAR *);
static DWORD FileType (LPWIN32_FIND_DATA);
static DWORD FileItem (HANDLE, TCHAR *, LPWIN32_FIND_DATA);
static DWORD FolderItem (HANDLE, TCHAR *, LPWIN32_FIND_DATA);

static DWORD RecursReallyPath(HANDLE hReally, const TCHAR *szPath) {
    BOOL ok = TRUE;
    TCHAR szSearchName [MAX_PATH], szCurrentPath [MAX_PATH];
    TCHAR szPathString [MAX_PATH] = _T("");
    //
    if(!szPath) return ((DWORD)-1);
    if(!GetCurrentDirectory (MAX_PATH, szCurrentPath)) return ((DWORD)-1);
    //
    _stprintf_s(szSearchName, _T("%s\\"), szPath);
    if(INodeUtili::FolderAdd(hReally, _T("\\"))) // add root
		return ((DWORD)-1);
    //
    if(!SetCurrentDirectory (szSearchName)) return ((DWORD)-1);
    ok = TraverseDirectory (hReally, _T ("*"), szPathString);
    if(!SetCurrentDirectory (szCurrentPath)) return ((DWORD)-1);
    //
    return ok ? 0: -1;
}

/*	Traverse a directory - Carrying out an implementation-specific
"action" for every name encountered. The action in this version is "Delete". */
/*	PathName: Relative or absolute pathname to traverse. */
static BOOL TraverseDirectory (HANDLE hReally, TCHAR *szPathName, TCHAR *szPathString) {
    HANDLE SearchHandle;
    WIN32_FIND_DATA FindData;
    DWORD dwFileType;
    DWORD lFolderInde;

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
            FileItem (hReally, szPathString, &FindData);
        } else if (TYPE_DIRE & dwFileType) {
            lFolderInde = FolderItem (hReally, szPathString, &FindData);
        }

        /* Traverse the subdirectory on the second pass. */
        if (TYPE_DIRE & dwFileType) {
            // fprintf(stderr, "----------------%s\n", szPathString);
            SetCurrentDirectory (FindData.cFileName);
            struti::appe_path(szPathString, FindData.cFileName);
            //
            TraverseDirectory (hReally, _T ("*"), szPathString);
            //
            struti::dele_last_path(szPathString, FindData.cFileName);
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
static DWORD FileItem (HANDLE hReally, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
	TCHAR szFileName[MAX_PATH];
	struti::full_name(szFileName, szPathString, pFileData->cFileName);
	INodeUtili::FileAdd(hReally, szFileName, &pFileData->ftLastWriteTime);
_LOG_DEBUG( _T("prepa file add:%s"), szFileName);
    return 0x00;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static DWORD FolderItem (HANDLE hReally, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData) {
	TCHAR szFilderName[MAX_PATH];
	struti::full_name(szFilderName, szPathString, pFileData->cFileName);
	INodeUtili::FolderAdd(hReally, szFilderName);
_LOG_DEBUG( _T("prepa folder add:%s"), szFilderName);
    return 0x00;
}
