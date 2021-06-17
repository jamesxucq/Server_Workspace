#include "StdAfx.h"
#include "CommonUtility.h"

#include "RiverFolder.h"
#include "FilesRiver.h"
#include "../../clientcom/utility/ulog.h"

#pragma comment(lib, "..\\third party\\dbz\\dbz.lib")

static DWORD ValidFilesByDirectory(const TCHAR *szPath);

CFilesRiver::CFilesRiver(void)
{
}

CFilesRiver::~CFilesRiver(void)
{
}

CFilesRiver objFilesRiver;

DWORD CFilesRiver::Initialize(const TCHAR *szLocation) {
	CommonUtility::get_name(m_szFilesRiver, szLocation, FILES_RIVER_DEFAULT);
	m_hFilesRiver = CreateFile( m_szFilesRiver, 
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

	// open the database
	TCHAR szFileNsake[MAX_PATH];
	char fileName[MAX_PATH];
	CommonUtility::get_name(szFileNsake, szLocation, FILES_NSAKE_DEFAULT);
	CommonUtility::unicode_ansi(fileName, szFileNsake);
	// open the database
	if (!db.open(fileName, TreeDB::OWRITER | TreeDB::OCREATE)) {
		cerr << "open error: " << db.error().name() << endl;
	}

	return 0;
}

DWORD CFilesRiver::Finalize() {
	// close the database
	if (!db.close()) {
		cerr << "close error: " << db.error().name() << endl;
	}

	if(m_hFilesRiver != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hFilesRiver );
		m_hFilesRiver = INVALID_HANDLE_VALUE;
	}

	return 0;
}

DWORD CFilesRiver::InsEntry(const TCHAR *szFileName, FILETIME *ftCreationTime, DWORD lParentIndex, DWORD lSiblingIndex) {
	struct FileEntry tFileEntry;

	DWORD lFileIndex = EntryUtility::FindEmptyEntry(m_hFilesRiver);
	/////////////////////////////////////////////////////
	INIT_FILE_ENTRY(tFileEntry)
	_tcscpy_s(tFileEntry.szFileName, szFileName);
	tFileEntry.ftCreationTime = *ftCreationTime;
	tFileEntry.parent = lParentIndex;
	tFileEntry.sibling = lSiblingIndex;
	///////////////////////
	DWORD key_len = _tcslen(tFileEntry.szFileName);
	DWORD value;
	/////////////////////////////////////////////////////////////////
	// retrieve a record
	if (0 < db.get((const char *)tFileEntry.szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
		tFileEntry.nsake_bin = value;
	}
	// store records
	if (!db.set((const char *)tFileEntry.szFileName, key_len<<1, (char *)&lFileIndex, sizeof(DWORD))) {
		return INVALID_INDEX_VALUE;
	}
	/////////////////////////////////////////////////////
	if(INVALID_INDEX_VALUE == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex))
		return INVALID_INDEX_VALUE;
	/////////////////////////////////////////////////////

	return lFileIndex;
}

DWORD CFilesRiver::DelFileEntry(DWORD *lNextIndex, const TCHAR *szFileName, DWORD lSiblingIndex) {
	struct FileEntry tFileEntry;
	DWORD lFileIndex, lNsakeIndex, lPrevIndex = INVALID_INDEX_VALUE;

	lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSiblingIndex);
	while (_tcscmp(tFileEntry.szFileName, szFileName) && INVALID_INDEX_VALUE!=lFileIndex) {
		lPrevIndex = lFileIndex;
		lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);	
	}
	if(INVALID_INDEX_VALUE != lPrevIndex)
		EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_SIBLING, tFileEntry.sibling);
	/////////////////////////////////////////////////////
	*lNextIndex = tFileEntry.sibling;
	/////////////////////////////////////////////////////
	DWORD key_len = _tcslen(szFileName);
	DWORD value;
	// retrieve a record
	if(0 < db.get((const char *)szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
		lPrevIndex = INVALID_INDEX_VALUE;
		lNsakeIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, (DWORD)value);
		while (lFileIndex!=lNsakeIndex && INVALID_INDEX_VALUE!=lNsakeIndex) {
			lPrevIndex = lNsakeIndex;
			lNsakeIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.nsake_bin);	
		}
		if(INVALID_INDEX_VALUE == lPrevIndex) {
			// store records
			if (!db.set((const char *)tFileEntry.szFileName, key_len<<1, (char *)&tFileEntry.nsake_bin, sizeof(DWORD))) {
				return -1;
			}
		} else EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_NSAKE, tFileEntry.nsake_bin);
	}
	/////////////////////////////////////////////////////
	EntryUtility::AddEmptyEntry(m_hFilesRiver, lFileIndex);

	return lFileIndex;
}

