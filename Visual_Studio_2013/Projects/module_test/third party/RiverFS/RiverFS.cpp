#include "StdAfx.h"

#include "CommonUtility.h"
#include "CreatRiver.h"
#include "../../clientcom/utility/ulog.h"

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

//#define RIVER_OPERATE_COPY	0x0001
//#define RIVER_OPERATE_MOVE	0x0002
//#define RIVER_OPERATE_ADD	0x0004
DWORD RiverFS::FolderIdenti(TCHAR *szExistsPath, const TCHAR *szFolderPath, FILETIME *ftCreationTime) {
	DWORD dwIdenValue = RIVER_OPERATE_ADD;

	TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
	if(fileName) {
		if(objRiverFolder.FindRecycled(NO_ROOT(szExistsPath), ++fileName, ftCreationTime)) {
			dwIdenValue = RIVER_OPERATE_MOVE;
			PATH_ROOT(szExistsPath, szFolderPath)
		} else if(objRiverFolder.FindNsake(NO_ROOT(szExistsPath), fileName, ftCreationTime)) {
			dwIdenValue = RIVER_OPERATE_COPY;
			PATH_ROOT(szExistsPath, szFolderPath)
		}
	}

	return dwIdenValue;
}

DWORD RiverFS::ValidFolderDup(const TCHAR *szFolderPath, FILETIME *ftCreationTime) {
	TCHAR szExistsPath[MAX_PATH];
	DWORD dwIsDupli = 0;

	TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
	if(fileName) {
		if(objRiverFolder.FindNsake(NO_ROOT(szExistsPath), fileName, ftCreationTime)) {
			if(!_tcscmp(NO_ROOT(szExistsPath), NO_ROOT(szFolderPath))) dwIsDupli = 1;
logger(_T("c:\\river.log"), _T("dir valid:%s %s\r\n"), szExistsPath, szFolderPath);
		}
	}

	return dwIsDupli;
}

DWORD RiverFS::FolderAppend(const TCHAR *szFolderPath, FILETIME *ftCreationTime) {
	DWORD lParentIndex, retValue = -1;

	TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
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

	TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
	if(fileName && fileName!=szFolderPath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lParentIndex = objRiverFolder.FolderIndexName(NO_ROOT(szFolderPath));	
		fileName[0] = _T('\\');
		retValue = objRiverFolder.EntryRestore(NO_ROOT(szFolderPath), ftCreationTime, lParentIndex);
	} else retValue = objRiverFolder.EntryRestore(NO_ROOT(szFolderPath), ftCreationTime, ROOT_INDEX_VALUE);

	return retValue;
}

DWORD RiverFS::FolderMoved(const TCHAR *szExistsPath, const TCHAR *szMovePath) {
	DWORD lParentIndex, retValue = -1;

	TCHAR *fileName = _tcsrchr((TCHAR *)szMovePath, _T('\\'));
	if(fileName && fileName!=szMovePath+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lParentIndex = objRiverFolder.FolderIndexName(NO_ROOT(szMovePath));	
		fileName[0] = _T('\\');
		retValue = objRiverFolder.MoveEntry(NO_ROOT(szExistsPath), NO_ROOT(szMovePath), lParentIndex);
	} else retValue = objRiverFolder.MoveEntry(NO_ROOT(szExistsPath), NO_ROOT(szMovePath), ROOT_INDEX_VALUE);

	return retValue;
}

