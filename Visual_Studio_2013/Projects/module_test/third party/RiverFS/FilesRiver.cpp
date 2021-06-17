#include "StdAfx.h"
#include "CommonUtility.h"

#include "RiverFolder.h"
#include "FilesRiver.h"
#include "../../clientcom/utility/ulog.h"

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

DWORD CFilesRiver::InsEntry(const TCHAR *szFileName, unsigned char *sFileCsum, DWORD lParentIndex, DWORD lSiblingIndex) {
	struct FileEntry tFileEntry;

	DWORD lFileIndex = EntryUtility::FindEmptyEntry(m_hFilesRiver);
	/////////////////////////////////////////////////////
	INIT_FILE_ENTRY(tFileEntry)
	_tcscpy_s(tFileEntry.szFileName, szFileName);
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
	memcpy(tFileEntry.sFileCsum, sFileCsum, FILESUM_LENGTH);
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

DWORD CFilesRiver::EntryRestore(const TCHAR *szFileName, unsigned char *sFileCsum, DWORD lParentIndex, DWORD lSiblingIndex) {
	struct FileEntry tFillEntry, tFileEntry;
	DWORD lEmptyIndex;	
	DWORD rlen, wlen;
	///////////////////////////////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError())
		return INVALID_INDEX_VALUE;
	if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	//////////////////////////
	lEmptyIndex = tFillEntry.nsake_bin;
	if(INVALID_INDEX_VALUE == EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, lEmptyIndex))
		return INVALID_INDEX_VALUE;
	///////////////////////////////////////////////////////////////////////
	if(!_tcscmp(tFileEntry.szFileName, szFileName) && !memcmp(&tFileEntry.sFileCsum, sFileCsum, FILESUM_LENGTH)) {
		//////////////////////////
		tFillEntry.nsake_bin = tFileEntry.nsake_bin;
		OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
		OverLapped.Offset = lEmptyOffset;
		if(!WriteFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
			return INVALID_INDEX_VALUE;
		//////////////////////////
		tFileEntry.nsake_bin = RebuildEntry(szFileName, &db, lEmptyIndex);
		tFileEntry.parent = lParentIndex;
		tFileEntry.sibling = lSiblingIndex;

		if(INVALID_INDEX_VALUE == EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, lEmptyIndex))
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
						EntryUtility::ModifyNode(hFilesRiver, lPrevIndex, FILE_INDEX_NSAKE, tFileEntry.nsake_bin);
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
	struct FileEntry tFillEntry;
	DWORD lEmptyIndex;	
	DWORD rlen, wlen;

	if(INVALID_INDEX_VALUE == lSiblingIndex) return -1;
	//////////////////////////////////////////////
	if(IndexDelete(m_hFilesRiver, &db, lSiblingIndex)) return -1;
	//////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError())
		return -1;
	if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}

	lEmptyIndex = tFillEntry.nsake_bin;
	tFillEntry.nsake_bin = lSiblingIndex;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	OverLapped.Offset = lEmptyOffset;
	if(!WriteFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &wlen, &OverLapped))
		return -1;
	//////////////////////////////////////////////
	EntryUtility::ModifyNode(m_hFilesRiver, lSiblingIndex, FILE_INDEX_NSAKE, lEmptyIndex);

	return 0;
}

#define DUPLI_FAILED		0
#define DUPLI_SUCCESS		0x00000001
#define DUPLI_MODIFY		0x00000002

static DWORD IsFileDuplicate(struct FileEntry *pFileEntry, struct FileID *pFileID) {
	wchar_t szParentName[MAX_PATH], szDupName[MAX_PATH];
	DWORD dwIsDupli = DUPLI_FAILED;
	//////////////////////////////////////////////////////////////////
	objRiverFolder.FolderNameIndex(szParentName, pFileEntry->parent);
	PATH_ROOT(szDupName, pFileID->szFileName)
	_tcscpy_s(NO_ROOT(szDupName), MAX_PATH-ROOT_LENGTH, szParentName);
	if(_T('\0') != *(szDupName+0x03)) _tcscat_s(szDupName, MAX_PATH, _T("\\"));
	_tcscat_s(szDupName, MAX_PATH, pFileEntry->szFileName);
	//////////////////////////////////////////////////////////////////
	FILETIME lastwrite;
	DWORD dwFileSize;
	CommonUtility::FileSizeTimeExt(&dwFileSize, &lastwrite, szDupName);
	if(pFileID->lFileSize == dwFileSize) {
		if(memcmp(&pFileEntry->ftLastWriteTime, &lastwrite, sizeof(FILETIME))) {
				CommonUtility::file_csum(pFileEntry->sFileCsum, szDupName);
				pFileEntry->ftLastWriteTime = lastwrite;
				dwIsDupli |= DUPLI_MODIFY;
		}
		//////////////////////////////////
		if(!memcmp(pFileEntry->sFileCsum, pFileID->sFileCsum, FILESUM_LENGTH)) dwIsDupli |= DUPLI_SUCCESS;
	}
	//////////////////////////////////////////////////////////////////
	return dwIsDupli;
}

