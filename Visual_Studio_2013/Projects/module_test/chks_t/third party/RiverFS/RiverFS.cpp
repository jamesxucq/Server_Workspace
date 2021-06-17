#include "StdAfx.h"

#include "CommonUtility.h"
#include "CreatRiver.h"
#include "ChksUtility.h"
#include "../Logger.h"
// #include "../../clientcom/utility/ulog.h"

#include "RiverFS.h"

DWORD RiverFS::Initialize(TCHAR *szLocation, const TCHAR *szDriveRoot)
{
    if(objCreatRiver.BuildRiverEnviro(szLocation, szDriveRoot))
        return ((DWORD)-1);
    if(objChksList.Initialize(szLocation)) return ((DWORD)-1);
    if(objFilesRiver.Initialize(szLocation)) return ((DWORD)-1);
    if(objRiverFolder.Initialize(szLocation)) return ((DWORD)-1);
    return 0;
}

DWORD RiverFS::Finalize()
{
    objRiverFolder.Finalize();
    objFilesRiver.Finalize();
    objChksList.Finalize();
    return 0;
}

//

DWORD RiverFS::FolderIdenti(TCHAR *szExistsPath, const TCHAR *szFolderPath, FILETIME *ftCreationTime)
{
    if(objRiverFolder.IsDirectory(NO_ROOT(szFolderPath))) // repetition insert
        return RIVER_OPERATE_FOUND;
    //
    DWORD dwIdenValue = RIVER_OPERATE_ADD;
    TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    if(fileName) {
        if(objRiverFolder.FindRecycled(NO_ROOT(szExistsPath), ++fileName, ftCreationTime)) {
            dwIdenValue = RIVER_OPERATE_MOVE;
        } else if(objRiverFolder.FindIsonym(NO_ROOT(szExistsPath), fileName, ftCreationTime)) {
            dwIdenValue = RIVER_OPERATE_COPY;
        }
        PATH_ROOT(szExistsPath, szFolderPath)
    }
    //
    return dwIdenValue;
}

DWORD RiverFS::PassFolderValid(const TCHAR *szFolderPath, FILETIME *ftCreationTime)
{
    TCHAR szExistsPath[MAX_PATH];
    DWORD dwPassValid = 0;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    if(fileName) {
        if(objRiverFolder.FindIsonym(NO_ROOT(szExistsPath), ++fileName, ftCreationTime)) {
            if(!_tcscmp(NO_ROOT(szExistsPath), NO_ROOT(szFolderPath))) dwPassValid = 1;
        }
    }
    //
    return dwPassValid;
}

DWORD RiverFS::FolderAppend(const TCHAR *szFolderPath, FILETIME *ftCreationTime)
{
    FolderShadow *pshado;
    DWORD reval = -1;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    if(szFolderPath+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        pshado = objRiverFolder.FolderShadowName(NO_ROOT(szFolderPath));
//		if(!pshado) logger(_T("c:\\river.log"), _T("do not find parent:%s\r\n"), szFolderPath);
        fileName[0] = _T('\\');
        reval = objRiverFolder.InsEntry(NO_ROOT(szFolderPath), ftCreationTime, pshado);
    } else {
        if(pshado = objRiverFolder.FolderShadowPtr(ROOT_INDE_VALU))
            reval = objRiverFolder.InsEntry(NO_ROOT(szFolderPath), ftCreationTime, pshado);
    }
    //
    return reval;
}

DWORD RiverFS::FolderAppend(const TCHAR *szFolderPath, const TCHAR *szDriveRoot)
{
    FolderShadow *pshado;
    FILETIME ftCreationTime;
    TCHAR szFolderName[MAX_PATH];
    DWORD reval = -1;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    if(szFolderPath != fileName) {
        fileName[0] = _T('\0');
        pshado = objRiverFolder.FolderShadowName(szFolderPath);
        fileName[0] = _T('\\');
        if(pshado) {
            _stprintf_s(szFolderName, MAX_PATH, _T("%s%s"), szDriveRoot, szFolderPath);
            CommonUtility::CreationTime(&ftCreationTime, szFolderName);
            reval = objRiverFolder.InsEntry(szFolderPath, &ftCreationTime, pshado);
        }
    } else {
        if(pshado = objRiverFolder.FolderShadowPtr(ROOT_INDE_VALU)) {
            _stprintf_s(szFolderName, MAX_PATH, _T("%s%s"), szDriveRoot, szFolderPath);
            CommonUtility::CreationTime(&ftCreationTime, szFolderName);
            reval = objRiverFolder.InsEntry(szFolderPath, &ftCreationTime, pshado);
        }
    }
    //
    return reval;
}

