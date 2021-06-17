#pragma once


class  CCreatRiver
{
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
    DWORD CreateEnvironment(const TCHAR *szDriveRoot);
    DWORD BuildFileIsonym(const TCHAR *szFileIsonym, HANDLE hFilesRiver);
public:
    DWORD BuildRiverEnviro(const TCHAR *szLocation, const TCHAR *szDriveRoot);
};

extern CCreatRiver objCreatRiver;