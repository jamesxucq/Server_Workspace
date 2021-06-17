#pragma once

//#include "clientcom/clientcom.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define HANDLE_FINISH                   0x00000000 // stop
#define HANDLE_UNDONE                   0x00000001 // continue

#define OVERWRITE_STATUS_FAULT					-1
#define OVERWRITE_STATUS_SUCCESS				0x00000008
#define OVERWRITE_STATUS_FAILED					0x00000010

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
namespace NTRANSHandler {
	/////////////////////////////////////////////////
	//          below about sync opt
	DWORD HandleGetAnchorRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length);
	DWORD HandleGetListRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length);
	DWORD HandleGetFileRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length);
	DWORD HandleGetCsumRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length);

	DWORD HandlePostSend(struct TRANSSession *pTRANSSession, char *body_buffer, size_t *body_length);
	DWORD HandlePutFileSend(struct TRANSSession *pTRANSSession, char *body_buffer, size_t *body_length);

	DWORD HandlePutCsumRecv(struct TRANSSession *pTRANSSession, char *body_buffer, size_t body_length);
	DWORD HandlePutCsumSend(struct TRANSSession *pTRANSSession, char *body_buffer, size_t *body_length);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	DWORD HandleOverwriteModified(struct TaskNode *task_node, const TCHAR *szDriveRoot);
};
