#include "StdAfx.h"
#include "Anchor.h"

#include "StringUtility.h"
#include "third_party.h"
#include "FileUtility.h"
//#include "ParseConfig.h"

CAnchor::CAnchor(void):
m_hAnchorIndex(INVALID_HANDLE_VALUE)
,m_hAnchorData(INVALID_HANDLE_VALUE)
,m_hLockedFiles(INVALID_HANDLE_VALUE)
{
	memset(m_szAnchorIndex, '\0', MAX_PATH);
	memset(m_szAnchorData, '\0', MAX_PATH);
	memset(m_szLockedFiles, '\0', MAX_PATH);
}

CAnchor::~CAnchor(void) {
}

CAnchor objAnchor;

DWORD CAnchor::Initialize(TCHAR *szAnchorIndex, TCHAR *szAnchorData, TCHAR *szLocation) {
	nstriutility::get_name(m_szLockedFiles, szLocation, LOCKED_FILES_DEFAULT);
	m_hLockedFiles = INVALID_HANDLE_VALUE;

	nstriutility::get_name(m_szAnchorIndex, szLocation, szAnchorIndex);
	m_hAnchorIndex = CreateFile( m_szAnchorIndex, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hAnchorIndex == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	} 

	nstriutility::get_name(m_szAnchorData, szLocation, szAnchorData);
	m_hAnchorData = CreateFile( m_szAnchorData, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hAnchorData == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}

	return 0;
}

DWORD CAnchor::Finalize() {
	if(m_hAnchorIndex != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hAnchorIndex );
		m_hAnchorIndex = INVALID_HANDLE_VALUE;
	}
	if(m_hAnchorData != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hAnchorData );
		m_hAnchorData = INVALID_HANDLE_VALUE;
	}
	if(m_hLockedFiles != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hLockedFiles );
		m_hLockedFiles = INVALID_HANDLE_VALUE;
	}

	return 0;
}

DWORD CAnchor::LoadLastFiles(FilesVec *pFilesCache, DWORD *uipLastAnchor) {
	struct IndexValue tIndexValue;

	if(!pFilesCache || !uipLastAnchor) return -1;

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	if(LoadLastIndexValue(&tIndexValue)) return -1;

	if(LoadFilesByIndexValue(pFilesCache, &tIndexValue)) 
		return -1;

	memcpy(&m_tLastIndex, &tIndexValue, sizeof(struct IndexValue));
	*uipLastAnchor = tIndexValue.AnchorNumber;

	return 0; //succ
}

DWORD CAnchor::SaveFilesStructVec(DWORD dwAnchor) {
	return 0; //succ
}


