#pragma once


class  CCreatRiver {
public:
    CCreatRiver(void);
    ~CCreatRiver(void);
private:
    DWORD CheckRiverEnviro();
    //
    HANDLE m_hChksList;
    HANDLE m_hFilesRiver;
    HANDLE m_hRiverFolder;
    TCHAR m_szFileIsonym[MAX_PATH];
    //
    DWORD InitializeHandle(const TCHAR *szLocation);
    VOID FinalizeHandle();
    //
    DWORD CreateEnviro(const TCHAR *szDriveLetter);
    DWORD BuildFileIsonym(const TCHAR *szFileIsonym, HANDLE hFilesRiver);
public:
    DWORD BuildRiverEnviro(const TCHAR *szLocation, const TCHAR *szDriveLetter);
};

extern CCreatRiver objCreatRiver;