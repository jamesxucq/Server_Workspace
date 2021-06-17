#include "StdAfx.h"
#include "Anchor.h"

#include "iattb_type.h"
#include "StringUtility.h"
#include "third_party.h"
#include "FileUtility.h"

CAnchor::CAnchor(void):
    m_hAnchorInde(INVALID_HANDLE_VALUE)
    ,m_hAnchorData(INVALID_HANDLE_VALUE)
{
    MKZEO(m_szAnchorInde);
    MKZEO(m_szAnchorData);
    memset(&m_tLastInde, 0, sizeof(struct IndexValue));
}

CAnchor::~CAnchor(void) {
}

CAnchor objAnchor;

DWORD CAnchor::Initialize(TCHAR *szAnchorInde, TCHAR *szAnchorData, TCHAR *szLocation) {
    struti::get_name(m_szAnchorInde, szLocation, szAnchorInde);
    m_hAnchorInde = CreateFile( m_szAnchorInde,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                NULL);
    if(INVALID_HANDLE_VALUE == m_hAnchorInde) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
//
    struti::get_name(m_szAnchorData, szLocation, szAnchorData);
    m_hAnchorData = CreateFile( m_szAnchorData,
                                GENERIC_WRITE | GENERIC_READ,
                                NULL, /* FILE_SHARE_READ */
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
                                NULL);
    if( INVALID_HANDLE_VALUE == m_hAnchorData ) {
        _LOG_WARN( _T("create file failed: %d"), GetLastError() );
        return ((DWORD)-1);
    }
//
    return 0x00;
}

DWORD CAnchor::Finalize() {
    if(INVALID_HANDLE_VALUE != m_hAnchorInde) {
        CloseHandle( m_hAnchorInde );
        m_hAnchorInde = INVALID_HANDLE_VALUE;
    }
    if(INVALID_HANDLE_VALUE != m_hAnchorData) {
        CloseHandle( m_hAnchorData );
        m_hAnchorData = INVALID_HANDLE_VALUE;
    }
    return 0x00;
}

DWORD CAnchor::LoadLastActio(FilesVec *pActioVec, DWORD *dwLastAnchor) {
    struct IndexValue tIndeValue;
    memset(&tIndeValue, '\0', sizeof(struct IndexValue));
    if(LoadLastIndeValue(&tIndeValue)) 
		return ((DWORD)-1);
//
    if(LoadActioByIndeValue(pActioVec, &tIndeValue))
        return ((DWORD)-1);
//
    memcpy(&m_tLastInde, &tIndeValue, sizeof(struct IndexValue));
    *dwLastAnchor = tIndeValue.AnchorNumber;
//
    return 0x00; // succ
}