DWORD RiverFS::FolderRestore(const TCHAR *szFolderPath, FILETIME *ftCreationTime)
{
    FolderShadow *pshado;
    DWORD reval = -1;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    if(szFolderPath+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        pshado = objRiverFolder.FolderShadowName(NO_ROOT(szFolderPath));
        fileName[0] = _T('\\');
        reval = objRiverFolder.EntryRestore(NO_ROOT(szFolderPath), ftCreationTime, pshado);
    } else reval = objRiverFolder.EntryRestore(NO_ROOT(szFolderPath), ftCreationTime, objRiverFolder.FolderShadowPtr(ROOT_INDE_VALU));
    //
    return reval;
}

DWORD RiverFS::FolderMoved(const TCHAR *szExistsPath, const TCHAR *szMovePath)
{
    FolderShadow *pshado;
    DWORD reval = -1;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szMovePath, _T('\\'));
    if(szMovePath+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        pshado = objRiverFolder.FolderShadowName(NO_ROOT(szMovePath));
        fileName[0] = _T('\\');
        reval = objRiverFolder.MoveEntry(NO_ROOT(szExistsPath), NO_ROOT(szMovePath), pshado);
    } else reval = objRiverFolder.MoveEntry(NO_ROOT(szExistsPath), NO_ROOT(szMovePath), objRiverFolder.FolderShadowPtr(ROOT_INDE_VALU));
    //
    return reval;
}

//
BOOL RiverFS::FileExists(const TCHAR *szFileName)
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFileName, _T('\\'));
    if(szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFileName));
        fileName[0] = _T('\\');
    } else lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileIndex = objFilesRiver.FindFileEntry(++fileName, lSiblingIndex);
    //
    return INVA_INDE_VALU == lFileIndex? FALSE: TRUE;
}

DWORD RiverFS::FileIdenti(TCHAR *szExistsFile, struct FileID *pFileID)
{
    if(FileExists(pFileID->szFileName)) // repetition insert
        return RIVER_OPERATE_FOUND;
    //
    DWORD dwIdenValue = RIVER_OPERATE_ADD;
    TCHAR szParentName[MAX_PATH];
    //
    TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
    if(fileName) { //
        BOOL bPassValid = FALSE;
        ULONG lParentIndex = objFilesRiver.FindRecycled(&bPassValid, ++fileName, pFileID->szFileChks);
        if(INVA_INDE_VALU != lParentIndex) {
            objRiverFolder.FolderNameIndex(szParentName, lParentIndex);
            PATH_ROOT(szExistsFile, pFileID->szFileName)
            _tcscpy_s(NO_ROOT(szExistsFile), MAX_PATH-ROOT_LENGTH, szParentName);
            if(_T('\0') != *(szExistsFile+0x03)) _tcscat_s(szExistsFile, MAX_PATH, _T("\\"));
            _tcscat_s(szExistsFile, MAX_PATH, fileName);
            if(bPassValid) dwIdenValue = RIVER_OPERATE_MOVE;
            else dwIdenValue = OPERATE_MOVE_MODIFY;
        } else { //
            lParentIndex = objFilesRiver.FindIsonym(fileName, pFileID);
            if(INVA_INDE_VALU != lParentIndex) {
                objRiverFolder.FolderNameIndex(szParentName, lParentIndex);
                PATH_ROOT(szExistsFile, pFileID->szFileName)
                _tcscpy_s(NO_ROOT(szExistsFile), MAX_PATH-ROOT_LENGTH, szParentName);
                if(_T('\0') != *(szExistsFile+0x03)) _tcscat_s(szExistsFile, MAX_PATH, _T("\\"));
                _tcscat_s(szExistsFile, MAX_PATH, fileName);
                dwIdenValue = RIVER_OPERATE_COPY;
            }
        } //
    }
    //
    return dwIdenValue;
}

DWORD RiverFS::PassFileValid(const TCHAR *szExistsFile, struct FileID *pFileID)
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szExistsFile, _T('\\'));
    if(szExistsFile+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szExistsFile));
        fileName[0] = _T('\\');
    } else lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileIndex = objFilesRiver.FindFileEntry(++fileName, lSiblingIndex, pFileID->szFileChks);
    //
    return (INVA_INDE_VALU == lFileIndex) ? 0: 1;
}

