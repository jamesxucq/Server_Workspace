#include "StdAfx.h"

#include "CommonUtility.h"
#include "CreatRiver.h"
#include "ChksUtility.h"
#include "../Logger.h"
#include "../../clientcom/utility/ulog.h"

#include "RiverFS.h"

DWORD RiverFS::Initialize(TCHAR *szLocation, const TCHAR *szDriveLetter) {
    if(objCreatRiver.BuildRiverEnviro(szLocation, szDriveLetter))
        return ((DWORD)-1);
    if(objChksList.Initialize(szLocation)) return ((DWORD)-1);
    if(objFilesRiver.Initialize(szLocation)) return ((DWORD)-1);
    if(objRiverFolder.Initialize(szLocation)) return ((DWORD)-1);
    return 0x00;
}

DWORD RiverFS::Finalize() {
    objRiverFolder.Finalize();
    objFilesRiver.Finalize();
    objChksList.Finalize();
    return 0x00;
}

//
static DWORD InseFolder(const TCHAR *szFolderPath, const TCHAR *szDriveLetter) {
    FolderShadow *pshado;
	FILETIME ftCreatTime = {0};
    TCHAR szFolderName[MAX_PATH];
    DWORD retValue = ((DWORD)-1);
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    if(szFolderPath != fileName) {
        fileName[0] = _T('\0');
        pshado = objRiverFolder.FolderShadowName(szFolderPath);
        fileName[0] = _T('\\');
        if(pshado) {
            _stprintf_s(szFolderName, MAX_PATH, _T("%s%s"), szDriveLetter, szFolderPath);
            comutil::CreationTime(&ftCreatTime, szFolderName);
            retValue = objRiverFolder.InsEntry(szFolderPath, &ftCreatTime, pshado);
        }
    } else {
        if(pshado = objRiverFolder.FolderShadowPtr(ROOT_INDE_VALU)) {
            _stprintf_s(szFolderName, MAX_PATH, _T("%s%s"), szDriveLetter, szFolderPath);
            comutil::CreationTime(&ftCreatTime, szFolderName);
            retValue = objRiverFolder.InsEntry(szFolderPath, &ftCreatTime, pshado);
        }
    }
    //
    return retValue;
}

static BOOL FolderMaker(const TCHAR *szFolderPath, const TCHAR *szDriveLetter) {
    TCHAR szParent[MAX_PATH];
    //
    if(objRiverFolder.IsDirectory(szFolderPath) || !InseFolder(szFolderPath, szDriveLetter))
        return TRUE;
    comutil::GetFileFolderPathW(szParent, szFolderPath);
    if (!FolderMaker(szParent, szDriveLetter) || InseFolder(szFolderPath, szDriveLetter))
        return FALSE;
    //
    return TRUE;
}

DWORD RiverFS::FolderAppendVc(const TCHAR *szFolderPath, FILETIME *ftCreatTime, const TCHAR *szDriveLetter) {
    FolderShadow *pshado;
    DWORD retValue = ((DWORD)-1);
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFolderPath, _T('\\'));
    if(szFolderPath != fileName) {
        fileName[0] = _T('\0');
        pshado = objRiverFolder.FolderShadowName(szFolderPath);
//		if(!pshado) logger(_T("c:\\river.log"), _T("do not find parent:%s\r\n"), szFolderPath);
		if(!pshado) {
			FolderMaker(szFolderPath, szDriveLetter);
			pshado = objRiverFolder.FolderShadowName(szFolderPath);
		}
        fileName[0] = _T('\\');
		retValue = objRiverFolder.InsEntry(szFolderPath, ftCreatTime, pshado);
    } else {
_LOG_DEBUG(_T("dire parent is root."));
        if(pshado = objRiverFolder.FolderShadowPtr(ROOT_INDE_VALU))
            retValue = objRiverFolder.InsEntry(szFolderPath, ftCreatTime, pshado);
    }
    //
    return retValue;
}