char *get_md5_text(unsigned char *md5sum) {
	static char sum_text[33];
	for (int index=0; index<DIGEST_LENGTH; index++)
		sprintf(sum_text+index*2, "%02x", md5sum[index]);
	sum_text[32] = '\0';
	return sum_text;
}

DWORD CFilesRiver::FindNsake(const TCHAR *szFileName, struct FileID *pFileID) {
	struct FileEntry tFileEntry;
	DWORD value;	
	DWORD dwParentIndex = INVALID_INDEX_VALUE;
	////////////////////////////////////////////
	DWORD key_len = _tcslen(szFileName);
	if(0 < db.get((const char *)szFileName, key_len<<1, (char *)&value, sizeof(DWORD))) {
logger(_T("c:\\river.log"), _T("db get value: %d\r\n"), value);
		while (INVALID_INDEX_VALUE != EntryUtility::ReadNode(m_hFilesRiver, &tFileEntry, value)) {
			DWORD dwIsDupli = IsFileDuplicate(&tFileEntry, pFileID);
			if(DUPLI_MODIFY & dwIsDupli) EntryUtility::WriteNode(m_hFilesRiver, &tFileEntry, value);
			if(DUPLI_SUCCESS & dwIsDupli) {
				dwParentIndex = tFileEntry.parent;
				break;
			}
			value = tFileEntry.nsake_bin;
logger(_T("c:\\river.log"), _T("file csum :%s valid csum: %s\r\n"), get_md5_text(tFileEntry.sFileCsum), get_md5_text(pFileID->sFileCsum));
		}
	}
	////////////////////////////////////////////
	return dwParentIndex;
}

//
TCHAR *get_md5sum_text(unsigned char *md5sum) {
	static TCHAR sum_text[33];

	for (int index=0; index<16; index++)
		_stprintf_s(sum_text+index*2, 33, _T("%02x"), md5sum[index]);
	sum_text[32] = _T('\0');

	return sum_text;
}

DWORD CFilesRiver::FindRecycled(const TCHAR *szFileName, unsigned char *sFileCsum) {
	struct FileEntry tFillEntry;
	DWORD rlen;
	DWORD dwParentIndex = INVALID_INDEX_VALUE;

	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError()) 
		return INVALID_INDEX_VALUE;
	if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return INVALID_INDEX_VALUE;
	}
	//////////////////////////////////////////////////////////////////////////////
	if(INVALID_INDEX_VALUE != tFillEntry.nsake_bin) {
		if(INVALID_INDEX_VALUE != EntryUtility::ReadNode(m_hFilesRiver, &tFillEntry, tFillEntry.nsake_bin)) {
			if(!memcmp(&tFillEntry.sFileCsum, sFileCsum, FILESUM_LENGTH) && !_tcscmp(tFillEntry.szFileName, szFileName)) {
				dwParentIndex = tFillEntry.parent;
			}
		}
	}

	return dwParentIndex;
}

DWORD CFilesRiver::VoidRecycled() {
	struct FileEntry tFillEntry;	
	DWORD rlen;
	//////////////////////////////////////////////
	DWORD lEmptyOffset = SetFilePointer(m_hFilesRiver, -(LONG)sizeof(struct FileEntry), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==lEmptyOffset && NO_ERROR!=GetLastError())
		return -1;
	if(!ReadFile(m_hFilesRiver, &tFillEntry, sizeof(struct FileEntry), &rlen, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}
	//////////////////////////////////////////////
	if(INVALID_INDEX_VALUE != tFillEntry.nsake_bin) 
		EntryUtility::VoidNode(m_hFilesRiver, tFillEntry.nsake_bin);

	return 0;
}