ULONG RiverFS::FileAppend(struct FileID *pFileID)
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
    if(pFileID->szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(pFileID->szFileName));
        fileName[0] = _T('\\');
        lFileIndex = objFilesRiver.InsEntry(++fileName, pFileID, lParentIndex, lSiblingIndex);
        objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileIndex = objFilesRiver.InsEntry(++fileName, pFileID, ROOT_INDE_VALU, lSiblingIndex);
        objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileIndex);
    }
    //
    return lFileIndex;
}

ULONG RiverFS::DeleteFile(const TCHAR *szFileName)
{
    ULONG lNextIndex, lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFileName, _T('\\'));
    if(szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFileName));
        fileName[0] = _T('\\');
        lFileIndex = objFilesRiver.DelFileEntry(&lNextIndex, ++fileName, lSiblingIndex);
        if(lSiblingIndex == lFileIndex)
            objRiverFolder.SetChildIndex(lParentIndex, lNextIndex);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileIndex = objFilesRiver.DelFileEntry(&lNextIndex, ++fileName, lSiblingIndex);
        if(lSiblingIndex == lFileIndex)
            objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lNextIndex);
    }
    //
    return lFileIndex;
}

ULONG RiverFS::FileRestore(const TCHAR *szFileName)
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFileName, _T('\\'));
    if(szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFileName));
        fileName[0] = _T('\\');
        lFileIndex = objFilesRiver.EntryRestore(++fileName, lParentIndex, lSiblingIndex, szFileName);
        objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileIndex = objFilesRiver.EntryRestore(++fileName, ROOT_INDE_VALU, lSiblingIndex, szFileName);
        objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileIndex);
    }
    //
    return lFileIndex;
}

ULONG RiverFS::FileRestore(struct FileID *pFileID)
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
    if(pFileID->szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(pFileID->szFileName));
        fileName[0] = _T('\\');
        lFileIndex = objFilesRiver.EntryRestore(++fileName, lParentIndex, lSiblingIndex, pFileID);
        objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileIndex = objFilesRiver.EntryRestore(++fileName, ROOT_INDE_VALU, lSiblingIndex, pFileID);
        objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileIndex);
    }
    //
    return lFileIndex;
}

ULONG RiverFS::FileMoved(const TCHAR *szExistsFile, struct FileID *pFileID)
{
    ULONG lNextIndex, lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szExistsFile, _T('\\'));
    if(szExistsFile+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szExistsFile));
        fileName[0] = _T('\\');
        lFileIndex = objFilesRiver.ExistsEntry(&lNextIndex, ++fileName, lSiblingIndex);
        if(lSiblingIndex == lFileIndex)
            objRiverFolder.SetChildIndex(lParentIndex, lNextIndex);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileIndex = objFilesRiver.ExistsEntry(&lNextIndex, ++fileName, lSiblingIndex);
        if(lSiblingIndex == lFileIndex)
            objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lNextIndex);
    }
    //
    fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
    if(pFileID->szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(pFileID->szFileName));
        fileName[0] = _T('\\');
        objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
        if(INVA_INDE_VALU != lSiblingIndex)
            lFileIndex = objFilesRiver.MoveEntry(lFileIndex, lParentIndex, lSiblingIndex, pFileID);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileIndex);
        if(INVA_INDE_VALU != lSiblingIndex)
            lFileIndex = objFilesRiver.MoveEntry(lFileIndex, ROOT_INDE_VALU, lSiblingIndex, pFileID);
    }
    //
    return lFileIndex;
}

