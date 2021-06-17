#include "StdAfx.h"
#include "stdlib.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "QueryXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<uid>12345321</uid>
<address port="325">123.354.345.345</address>
<serv_locked>true</serv_locked>
<cached_anchor>1</cached_anchor>
<session_id>123456789</session_id>
</query>
 */

#define XML_QUERY_ROOT					"query"
#define XML_UID							"uid"
#define XML_SERV_ADDRESS				"address"
#define XML_SERV_PORT					"port"
#define XML_SERV_LOCKED					"serv_locked"
#define XML_CACHE_ANCHOR				"cached_anchor"
#define XML_SESSION_ID					"session_id"

enum QUERY
{
	ENUM_UNKNOWD = 0,
	ENUM_QUERY_ROOT,
	ENUM_UID,
	ENUM_SERV_ADDRESS,
	ENUM_SERV_PORT,
	ENUM_SERV_LOCKED,
	ENUM_CACHE_ANCHOR,
	ENUM_SESSION_ID
};


CQueryXml::CQueryXml(void)
{
	m_xmlQuery.pXmlQuery = NULL;
	m_xmlQuery.Depth = 0;
	m_xmlQuery.Element = ENUM_UNKNOWD;
}

CQueryXml::~CQueryXml(void)
{
}

CQueryXml objQueryXml;

static inline int GetID(const char*name){
	if(!strcmp(name, XML_QUERY_ROOT))	return ENUM_QUERY_ROOT;
	else if(!strcmp(name, XML_UID))			return ENUM_UID;
	else if(!strcmp(name, XML_SERV_ADDRESS))	return ENUM_SERV_ADDRESS;
	else if(!strcmp(name, XML_SERV_PORT))	return ENUM_SERV_PORT;
	else if(!strcmp(name, XML_SERV_LOCKED))	return ENUM_SERV_LOCKED;
	else if(!strcmp(name, XML_CACHE_ANCHOR))	return ENUM_CACHE_ANCHOR;
	else if(!strcmp(name, XML_SESSION_ID))	return ENUM_SESSION_ID;

	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_QUERY* xmlQuery, const char *szValue)
{
	if(!szValue) return -1;

	struct XmlQuery *pXmlQuery = xmlQuery->pXmlQuery;
	switch(xmlQuery->Element){
   case ENUM_QUERY_ROOT:
	   break;
   case ENUM_UID:
	   pXmlQuery->uiUID = atoi(szValue);
	   break;
   case ENUM_SERV_ADDRESS:
	   strcpy_s(pXmlQuery->address.sin_addr, szValue);
	   break;
   case ENUM_SERV_PORT:
	   pXmlQuery->address.sin_port = atoi(szValue);
	   break;
   case ENUM_SERV_LOCKED:
	   strcpy_s(pXmlQuery->serv_locked, szValue);
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

	return 0;
}


/*----------------------------------------------------------------------
*
*  xml��������,
*  ���²���˵��
*  data��ʹ��XML_SetUserData���õĲ���,expat�����д���,����������û��ص���������
*  el��Ԫ����
*  attr������-ֵ�б�,����Ϊattr[0]=attr[1],���һ����NULL
*  
*----------------------------------------------------------------------*/
static void XMLCALL xmlstart(void *data, const char *el, const char **attr)
{
	//  ������xmlԪ�صĿ�ʼ��־ʱ������������,���Կ���ӡ��ʾ�Ľ��
	printf("start element:<%s>\n",el);
	struct XML_QUERY* xmlQuery = (struct XML_QUERY*) data;

	for( int index=0; attr[index]; index+=2 ){
		xmlQuery->Element = GetID(attr[index]);
		SetElementValue(xmlQuery,attr[index+1]);
	}

	xmlQuery->Element = GetID(el);
	xmlQuery->Depth ++;
}

static void XMLCALL xmlend(void *data, const char *el)
{
	//  ������xml�ļ��Ľ���Ԫ��ʱ������������
	printf("end element:</%s>\n",el);
	((struct XML_QUERY *)data)->Element = ENUM_UNKNOWD;
	((struct XML_QUERY *)data)->Depth --;
}

static void XMLCALL parsedata(void *udata, const XML_Char *stri, int length)
{
	// �������ݣ�ע������s������\0�����ģ�����ֻȡlength����
	char buffer[LINE_LENGTH];

	memset(buffer, '\0', sizeof(buffer));
	//ConvertFromUTF8(buffer, sizeof(buffer), stri, length);
	strncpy_s(buffer, stri, length);
	printf("data is:[%s]\n", buffer);
	SetElementValue((struct XML_QUERY *)udata, buffer);
}

DWORD CQueryXml::Initialize(struct XmlQuery *pXmlQuery)
{
	if(!pXmlQuery) return -1;
	m_xmlQuery.pXmlQuery = pXmlQuery;

	//  ����,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return -1;

	//  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);

	//  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
	XML_SetUserData(m_tXmlParser, &m_xmlQuery);

	//  ���õ���������ʱʲô����,
	//  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
	//  ����"hello.gif"������parsedata,����/img������xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�

	return 0;
}

void CQueryXml::Finalize()
{
	m_xmlQuery.pXmlQuery = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CQueryXml::ParseQueryXml(const char *buffer, int length)
{
	int done = 0;
	if (XML_Parse(m_tXmlParser, buffer, length, done) == XML_STATUS_ERROR) {
		fprintf(stderr, "%s at line %u\n",
			XML_ErrorString(XML_GetErrorCode(m_tXmlParser)),
			XML_GetCurrentLineNumber(m_tXmlParser));
		return -1;
	}

	return 0;
}