DWORD RiverFS::FindIdenti(TCHAR *szExistsFile, struct FileID *pFileID) {
	DWORD dwIdenValue = RIVER_OPERATE_ADD;
	TCHAR szParentName[MAX_PATH];

	TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
	if(fileName) {
		///////////////////////////////////////////////////////////////////////////////
		DWORD dwParentIndex = objFilesRiver.FindRecycled(++fileName, pFileID->sFileCsum);
		if(INVALID_INDEX_VALUE != dwParentIndex) {
			objRiverFolder.FolderNameIndex(szParentName, dwParentIndex);
			PATH_ROOT(szExistsFile, pFileID->szFileName)
			_tcscpy_s(NO_ROOT(szExistsFile), MAX_PATH-ROOT_LENGTH, szParentName);
			if(_T('\0') != *(szExistsFile+0x03)) _tcscat_s(szExistsFile, MAX_PATH, _T("\\"));
			_tcscat_s(szExistsFile, MAX_PATH, fileName);
			dwIdenValue = RIVER_OPERATE_MOVE;
		///////////////////////////////////////////////////////////////////////////////
		} else {
			dwParentIndex = objFilesRiver.FindNsake(fileName, pFileID);
			if(INVALID_INDEX_VALUE != dwParentIndex) {
				objRiverFolder.FolderNameIndex(szParentName, dwParentIndex);
				PATH_ROOT(szExistsFile, pFileID->szFileName)
				_tcscpy_s(NO_ROOT(szExistsFile), MAX_PATH-ROOT_LENGTH, szParentName);
				if(_T('\0') != *(szExistsFile+0x03)) _tcscat_s(szExistsFile, MAX_PATH, _T("\\"));
				_tcscat_s(szExistsFile, MAX_PATH, fileName);
				dwIdenValue = RIVER_OPERATE_COPY;
			}
		}
		///////////////////////////////////////////////////////////////////////////////
	}

	return dwIdenValue;
}

DWORD RiverFS::ValidFileDup(struct FileID *pFileID) {
	DWORD dwIsDupli = 0;

	TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
	if(fileName) {
		if(INVALID_INDEX_VALUE != objFilesRiver.FindNsake(fileName, pFileID)) dwIsDupli = 1;
logger(_T("c:\\river.log"), _T("file valid:%s %s\r\n"), fileName, pFileID->szFileName);
	}

	return dwIsDupli;
}

DWORD RiverFS::FileAppend(struct FileID *pFileID) {
	DWORD lSiblingIndex, lParentIndex, lFileIndex = -1;

	TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
	if(fileName && fileName!=pFileID->szFileName+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(pFileID->szFileName));	
		fileName[0] = _T('\\');
		lFileIndex = objFilesRiver.InsEntry(++fileName, pFileID->sFileCsum, lParentIndex, lSiblingIndex);
		objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
	} else {
		lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDEX_VALUE);
		lFileIndex = objFilesRiver.InsEntry(++fileName, pFileID->sFileCsum, ROOT_INDEX_VALUE, lSiblingIndex);
		objRiverFolder.SetChildIndex(ROOT_INDEX_VALUE, lFileIndex);
	}

	return lFileIndex;
}

DWORD RiverFS::DeleteFile(const TCHAR *szFilePath) {
	DWORD lNextIndex, lSiblingIndex, lParentIndex, lFileIndex = -1;

	TCHAR *fileName = _tcsrchr((TCHAR *)szFilePath, _T('\\'));
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

DWORD RiverFS::FileRestore(struct FileID *pFileID) {
	DWORD lSiblingIndex, lParentIndex, lFileIndex = -1;

	TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
	if(fileName && fileName!=pFileID->szFileName+ROOT_LENGTH) {
		fileName[0] = _T('\0');
		lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(pFileID->szFileName));
		fileName[0] = _T('\\');
		lFileIndex = objFilesRiver.EntryRestore(++fileName, pFileID->sFileCsum, lParentIndex, lSiblingIndex);
		objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
	} else {
		lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDEX_VALUE);
		lFileIndex = objFilesRiver.EntryRestore(++fileName, pFileID->sFileCsum, ROOT_INDEX_VALUE, lSiblingIndex);
		objRiverFolder.SetChildIndex(ROOT_INDEX_VALUE, lFileIndex);	
	}

	return lFileIndex;
}

DWORD RiverFS::FileMoved(const TCHAR *szExistsPath, const TCHAR *szMovePath) {
	DWORD lNextIndex, lSiblingIndex, lParentIndex, lFileIndex = -1;

	TCHAR *fileName = _tcsrchr((TCHAR *)szExistsPath, _T('\\'));
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

VOID RiverFS::VoidRecycled() {
	objRiverFolder.VoidRecycled();
	objFilesRiver.VoidRecycled();
}