ULONG RiverFS::FileMoved(const TCHAR *szExistsFile, const TCHAR *szMoveFile)
{
    ULONG lNextIndex, lSiblingIndex, lParentIndex, lFileIndex;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szExistsFile, _T('\\'));
    if(szExistsFile+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szExistsFile));
        fileName[0] = _T('\\');
        lFileIndex = objFilesRiver.ExistsEntry(&lNextIndex, ++fileName, lSiblingIndex);
        if(lSiblingIndex == lFileIndex)
            objRiverFolder.SetChildIndex(lParentIndex, lNextIndex);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileIndex = objFilesRiver.ExistsEntry(&lNextIndex, ++fileName, lSiblingIndex);
        if(lSiblingIndex == lFileIndex)
            objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lNextIndex);
		lParentIndex = ROOT_INDE_VALU;
    }
	// ÈÝ´í INVA_INDE_VALU == lFileIndex
    if(INVA_INDE_VALU == lFileIndex) { // do not find file entry, insert new entry.
        lFileIndex = objFilesRiver.InsEntry(NULL, 0, fileName, lParentIndex, lSiblingIndex);
        objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
    }
    //
    fileName = _tcsrchr((TCHAR *)szMoveFile, _T('\\'));
    if(szMoveFile+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szMoveFile));
        fileName[0] = _T('\\');
        objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
        if(INVA_INDE_VALU != lSiblingIndex)
            lFileIndex = objFilesRiver.MoveEntry(lFileIndex, lParentIndex, lSiblingIndex, szMoveFile);
    } else {
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileIndex);
        if(INVA_INDE_VALU != lSiblingIndex)
            lFileIndex = objFilesRiver.MoveEntry(lFileIndex, ROOT_INDE_VALU, lSiblingIndex, szMoveFile);
    }
    //
    return lFileIndex;
}

//
VOID RiverFS::VoidRecycled()
{
    objRiverFolder.VoidRecycled();
    objFilesRiver.VoidRecycled();
}

//
DWORD RiverFS::InitFileID(struct FileID *pFileID, const TCHAR *szFileName, BOOL bFileExists) // maybe file not exists
{
    _tcscpy_s(pFileID->szFileName, MAX_PATH, szFileName);
    if(bFileExists) {
        HANDLE hRiveChks = ChksUtility::ChksFileHandle();
        if(INVALID_HANDLE_VALUE == hRiveChks) return 0x01;
        ChksUtility::CreatChksFile(hRiveChks, szFileName);
        //
        ChksUtility::FileSha1Chks(pFileID->szFileChks, hRiveChks);
        CommonUtility::FileSizeTimeExt(&pFileID->qwFileSize, &pFileID->ftLastWrite, szFileName);
        pFileID->hRiveChks = hRiveChks;
    }
    return 0x00;
}

DWORD RiverFS::ChunkWriteUpdate(const TCHAR *szFileName, DWORD iListPosit, FILETIME *ftLastWrite) // no root
{
    TCHAR szFilePath[MAX_PATH];
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    ULONG lListIndex = -1;
    DWORD iListLength;
    //
// _LOG_TRACE(_T("chunk write update, file:%s posit:%d last_write:%llu"), szFileName, iListPosit, *ftLastWrite);
    _tcscpy_s(szFilePath, MAX_PATH, szFileName);
    TCHAR *fileName = _tcsrchr(szFilePath, _T('\\'));
    if(szFilePath != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, szFilePath);
        fileName[0] = _T('\\');
    } else {
        lParentIndex = ROOT_INDE_VALU;
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    }
    lFileIndex = objFilesRiver.FindFileEntry(&lListIndex, &iListLength, ++fileName, lSiblingIndex);
    //
    if(INVA_INDE_VALU == lFileIndex) return 0x01; // do not find file entry, insert new entry
    if(objChksList.ChunkLastWrite(lListIndex, &iListLength, iListPosit, ftLastWrite))
        objFilesRiver.FileChklenUpdate(lFileIndex, iListLength);
    //
    return 0x00;
}

BOOL RiverFS::FolderMaker(const TCHAR *szFolderPath, const TCHAR *szDriveRoot)
{
    TCHAR szParent[MAX_PATH];
    //
    if(objRiverFolder.IsDirectory(szFolderPath) || !FolderAppend(szFolderPath, szDriveRoot))
        return TRUE;
    CommonUtility::GetFileFolderPath(szParent, szFolderPath);
    if (!FolderMaker(szParent, szDriveRoot) || FolderAppend(szFolderPath, szDriveRoot))
        return FALSE;
    //
    return TRUE;
}

