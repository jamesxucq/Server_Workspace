#include "StdAfx.h"
#include "CommonUtility.h"

#include "RiverFolder.h"
#include "FilesRiver.h"
#include "CreatRiver.h"

#include "..\dbz\kchashdb.h"

using namespace std;
using namespace kyotocabinet;

#pragma comment(lib, "..\\third party\\dbz\\dbz.lib")

CCreatRiver::CCreatRiver(void)
{
}

CCreatRiver::~CCreatRiver(void)
{
}

CCreatRiver objCreatRiver;
static DWORD FileDirectory(HANDLE hFilesRiver, HANDLE hRiverFolder, const TCHAR *szPath);

DWORD CCreatRiver::InitializeHandle(const TCHAR *szLocation){
	TCHAR szFileName[MAX_PATH];

	CommonUtility::get_name(szFileName, szLocation, FILES_RIVER_DEFAULT);
	m_hFilesRiver = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hFilesRiver == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}

	CommonUtility::get_name(szFileName, szLocation, RIVER_FOLDER_DEFAULT);
	m_hRiverFolder = CreateFile( szFileName, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hRiverFolder == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}

	CommonUtility::get_name(m_szFileNsake, szLocation, FILES_NSAKE_DEFAULT);
	return 0;
}

VOID CCreatRiver::FinalizeHandle(){
	if(m_hRiverFolder != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hRiverFolder );
		m_hRiverFolder = INVALID_HANDLE_VALUE;
	}

	if(m_hFilesRiver != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hFilesRiver );
		m_hFilesRiver = INVALID_HANDLE_VALUE;
	}
}

DWORD CCreatRiver::CreateEnvirn(const TCHAR *szDriveRoot) {
	//////////////////////////////////////////////////////////////
	DWORD lOffset = SetFilePointer(m_hFilesRiver, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==lOffset && NO_ERROR!=GetLastError()) 
		return -1;
	lOffset = SetEndOfFile(m_hFilesRiver);
	if(FALSE==lOffset && NO_ERROR!=GetLastError()) 
		return -1;
	//////////////////////////////////////////////////////////////
	lOffset = SetFilePointer(m_hRiverFolder, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==lOffset && NO_ERROR!=GetLastError()) 
		return -1;
	lOffset = SetEndOfFile(m_hRiverFolder);
	if(FALSE==lOffset && NO_ERROR!=GetLastError()) 
		return -1;
	//////////////////////////////////////////////////////////////
	DWORD dwBuildValue = FileDirectory(m_hFilesRiver, m_hRiverFolder, szDriveRoot);
	if(!dwBuildValue) {
		RiverUtility::FillEmptyEntry(m_hFilesRiver, m_hRiverFolder);
		DeleteFile(m_szFileNsake);
		dwBuildValue = BuildFileNsake(m_szFileNsake, m_hFilesRiver);
	}
	//////////////////////////////////////////////////////////////
	return dwBuildValue;
}

DWORD CCreatRiver::ValidRiverEnvirn() {	
	if(m_hFilesRiver == INVALID_HANDLE_VALUE) return -1;
	if(!GetFileSize(m_hFilesRiver, NULL)) return -1;

	if(m_hRiverFolder == INVALID_HANDLE_VALUE) return -1;
	if(!GetFileSize(m_hRiverFolder, NULL)) return -1;

	if(!CommonUtility::FileExists(m_szFileNsake)) return -1;

	return 0;
}

DWORD CCreatRiver::BuildRiverEnvirn(const TCHAR *szLocation, const TCHAR *szDriveRoot) {
	DWORD build_result = 0;

	if(InitializeHandle(szLocation)) return -1;
	if(ValidRiverEnvirn()) build_result = CreateEnvirn(szDriveRoot);
	FinalizeHandle();

	return build_result;
}

