#pragma once

//
#define ADDITION_ACTION_LOGGER(ACTION_FILES, ACTION, FILE_NAME, TIME_STAMP) { \
	DWORD dwWritenSize; \
	ACTION.dwActioType = ADDI; \
	_tcscpy(ACTION.szFileName, NO_LETT(FILE_NAME)); \
	ACTION.ulTimestamp = TIME_STAMP; \
	WriteFile(ACTION_FILES, &ACTION, sizeof(struct Action), &dwWritenSize, NULL); \
	FILE_ICON_DISPLAY(FILE_NAME, CONV_ACTION_SETA(ADDI)); \
}

#define MODIFY_ACTION_LOGGER(ACTION_FILES, ACTION, FILE_NAME, TIME_STAMP) { \
	DWORD dwWritenSize; \
	ACTION.dwActioType = MODIFY; \
	_tcscpy(ACTION.szFileName, NO_LETT(FILE_NAME)); \
	ACTION.ulTimestamp = TIME_STAMP; \
	WriteFile(ACTION_FILES, &ACTION, sizeof(struct Action), &dwWritenSize, NULL); \
	FILE_ICON_DISPLAY(FILE_NAME, CONV_ACTION_SETA(MODIFY)); \
}

#define DELETE_ACTION_LOGGER(ACTION_FILES, ACTION, FILE_NAME, TIME_STAMP) { \
	DWORD dwWritenSize; \
	ACTION.dwActioType = DELE; \
	_tcscpy(ACTION.szFileName, NO_LETT(FILE_NAME)); \
	ACTION.ulTimestamp = TIME_STAMP; \
	WriteFile(ACTION_FILES, &ACTION, sizeof(struct Action), &dwWritenSize, NULL); \
	FILE_ICON_DISPLAY(FILE_NAME, CONV_ACTION_SETA(DELE)); \
}

#define RENAME_ACTION_LOGGER(ACTION_FILES, ACTION, EXIST_NAME, NEW_NAME, TIME_STAMP) { \
	DWORD dwWritenSize; \
	ACTION.dwActioType = DELE; \
	_tcscpy(ACTION.szFileName, NO_LETT(EXIST_NAME)); \
	WriteFile(ACTION_FILES, &ACTION, sizeof(struct Action), &dwWritenSize, NULL); \
	FILE_ICON_DISPLAY(EXIST_NAME, CONV_ACTION_SETA(DELE)); \
	ACTION.dwActioType = ADDI; \
	_tcscpy(ACTION.szFileName, NO_LETT(NEW_NAME)); \
	ACTION.ulTimestamp = TIME_STAMP; \
	WriteFile(ACTION_FILES, &ACTION, sizeof(struct Action), &dwWritenSize, NULL); \
	FILE_ICON_DISPLAY(NEW_NAME, CONV_ACTION_SETA(ADDI)); \
}

//
struct Action {
	DWORD dwActioType;
	TCHAR szFileName[MAX_PATH];
	DWORD ulTimestamp;
};

namespace ActionUtility {
	int ActioCopy(HANDLE hTo, HANDLE hFrom);
	int EraseActio(HANDLE hErase);
	//
	VOID LockActioApp(HANDLE hLockActio, TCHAR *szFileName, DWORD dwActioType);
};