static DWORD RebuildEntry(const TCHAR *szFileName, TreeDB *dbp, DWORD lFileIndex) {
	DWORD lNsakeIndex = INVALID_INDEX_VALUE;
	///////////////////////
	DWORD key_len = _tcslen(szFileName);
	DWORD value;
	/////////////////////////////////////////////////////////////////
	// retrieve a record
	if (0 < dbp->get((const char *)szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
		lNsakeIndex = value;
	}
	// store records
	if (!dbp->set((const char *)szFileName, key_len<<1, (char *)&lFileIndex, sizeof(DWORD))) {
		return INVALID_INDEX_VALUE;
	}

	return lNsakeIndex;
}

DWORD CFilesRiver::EntryRestore(const TCHAR *szFileName, FILETIME *ftCreationTime, DWORD lParentIndex, DWORD lSiblingIndex) {
	struct FileEntry tFileEntry, tEmptyEntry;
	DWORD lEmptyIndex;	
	DWORD rlen, wlen;

	///////////////////////////////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError())
		return INVALID_INDEX_VALUE;
	if(!ReadFile(m_hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	//////////////////////////
	lEmptyIndex = tFileEntry.nsake_bin;
	if(INVALID_INDEX_VALUE == EntryUtility::ReadNode(m_hFilesRiver, &tEmptyEntry, lEmptyIndex))
		return INVALID_INDEX_VALUE;
	///////////////////////////////////////////////////////////////////////
	if(!_tcscmp(tEmptyEntry.szFileName, szFileName) && !memcmp(&tEmptyEntry.ftCreationTime, ftCreationTime, sizeof(FILETIME))) {
		//////////////////////////
		tFileEntry.nsake_bin = tEmptyEntry.nsake_bin;
		OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		OverLapped.Offset = lEmptyOffset;
		if(!WriteFile(m_hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
			return INVALID_INDEX_VALUE;
		//////////////////////////
		tEmptyEntry.nsake_bin = RebuildEntry(szFileName, &db, lEmptyIndex);
		tEmptyEntry.parent = lParentIndex;
		tEmptyEntry.sibling = lSiblingIndex;

		if(INVALID_INDEX_VALUE == EntryUtility::WriteNode(m_hFilesRiver, &tEmptyEntry, lEmptyIndex))
			return INVALID_INDEX_VALUE;
		//////////////////////////
	}
	///////////////////////////////////////////////////////////////////////
	return lEmptyIndex;
}

DWORD CFilesRiver::ExistsEntry(DWORD *lNextIndex, const TCHAR *szFileName, DWORD lSiblingIndex) {
	struct FileEntry tFileEntry;
	DWORD lFileIndex, lPrevIndex = INVALID_INDEX_VALUE;

	lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lSiblingIndex);
	while (_tcscmp(tFileEntry.szFileName, szFileName)) {
		lPrevIndex = lFileIndex;
		lFileIndex = EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, tFileEntry.sibling);	
	}
	if(INVALID_INDEX_VALUE != lPrevIndex)
		EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_SIBLING, tFileEntry.sibling);
	*lNextIndex = tFileEntry.sibling;

	return lFileIndex;
}

DWORD CFilesRiver::MoveEntry(DWORD lFileIndex, DWORD lParentIndex, DWORD lSiblingIndex) {
	struct FileEntry tFileEntry;
	//////////////////////////////////////////////
	if(INVALID_INDEX_VALUE == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileIndex))
		return -1;
	tFileEntry.parent = lParentIndex;
	tFileEntry.sibling = lSiblingIndex;
	//////////////////////////////////////////////
	if(INVALID_INDEX_VALUE == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lFileIndex))
		return -1;

	return 0;
}