DWORD CAnchor::FlushActioVec(FilesVec *pActioVec) {
    FilesVec::iterator iter;
    DWORD dwWritenSize = 0x00;
//
    LONG lFileSizeHigh = (DWORD)(m_tLastInde.FileOffset>>32);
    DWORD dwResult = SetFilePointer(m_hAnchorData, (DWORD)(m_tLastInde.FileOffset&0xFFFFFFFF), &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
	//
	if(pActioVec) {
	    for(iter=pActioVec->begin(); pActioVec->end()!=iter; ++iter) {
		    if(!WriteFile(m_hAnchorData, *iter, sizeof(struct attent), &dwWritenSize, NULL))
			    return ((DWORD)-1);
		}	
	}
	//
    dwResult = SetEndOfFile(m_hAnchorData);
    if(FALSE==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    FlushFileBuffers(m_hAnchorData);
//
    return 0x00; // succ
}

DWORD  CAnchor::LoadLastIndeValue(struct IndexValue *pIndeValue) {
    unsigned __int64 qwFileSize;
    DWORD dwFileSizeHigh, dwReadSize = 0x00;
//
    memset(pIndeValue, '\0', sizeof(struct IndexValue));
    qwFileSize = GetFileSize(m_hAnchorInde, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    if(0x00 == qwFileSize) return 0x00;
//
    DWORD dwResult = SetFilePointer(m_hAnchorInde, -(LONG)sizeof(struct IndexValue), NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!ReadFile(m_hAnchorInde, pIndeValue, sizeof(struct IndexValue), &dwReadSize, NULL) || !dwReadSize) {
        // if(ERROR_HANDLE_EOF != GetLastError()) 
		return ((DWORD)-1);
    }
//
    return 0x00;
}

DWORD CAnchor::LoadLastAnchor(DWORD *dwAnchor) {
    struct IndexValue tIndeValue;
    memset(&tIndeValue, '\0', sizeof(struct IndexValue));
    if(LoadLastIndeValue(&tIndeValue))
        return ((DWORD)-1);
    *dwAnchor = tIndeValue.AnchorNumber;
    return 0x00;
}


DWORD CAnchor::AddNewAnchor(DWORD dwNewAnchor) {
    struct IndexValue tIndeValue;
    unsigned __int64 qwFileSize;
    DWORD dwFileSizeHigh, dwWritenSize = 0x00;
//
    memset(&tIndeValue, '\0', sizeof(struct IndexValue));
//
    FlushFileBuffers(m_hAnchorData);
    qwFileSize = GetFileSize(m_hAnchorData, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    memset(&tIndeValue, '\0', sizeof(struct IndexValue));
    tIndeValue.AnchorNumber = dwNewAnchor;
    tIndeValue.FileOffset = qwFileSize;
//
    DWORD dwResult = SetFilePointer(m_hAnchorInde, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    if(!WriteFile(m_hAnchorInde, &tIndeValue, sizeof(struct IndexValue), &dwWritenSize, NULL))
        return ((DWORD)-1);
    FlushFileBuffers(m_hAnchorInde);
//
    memcpy(&m_tLastInde, &tIndeValue, sizeof(struct IndexValue));
    return 0x00;
}

DWORD CAnchor::LoadActioByIndeValue(FilesVec *pActioVec, struct IndexValue *pIndeValue) {
    struct attent tAttent;
    struct attent *atte;
    DWORD dwReadSize = 0x00;
//    memset(&tAttent, '\0', sizeof(struct attent));
    LONG lFileSizeHigh = (DWORD)(pIndeValue->FileOffset>>32);
    DWORD dwResult = SetFilePointer(m_hAnchorData, (DWORD)(pIndeValue->FileOffset&0xFFFFFFFF), &lFileSizeHigh, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    do {
        if(!ReadFile(m_hAnchorData, &tAttent, sizeof(struct attent), &dwReadSize, NULL)) {
            // if(ERROR_HANDLE_EOF != GetLastError()) 
			return ((DWORD)-1);
        }
        if(sizeof(struct attent) == dwReadSize) {
            atte = NFilesVec::AppNewNode(pActioVec);
            memcpy(atte, &tAttent, sizeof(struct attent));
// _LOG_DEBUG(_T("+++ load action file name:%s, action_type%08X"), atte->file_name, atte->action_type);
        }
    } while(sizeof(struct attent) == dwReadSize);
//
    return 0x00;
}

DWORD CAnchor::SearchAnchor(struct IndexValue *pIndeStart, struct IndexValue *pIndeEnd, DWORD dwAnchor) {
    HANDLE hMapFile;
    unsigned __int64 qwFileSize;
    DWORD dwFileSizeHigh;
    struct IndexValue *pIndeValue = NULL;
    struct IndexValue *mid_toke, *min_toke, *max_toke;
    DWORD dwAnchorFound = SEARCH_NOT_FOUND;
//
    if(!pIndeStart || !pIndeEnd || 0>dwAnchor)
        return SEARCH_FAULT;
    memset(pIndeStart, '\0', sizeof(struct IndexValue));
    memset(pIndeEnd, '\0', sizeof(struct IndexValue));
//
    qwFileSize = GetFileSize(m_hAnchorInde, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError())
        return SEARCH_FAULT;
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    if(0x00==qwFileSize || 0x00==dwAnchor) {
        memset(pIndeStart, '\0', sizeof(struct IndexValue));
        memset(pIndeEnd, '\0', sizeof(struct IndexValue));
        return SEARCH_FOUND;
    }
//
    // View must always start on a multiple
    // of the allocation granularity
    SYSTEM_INFO sinf;
    GetSystemInfo(&sinf);
//
    hMapFile = CreateFileMapping(m_hAnchorInde, NULL, PAGE_READWRITE, 0, 0, NULL);
    if(NULL == hMapFile) {
        _LOG_WARN( _T("create file mapping failed.") );
        return SEARCH_FAULT;
    }
//
    // Determine the number of bytes to be mapped in this view
    DWORD dwBytesInBlock = sinf.dwAllocationGranularity;
    if (qwFileSize < sinf.dwAllocationGranularity)
        dwBytesInBlock = (DWORD)qwFileSize;
//
    pIndeValue = (struct IndexValue *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, dwBytesInBlock);
    if(!pIndeValue) return SEARCH_FAULT;
//
    UINT dwIndeNum = (UINT)(qwFileSize/sizeof(struct IndexValue));
    max_toke = pIndeValue + dwIndeNum - 1;
    memcpy(pIndeEnd, max_toke, sizeof(struct IndexValue));
    if (!pIndeEnd->AnchorNumber) {
        memset(pIndeStart, '\0', sizeof(struct IndexValue));
        _LOG_WARN( _T("create file mapping failed."));
        UnmapViewOfFile(pIndeValue);
        CloseHandle( hMapFile );
        return SEARCH_FAULT;
    }
//
    max_toke = pIndeValue + dwIndeNum - 1;
    min_toke = pIndeValue;
    mid_toke = min_toke + (dwIndeNum >> 1);
    for(; ; ) {
        if(dwAnchor > mid_toke->AnchorNumber) {
            min_toke = mid_toke;
            dwIndeNum = (UINT)(((max_toke-min_toke)/sizeof(struct IndexValue)) >> 1);
            mid_toke = min_toke + (dwIndeNum?dwIndeNum:1);
        } else if(dwAnchor < mid_toke->AnchorNumber) {
            max_toke = mid_toke;
            dwIndeNum = (UINT)(((max_toke-min_toke)/sizeof(struct IndexValue)) >> 1);
            mid_toke = min_toke + dwIndeNum;
        } else if(dwAnchor == mid_toke->AnchorNumber) {
            memcpy(pIndeStart, mid_toke, sizeof(struct IndexValue));
            dwAnchorFound = SEARCH_FOUND;
            break;
        }
        if (min_toke == max_toke) {
            dwAnchorFound = SEARCH_NOT_FOUND;
            break;
        }
    }
    // Unmap the view; we don 't want multiple views
    // in our address space
    UnmapViewOfFile(pIndeValue);
    CloseHandle( hMapFile );
//
    return dwAnchorFound;
}

DWORD CAnchor::LoadActioByIndeValue(FilesVec *pActioVec, struct IndexValue *pIndeStart, struct IndexValue *pIndeEnd) {
    HANDLE hMapFile;
    unsigned __int64 qwFileSize;
    DWORD dwFileSizeHigh;
    struct attent *atte, *pbDataAttent = NULL, *pbAttentPos = NULL;
//
    if(!pActioVec || !pIndeStart || !pIndeEnd)
        return ((DWORD)-1);
//
    qwFileSize = GetFileSize(m_hAnchorData, &dwFileSizeHigh);
    if(INVALID_FILE_SIZE==qwFileSize && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    qwFileSize += (((unsigned __int64)dwFileSizeHigh) << 32);
    if(0x00==qwFileSize) return 0x00;
//
    hMapFile = CreateFileMapping(m_hAnchorData, NULL, PAGE_READWRITE, 0, 0, NULL);
    if(NULL == hMapFile) {
        _LOG_WARN( _T("create file mapping failed.") );
        return ((DWORD)-1);
    }
    unsigned long mlen = (int)(pIndeEnd->FileOffset - pIndeStart->FileOffset);
    pbAttentPos = pbDataAttent = (struct attent *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, (DWORD)(pIndeStart->FileOffset>>32), (DWORD)(pIndeStart->FileOffset&0xFFFFFFFF), mlen);
    if(!pbDataAttent) return ((DWORD)-1);
//
    for(unsigned int inde=0; inde< mlen/(int)sizeof(struct attent); inde++) {
        atte = NFilesVec::AppNewNode(pActioVec);
        memcpy(atte, pbAttentPos, sizeof(struct attent));
// _LOG_DEBUG(_T("name:%s type:%08X size:%llu"), pbAttentPos->file_name, pbAttentPos->action_type, pbAttentPos->file_size);
        pbAttentPos ++;
    }
//
    UnmapViewOfFile(pbDataAttent);
    CloseHandle( hMapFile );
//
    return 0x00;
}

DWORD CAnchor::EraseAnchor() {
    DWORD dwResult;
    if(INVALID_HANDLE_VALUE != m_hAnchorData) {
        dwResult = SetFilePointer(m_hAnchorData, 0, NULL, FILE_BEGIN);
        if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
            return ((DWORD)-1);
        dwResult = SetEndOfFile(m_hAnchorData);
        if(FALSE==dwResult && NO_ERROR!=GetLastError())
            return ((DWORD)-1);
        FlushFileBuffers(m_hAnchorData);
    }
//
    if(INVALID_HANDLE_VALUE != m_hAnchorInde) {
        dwResult = SetFilePointer(m_hAnchorInde, 0, NULL, FILE_BEGIN);
        if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
            return ((DWORD)-1);
        dwResult = SetEndOfFile(m_hAnchorInde);
        if(FALSE==dwResult && NO_ERROR!=GetLastError())
            return ((DWORD)-1);
        FlushFileBuffers(m_hAnchorInde);
    }
//
    return 0x00;
}