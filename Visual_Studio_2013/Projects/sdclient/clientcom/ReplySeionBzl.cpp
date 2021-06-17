#include "StdAfx.h"
#include "ReplySeionBzl.h"

struct ValidSeion* NValSeionBzl::Initialize(struct ClientConfig *pClientConfig) {
	if(!pClientConfig) return NULL;
	//
	struct SeionBuffer *sbuffer = &objValidSeion.buffer;
	struct ListData *ldata = objValidSeion.ldata = &tListData;
	//
	sbuffer->head_buffer = sbuffer->srecv_buffer;
	sbuffer->body_buffer = NULL; // modify bu james 20100722
	//
	strcon::ustr_utf8(ldata->szVersion, pClientConfig->szVersion);
	strcon::ustr_utf8(ldata->szUserName, pClientConfig->szUserName);
	strcon::ustr_utf8(ldata->szPassword, pClientConfig->szPassword);
	strcon::ustr_utf8(ldata->szLocalIdenti, pClientConfig->szLocalIdenti);
	//
	return &objValidSeion;
}


DWORD NValSeionBzl::Finalize() {
	return 0x00;
}

struct ComandSeion* NCmdSeionBzl::Initialize(struct ClientConfig *pClientConfig) {
	if(!pClientConfig) return NULL;
	//
	struct SeionBuffer *sbuffer = &objCMDSeion.buffer;
	struct ListData *ldata = objCMDSeion.ldata = &tListData;
	//
	sbuffer->head_buffer = sbuffer->srecv_buffer;
	sbuffer->body_buffer = NULL; // modify bu james 20100722
	//
	strcon::ustr_utf8(ldata->szUserName, pClientConfig->szUserName);
	strcon::ustr_utf8(ldata->szPassword, pClientConfig->szPassword);
	strcon::ustr_utf8(ldata->szLocalIdenti, pClientConfig->szLocalIdenti);
	//
	return &objCMDSeion;
}


DWORD NCmdSeionBzl::Finalize() {
	return 0x00;
}