DWORD RiverFS::FolderRecursINodeA(HANDLE hRecursINodeA, const TCHAR *szFolderPath) {
	struct INodeUtili::INodeA tINodeA;
	FolderShadow *pshado;
    DWORD retValue = ((DWORD)-1);
	//
	// initial
	retValue = INodeUtili::ResetINodeHandle(hRecursINodeA, TRUE);
_LOG_DEBUG(_T("---- szFolderPath:%s"), szFolderPath);
	INodeUtili::INodeAApp(hRecursINodeA, szFolderPath, INTYPE_DIRECTORY);
	//
	ULONG lFolderInde = ROOT_INDE_VALU;
	for (; ; ++lFolderInde) {
		lFolderInde = INodeUtili::FindFolderINodeA(hRecursINodeA, &tINodeA, lFolderInde);
		if(INVA_INDE_VALU == lFolderInde) break;
		//
		pshado = objRiverFolder.FolderShadowName(tINodeA.szINodeName);
_LOG_DEBUG(_T("---- pshado:%08X inde:%d name:%s child:%08X leaf_inde:%lu"), pshado, pshado->inde, pshado->szFolderName, pshado->child, pshado->leaf_inde);
		objRiverFolder.FindSibliAppA(hRecursINodeA, pshado->child);
		objFilesRiver.FindSibliAppA(hRecursINodeA, pshado->leaf_inde, pshado->szFolderName);
	}
	//
    return retValue;
}

DWORD RiverFS::FolderMoved(const TCHAR *szExistPath, const TCHAR *szNewFile, const TCHAR *szDriveLetter) {
    FolderShadow *pshado;
    DWORD retValue = ((DWORD)-1);
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szNewFile, _T('\\'));
    if(szNewFile != fileName) {
        fileName[0] = _T('\0');
        pshado = objRiverFolder.FolderShadowName(szNewFile);
		if(!pshado) {
			FolderMaker(szNewFile, szDriveLetter);
			pshado = objRiverFolder.FolderShadowName(szNewFile);
		}
        fileName[0] = _T('\\');
        retValue = objRiverFolder.MoveEntry(szExistPath, szNewFile, pshado);
    } else retValue = objRiverFolder.MoveEntry(szExistPath, szNewFile, objRiverFolder.FolderShadowPtr(ROOT_INDE_VALU));
    //
	objRiverFolder.CTimeUpdate(szNewFile, szDriveLetter);
    return retValue;
}

//
BOOL RiverFS::FileExist(const TCHAR *szFileName) { // no letter
    ULONG lSibliInde, lParentInde, lFileInde;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFileName, _T('\\'));
    if(szFileName != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFileName);
        fileName[0] = _T('\\');
    } else lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileInde = objFilesRiver.FindFileEntry(++fileName, lSibliInde);
    //
    return INVA_INDE_VALU == lFileInde? FALSE: TRUE;
}

DWORD RiverFS::FindDupliFile(TCHAR *szExistFile, const TCHAR *szFileName, const TCHAR *szDriveLetter, unsigned __int64 qwFileSize, unsigned char *szSha1Chks) {
	// check repetition insert
	if(FileExist(szFileName)) return RIVER_OPERATE_FOUND;
	//
	ULONG lParentInde;
	DWORD dwIdenValue = RIVER_OPERATE_FOUND;
	TCHAR szParentName[MAX_PATH];
	//
	TCHAR *fileName = _tcsrchr((TCHAR *)szFileName, _T('\\'));
_LOG_DEBUG(_T("++++++++ file identi, szFileName:%s fileName:%s"), szFileName, fileName);
	// logger(_T("c:\\river.log"), _T("\r\nfile name: %s\r\n"), fileName);
	if(fileName) { //
		lParentInde = objFilesRiver.FindIsonym(++fileName, szDriveLetter, qwFileSize, szSha1Chks);
_LOG_DEBUG(_T("++++++++ find isonym, lParentInde:%lu fileName:%s"), lParentInde, fileName);
		// logger(_T("c:\\river.log"), _T("isonym, lParentInde: %u\r\n"), lParentInde);
		if(INVA_INDE_VALU != lParentInde) {
			objRiverFolder.FolderNameIndex(szParentName, lParentInde);
			_tcscpy_s(szExistFile, MAX_PATH, szParentName);
			if(_T('\0') != *(szExistFile+0x01)) _tcscat_s(szExistFile, MAX_PATH, _T("\\"));
			_tcscat_s(szExistFile, MAX_PATH, fileName);
			dwIdenValue = RIVER_OPERATE_COPY;
		}
	} //
	//
_LOG_DEBUG(_T("++++++++ dwIdenValue:%d"), dwIdenValue);
	return dwIdenValue;
}