static DWORD RebuildEntry(HANDLE hFilesRiver, TreeDB *dbp, DWORD lSiblingIndex) {
	struct FileEntry tFileEntry;
	DWORD key_len, value;
	///////////////////////
	DWORD lEmptyIndex = lSiblingIndex;
	while(INVALID_INDEX_VALUE != EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lEmptyIndex)) {
		/////////////////////////////////////////////////////////////////
		// retrieve a record
		key_len = _tcslen(tFileEntry.szFileName);
		if (0 < dbp->get((const char *)tFileEntry.szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
			tFileEntry.nsake_bin = value;
		}
		// store records
		if (!dbp->set((const char *)tFileEntry.szFileName, key_len<<1, (char *)&lEmptyIndex, sizeof(DWORD))) {
			return -1;
		}
		/////////////////////////////////////////////////////////////////
		if(INVALID_INDEX_VALUE == EntryUtility::WriteNode(hFilesRiver, &tFileEntry, lEmptyIndex))
			return INVALID_INDEX_VALUE;
		/////////////////////////////////////////////////////////////////
		lEmptyIndex = tFileEntry.sibling;
	}

	return 0;
}

DWORD CFilesRiver::EntryRestore(DWORD lSiblingIndex) {
	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	struct FileEntry tFileEntry;
	DWORD lPrevIndex, lEmptyIndex;
	lPrevIndex = lEmptyIndex = lEmptyOffset / sizeof(struct FileEntry);
	while(INVALID_INDEX_VALUE != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lEmptyIndex)) {
		if(lSiblingIndex == lEmptyIndex) {
			EntryUtility::ModifyNode(m_hFilesRiver, lPrevIndex, FILE_INDEX_NSAKE, tFileEntry.nsake_bin);
			RebuildEntry(m_hFilesRiver, &db, lSiblingIndex);
			break;
		}
		lPrevIndex = lEmptyIndex;
		lEmptyIndex = tFileEntry.nsake_bin;
	}

	return 0;
}

