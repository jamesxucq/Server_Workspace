#include "StdAfx.h"

#include "clientcom.h"
#include "third_party.h"

#include "RegistXml.h"
#include "SettingsXml.h"
#include "QueryXml.h"

#include "ReplyHandler.h"

DWORD NReplyHandler::HandleRegistRecv(struct RegistValue *pRegistValue, char *regist_xml, int length) 
{
	struct XmlRegister tXmlRegister;

	objRegistXml.Initialize(&tXmlRegister);
	if(objRegistXml.ParseRegistXml(regist_xml, length)) return -1;
	objRegistXml.Finalize();

	// LOG_TRACE_ANSI("uid: %d", tXmlRegister.uid);
	LOG_TRACE_ANSI("cache_host: %s", tXmlRegister.cache_address.sin_addr);
	// LOG_TRACE_ANSI("cache_port: %d", tXmlRegister.cache_address.sin_port);
	LOG_TRACE_ANSI("access_key: %s", tXmlRegister.access_key);

	pRegistValue->uiUID = tXmlRegister.uid;
	strcpy_s(pRegistValue->cache_address.sin_addr, HOSTNAME_LENGTH, tXmlRegister.cache_address.sin_addr);
	pRegistValue->cache_address.sin_port = tXmlRegister.cache_address.sin_port;
	strcpy_s(pRegistValue->access_key, KEY_LENGTH, tXmlRegister.access_key);

	return 0;
}

DWORD NReplyHandler::HandleSettingsRecv(struct ClientValue *pClientValue, char *cinform_xml, int length)
{
	struct XmlSettings tXmlSettings;

	objSettingsXml.Initialize(&tXmlSettings);
	if(objSettingsXml.ParseSettingsXml(cinform_xml, length)) return -1;
	objSettingsXml.Finalize();

	LOG_TRACE_ANSI("client_version: %s", tXmlSettings.client_version);
	// LOG_TRACE_ANSI("pool_size: %d", tXmlSettings.pool_size);

	pClientValue->uiUID = tXmlSettings.uid;
	nstriconv::ansi_unicode(pClientValue->client_version, tXmlSettings.client_version);
	pClientValue->pool_size = tXmlSettings.pool_size;

	return 0;
}

DWORD NReplyHandler::HandleQueryRecv(struct QueryValue *pQueryValue, char *query_xml, int length)
{
	struct XmlQuery tXmlQuery;

	objQueryXml.Initialize(&tXmlQuery);
	if(objQueryXml.ParseQueryXml(query_xml, length)) return -1;
	objQueryXml.Finalize();

	LOG_TRACE_ANSI("query body txt:%s", query_xml);
	LOG_TRACE_ANSI("session_id: %s", tXmlQuery.session_id);
	// LOG_TRACE_ANSI("serv_locked: %d", tXmlQuery.serv_locked);
	// LOG_TRACE_ANSI("cached_anchor: %d", tXmlQuery.cached_anchor);

	pQueryValue->uiUID = tXmlQuery.uiUID;
	strcpy_s(pQueryValue->address.sin_addr, HOSTNAME_LENGTH, tXmlQuery.address.sin_addr);
	pQueryValue->address.sin_port = tXmlQuery.address.sin_port;
	if(strcmp("true", tXmlQuery.serv_locked)) pQueryValue->serv_locked = 0;
	else pQueryValue->serv_locked = 1;
	pQueryValue->cached_anchor = tXmlQuery.cached_anchor;
	strcpy_s(pQueryValue->session_id, SESSION_LENGTH, tXmlQuery.session_id);

	return 0;
}