ULONG RiverFS::FileAppend(struct FileID *pFileID, const TCHAR *szDriveLetter) {
    ULONG lSibliInde, lParentInde, lFileInde;
    //
    TCHAR *fileName = _tcsrchr(pFileID->szFileName, _T('\\'));
    if(pFileID->szFileName != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, pFileID->szFileName);
		if(INVA_INDE_VALU == lParentInde) {
// _LOG_DEBUG(_T("++++++++ make fold:%s"), pFileID->szFileName);
			FolderMaker(pFileID->szFileName, pFileID->szFileName);
			objRiverFolder.FileSiblingIndex(&lParentInde, pFileID->szFileName);
		}
        fileName[0] = _T('\\');
        lFileInde = objFilesRiver.InsEntry(++fileName, pFileID, lParentInde, lSibliInde);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(lParentInde, lFileInde);
    } else {
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileInde = objFilesRiver.InsEntry(++fileName, pFileID, ROOT_INDE_VALU, lSibliInde);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileInde);
    }
    //
    return lFileInde;
}

ULONG RiverFS::DeleFileV(const TCHAR *szFileName) {
    ULONG lNextInde, lSibliInde, lParentInde, lFileInde;
    //
    TCHAR *fileName = _tcsrchr((TCHAR *)szFileName, _T('\\'));
    if(szFileName != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFileName);
        fileName[0] = _T('\\');
        lFileInde = objFilesRiver.DeliFileEntry(&lNextInde, ++fileName, lSibliInde);
        if(lSibliInde == lFileInde) 
			objRiverFolder.SetChildIndex(lParentInde, lNextInde);
    } else {
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileInde = objFilesRiver.DeliFileEntry(&lNextInde, ++fileName, lSibliInde);
        if(lSibliInde == lFileInde) 
			objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lNextInde);
    }
    //
    return lFileInde;
}

ULONG RiverFS::FileMoveV(const TCHAR *szExistFile, const TCHAR *szNewFile, const TCHAR *szDriveLetter) {
	BOOL bCTimeUpdate = FALSE;
    ULONG lNextInde, lSibliInde, lParentInde, lFileInde;
    //
    TCHAR *eFileName = _tcsrchr((TCHAR *)szExistFile, _T('\\'));
    if(szExistFile != eFileName) {
        eFileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szExistFile);
        eFileName[0] = _T('\\');
        lFileInde = objFilesRiver.GetExistEntry(&lNextInde, &bCTimeUpdate, ++eFileName, lSibliInde);
        if(lSibliInde == lFileInde) 
			objRiverFolder.SetChildIndex(lParentInde, lNextInde);
    } else {
        lParentInde = ROOT_INDE_VALU;
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileInde = objFilesRiver.GetExistEntry(&lNextInde, &bCTimeUpdate, ++eFileName, lSibliInde);
        if(lSibliInde == lFileInde) 
			objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lNextInde);
    }
    // ÈÝ´í INVA_INDE_VALU == lFileInde
	TCHAR szFileName[MAX_PATH];
	FILETIME ftCreatTime = {0};
    if(INVA_INDE_VALU == lFileInde) { // do not find file entry, insert new entry.
		ABSOLU_PATH(szFileName, szDriveLetter, szNewFile)
		comutil::CreationTime(&ftCreatTime, szFileName);
        lFileInde = objFilesRiver.InsBlankEntry(eFileName, &ftCreatTime);
_LOG_DEBUG(_T("fault tolerance. fileName:%s"), eFileName);
    } else if(bCTimeUpdate) {
		ABSOLU_PATH(szFileName, szDriveLetter, szNewFile)
		comutil::CreationTime(&ftCreatTime, szFileName);
        objFilesRiver.CTimeUpdate(lFileInde, &ftCreatTime);
_LOG_DEBUG(_T("update ctime:%s"), comutil::ftim_unis(&ftCreatTime));
	}
    //
    TCHAR *nFileName = _tcsrchr((TCHAR *)szNewFile, _T('\\'));
    if(szNewFile != nFileName) {
        nFileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szNewFile);
		if(INVA_INDE_VALU == lParentInde) {
// _LOG_DEBUG(_T("file move make folder!"));
			FolderMaker(szNewFile, szDriveLetter);
			objRiverFolder.FileSiblingIndex(&lParentInde, szNewFile);
		}
        nFileName[0] = _T('\\');
        lFileInde = objFilesRiver.MoveEntry(lFileInde, lParentInde, lSibliInde, ++nFileName);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(lParentInde, lFileInde);
    } else {
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileInde = objFilesRiver.MoveEntry(lFileInde, ROOT_INDE_VALU, lSibliInde, ++nFileName);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileInde);
    }
	//
    return lFileInde;
}