static DWORD IndexDelete(HANDLE hFilesRiver, TreeDB *dbp, DWORD lSiblingIndex) {
	struct FileEntry tDelEntry, tFileEntry;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	DWORD key_len, value;
	///////////////////////
	DWORD lPrevIndex, lNsakeIndex, lDelIndex = lSiblingIndex;
	while(INVALID_INDEX_VALUE != EntryUtility::ReadNode(hFilesRiver, &tDelEntry, lDelIndex)) {
		/////////////////////////////////////////////////////////////////
		// retrieve a record
		key_len = _tcslen(tDelEntry.szFileName);
		if (0 < dbp->get((const char *)tDelEntry.szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
			if(value == lDelIndex) { // remove records
				if(INVALID_INDEX_VALUE == tDelEntry.nsake_bin) {
					if (!dbp->remove((const char *)tDelEntry.szFileName, key_len<<1))
						return -1;
				} else {
					if (!dbp->set((const char *)tDelEntry.szFileName, key_len<<1, (char *)&tDelEntry.nsake_bin, sizeof(DWORD)))
						return -1;				
				}
			} else {
				lPrevIndex = lNsakeIndex = value;
				while(INVALID_INDEX_VALUE != EntryUtility::ReadNode(hFilesRiver, &tFileEntry, lNsakeIndex)) {
					if(lDelIndex == lNsakeIndex) {
						EntryUtility::ModifyNode(hFilesRiver, lNsakeIndex, FILE_INDEX_NSAKE, tFileEntry.nsake_bin);
						break;
					}
					/////////////////////////////////////////
					lPrevIndex = lNsakeIndex;
					lNsakeIndex = tFileEntry.nsake_bin;
				}
			}
		}
		/////////////////////////////////////////////////////////////////
		lDelIndex = tDelEntry.sibling;
	}

	return 0;
}

DWORD CFilesRiver::DelFolderEntry(DWORD lSiblingIndex) {
	struct FileEntry tFileEntry;
	DWORD lEmptyIndex;	
	DWORD rlen, wlen;

	if(IndexDelete(m_hFilesRiver, &db, lSiblingIndex)) return -1;
	//////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError())
		return -1;
	if(!ReadFile(m_hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}

	lEmptyIndex = tFileEntry.nsake_bin;
	tFileEntry.nsake_bin = lSiblingIndex;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lEmptyOffset;
	if(!WriteFile(m_hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
		return -1;
	//////////////////////////////////////////////
	EntryUtility::ModifyNode(m_hFilesRiver, lSiblingIndex, FILE_INDEX_NSAKE, lEmptyIndex);

	return 0;
}

struct CmpDupli {
	bool initial;
	unsigned char dup_csum[DUPLICATE_LENGTH];
	unsigned long file_size;
};

#define DUP_FAILED		0
#define DUP_SUCCESS		0x00000001
#define DUP_MODIFY		0x00000002
static DWORD FileDuplicate(struct FileEntry *pFileEntry, const TCHAR *szFilePath, struct CmpDupli *pCmpDupli) {
	wchar_t szParentName[MAX_PATH], szDupName[MAX_PATH];
	DWORD dwIsDuplicate = DUP_FAILED;
	//////////////////////////////////////////////////////////////////
	objRiverFolder.FolderNameIndex(szParentName, pFileEntry->parent);
	_tcscpy_s(NO_ROOT(szDupName), MAX_PATH-ROOT_LENGTH, szParentName);
	PATH_ROOT(szDupName, szFilePath)
	if(_T('\0') != *(szDupName+0x03)) _tcscat_s(szDupName, MAX_PATH, _T("\\"));
	_tcscat_s(szDupName, MAX_PATH, pFileEntry->szFileName);
	//////////////////////////////////////////////////////////////////
	FILETIME lastwrite;
	DWORD dwFileSize;
	CommonUtility::FileSizeTimeExt(&dwFileSize, &lastwrite, szDupName);
	if(_tcscmp(szFilePath, szDupName) && (pCmpDupli->file_size==dwFileSize)) {
		if(memcmp(&pFileEntry->ftLastWriteTime, &lastwrite, sizeof(FILETIME))) {
			CommonUtility::file_csum(pFileEntry->dup_csum, szDupName, 0);
			pFileEntry->ftLastWriteTime = lastwrite;
			dwIsDuplicate |= DUP_MODIFY;
		}
		//////////////////////////////////
		if(!pCmpDupli->initial) {
			CommonUtility::file_csum(pCmpDupli->dup_csum, szFilePath, 0);
			pCmpDupli->initial = true;
		}
		if(!memcmp(pFileEntry->dup_csum, pCmpDupli->dup_csum, DIGEST_LENGTH)) dwIsDuplicate |= DUP_SUCCESS;
	}
	//////////////////////////////////////////////////////////////////
	return dwIsDuplicate;
}

DWORD CFilesRiver::FindNsake(const TCHAR *szFileName, const TCHAR *szFilePath) {
	struct FileEntry tFileEntry;
	DWORD value;	
	DWORD dwParentIndex = INVALID_INDEX_VALUE;
	struct CmpDupli tCmpDupli = {false, 0, 0};
	////////////////////////////////////////////
	DWORD key_len = _tcslen(szFileName);
	if(0 < db.get((const char *)szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
		tCmpDupli.file_size = CommonUtility::FileSize(szFilePath);
		while (INVALID_INDEX_VALUE != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, value)) {
			DWORD dwDuplicate = FileDuplicate(&tFileEntry, szFilePath, &tCmpDupli);
			if(DUP_MODIFY & dwDuplicate) EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, value);
			if(DUP_SUCCESS & dwDuplicate) {
				dwParentIndex = tFileEntry.parent;
				break;
			}
			value = tFileEntry.nsake_bin;
		}
	}
	////////////////////////////////////////////
	return dwParentIndex;
}

DWORD CFilesRiver::FindRecycled(const TCHAR *szFileName, FILETIME *ftCreationTime) {
	struct FileEntry tFileEntry;
	DWORD rlen;
	DWORD dwParentIndex = INVALID_INDEX_VALUE;

	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;

	if(!ReadFile(m_hFilesRiver, &tFileEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	//////////////////////////////////////////////////////////////////////////////
	DWORD lFileIndex = tFileEntry.nsake_bin;
	if(INVALID_INDEX_VALUE != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lFileIndex)) {
		if(!memcmp(&tFileEntry.ftCreationTime, ftCreationTime, sizeof(FILETIME)) && 
			!_tcscmp(tFileEntry.szFileName, szFileName)) {
			dwParentIndex = tFileEntry.parent;
		}
	}

	return dwParentIndex;
}