DWORD CAnchor::FlushFilesVecFile(FilesVec *pFilesCache) {
	FilesVec::iterator iter;
	DWORD result;
	DWORD dwWritenSize = 0;

	result = SetFilePointer(m_hAnchorData, m_tLastIndex.FileOffset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	for(iter=pFilesCache->begin(); iter!=pFilesCache->end(); iter++) {
		if(!WriteFile(m_hAnchorData, *iter, sizeof(struct file_attrib), &dwWritenSize, NULL))
			return -1;
	}
	result = SetEndOfFile(m_hAnchorData);
	if(FALSE==result && NO_ERROR!=GetLastError()) 
		return -1;
	FlushFileBuffers(m_hAnchorData);

	return 0; //succ
}

DWORD  CAnchor::LoadLastIndexValue(OUT struct IndexValue *pLastIndex) {
	struct IndexValue tIndexValue;
	DWORD dwFileSize, dwFileHigh;
	DWORD dwReadSize = 0;

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	dwFileSize = GetFileSize(m_hAnchorIndex, &dwFileHigh);
	if(INVALID_FILE_SIZE==dwFileSize  && NO_ERROR!=GetLastError()) 
		return FALSE;
	if(0x00 == dwFileSize) {
		memset(pLastIndex, '\0', sizeof(struct IndexValue));
		return 0;
	}

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	DWORD result = SetFilePointer(m_hAnchorIndex, -(LONG)sizeof(struct IndexValue), NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	if(!ReadFile(m_hAnchorIndex, &tIndexValue, sizeof(struct IndexValue), &dwReadSize, NULL)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}
	FlushFileBuffers(m_hAnchorIndex);

	memcpy(pLastIndex, &tIndexValue, sizeof(struct IndexValue));
	return 0;
}

DWORD CAnchor::LoadLastAnchor(OUT DWORD *uipAnchor) {
	struct IndexValue tIndexValue;

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	if(LoadLastIndexValue(&tIndexValue))
		return -1;
	*uipAnchor = tIndexValue.AnchorNumber;

	return 0;
}


DWORD CAnchor::AddNewAnchor(DWORD dwNewAnchor) {
	struct IndexValue tIndexValue;
	DWORD dwFileSize, dwFileHigh;
	DWORD dwWritenSize = 0;

	memset(&tIndexValue, '\0', sizeof(struct IndexValue));

	FlushFileBuffers(m_hAnchorData);
	dwFileSize = GetFileSize(m_hAnchorData, &dwFileHigh);
	if(INVALID_FILE_SIZE==dwFileSize  && NO_ERROR!=GetLastError()) 
		return -1;
	memset(&tIndexValue, '\0', sizeof(struct IndexValue));
	tIndexValue.AnchorNumber = dwNewAnchor;
	tIndexValue.FileOffset = dwFileSize;

	DWORD result = SetFilePointer(m_hAnchorIndex, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	if(!WriteFile(m_hAnchorIndex, &tIndexValue, sizeof(struct IndexValue), &dwWritenSize, NULL))
		return -1;
	FlushFileBuffers(m_hAnchorIndex);

	memcpy(&m_tLastIndex, &tIndexValue, sizeof(struct IndexValue));
	return 0;
}


DWORD CAnchor::LoadFilesByIndexValue(FilesVec *pFilesVec, struct IndexValue *pIndexValue) {
	struct file_attrib tFileAttrib;
	struct file_attrib *fast;
	DWORD dwReadSize = 0;

	memset(&tFileAttrib, '\0', sizeof(struct file_attrib));
	DWORD result = SetFilePointer(m_hAnchorData, pIndexValue->FileOffset, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	do {
		if(!ReadFile(m_hAnchorData, &tFileAttrib, sizeof(struct file_attrib), &dwReadSize, NULL)) {
			if(ERROR_HANDLE_EOF != GetLastError()) return -1;
		}
		if(dwReadSize == sizeof(struct file_attrib)) {
			fast = NFilesVec::AppendNewNode(pFilesVec);
			memcpy(fast, &tFileAttrib, sizeof(struct file_attrib));
		}
	}while(dwReadSize == sizeof(struct file_attrib));

	return 0;
}

DWORD CAnchor::InsertLockedFiles(const TCHAR *szFileName, const DWORD modify_type) {
	struct file_attrib tFileAttrib;
	DWORD dwWritenSize = 0;

	memset(&tFileAttrib, '\0', sizeof(struct file_attrib));
	_tcscpy_s(tFileAttrib.file_name, MAX_PATH, szFileName);
	tFileAttrib.modify_type = modify_type;
	if(m_hLockedFiles == INVALID_HANDLE_VALUE) {
		m_hLockedFiles = CreateFile( m_szLockedFiles, 
			GENERIC_WRITE | GENERIC_READ,
			NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
			NULL, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 
			NULL);
		if( m_hLockedFiles == INVALID_HANDLE_VALUE ) {
			TRACE( _T("create file failed.\n") );
			TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
			return FALSE;
		} 
	}
	DWORD result = SetFilePointer(m_hLockedFiles, 0, NULL, FILE_END);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	if(!WriteFile(m_hLockedFiles, &tFileAttrib, sizeof(struct file_attrib), &dwWritenSize, NULL))
		return -1;
	FlushFileBuffers(m_hLockedFiles);

	return 0; //succ
}

DWORD CAnchor::LoadLockedFiles(FilesVec *pLockedFiles) {
	struct file_attrib tFileAttrib;
	struct file_attrib *fast;
	DWORD dwReadSize = 0;

	if(m_hLockedFiles == INVALID_HANDLE_VALUE) return 0x02;

	memset(&tFileAttrib, '\0', sizeof(struct file_attrib));
	DWORD result = SetFilePointer(m_hLockedFiles, 0, NULL, FILE_BEGIN);
	if(INVALID_SET_FILE_POINTER==result && NO_ERROR!=GetLastError()) 
		return -1;
	do {
		if(!ReadFile(m_hLockedFiles, &tFileAttrib, sizeof(struct file_attrib), &dwReadSize, NULL)) {
			if(ERROR_HANDLE_EOF != GetLastError()) return -1;
		}
		if(dwReadSize == sizeof(struct file_attrib)) {
			fast = NFilesVec::AppendNewNode(pLockedFiles);
			memcpy(fast, &tFileAttrib, sizeof(struct file_attrib));
		}
	}while(dwReadSize == sizeof(struct file_attrib));

	CloseHandle( m_hLockedFiles );
	m_hLockedFiles = INVALID_HANDLE_VALUE;
	DeleteFile(m_szLockedFiles);

	return 0; //succ
}

DWORD CAnchor::LoadLockedFilesTurnon(FilesVec *pLockedFiles) {
	if(m_hLockedFiles == INVALID_HANDLE_VALUE) {
		m_hLockedFiles = CreateFile( m_szLockedFiles, 
			GENERIC_WRITE | GENERIC_READ,
			NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
			NULL, 
			OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			NULL);
		if( m_hLockedFiles == INVALID_HANDLE_VALUE ) {
			TRACE( _T("create file failed.\n") );
			TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
			return FALSE;
		} 
	}
	return LoadLockedFiles(pLockedFiles);
}


DWORD CAnchor::SearchAnchor(struct IndexValue *pIndexStart, struct IndexValue *pIndexEnd, DWORD dwAnchor) {
	HANDLE hMapFile;
	DWORD dwFileSize, dwFileHigh;
	struct IndexValue *pIndexValue = NULL;
	struct IndexValue *MidPosition, *MinPosition, *MaxPosition;
	DWORD dwAnchorFound = SEARCH_NOT_FOUND;

	if(!pIndexStart || !pIndexEnd || 0>dwAnchor)
		return SEARCH_FAULT;
	memset(pIndexStart, '\0', sizeof(struct IndexValue));
	memset(pIndexEnd, '\0', sizeof(struct IndexValue));

	dwFileSize = GetFileSize(m_hAnchorIndex, &dwFileHigh);
	if(INVALID_FILE_SIZE==dwFileSize  && NO_ERROR!=GetLastError()) 
		return SEARCH_FAULT;
	if(0x00==dwFileSize || 0x00==dwAnchor) {
		memset(pIndexStart, '\0', sizeof(struct IndexValue));		
		memset(pIndexEnd, '\0', sizeof(struct IndexValue));
		return SEARCH_FOUND;
	}

	hMapFile = CreateFileMapping(m_hAnchorIndex, NULL, PAGE_READWRITE, 0, dwFileSize, NULL);
	if(hMapFile == NULL) {
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return SEARCH_FAULT;
	}
	pIndexValue = (struct IndexValue *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, dwFileSize);
	if(!pIndexValue) return SEARCH_FAULT; 

	UINT nIndexNum = dwFileSize/sizeof(struct IndexValue);
	MaxPosition = pIndexValue + nIndexNum - 1;
	memcpy(pIndexEnd, MaxPosition, sizeof(struct IndexValue));
	if (!pIndexEnd->AnchorNumber){
		memset(pIndexStart, '\0', sizeof(struct IndexValue));
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		UnmapViewOfFile(pIndexValue);
		CloseHandle( hMapFile );
		return SEARCH_FAULT;
	}

	MaxPosition = pIndexValue + nIndexNum - 1;
	MinPosition = pIndexValue;
	MidPosition = MinPosition + (nIndexNum >> 1);
	for(;;) {
		if(dwAnchor > MidPosition->AnchorNumber) {
			MinPosition = MidPosition;
			nIndexNum = (UINT)(((MaxPosition-MinPosition)/sizeof(struct IndexValue)) >> 1);
			MidPosition = MinPosition + (nIndexNum?nIndexNum:1);
		} else if(dwAnchor < MidPosition->AnchorNumber) {
			MaxPosition = MidPosition;
			nIndexNum = (UINT)(((MaxPosition-MinPosition)/sizeof(struct IndexValue)) >> 1);
			MidPosition = MinPosition + nIndexNum;
		} else if(dwAnchor == MidPosition->AnchorNumber) {
			memcpy(pIndexStart, MidPosition, sizeof(struct IndexValue));
			dwAnchorFound = SEARCH_FOUND;
			break;
		}
		if (MinPosition == MaxPosition) {
			dwAnchorFound = SEARCH_NOT_FOUND;
			break;
		}
	}

	UnmapViewOfFile(pIndexValue);
	CloseHandle( hMapFile );

	return dwAnchorFound;
}

DWORD CAnchor::LoadFilesByIndexValue(FilesVec *pFilesVec, struct IndexValue *pIndexStart, struct IndexValue *pIndexEnd) {
	HANDLE hMapFile;
	DWORD dwFileSize, dwFileHigh;
	struct file_attrib *fast;
	struct file_attrib *pDataFilesAttrib = NULL;
	struct file_attrib *pFilesAttribPos = NULL;

	if(!pFilesVec || !pIndexStart || !pIndexEnd) 
		return -1;

	dwFileSize = GetFileSize(m_hAnchorData, &dwFileHigh);
	if(INVALID_FILE_SIZE==dwFileSize  && NO_ERROR!=GetLastError()) 
		return -1;
	if(0x00==dwFileSize) return 0;

	hMapFile = CreateFileMapping(m_hAnchorData, NULL, PAGE_READWRITE, 0, dwFileSize, NULL);
	if(hMapFile == NULL) {
		TRACE( _T("create file mapping failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}
	int mlen = pIndexEnd->FileOffset - pIndexStart->FileOffset;
	pFilesAttribPos = pDataFilesAttrib = (struct file_attrib *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, pIndexStart->FileOffset, mlen);
	if(!pDataFilesAttrib) return -1; 	
	for(int index=0; index< mlen/(int)sizeof(struct file_attrib); index++){
		fast = NFilesVec::AppendNewNode(pFilesVec);
		memcpy(fast, pFilesAttribPos, sizeof(struct file_attrib));
		pFilesAttribPos ++;
	}

	UnmapViewOfFile(pDataFilesAttrib);
	CloseHandle( hMapFile );

	return 0;
}