ULONG RiverFS::FileCopyV(const TCHAR *szExistFile, const TCHAR *szNewFile, const TCHAR *szDriveLetter){
    ULONG lSibliInde, lParentInde, lFileInde;
    //
    TCHAR *eFileName = _tcsrchr((TCHAR *)szExistFile, _T('\\'));
    if(szExistFile != eFileName) {
        eFileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szExistFile);
        eFileName[0] = _T('\\');
        lFileInde = objFilesRiver.FindFileEntry(++eFileName, lSibliInde);
    } else {
        lParentInde = ROOT_INDE_VALU;
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileInde = objFilesRiver.FindFileEntry(++eFileName, lSibliInde);
    }
    // ÈÝ´í INVA_INDE_VALU == lFileInde
    if(INVA_INDE_VALU == lFileInde) { // do not find file entry, insert new entry.
		FILETIME ftCreatTime = {0};
        lFileInde = objFilesRiver.InsEntry(NULL, 0x00, eFileName, &ftCreatTime, lParentInde, lSibliInde);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(lParentInde, lFileInde);
		_LOG_WARN(_T("fault tolerance. fileName:%s"), eFileName);
    }
    //
    TCHAR *nFileName = _tcsrchr((TCHAR *)szNewFile, _T('\\'));
    if(szNewFile != nFileName) {
        nFileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szNewFile);
		if(INVA_INDE_VALU == lParentInde) {
			FolderMaker(szNewFile, szDriveLetter);
			objRiverFolder.FileSiblingIndex(&lParentInde, szNewFile);
		}
        nFileName[0] = _T('\\');
        lFileInde = objFilesRiver.CopyEntry(lFileInde, lParentInde, lSibliInde, ++nFileName);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(lParentInde, lFileInde);
    } else {
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
        lFileInde = objFilesRiver.CopyEntry(lFileInde, ROOT_INDE_VALU, lSibliInde, ++nFileName);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(ROOT_INDE_VALU, lFileInde);
    }
    //
    return lFileInde;
}

//
VOID RiverFS::InitFolderID(struct FileID *pFolderID, const TCHAR *szFileName) {
	_tcscpy_s(pFolderID->szFileName, MAX_PATH, NO_LETT(szFileName));
	comutil::FileCLTime(&pFolderID->ftCreatTime, &pFolderID->ftLastWrite, szFileName);
// _LOG_DEBUG( _T("pFileID->ftCreatTime:%s"), comutil::ftim_unis(&pFileID->ftCreatTime));
}

static TCHAR *ChksSha1(unsigned char *sha1sum) {
    static TCHAR chksum_str[64];
    for (int inde=0; inde<SHA1_DIGEST_LEN; inde++)
        _stprintf_s (chksum_str+inde*2, 41, _T("%02x"), sha1sum[inde]);
    chksum_str[40] = _T('\0');
//
	return chksum_str;
}

VOID RiverFS::InitFileID(struct FileID *pFileID, const TCHAR *szFileName) {
	_tcscpy_s(pFileID->szFileName, MAX_PATH, NO_LETT(szFileName));
	pFileID->hRiveChks = ChksUtility::ChksFileHandle();
	if(INVALID_HANDLE_VALUE != pFileID->hRiveChks) {
_LOG_DEBUG(_T("szFileName:%s"), szFileName);
		ChksUtility::CreatChksFile(pFileID->hRiveChks, szFileName);
		ChksUtility::FileSha1Chks(pFileID->szFileChks, pFileID->hRiveChks);
_LOG_DEBUG(_T("++++++++ file sha1:%s"), ChksSha1(pFileID->szFileChks));
		comutil::FileSizeCLTime(&pFileID->qwFileSize, &pFileID->ftCreatTime, 
			&pFileID->ftLastWrite, szFileName);	
	}
// _LOG_DEBUG( _T("pFileID->ftCreatTime:%s"), comutil::ftim_unis(&pFileID->ftCreatTime));
// _LOG_DEBUG( _T("pFileID->ftLastWrite:%s"), comutil::ftim_unis(&pFileID->ftLastWrite));
}

