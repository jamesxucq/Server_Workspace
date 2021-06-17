#pragma once

#include "common_macro.h"
#include "FileUtility.h"
#include "FilesVec.h"

#include <vector>
using std::vector;


#pragma	pack(1)
struct IndexValue {
    DWORD AnchorNumber;
    unsigned __int64 FileOffset;
};

// struct anchor_attrib {
//	WORD wLength;
//	//
//	DWORD action_type;		/* add mod del list recu */
//	//
//	unsigned __int64 *qwFileSize;
//	FILETIME ftLastWrite;		/* When the item was last modified */
//	//
//	DWORD dwReserved;  /* filehigh */
//	WORD wNameLength;
// 	TCHAR szFileName[1];
// };
#pragma	pack()

class CAnchor {
public:
    CAnchor(void);
    ~CAnchor(void);
    // canchor base operate;
public:
    DWORD Initialize(TCHAR *szAnchorInde, TCHAR *szAnchorData, TCHAR *szLocation);
    DWORD Finalize();
    // anchor data file operate;
private:
    TCHAR m_szAnchorInde[MAX_PATH];
    TCHAR m_szAnchorData[MAX_PATH];
private:
    HANDLE m_hAnchorInde;
    HANDLE m_hAnchorData;
public:
    DWORD LoadLastActio(FilesVec *pActioVec, DWORD *dwAnchor);
    DWORD FlushActioVec(FilesVec *pActioVec);
private:
    struct IndexValue m_tLastInde;
public:
    DWORD LoadLastIndeValue(struct IndexValue *pIndeValue);
    DWORD LoadLastAnchor(DWORD *dwAnchor);
public:
    DWORD AddNewAnchor(DWORD dwNewAnchor);
    DWORD LoadActioByIndeValue(FilesVec *pActioVec, struct IndexValue *pIndeValue);
    // for lan sync
public:
#define SEARCH_FAULT        -1
#define SEARCH_FOUND        0x0000
#define SEARCH_NOT_FOUND    0x0001
    DWORD SearchAnchor(struct IndexValue *pIndeStart, struct IndexValue *pIndeEnd, DWORD dwAnchor);
    DWORD LoadActioByIndeValue(FilesVec *pActioVec, struct IndexValue *pIndeStart, struct IndexValue *pIndeEnd);
public:
    DWORD EraseAnchor();
};

extern CAnchor objAnchor;