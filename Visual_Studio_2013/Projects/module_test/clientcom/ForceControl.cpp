#include "StdAfx.h"
#include "common_macro.h"
#include "StringUtility.h"
#include "third_party.h"

#include "ForceControl.h"

CForceControl::CForceControl(void)
:m_hForceControl(INVALID_HANDLE_VALUE)
{
	memset(m_szForceControl, '\0', MAX_PATH);
}

CForceControl::~CForceControl(void)
{
}

CForceControl objForceControl;

DWORD CForceControl::Initialize(TCHAR *szLocation) {
	nstriutility::get_name(m_szForceControl, szLocation, FORCE_CONTROL_DEFAULT);
	m_hForceControl = CreateFile( m_szForceControl, 
		GENERIC_WRITE | GENERIC_READ,
		NULL, /*FILE_SHARE_READ, modify by james 20120112*/ 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if( m_hForceControl == INVALID_HANDLE_VALUE ) {
		TRACE( _T("create file failed.\n") );
		TRACE(_T(" File: %s Line: %d\n"), _T(__FILE__), __LINE__);
		return -1;
	}

	return 0;
}

DWORD CForceControl::Finalize() {
	if(m_hForceControl != INVALID_HANDLE_VALUE) {
		CloseHandle( m_hForceControl );
		m_hForceControl = INVALID_HANDLE_VALUE;
	}

	return 0;
}

DWORD CForceControl::LoadForceControl() {
	DWORD dwForceType;
	DWORD dwReadSize = 0;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file locks.
	if(!ReadFile(m_hForceControl, &dwForceType, sizeof(DWORD), &dwReadSize, &OverLapped)) {
		if(ERROR_HANDLE_EOF != GetLastError()) return -1;
	}
	if(dwReadSize != sizeof(DWORD)) return -1;

	return dwForceType;
}

DWORD CForceControl::SaveForceControl(DWORD dwForceType) {
	DWORD dwWritenSize = 0;

	if(STYPE_FORCE_SLOWLY!=dwForceType && STYPE_FORCE_NORMAL!=dwForceType)
		return -1;

	OVERLAPPED OverLapped = {0, 0, 0, 0, NULL}; // used for file unlocks.
	if(!WriteFile(m_hForceControl, &dwForceType, sizeof(DWORD), &dwWritenSize, &OverLapped))
		return -1;
	if(dwWritenSize != sizeof(DWORD)) return -1;
	FlushFileBuffers(m_hForceControl);

	return 0;
}