//
DWORD RiverFS::FolderAppendVs(const TCHAR *szFolderPath, const TCHAR *szDriveLetter) {
	TCHAR szFolderName[MAX_PATH];
	ULONG lFileInde;
	//
    _tcscpy_s(szFolderName, MAX_PATH, szFolderPath);
	ERASE_LACHR(szFolderName)
	lFileInde = INVA_INDE_VALU;
    objRiverFolder.FileSiblingIndex(&lFileInde, szFolderName);
    if(INVA_INDE_VALU == lFileInde) // do not find dir, recursive create
        FolderMaker(szFolderName, szDriveLetter);
    //
    return 0x00;
}

DWORD RiverFS::EraseFolderVs(const TCHAR *szFolderPath){
	TCHAR szFolderName[MAX_PATH];
	//
    _tcscpy_s(szFolderName, MAX_PATH, szFolderPath);
	ERASE_LACHR(szFolderName)
    return objRiverFolder.EntryEraseRecurs(szFolderName);
}

DWORD RiverFS::ChunkLawiUpdate(const TCHAR *szFileName, DWORD dwListPosit, DWORD chunk_length, FILETIME *ftLastWrite) { // no letter
    TCHAR szFilePath[MAX_PATH];
    ULONG lSibliInde, lParentInde, lFileInde;
    ULONG lListInde = INVA_INDE_VALU;
    DWORD dwListLength;
    //
_LOG_DEBUG(_T("chunk write update, file:%s posit:%d last_write:%llu"), szFileName, dwListPosit, *ftLastWrite);
    _tcscpy_s(szFilePath, MAX_PATH, szFileName);
    TCHAR *fileName = _tcsrchr(szFilePath, _T('\\'));
    if(szFilePath != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFilePath);
        fileName[0] = _T('\\');
    } else {
        lParentInde = ROOT_INDE_VALU;
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    }
    lFileInde = objFilesRiver.FindFileEntry(&lListInde, &dwListLength, ++fileName, lSibliInde);
    //
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1); 
	// do not find file entry, insert new entry
    if(objChksList.ChunkLawiUpdate(&lListInde, &dwListLength, dwListPosit, chunk_length, ftLastWrite))
        objFilesRiver.FileChklenUpdate(lFileInde, lListInde, dwListLength);
    //
    return 0x00;
}

DWORD RiverFS::FileLawiUpdate(const TCHAR *szFileName, FILETIME *ftLastWrite) { // no letter
    TCHAR szFilePath[MAX_PATH];
    ULONG lSibliInde, lParentInde, lFileInde;
    //
_LOG_DEBUG(_T("file write update, file:%s last_write:%llu"), szFileName, *ftLastWrite);
    _tcscpy_s(szFilePath, MAX_PATH, szFileName);
    TCHAR *fileName = _tcsrchr(szFilePath, _T('\\'));
    if(szFilePath != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFilePath);
        fileName[0] = _T('\\');
    } else {
        lParentInde = ROOT_INDE_VALU;
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    }
    lFileInde = objFilesRiver.FindFileEntry(++fileName, lSibliInde);
    //
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1); 
	// do not find file entry, insert new entry
    objFilesRiver.FileLawiUpdate(lFileInde, ftLastWrite);
    //
    return 0x00;
}

#define FILE_CETI_INFO(szFileName, szDriveLetter) \
	TCHAR szFileFind[MAX_PATH]; \
	_stprintf_s(szFileFind, MAX_PATH, _T("%s%s"), szDriveLetter, szFileName); \
    WIN32_FIND_DATA fileInfo; \
    HANDLE hFind; \
    FILETIME ftCreatTime = {0}; \
    hFind = FindFirstFile(szFileFind, &fileInfo); \
    if(INVALID_HANDLE_VALUE != hFind) { \
		memcpy(&ftCreatTime, &fileInfo.ftCreationTime, sizeof(FILETIME)); \
    } \
    FindClose(hFind);

