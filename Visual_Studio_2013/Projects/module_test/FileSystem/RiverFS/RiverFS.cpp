#include "StdAfx.h"

// #include "FilesVec.h"
#include "CommonUtility.h"
#include "CreatRiver.h"
#include "FilesRiver.h"

#include "RiverFS.h"

DWORD RiverFS::Initialize(TCHAR *szLocation, const TCHAR *szDriveRoot) {
	if(objCreatRiver.BuildRiverEnvirn(szLocation, szDriveRoot))
		return -1;
	if(objFilesRiver.Initialize(szLocation)) return -1;
	if(objRiverFolder.Initialize(szLocation)) return -1;
	return 0;
}

DWORD RiverFS::Finalize() {
	if(objRiverFolder.Finalize()) return -1;
	if(objFilesRiver.Finalize()) return -1;
	return 0;
}

//#define RIVER_OPERATION_COPY	0x0001
//#define RIVER_OPERATION_MOVE	0x0002
//#define RIVER_OPERATION_ADD	0x0004
DWORD RiverFS::FolderIdenti(TCHAR *szExistsPath, const TCHAR *szFolderPath, FILETIME *ftCreationTime) {
	DWORD dwOperation = RIVER_OPERATION_ADD;

	wchar_t *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
	if(fileName) {
		if(objRiverFolder.FindRecycled(NO_ROOT(szExistsPath), ++fileName, ftCreationTime)) {
			dwOperation = RIVER_OPERATION_MOVE;
			PATH_ROOT(szExistsPath, szFolderPath)
		} else if(objRiverFolder.FindNsake(NO_ROOT(szExistsPath), fileName, ftCreationTime)) {
			dwOperation = RIVER_OPERATION_COPY;
			PATH_ROOT(szExistsPath, szFolderPath)
		}
	}

	return dwOperation;
}

DWORD RiverFS::FolderAppend(const TCHAR *szFolderPath, FILETIME *ftCreationTime) {
	DWORD lParentIndex, retValue = -1;

	wchar_t *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
	if(fileName && fileName!=szFolderPath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lParentIndex = objRiverFolder.FolderIndexName(NO_ROOT(szFolderPath));	
		fileName[0] = _T('\\');
		retValue = objRiverFolder.InsEntry(NO_ROOT(szFolderPath), ftCreationTime, lParentIndex);
	} else retValue = objRiverFolder.InsEntry(NO_ROOT(szFolderPath), ftCreationTime, ROOT_INDEX_VALUE);

	return retValue;
}

DWORD RiverFS::FolderRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime) {
	DWORD lParentIndex, retValue = -1;

	wchar_t *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
	if(fileName && fileName!=szFolderPath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lParentIndex = objRiverFolder.FolderIndexName(NO_ROOT(szFolderPath));	
		fileName[0] = _T('\\');
		retValue = objRiverFolder.EntryRestore(NO_ROOT(szFolderPath), ftCreationTime, lParentIndex);
	} else retValue = objRiverFolder.EntryRestore(NO_ROOT(szFolderPath), ftCreationTime, ROOT_INDEX_VALUE);

	return retValue;
}

DWORD RiverFS::FolderMove(const TCHAR *szExistsPath, const TCHAR *szMovePath) {
	DWORD lParentIndex, retValue = -1;

	wchar_t *fileName = _tcsrchr((TCHAR *)szMovePath, _T('\\'));
	if(fileName && fileName!=szMovePath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lParentIndex = objRiverFolder.FolderIndexName(NO_ROOT(szMovePath));	
		fileName[0] = _T('\\');
		retValue = objRiverFolder.MoveEntry(NO_ROOT(szExistsPath), NO_ROOT(szMovePath), lParentIndex);
	} else retValue = objRiverFolder.MoveEntry(NO_ROOT(szExistsPath), NO_ROOT(szMovePath), ROOT_INDEX_VALUE);

	return retValue;
}

DWORD RiverFS::FindIdenti(TCHAR *szExistsPath, const TCHAR *szFilePath, FILETIME *ftCreationTime) {
	DWORD dwOperation = RIVER_OPERATION_ADD;
	wchar_t szParentName[MAX_PATH];

	wchar_t *fileName = _tcsrchr((TCHAR *)szFilePath, _T('\\'));
	if(fileName) {
		///////////////////////////////////////////////////////////////////////////////
		DWORD dwParentIndex = objFilesRiver.FindRecycled(++fileName, ftCreationTime);
		if(INVALID_INDEX_VALUE != dwParentIndex) {
			objRiverFolder.FolderNameIndex(szParentName, dwParentIndex);		
			_tcscpy_s(NO_ROOT(szExistsPath), MAX_PATH-ROOT_LENGTH, szParentName);
			PATH_ROOT(szExistsPath, szFilePath)
			if(_T('\0') != *(szExistsPath+0x03)) _tcscat_s(szExistsPath, MAX_PATH, _T("\\"));
			_tcscat_s(szExistsPath, MAX_PATH, fileName);
			dwOperation = RIVER_OPERATION_MOVE;
		///////////////////////////////////////////////////////////////////////////////
		} else {
			dwParentIndex = objFilesRiver.FindNsake(fileName, szFilePath);
			if(INVALID_INDEX_VALUE != dwParentIndex) {
				objRiverFolder.FolderNameIndex(szParentName, dwParentIndex);
				_tcscpy_s(NO_ROOT(szExistsPath), MAX_PATH-ROOT_LENGTH, szParentName);
				PATH_ROOT(szExistsPath, szFilePath)
				if(_T('\0') != *(szExistsPath+0x03)) _tcscat_s(szExistsPath, MAX_PATH, _T("\\"));
				_tcscat_s(szExistsPath, MAX_PATH, fileName);
				dwOperation = RIVER_OPERATION_COPY;
			}
		}
		///////////////////////////////////////////////////////////////////////////////
	}

	return dwOperation;
}

