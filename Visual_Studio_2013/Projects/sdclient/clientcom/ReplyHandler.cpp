#include "StdAfx.h"

#include "clientcom.h"
#include "third_party.h"

#include "RegistXml.h"
#include "SettingsXml.h"
#include "QueryXml.h"

#include "ReplyHandler.h"

DWORD NReplyHandler::HandleRegistRecv(struct RegistValue *pRegistValue, char *regist_xml, int length) {
	struct XmlRegister tXmlRegister = {0};
	objRegistXml.Initialize(&tXmlRegister);
	if(objRegistXml.ParseRegistXml(regist_xml, length)) return ((DWORD)-1);
	objRegistXml.Finalize();
	// _LOG_ANSI("uid: %d", tXmlRegister.uid);
// _LOG_ANSI("cache_host: %s", tXmlRegister.cache_address.sin_addr);
	// _LOG_ANSI("cache_port: %d", tXmlRegister.cache_address.sin_port);
// _LOG_ANSI("access_key: %s", tXmlRegister.access_key);
	pRegistValue->uiUID = tXmlRegister.uid;
	strcpy_s(pRegistValue->cache_address.sin_addr, HOSTNAME_LENGTH, tXmlRegister.cache_address.sin_addr);
	pRegistValue->cache_address.sin_port = tXmlRegister.cache_address.sin_port;
	strcpy_s(pRegistValue->access_key, KEY_LENGTH, tXmlRegister.access_key);
	//
	return 0x00;
}

struct value_nod {
	const char *text;
	int value;
};

#define TEXT_VALUE(VALUE, TABLE, TEXT) { \
struct value_nod *item; \
	for(item = (struct value_nod *)TABLE; item->text; ++item) \
	if(!strcmp(item->text, TEXT)) \
	break; \
	VALUE = item->value; \
}

static const struct value_nod bool_table[] = {
	{"false", 0},
	{"true", 1},
	{NULL, 0}
};

DWORD NReplyHandler::HandleSettingsRecv(struct ClientValue *pClientValue, char *cinform_xml, int length) {
	struct XmlSettings tXmlSettings = {0};
	//
	objSettingsXml.Initialize(&tXmlSettings);
	if(objSettingsXml.ParseSettingsXml(cinform_xml, length)) return ((DWORD)-1);
	objSettingsXml.Finalize();
// _LOG_ANSI("client_version: %s", tXmlSettings.client_version);
// _LOG_ANSI("pool_size: %d", tXmlSettings.pool_size);
	pClientValue->uiUID = tXmlSettings.uid;
	strcon::ansi_ustr(pClientValue->client_version, tXmlSettings.client_version);
	pClientValue->pool_size = tXmlSettings.pool_size;
	TEXT_VALUE(pClientValue->data_bomb, bool_table, tXmlSettings.data_bomb)
	//
	return 0x00;
}

DWORD NReplyHandler::HandleQueryRecv(struct QueryValue *pQueryValue, char *query_xml, int length) {
	struct XmlQuery tXmlQuery = {0};
	//
	objQueryXml.Initialize(&tXmlQuery);
	if(objQueryXml.ParseQueryXml(query_xml, length)) return ((DWORD)-1);
	objQueryXml.Finalize();
// _LOG_ANSI("query body txt:%s", query_xml);
// _LOG_ANSI("req_valid: %s", tXmlQuery.req_valid);
// _LOG_ANSI("session_id: %s", tXmlQuery.session_id);
// _LOG_ANSI("worker_locked: %d", tXmlQuery.worker_locked);
	pQueryValue->uiUID = tXmlQuery.uiUID;
	strcpy_s(pQueryValue->req_valid, VALID_LENGTH, tXmlQuery.req_valid);
	strcpy_s(pQueryValue->address.sin_addr, HOSTNAME_LENGTH, tXmlQuery.address.sin_addr);
	pQueryValue->address.sin_port = tXmlQuery.address.sin_port;
	pQueryValue->cached_anchor = tXmlQuery.cached_anchor;
	strcpy_s(pQueryValue->session_id, SESSION_LENGTH, tXmlQuery.session_id);
	//
	return 0x00;
}