DWORD RiverFS::ChunkUpdate(const TCHAR *szFileName, DWORD dwListPosit, DWORD chunk_length, unsigned char *md5_chks, FILETIME *ftLastWrite, const TCHAR *szDriveLetter) {
    TCHAR szFilePath[MAX_PATH];
    ULONG lSibliInde, lParentInde, lFileInde;
    ULONG lListInde = INVA_INDE_VALU;
    DWORD dwListLength;
    //
_LOG_DEBUG(_T("chunk update, file:%s posit:%d last_write:%llu"), szFileName, dwListPosit, *ftLastWrite);
    _tcscpy_s(szFilePath, MAX_PATH, szFileName);
    lParentInde = INVA_INDE_VALU;
    TCHAR *fileName = _tcsrchr(szFilePath, _T('\\'));
    if(szFilePath != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFilePath);
        if(INVA_INDE_VALU == lParentInde) { // do not find dir, recursive create
            FolderMaker(szFilePath, szDriveLetter);
            lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFilePath);
        }
        fileName[0] = _T('\\');
    } else {
        lParentInde = ROOT_INDE_VALU;
        lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    }
    lFileInde = objFilesRiver.FindFileEntry(&lListInde, &dwListLength, ++fileName, lSibliInde);
    // do not find file entry, insert new entry.
	if(INVA_INDE_VALU == lFileInde) {
_LOG_DEBUG(_T("do not find file entry, insert new entry."));
		FILE_CETI_INFO(szFileName, szDriveLetter)
		dwListLength = dwListPosit;
		if(chunk_length) dwListLength++;
		lFileInde = objFilesRiver.InsEntry(&lListInde, dwListLength, fileName, &ftCreatTime, lParentInde, lSibliInde);
		if(INVA_INDE_VALU != lFileInde) 
			objRiverFolder.SetChildIndex(lParentInde, lFileInde);
	}
	//
    if(objChksList.ChunkUpdate(&lListInde, &dwListLength, dwListPosit, chunk_length, md5_chks, ftLastWrite))
        objFilesRiver.FileChklenUpdate(lFileInde, lListInde, dwListLength);
    //
    return 0x00;
}

#define FILE_CTTI_INFO(szFileName) \
    WIN32_FIND_DATA fileInfo; \
    HANDLE hFind; \
	unsigned __int64 qwFileSize = 0x00; \
    FILETIME ftCreatTime = {0}; \
    hFind = FindFirstFile(szFileName, &fileInfo); \
    if(INVALID_HANDLE_VALUE != hFind) { \
		qwFileSize = fileInfo.nFileSizeLow; \
		qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32; \
		memcpy(&ftCreatTime, &fileInfo.ftCreationTime, sizeof(FILETIME)); \
    } \
    FindClose(hFind);

DWORD RiverFS::FileUpdate(const TCHAR *szFileName, FILETIME *ftLastWrite, const TCHAR *szDriveLetter) {
    TCHAR szFilePath[MAX_PATH];
    ULONG lSibliInde, lParentInde, lFileInde;
	ULONG lListInde = INVA_INDE_VALU;
    //
_LOG_DEBUG(_T("file write update, file:%s last_write:%llu"), szFileName, *ftLastWrite);
    _tcscpy_s(szFilePath, MAX_PATH, szFileName);
    TCHAR *fileName = _tcsrchr(szFilePath, _T('\\'));
    if(szFilePath != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFilePath);
        fileName[0] = _T('\\');
    } else lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileInde = objFilesRiver.FindFileEntry(&lListInde, ++fileName, lSibliInde);
    //
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1); 
	// do not find file entry, insert new entry
	ABSOLU_PATH(szFilePath, szDriveLetter, szFileName)
	FILE_CTTI_INFO(szFilePath)
	unsigned char szSha1Chks[SHA1_DIGEST_LEN];
	objChksList.FileSha1Chks(szSha1Chks, lListInde);
    objFilesRiver.FileTimeChksUpdate(lFileInde, qwFileSize, ftLastWrite, szSha1Chks, &ftCreatTime);
    //
    return 0x00;
}

