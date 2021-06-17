#pragma once

#include "Session.h"
#include "ParseConfig.h"

struct RegistValue {
	UINT uiUID;
	serv_address cache_address;;
	char access_key[KEY_LENGTH];
};

struct ClientValue {
	TCHAR client_version[VERSION_LENGTH];
	UINT uiUID;
	int pool_size;
	int data_bomb;
};

struct QueryValue {
	UINT uiUID;
	char req_valid[VALID_LENGTH];
	serv_address address;
	// DWORD worker_locked; // 0:false 1:true
	DWORD cached_anchor;
	char session_id[SESSION_LENGTH];
};

namespace NReplyHandler {
	DWORD HandleRegistRecv(struct RegistValue *pRegistValue, char *regist_xml, int length);
	DWORD HandleSettingsRecv(struct ClientValue *pClientValue, char *cinform_xml, int length);
	DWORD HandleQueryRecv(struct QueryValue *pQueryValue, char *query_xml, int length);
};
