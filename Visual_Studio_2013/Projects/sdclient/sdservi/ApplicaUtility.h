// ApplicaUtility.h
#ifndef _APPLICA_UTILITY_H_
#define _APPLICA_UTILITY_H_

	DWORD __stdcall ExecuteApplica(TCHAR *szApplicaPath);
	DWORD __stdcall EndApplica(TCHAR *szProcessName);
	DWORD __stdcall ExecuteApplicaEx(TCHAR *szApplicaPath, TCHAR *szProcessName, bool fFailIfExist);

	DWORD __stdcall AddiApplicaAuto(TCHAR *szApplicaPath);
	DWORD __stdcall AddiApplicaAutoEx(TCHAR *szApplicaPath);
	DWORD __stdcall DeliApplicaAuto(TCHAR *szApplicaPath);

#endif /* _APPLICA_UTILITY_H_ */