DWORD RiverFS::FileUpdate(const TCHAR *szFileName, FILETIME *ftLastWrite) {
    ULONG lSibliInde, lParentInde, lFileInde;
	ULONG lListInde = INVA_INDE_VALU;
    //
_LOG_DEBUG(_T("file write update, file:%s last_write:%llu"), szFileName, *ftLastWrite);
    TCHAR *fileName = _tcsrchr((TCHAR *)szFileName, _T('\\'));
    if(szFileName+LETT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, NO_LETT(szFileName));
        fileName[0] = _T('\\');
    } else lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileInde = objFilesRiver.FindFileEntry(&lListInde, ++fileName, lSibliInde);
    //
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1); 
	// do not find file entry, insert new entry
	DWORD dwRelayVal = objFilesRiver.ValidFileSha1(lFileInde, ftLastWrite);
    if(0x02 == dwRelayVal) {
		FILE_CTTI_INFO(szFileName)
		unsigned char szSha1Chks[SHA1_DIGEST_LEN];
		objChksList.FileSha1Chks(szSha1Chks, lListInde);
		objFilesRiver.FileTimeChksUpdate(lFileInde, qwFileSize, ftLastWrite, szSha1Chks, &ftCreatTime);
	}
    //
    return 0x00;
}

// 
// 0x00:succ 0x01:no content 0x02:continue -1:error 
// file size is 0, sha1 checksum is 0;
DWORD RiverFS::FileSha1Relay(unsigned char *szSha1Chks, TCHAR *szFileName) {
    ULONG lSibliInde, lParentInde, lFileInde;
    ULONG lListInde = INVA_INDE_VALU;
    DWORD dwListLength;
    //
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName+LETT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, NO_LETT(szFileName));
        fileName[0] = _T('\\');
    } else lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileInde = objFilesRiver.FindFileEntry(&lListInde, &dwListLength, ++fileName, lSibliInde);
    //
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1);
	unsigned __int64 qwFileSize = 0x00;
	FILETIME ftLastWrite = {0};
	DWORD dwRelayVal = objFilesRiver.ValidFileSha1(szSha1Chks, &qwFileSize, &ftLastWrite, lFileInde, szFileName);
    if(0x02 == dwRelayVal) {
        DWORD dwRiveValue = objChksList.FileSha1Relay(szSha1Chks, szFileName, &lListInde, &dwListLength);
        if(!dwRiveValue || (0x01&0x04)&dwRiveValue)
            objFilesRiver.FileUpdate(lFileInde, qwFileSize, szSha1Chks, &ftLastWrite, lListInde, dwListLength);
        if(0x02 & dwRiveValue) dwRelayVal = 0x02; // continue
		else if(0x04 & dwRiveValue) dwRelayVal = 0x01; // no content
		else dwRelayVal = 0x00;
    }
    //
_LOG_DEBUG(_T("dwRelayVal:%d"), dwRelayVal);
    return dwRelayVal;
}

// update the sha1 chks
DWORD RiverFS::FileAttribCache(unsigned __int64 *qwFileSize, unsigned char *szSha1Chks, FILETIME *ftCreatTime, TCHAR *szFileName) {
    ULONG lSibliInde, lParentInde, lFileInde;
    ULONG lListInde = INVA_INDE_VALU;
    //
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, szFileName);
        fileName[0] = _T('\\');
    } else lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileInde = objFilesRiver.FindFileEntry(++fileName, lSibliInde);
	//
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1);
	objFilesRiver.FileAttrib(qwFileSize, szSha1Chks, ftCreatTime, lFileInde);
    //
	return 0x00;
}

//
#define FILE_LAWI_INFO(szFileName) \
    WIN32_FIND_DATA fileInfo; \
    HANDLE hFind; \
	unsigned __int64 qwFileSize = 0x00; \
    FILETIME ftLastWrite = {0}; \
    hFind = FindFirstFile(szFileName, &fileInfo); \
    if(INVALID_HANDLE_VALUE != hFind) { \
        qwFileSize = fileInfo.nFileSizeLow; \
        qwFileSize += ((unsigned __int64)fileInfo.nFileSizeHigh) << 32; \
		memcpy(&ftLastWrite, &fileInfo.ftLastWriteTime, sizeof(FILETIME)); \
    } \
    FindClose(hFind);

