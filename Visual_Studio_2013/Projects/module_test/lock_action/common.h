

#include<windows.h>
//
#define mkzero(txt) txt[0] = '\0'
#define MKZERO(text) text[0] = _T('\0')




namespace nstriutility
{
inline TCHAR *get_name(TCHAR *name, const TCHAR *location, const TCHAR *subdir)
{
    TCHAR wdesti[MAX_PATH];
    //
    if(!name) return NULL;
    //
    if (*subdir == _T('~'))
        _stprintf_s(wdesti, _T("%s%s"), location, subdir+1);
    else _stprintf_s(wdesti, _T("%s%s"), location, subdir);
    _tcscpy_s(name, wcslen(wdesti)+1, wdesti);
    //
    return name;
}
};

struct Action {
	DWORD dwActionType;
	TCHAR szFileName[MAX_PATH];
	DWORD ulTimestamp;
};


//
#define INVA_ATTR				0x00000000
//
// file real status
#define ADD                     0x00000001 // addition
#define MODIFY                  0x00000002 // modify
#define DEL                     0x00000004 // delete
#define EXIST                  0x00000008 // exists
#define COPY                    0x00000010 // copy
#define MOVE                    0x00000020 // move
//
// list status
#define RECURSIVE               0x00000040 // recursive
#define LIST                    0x00000080 // list
//
// dir virtual status
#define DIRECTORY	            0x00001000 // directory
//
#define PROPERTY                0x00010000 // property
//
#define LOCK					PROPERTY | 0x0001 // lock 
#define UNLOCK					PROPERTY | 0x0002 // unlink
#define READONLY				PROPERTY | 0x0004 // read only
#define WRITEABLE				PROPERTY | 0x0008 // writeable
#define SHARED					PROPERTY | 0x0010 // shared
#define EXCLUSIVE				PROPERTY | 0x0020 // exclusive
//
// #define ERASE	                0x00020000 // erase
//


//
#define NO_ROOT(file_path)				(file_path) + 2
#define ROOT_LENGTH						2

namespace NFileUtility {
#define FILE_EXLOCK		((DWORD)-1)
#define FILE_UNLOCK		0
#define FILE_LOCKED		0x0001
inline DWORD IsFileLocked(const TCHAR *szFileName)   // -1:error 0:not lock 1:locked
{
    HANDLE hFileLocked;
    //
    if(!szFileName) return ((DWORD)-1);
    hFileLocked = CreateFile( szFileName,
                              /* GENERIC_WRITE | */ GENERIC_READ,
                              FILE_SHARE_READ /* | FILE_SHARE_WRITE */,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);
    if( INVALID_HANDLE_VALUE == hFileLocked ) {
        if(ERROR_SHARING_VIOLATION == GetLastError()) return 0x01;
        return ((DWORD)-1);
    }
    //
    if(INVALID_HANDLE_VALUE != hFileLocked) {
        CloseHandle( hFileLocked );
        hFileLocked = INVALID_HANDLE_VALUE;
    }
    //
    return 0;
}

inline BOOL FileExists(TCHAR *szFileName)
{
    WIN32_FIND_DATA wfd;
    BOOL bFound = FALSE;
    //
    HANDLE hFind = FindFirstFile(szFileName, &wfd);
    if (INVALID_HANDLE_VALUE!=hFind) bFound = TRUE;
    //
    FindClose(hFind);
    return bFound;
}
};