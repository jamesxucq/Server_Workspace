#include "StdAfx.h"

#include "clientcom/lupdatecom.h"
#include "InsLibrary.h"

#define LINE_SIZE 1024

BOOL EnableWow64FsRedirection (BOOL enable) {
	typedef BOOLEAN (__stdcall *Wow64EnableWow64FsRedirection_t) (BOOL enable);
	Wow64EnableWow64FsRedirection_t wow64EnableWow64FsRedirection = (Wow64EnableWow64FsRedirection_t) GetProcAddress (GetModuleHandle (_T("kernel32")), "Wow64EnableWow64FsRedirection");

	if (!wow64EnableWow64FsRedirection)
		return FALSE;

	return wow64EnableWow64FsRedirection (enable);
}

BOOL NInsLibrary::RegistLibrary(TCHAR *szLibraryPath) {
	TCHAR szSystemDirectory[LINE_SIZE];
	TCHAR szCommandLine[LINE_SIZE];

	DWORD dwProcess = SystemCall::ProcessorArchitecture();
	if ((PROCESS_AMD64|PROCESS_INTEL_IA64) & dwProcess) EnableWow64FsRedirection(FALSE);

	if (GetSystemDirectory(szSystemDirectory, LINE_SIZE)) {
		_stprintf_s(szCommandLine, _T("\"%s\\regsvr32.exe\" /s \"%s\""), szSystemDirectory, szLibraryPath);

		PROCESS_INFORMATION pi;
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		if (CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			CloseHandle(pi.hThread);
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
		}

	}

	if ((PROCESS_AMD64|PROCESS_INTEL_IA64) & dwProcess) EnableWow64FsRedirection(TRUE);

	return TRUE;
}

BOOL NInsLibrary::UnregLibrary(TCHAR *szLibraryPath) {
	TCHAR szSystemDirectory[LINE_SIZE];
	TCHAR szCommandLine[LINE_SIZE];

	DWORD dwProcess = SystemCall::ProcessorArchitecture();
	if ((PROCESS_AMD64|PROCESS_INTEL_IA64) & dwProcess) EnableWow64FsRedirection(FALSE);

	if (GetSystemDirectory(szSystemDirectory, LINE_SIZE)) {
		_stprintf_s(szCommandLine, _T("\"%s\\regsvr32.exe\" /s /u \"%s\""), szSystemDirectory, szLibraryPath);

		PROCESS_INFORMATION pi;
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		if (CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			CloseHandle(pi.hThread);
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
		}

	}

	if ((PROCESS_AMD64|PROCESS_INTEL_IA64) & dwProcess) EnableWow64FsRedirection(TRUE);

	return TRUE;
}
