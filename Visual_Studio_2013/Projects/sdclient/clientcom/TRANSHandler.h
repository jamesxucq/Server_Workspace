#pragma once

//#include "clientcom/clientcom.h"

//
namespace NTRANSHandler { // below about sync opt
#define HANDLE_FAULT                    -1 // continue
#define HANDLE_FINISH                   0x0000 // stop
#define HANDLE_UNDONE                   0x0001 // continue
	int HandleGetAnchorRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length);
	int HandleGetListRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length);
	int HandleGetFileRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length);
	int HandleGetChksRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length);
	//
	int HandlePostSend(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t *body_length);
	int HandlePutFileSend(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t *body_length);
	//
	int HandlePutChksRecv(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t body_length);
	int HandlePutChksSend(struct TRANSSession *pTRANSSeion, char *body_buffer, size_t *body_length);
	//
#define OWRITE_STATUS_FAULT				((DWORD)-1)
#define OWRITE_STATUS_SUCCESS			0x0000
#define OWRITE_STATUS_FAILED			0x0001
	DWORD HandleOwriteModified(struct TaskNode *task_node, const TCHAR *szDriveLetter);
};