////////////////////////////////////////////////////////////////////////////////////////
DWORD CCreatRiver::BuildFileNsake(const TCHAR *szFileNsake, HANDLE hFilesRiver) {
	char fileName[MAX_PATH];
	struct FileEntry tFileEntry;

	CommonUtility::unicode_ansi(fileName, szFileNsake);
	// create the database object
	TreeDB db;

	// open the database
	if (!db.open(fileName, TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "open error: " << db.error().name() << endl;
	}

	DWORD value;
	DWORD lFileIndex;
	//////////////////////////////////////
	for(lFileIndex = 0; ; lFileIndex++) {
		if(INVALID_INDEX_VALUE == EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lFileIndex))
			break;
		/////////////////////////////////////////////////////////////////
		DWORD key_len = _tcslen(tFileEntry.szFileName);
		// retrieve a record
		if (0 < db.get((const char *)tFileEntry.szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
			if(INVALID_INDEX_VALUE == RiverUtility::InsFileNsake(hFilesRiver, lFileIndex, (DWORD)value)) {
				db.close();
				return (EXIT_FAILURE);
			}
		}
		// store records
		if (!db.set((const char *)tFileEntry.szFileName, key_len<<1, (char *)&lFileIndex, sizeof(DWORD))) {
			fprintf(stderr, "Key already exists.\n");
		}
	}

	// close the database
	if (!db.close()) {
		cerr << "close error: " << db.error().name() << endl;
	}

	return (EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////////////
/* Limits and constants. */
#define TYPE_FILE		0x01		/* Used in ls, rm, and lsFP */
#define TYPE_DIR		0x02
#define TYPE_DOT		0x04
#define TYPE_RECY		0x08

static BOOL TraverseDirectory (HANDLE, HANDLE, TCHAR *, TCHAR *, DWORD);
static DWORD FileType (LPWIN32_FIND_DATA);
static DWORD FileItem (HANDLE, HANDLE, LPWIN32_FIND_DATA, DWORD);
static DWORD FolderItem (HANDLE, TCHAR *, LPWIN32_FIND_DATA, DWORD);

static DWORD FileDirectory(HANDLE hFilesRiver, HANDLE hRiverFolder, const TCHAR *szPath) {
	BOOL ok = TRUE;
	TCHAR szSearchName [MAX_PATH], szCurrentPath [MAX_PATH];
	TCHAR szPathString [MAX_PATH] = _T("");

	if(!szPath) return -1;
	if(!GetCurrentDirectory (MAX_PATH, szCurrentPath)) return -1;

	_stprintf_s(szSearchName, _T("%s\\"), szPath);
	DWORD lFolderIndex = RiverUtility::InitializeEntry(hRiverFolder, _T("\\"));
	if(INVALID_INDEX_VALUE == lFolderIndex) return -1;

	if(!SetCurrentDirectory (szSearchName)) return -1;
	ok = TraverseDirectory (hFilesRiver, hRiverFolder, _T ("*"), szPathString, lFolderIndex);
	if(!SetCurrentDirectory (szCurrentPath)) return -1;

	return ok ? 0: -1;
}

/*	Traverse a directory - Carrying out an implementation-specific
"action" for every name encountered. The action in this version is "Delete". */
/*	PathName: Relative or absolute pathname to traverse. */
static BOOL TraverseDirectory (HANDLE hFilesRiver, HANDLE hRiverFolder, TCHAR *szPathName, TCHAR *szPathString, DWORD lParentIndex) {
	HANDLE SearchHandle;
	WIN32_FIND_DATA FindData;
	DWORD dwFileType;
	DWORD lFolderIndex;

	/*	Open up the directory search handle and get the
	first file name to satisfy the pathname.
	Make two passes. The first processes the files
	and the second processes the directories. */

	SearchHandle = FindFirstFile (szPathName, &FindData);
	/* A deleted file will not be found on the second pass */
	if (SearchHandle == INVALID_HANDLE_VALUE) return TRUE;

	/* Scan the directory and its subdirectories
	for files satisfying the pattern. */
	do {
		/* For each file located, get the type. Delete files on pass 1.
		On pass 2, recursively process the subdirectory
		contents, if the recursive option is set. */

		dwFileType = FileType (&FindData); /* Delete a file. */
		if(dwFileType & TYPE_FILE) {
			FileItem (hRiverFolder, hFilesRiver, &FindData, lParentIndex);
		} else if (dwFileType & TYPE_DIR) {
			lFolderIndex = FolderItem (hRiverFolder, szPathString, &FindData, lParentIndex);		
		}

		/* Traverse the subdirectory on the second pass. */
		if (dwFileType & TYPE_DIR) {
			//printf("----------------%s\n", szPathString);
			SetCurrentDirectory (FindData.cFileName);
			CommonUtility::append_path(szPathString, FindData.cFileName);
			////////////////////////////////////////////////////////////
			TraverseDirectory (hFilesRiver, hRiverFolder, _T ("*"), szPathString, lFolderIndex);
			////////////////////////////////////////////////////////////
			CommonUtility::del_last_path(szPathString, FindData.cFileName);
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
TYPE_DIR:	If this is a directory other than . or ..
TYPE_DOT:	If this is . or .. directory */
static DWORD FileType (LPWIN32_FIND_DATA pFileData) {
	DWORD dwFileType = TYPE_FILE;
	if (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		if (!lstrcmp(pFileData->cFileName, _T (".")) || !lstrcmp(pFileData->cFileName, _T ("..")))
			dwFileType = TYPE_DOT;
		else if(!_tcsncmp(pFileData->cFileName, _T ("RECYCLER"), 0x08)) dwFileType = TYPE_RECY;
		else dwFileType = TYPE_DIR;

		return dwFileType;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static DWORD FileItem (HANDLE hRiverFolder, HANDLE hFilesRiver, LPWIN32_FIND_DATA pFileData, DWORD lParentIndex) {
	DWORD lFileIndex = SetFilePointer(hFilesRiver, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lFileIndex && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;
	lFileIndex /= sizeof(struct FileEntry);

	DWORD lSiblingIndex = RiverUtility::InsFolderLeaf(hRiverFolder, lParentIndex, lFileIndex);
	if(INVALID_INDEX_VALUE == RiverUtility::InsFileItem(hFilesRiver, lFileIndex, lSiblingIndex, lParentIndex, pFileData)) 
		return INVALID_INDEX_VALUE;

	return lFileIndex;
}

/* Function to process (delete, in this case) the file or directory.
Interact with the user if the interact flag is set.
/* ParName is the parent directory name, terminated with
a \ or the null string. szFileName has no wildcards at this point. */
static DWORD FolderItem (HANDLE hRiverFolder, TCHAR *szPathString, LPWIN32_FIND_DATA pFileData, DWORD lParentIndex) {
	DWORD lFolderIndex = SetFilePointer(hRiverFolder, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lFolderIndex && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;
	lFolderIndex /= sizeof(struct FolderEntry);

	if(INVALID_INDEX_VALUE == RiverUtility::InsFolderItem(hRiverFolder, lFolderIndex, lParentIndex, szPathString, pFileData))
		return INVALID_INDEX_VALUE;

	return lFolderIndex;
}
