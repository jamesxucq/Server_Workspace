#include "StdAfx.h"
#include "stdlib.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "QueryXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<uid>2</uid>
<validation>VB48uIcNsS9JvOaY0btu6Kaqmo1wLGpY</validation>
<worker address="192.168.1.103" port="8090" />
<cached_anchor>0</cached_anchor>
<session_id>9209-1348697447-2</session_id>
</query>
*/

#define XML_QUERY_ROOT					"query"
#define XML_UID							"uid"
#define XML_REQVALID					"validation"
#define XML_WORKER_ADDR					"address"
#define XML_WORKER_PORT					"port"
#define XML_CACHE_ANCHOR				"cached_anchor"
#define XML_SESSION_ID					"session_id"

enum QUERY {
	ENUM_UNKNOWD = 0,
	ENUM_QUERY_ROOT,
	ENUM_UID,
	ENUM_REQVALID,
	ENUM_WORKER_ADDR,
	ENUM_WORKER_PORT,
	ENUM_CACHE_ANCHOR,
	ENUM_SESSION_ID
};


CQueryXml::CQueryXml(void) {
	m_tXmlQuery.pXmlQuery = NULL;
	m_tXmlQuery.Depth = 0;
	m_tXmlQuery.Element = ENUM_UNKNOWD;
}

CQueryXml::~CQueryXml(void) {
}

CQueryXml objQueryXml;

static inline int GetID(const char*name) {
	if(!strcmp(name, XML_QUERY_ROOT))	return ENUM_QUERY_ROOT;
	else if(!strcmp(name, XML_UID))			return ENUM_UID;
	else if(!strcmp(name, XML_REQVALID))	return ENUM_REQVALID;
	else if(!strcmp(name, XML_WORKER_ADDR))	return ENUM_WORKER_ADDR;
	else if(!strcmp(name, XML_WORKER_PORT))	return ENUM_WORKER_PORT;
	else if(!strcmp(name, XML_CACHE_ANCHOR))	return ENUM_CACHE_ANCHOR;
	else if(!strcmp(name, XML_SESSION_ID))	return ENUM_SESSION_ID;
//
	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_QUERY* xmlQuery, const char *szValue) {
	if(!szValue) return -1;
//
	struct XmlQuery *pXmlQuery = xmlQuery->pXmlQuery;
	switch(xmlQuery->Element){
   case ENUM_QUERY_ROOT:
	   break;
   case ENUM_UID:
	   pXmlQuery->uiUID = atoi(szValue);
	   break;
   case ENUM_REQVALID:
	   strcpy_s(pXmlQuery->req_valid, szValue);
	   break;
   case ENUM_WORKER_ADDR:
	   strcpy_s(pXmlQuery->address.sin_addr, szValue);
	   break;
   case ENUM_WORKER_PORT:
	   pXmlQuery->address.sin_port = atoi(szValue);
	   break;
   case ENUM_CACHE_ANCHOR:
	   pXmlQuery->cached_anchor = atoi(szValue);
	   break;
   case ENUM_SESSION_ID:
	   strcpy_s(pXmlQuery->session_id, szValue);
	   break;
   default:    
	   break;
	}
//
	return 0;
}


/*----------------------------------------------------------------------
*
*  xml解析函数,
*  以下参数说明
*  data是使用XML_SetUserData设置的参数,expat不进行处理,会把它交给用户回调函数处理
*  el是元素名
*  attr是属性-值列表,样子为attr[0]=attr[1],最后一个是NULL
*  
*----------------------------------------------------------------------*/
static VOID XMLCALL xmlstart(VOID *data, const char *el, const char **attr) {
	//  当碰到xml元素的开始标志时会调用这个函数,可以看打印显示的结果
	fprintf(stderr, "start element:<%s>\n",el);
	struct XML_QUERY* xmlQuery = (struct XML_QUERY*) data;
//
	for( int inde=0; attr[inde]; inde+=2 ) {
		xmlQuery->Element = GetID(attr[inde]);
		SetElementValue(xmlQuery,attr[inde+1]);
	}
//
	xmlQuery->Element = GetID(el);
	xmlQuery->Depth ++;
}

static VOID XMLCALL xmlend(VOID *data, const char *el) {
	//  当碰到xml文件的结束元素时会调用这个函数
	fprintf(stderr, "end element:</%s>\n",el);
	((struct XML_QUERY *)data)->Element = ENUM_UNKNOWD;
	((struct XML_QUERY *)data)->Depth --;
}

static VOID XMLCALL parsedata(VOID *data, const XML_Char *stri, int length) {
	// 处理数据，注意这里s不是以\0结束的，而是只取length长度
	char buffer[LINE_LENGTH];
	// ConvertFromUTF8(buffer, sizeof(buffer), stri, length);
	strncpy_s(buffer, stri, length);
	buffer[length] = '\0';
	fprintf(stderr, "data is:[%s]\n", buffer);
	SetElementValue((struct XML_QUERY *)data, buffer);
}

DWORD CQueryXml::Initialize(struct XmlQuery *pXmlQuery) {
	if(!pXmlQuery) return ((DWORD)-1);
	m_tXmlQuery.pXmlQuery = pXmlQuery;
//
	//  解析,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return ((DWORD)-1);
//
	//  设置事件处理函数,当碰到开始元素和结可元素时调用哪个函数来处理
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);
//
	//  设置用户数据，如果需要传给处理函数附加的信息，那就使用它，不然就不用
	XML_SetUserData(m_tXmlParser, &m_tXmlQuery);
//
	//  设置当碰到数据时什么处理,
	//  如<img> "hello.gif"</img> 那么碰到img时会调用xmlstart,
	//  读完"hello.gif"后会调用parsedata,碰到/img后会调用xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//添加这个,处理不同字符集的xml文件
//
	return 0x00;
}

VOID CQueryXml::Finalize() {
	m_tXmlQuery.pXmlQuery = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CQueryXml::ParseQueryXml(const char *buffer, unsigned int length) {
	int done = 0;
	if (XML_Parse(m_tXmlParser, buffer, length, done) == XML_STATUS_ERROR) {
		fprintf(stderr, "%s at line %u\n",
			XML_ErrorString(XML_GetErrorCode(m_tXmlParser)),
			XML_GetCurrentLineNumber(m_tXmlParser));
		return ((DWORD)-1);
	}
//
	return 0x00;
}