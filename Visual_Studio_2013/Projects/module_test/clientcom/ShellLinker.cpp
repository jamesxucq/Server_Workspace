#include "StdAfx.h"
#include "ShellLinker.h"

#define LINK_WRITE_TIMEOUT		3200

CShellLinker::CShellLinker(void):
creat_ok(false)
{
}

CShellLinker::~CShellLinker(void) {
}

void CShellLinker::LinkOpen(TCHAR *szLinkName) {
	// Create the IPC client
	creat_ok = client.create(szLinkName);
}

void CShellLinker::CloseLink() {
	creat_ok = false;
	client.close();
}


DWORD CShellLinker::LinkSend(char *szMessage, size_t length) {
	// Write some data
	return creat_ok? client.write(szMessage, length, LINK_WRITE_TIMEOUT): 0;
}

CCommandLinker::CCommandLinker(void) {
}

CCommandLinker::~CCommandLinker(void) {
}
CCommandLinker objCommandLinker;

DWORD CCommandLinker::Initialize() {
	LinkOpen(IPC_COMMAND_SHARED);
	return 0;
}

DWORD CCommandLinker::Finalize() {
	CloseLink();
	return 0;
}

DWORD CCommandLinker::SendCommand(DWORD Command) {
	struct CacheCommand tCommand;

	memset(&tCommand, '\0', sizeof(struct CacheCommand));
	tCommand.file_status = Command;

	// Write some data
	return creat_ok? client.write(&tCommand, sizeof(struct CacheCommand), LINK_WRITE_TIMEOUT): 0;
}

DWORD CCommandLinker::SendCommand(DWORD Command, const TCHAR *path) {
	struct CacheCommand tCommand;

	memset(&tCommand, '\0', sizeof(struct CacheCommand));
	_tcscpy_s(tCommand.path, MAX_PATH, path);
	tCommand.file_status = Command;

	// Write some data
	return creat_ok? client.write(&tCommand, sizeof(struct CacheCommand), LINK_WRITE_TIMEOUT): 0;
}

