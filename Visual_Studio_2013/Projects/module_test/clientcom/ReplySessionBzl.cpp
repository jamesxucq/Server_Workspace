#include "StdAfx.h"
#include "ReplySessionBzl.h"

struct ValidateSession* NValSessionBzl::Initialize(struct ClientConfig *pClientConfig)
{
	if(!pClientConfig) return NULL;

	//////////////////////////////////////////////////////////////
	struct SessionBuffer *psbuffer = &objValidateSession.buffer;
	struct ListInform *list_inform = objValidateSession.list_inform = &tListInform;

	//////////////////////////////////////////////////////////////
	psbuffer->head_buffer = psbuffer->srecv_buffer;
	psbuffer->body_buffer = NULL; //modify bu james 20100722

	//////////////////////////////////////////////////////////////
	nstriconv::unicode_utf8(list_inform->szVersion, pClientConfig->szVersion);
	nstriconv::unicode_utf8(list_inform->szUserName, pClientConfig->szUserName);
	nstriconv::unicode_utf8(list_inform->szPassword, pClientConfig->szPassword);
	nstriconv::unicode_utf8(list_inform->szLocalIdenti, pClientConfig->szLocalIdenti);

	return &objValidateSession;
}


DWORD NValSessionBzl::Finalize()
{
	return 0;
}

struct CommandSession* NComSessionBzl::Initialize(struct ClientConfig *pClientConfig)
{
	if(!pClientConfig) return NULL;

	//////////////////////////////////////////////////////////////
	struct SessionBuffer *psbuffer = &objCommandSession.buffer;
	struct ListInform *list_inform = objCommandSession.list_inform = &tListInform;

	//////////////////////////////////////////////////////////////
	psbuffer->head_buffer = psbuffer->srecv_buffer;
	psbuffer->body_buffer = NULL; //modify bu james 20100722

	//////////////////////////////////////////////////////////////
	nstriconv::unicode_utf8(list_inform->szUserName, pClientConfig->szUserName);
	nstriconv::unicode_utf8(list_inform->szPassword, pClientConfig->szPassword);
	nstriconv::unicode_utf8(list_inform->szLocalIdenti, pClientConfig->szLocalIdenti);

	return &objCommandSession;
}


DWORD NComSessionBzl::Finalize()
{
	return 0;
}