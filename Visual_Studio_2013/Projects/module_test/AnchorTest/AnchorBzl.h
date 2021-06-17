#pragma once

#include "Logger.h"
//#include "ClientCom/ClientCom.h"
#include "AnchorCache.h"
#include "AnchorFile.h"
#include "FilesVec.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define NOROOT_PATH(FILE_PATH)			(FILE_PATH)+2
#define PASS_ROOT(FILE_PATH)			(FILE_PATH)+2


//0xffff0000 is Cache ctrl code
#define		SD_CACHECTRL_QUIT						0x00010000
#define		SD_CACHECTRL_ERASE						0x00020000
#define		SD_CACHECTRL_INSERT						0x00040000
#define		SD_CACHECTRL_CLEAR						0x00080000
#define		SD_CACHECTRL_SETVALUE					0x00100000
//#define		SD_CACHECTRL_WATCH_ROOT					0x00100000

///////////////////////////////////////////////////////////////////////////
//0x0000ffff is Cache data code
///////////////////////////////////////////////////////////////////////////
#define		FILE_STATUS_UNKNOWN						0x00000000
//////////////////////////////////////////////////////////////下段显示优先中
	//以下有向上的图标继承性
#define		FILE_STATUS_SYNCING						0x00000001
//////////////////////////////////////////////////////////////下段显示优先最小
#define		FILE_STATUS_DELETE						0x00000002
#define		FILE_STATUS_MODIFIED					0x00000004
#define		FILE_STATUS_ADDED						0x00000008
//////////////////////////////////////////////////////////////下段显示优先最大
#define		FILE_STATUS_CONFLICT					0x00000010 //优先0
	//以下有向下的图标继承性
#define		FILE_STATUS_LOCKED						0x00000020
#define		FILE_STATUS_READONLY					0x00000040
#define		FILE_STATUS_FORBID						0x00000080
//////////////////////////////////////////////////////////////
#define		FILE_STATUS_NORMAL						0x00001000
#define		FILE_STATUS_DIRECTORY					0x00002000
#define		FILE_STATUS_WATCHDIR					0x00004000

//#define		FILE_STATUS_WORKFILE					0x00002000
//#define		SD_WORK_DRIVE							0x00008000
///////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#define FLUSH_FILESVEC_COUNT	5
class CAnchorBzl
{
public:
	CAnchorBzl(void);
	~CAnchorBzl(void);
public:
	// return 0; succ/ return -1 error
	static DWORD Create(wchar_t *sSyncingType, wchar_t *sAnchorIndex, wchar_t *sAnchorData, wchar_t *sLocation);
	static DWORD Destroy();
private:
	static DWORD GetSyncingLockedFiles();
	static DWORD GetSlockedFilesTurnon();
public:
	static inline DWORD GetLastAnchor() 
	{ return OAnchorCache.m_uiLastAnchor; }
	static inline DWORD GetLastAnchAndLock() {
LOG_DEBUG(_T("set anchor file locked "));
		OAnchorCache.SetAnchorLockStatus(true); 
		return OAnchorCache.m_uiLastAnchor; 
	}
	static int UnlockAnchorFile();
	static int AddNewAnchAndUnlock(DWORD uiAnchor);
public:
	static DWORD AnchFilesVecInsert(wchar_t *sChangeName, char file_status); //0:succ 1:found -1:err
	static DWORD AnchFilesVecInsert(wchar_t *sExistName, wchar_t *sNewName); //0:succ 1:found -1:err
	static DWORD AnchFilesVecInsert(FilesAttribVec *vpLockedFilesVec);
private:
	static int CreatModifyFilesVec(FilesAttribVec *vpModifyFilesAtt, FilesAttribVec *vpIndexFilesAtt);
public:
	static int GetFastsyncFilesVecByAnchor(FilesAttribVec *vpFastsyncFileAttrib, DWORD uiAnchor);
	static inline FilesAttribVec *GetLastFilesVecPointer() {return OAnchorCache.GetFilesVecCachePointer();}
private:
	static DWORD ConvertFileStatusCode(char file_status);
public:
	//static inline DWORD FilesVecEmpty() { return (OAnchorCache.GetFilesVecCachePointer())->empty()? 0: 1;}
	static inline int CheckFilesChanged() { return (OAnchorCache.GetFilesVecCachePointer())->empty()? 0: 1;}//0 no ; not 0 yes
public:
	static inline void SetSyncingType(DWORD iSyncingType) { OAnchorFile.SaveSyncingType(iSyncingType); }
	static inline DWORD GetSyncingType() 
	{ return SYNCING_TYPE_SLOW == OAnchorFile.LoadSyncingType()?SYNCING_TYPE_SLOW:SYNCING_TYPE_FAST; }
};