DWORD RiverFS::ChunkUpdate(const TCHAR *szFileName, DWORD iListPosit, unsigned char *md5_chks, FILETIME *ftLastWrite, const TCHAR *szDriveRoot)
{
    TCHAR szFilePath[MAX_PATH];
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    ULONG lListIndex = -1;
    DWORD iListLength;
    //
    _LOG_TRACE(_T("chunk update, file:%s posit:%d last_write:%llu"), szFileName, iListPosit, *ftLastWrite);
    _tcscpy_s(szFilePath, MAX_PATH, szFileName);
    lParentIndex = INVA_INDE_VALU;
    TCHAR *fileName = _tcsrchr(szFilePath, _T('\\'));
    if(szFilePath != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, szFilePath);
        if(INVA_INDE_VALU == lParentIndex) { // do not find dir, recursive create
            FolderMaker(szFilePath, szDriveRoot);
            lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, szFilePath);
        }
        fileName[0] = _T('\\');
    } else {
        lParentIndex = ROOT_INDE_VALU;
        lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    }
    lFileIndex = objFilesRiver.FindFileEntry(&lListIndex, &iListLength, ++fileName, lSiblingIndex);
    //
    if(INVA_INDE_VALU == lFileIndex) { // do not find file entry, insert new entry.
        iListLength = 1 + iListPosit;
        lFileIndex = objFilesRiver.InsEntry(&lListIndex, iListLength, fileName, lParentIndex, lSiblingIndex);
        objRiverFolder.SetChildIndex(lParentIndex, lFileIndex);
    }
    if(objChksList.ChunkUpdate(lListIndex, &iListLength, iListPosit, md5_chks, ftLastWrite))
        objFilesRiver.FileChklenUpdate(lFileIndex, iListLength);
    //
    return 0x00;
}
//
DWORD RiverFS::FileSha1(unsigned char *pSha1Chks, TCHAR *szFileName)
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    ULONG lListIndex = -1;
    DWORD iListLength;
    //
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFileName));
        fileName[0] = _T('\\');
    } else lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileIndex = objFilesRiver.FindFileEntry(&lListIndex, &iListLength, ++fileName, lSiblingIndex);
    //
    if(INVA_INDE_VALU == lFileIndex) return 0x01;
    FILETIME ftLastWrite;
    if(objFilesRiver.ValidFileSha1(pSha1Chks, &ftLastWrite, lFileIndex, szFileName)) {
        objChksList.FileSha1(pSha1Chks, szFileName, lListIndex, &iListLength);
        objFilesRiver.FileSha1Update(lFileIndex, pSha1Chks, &ftLastWrite, iListLength);
    }
    //
    return 0x00;
}

DWORD RiverFS::ValidFileChks(TCHAR *szFileName, DWORD iListPosit)   // 0:okay 1:valid error
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    ULONG lListIndex = -1;
    DWORD iListLength;
    //
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFileName));
        fileName[0] = _T('\\');
    } else lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileIndex = objFilesRiver.FindFileEntry(&lListIndex, &iListLength, ++fileName, lSiblingIndex);
// _LOG_TRACE(_T("lFileIndex:%X file:%s"), lFileIndex, szFileName);
    if(INVA_INDE_VALU == lFileIndex) return 0x00;
    //
    return objChksList.ValidFileChks(szFileName, lListIndex, iListLength, iListPosit);
}

DWORD RiverFS::FileChks(HANDLE hRiveChks, TCHAR *szFileName, DWORD iListPosit)   // 0:okay 1:chkslen
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    ULONG lListIndex = -1;
    DWORD iListLength;
    //
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFileName));
        fileName[0] = _T('\\');
    } else lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileIndex = objFilesRiver.FindFileEntry(&lListIndex, &iListLength, ++fileName, lSiblingIndex);
    //
    if(INVA_INDE_VALU == lFileIndex) return 0x01;
    if(objChksList.FileChks(hRiveChks, szFileName, lListIndex, &iListLength, iListPosit))
        objFilesRiver.FileChklenUpdate(lFileIndex, iListLength);
    //
    return 0x00;
}

DWORD RiverFS::ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, DWORD iListPosit)
{
    ULONG lSiblingIndex, lParentIndex, lFileIndex;
    ULONG lListIndex = -1;
    DWORD iListLength;
    //
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName+ROOT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSiblingIndex = objRiverFolder.FileSiblingIndex(&lParentIndex, NO_ROOT(szFileName));
        fileName[0] = _T('\\');
    } else lSiblingIndex = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileIndex = objFilesRiver.FindFileEntry(&lListIndex, &iListLength, ++fileName, lSiblingIndex);
    //
    if(INVA_INDE_VALU == lFileIndex) return 0x01;
    if(objChksList.ChunkChks(pRiveChks, szFileName, lListIndex, &iListLength, iListPosit))
        objFilesRiver.FileChklenUpdate(lFileIndex, iListLength);
    //
    return 0x00;
}