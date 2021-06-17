#pragma once

#include "clientcom/lupdatecom.h"

class CDowndObject
{
public:
	CDowndObject(void);
	~CDowndObject(void);
public:
#define DOWND_THREAD_FAULT			-1
#define DOWND_THREAD_SUCCESS			0x0000
#define DOWND_THREAD_WAITING			0x0001
#define DOWND_THREAD_ACTIVE			0x0003
	volatile DWORD m_dwDowndThreadStatus;
	char update_xml[LOAD_BUFF_SIZE];
};
extern class CDowndObject objDowndObject;


namespace NDowndBzl
{
	//
	inline DWORD GetDowndThreadStatus() {
		DWORD dwThreadStatus;
		InterlockedExchange((LONG*)&dwThreadStatus, objDowndObject.m_dwDowndThreadStatus);
		return dwThreadStatus;
	}
	inline void SetDowndThreadStatus(DWORD dwThreadStatus)
	{ InterlockedExchange((LONG*)&objDowndObject.m_dwDowndThreadStatus, dwThreadStatus); }
	//
#define DOWND_VALUE_FAILED			-1
#define DOWND_VALUE_SUCCESS			0x0000
#define DOWND_VALUE_ACTIVE			0x0001
#define DOWND_LOAD_FILE				0x0003
#define DOWND_VALUE_UPDATE			0x0005
	DWORD PerfromDownload();
	//
};
