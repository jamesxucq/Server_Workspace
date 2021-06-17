#pragma once


class  CCreatRiver{
public:
	CCreatRiver(void);
	~CCreatRiver(void);
private:
	DWORD ValidRiverEnvirn();
	//////////////////////////////////////////////
	HANDLE m_hFilesRiver;
	HANDLE m_hRiverFolder;
	TCHAR m_szFileNsake[MAX_PATH];
	///////////////////////////
	DWORD InitializeHandle(const TCHAR *szLocation);
	VOID FinalizeHandle();
	///////////////////////////
	DWORD CreateEnvirn(const TCHAR *szDriveRoot);
	DWORD BuildFileNsake(const TCHAR *szFileNsake, HANDLE hFilesRiver);
public:
	DWORD BuildRiverEnvirn(const TCHAR *szLocation, const TCHAR *szDriveRoot);
};

extern CCreatRiver objCreatRiver;