DWORD RiverFS::FileChksRelay(HANDLE hRiveChks, TCHAR *szFileName, DWORD dwListPosit) {  // 0:okay 1:chkslen
    ULONG lSibliInde, lParentInde, lFileInde;
    ULONG lListInde = INVA_INDE_VALU;
    DWORD dwListLength;
    //
_LOG_DEBUG(_T("hRiveChks:%x dwListPosit:%d szFileName:%s"), hRiveChks, dwListPosit, szFileName);
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName+LETT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, NO_LETT(szFileName));
        fileName[0] = _T('\\');
    } else lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
// _LOG_DEBUG(_T("lSibliInde:%lu"), lSibliInde);
    lFileInde = objFilesRiver.FindFileEntry(&lListInde, &dwListLength, ++fileName, lSibliInde);
// _LOG_DEBUG(_T("lFileInde:%lu lListInde:%lu dwListLength:%u"), lFileInde, lListInde, dwListLength);
    //
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1);
	DWORD dwRelayVal = 0x00;
    DWORD dwRiveValue = objChksList.FileChksRelay(hRiveChks, szFileName, &lListInde, &dwListLength, dwListPosit);
    if(dwRiveValue) {
		if(0x01 & dwRiveValue) {
			FILE_LAWI_INFO(szFileName)
_LOG_DEBUG(_T("file chks relay lFileInde:%lu lListInde:%lu lw:%s name:%s"), lFileInde, lListInde, comutil::ftim_unis(&ftLastWrite), szFileName);
			objFilesRiver.FileLawiClenUpdate(lFileInde, qwFileSize, &ftLastWrite, lListInde, dwListLength);
		}
        if(0x02 & dwRiveValue) dwRelayVal = 0x02; // continue
		else if(0x04 & dwRiveValue) dwRelayVal = 0x01; // no content
    }
    //
    return dwRelayVal;
}

DWORD RiverFS::ChunkChks(struct riv_chks *pRiveChks, TCHAR *szFileName, DWORD dwListPosit) {
    ULONG lSibliInde, lParentInde, lFileInde;
    ULONG lListInde = INVA_INDE_VALU;
    DWORD dwListLength;
    //
    TCHAR *fileName = _tcsrchr(szFileName, _T('\\'));
    if(szFileName+LETT_LENGTH != fileName) {
        fileName[0] = _T('\0');
        lSibliInde = objRiverFolder.FileSiblingIndex(&lParentInde, NO_LETT(szFileName));
        fileName[0] = _T('\\');
    } else lSibliInde = objRiverFolder.FileSiblingIndex(ROOT_INDE_VALU);
    lFileInde = objFilesRiver.FindFileEntry(&lListInde, &dwListLength, ++fileName, lSibliInde);
    //
    if(INVA_INDE_VALU == lFileInde) return ((DWORD)-1);
    if(objChksList.ChunkChks(pRiveChks, szFileName, &lListInde, &dwListLength, dwListPosit))
        objFilesRiver.FileChklenUpdate(lFileInde, lListInde, dwListLength);
    //
    return 0x00;
}

DWORD RiverFS::RiverRecursINodeV(HANDLE hRecursINodeV) {
	struct INodeUtili::INodeV tINodeV;
	FolderShadow *pshado;
    DWORD retValue = ((DWORD)-1);
	//
	// initial
	retValue = INodeUtili::ResetINodeHandle(hRecursINodeV, TRUE);
// _LOG_DEBUG(_T("dwPathLength:%d szFolderPath:%s"), dwPathLength, szFolderPath);
	INodeUtili::FolderAdd(hRecursINodeV, _T("\\"));
	//
	ULONG lFolderInde = ROOT_INDE_VALU;
	for (; ; ++lFolderInde) {
		lFolderInde = INodeUtili::FindFolderINodeV(hRecursINodeV, &tINodeV, lFolderInde);
		if(INVA_INDE_VALU == lFolderInde) break;
		//
		pshado = objRiverFolder.FolderShadowName(tINodeV.szINodeName);
// _LOG_DEBUG(_T("pshado:%08X inde:%d name:%s child:%08X leaf_inde:%lu"), pshado, pshado->inde, pshado->szFolderName, pshado->child, pshado->leaf_inde);
		objRiverFolder.FindSibliAppV(hRecursINodeV, pshado->child);
		objFilesRiver.FindSibliAppV(hRecursINodeV, pshado->leaf_inde, pshado->szFolderName);
	}
	//
    return retValue;
}