DWORD RiverFS::FileAppend(const TCHAR *szFilePath, FILETIME *ftCreationTime) {
	DWORD lSiblingIndex, lParentIndex, lFileIndex = -1;

	wchar_t *fileName = _tcsrchr((TCHAR *)szFilePath, _T('\\'));
	if(fileName && fileName!=szFilePath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFilePath));	
		fileName[0] = _T('\\');
		lFileIndex = objFilesRiver.InsEntry(++fileName, ftCreationTime, lParentIndex, lSiblingIndex);
		objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
	} else {
		lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDEX_VALUE);
		lFileIndex = objFilesRiver.InsEntry(++fileName, ftCreationTime, ROOT_INDEX_VALUE, lSiblingIndex);
		objRiverFolder.SetChildIndex(ROOT_INDEX_VALUE, lFileIndex);
	}

	return lFileIndex;
}

DWORD RiverFS::DeleteFile(const TCHAR *szFilePath) {
	DWORD lNextIndex, lSiblingIndex, lParentIndex, lFileIndex = -1;

	wchar_t *fileName = _tcsrchr((TCHAR *)szFilePath, _T('\\'));
	if(fileName && fileName!=szFilePath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFilePath));
		fileName[0] = _T('\\');
		lFileIndex = objFilesRiver.DelFileEntry(&lNextIndex, ++fileName, lSiblingIndex);
		if(lSiblingIndex == lFileIndex)
			objRiverFolder.SetChildIndex(lParentIndex, lNextIndex);	
	} else {
		lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDEX_VALUE);
		lFileIndex = objFilesRiver.DelFileEntry(&lNextIndex, ++fileName, lSiblingIndex);
		if(lSiblingIndex == lFileIndex)
			objRiverFolder.SetChildIndex(ROOT_INDEX_VALUE, lNextIndex);
	}

	return lFileIndex;
}

DWORD RiverFS::FileRestore(const TCHAR *szFilePath, FILETIME *ftCreationTime) {
	DWORD lSiblingIndex, lParentIndex, lFileIndex = -1;

	wchar_t *fileName = _tcsrchr((TCHAR *)szFilePath, _T('\\'));
	if(fileName && fileName!=szFilePath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFilePath));
		fileName[0] = _T('\\');
		lFileIndex = objFilesRiver.EntryRestore(++fileName, ftCreationTime, lParentIndex, lSiblingIndex);
		objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
	} else {
		lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDEX_VALUE);
		lFileIndex = objFilesRiver.EntryRestore(++fileName, ftCreationTime, ROOT_INDEX_VALUE, lSiblingIndex);
		objRiverFolder.SetChildIndex(ROOT_INDEX_VALUE, lFileIndex);	
	}

	return lFileIndex;
}

DWORD RiverFS::FileMove(const TCHAR *szExistsPath, const TCHAR *szMovePath) {
	DWORD lNextIndex, lSiblingIndex, lParentIndex, lFileIndex = -1;

	wchar_t *fileName = _tcsrchr((TCHAR *)szExistsPath, _T('\\'));
	if(fileName && fileName!=szExistsPath+ROOT_LENGTH) {
		fileName[0] = _T('\0');	
		lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szExistsPath));
		fileName[0] = _T('\\');
		lFileIndex = objFilesRiver.ExistsEntry(&lNextIndex, ++fileName, lSiblingIndex);
		if(lSiblingIndex == lFileIndex)
			objRiverFolder.SetChildIndex(lParentIndex, lNextIndex);
	} else {
		lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDEX_VALUE);	
		lFileIndex = objFilesRiver.ExistsEntry(&lNextIndex, ++fileName, lSiblingIndex);
		if(lSiblingIndex == lFileIndex)
			objRiverFolder.SetChildIndex(ROOT_INDEX_VALUE, lNextIndex);
	}
	//////////////////////////////////////////////////////////////////////////////////////
	fileName = _tcsrchr((TCHAR *)szMovePath, _T('\\'));
	if(fileName && fileName!=szMovePath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szMovePath));
		fileName[0] = _T('\\');
		objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);	
		if(INVALID_INDEX_VALUE != lSiblingIndex)
			lFileIndex = objFilesRiver.MoveEntry(lFileIndex, lParentIndex, lSiblingIndex);
	} else {
		lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDEX_VALUE);
		objRiverFolder.SetChildIndex(ROOT_INDEX_VALUE, lFileIndex);	
		if(INVALID_INDEX_VALUE != lSiblingIndex)
			lFileIndex = objFilesRiver.MoveEntry(lFileIndex, ROOT_INDEX_VALUE, lSiblingIndex);
	}

	return lFileIndex;
}