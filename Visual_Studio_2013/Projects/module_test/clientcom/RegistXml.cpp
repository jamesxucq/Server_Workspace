#include "StdAfx.h"
#include "stdlib.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "RegistXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<register version="1.2.1">
<uid>12345321</uid>
<cache_address port="325">123.354.345.345</cache_address>
<access_key>2343ewr544r454rewfa45qwee</access_key>
</register>
*/

#define XML_REGIST_ROOT					"register"
#define XML_UID							"uid"
#define XML_CACHE_ADDRESS				"cache_address"
#define XML_CACHE_PORT					"port"
#define XML_ACCESS_KEY					"access_key"

enum REGISTER
{
	ENUM_UNKNOWD = 0,
	ENUM_REGIST_ROOT,
	ENUM_UID,
	ENUM_CACHE_ADDRESS,
	ENUM_CACHE_PORT,
	ENUM_ACCESS_KEY
};

CRegistXml::CRegistXml(void)
{
	m_xmlRegister.pXmlRegister = NULL;
	m_xmlRegister.Depth = 0;
	m_xmlRegister.Element = ENUM_UNKNOWD;
}

CRegistXml::~CRegistXml(void)
{
}

CRegistXml objRegistXml;

static inline int GetID(const char*name){
	if(!strcmp(name, XML_REGIST_ROOT))	return ENUM_REGIST_ROOT;
	else if(!strcmp(name, XML_UID))	return ENUM_UID;
	else if(!strcmp(name, XML_CACHE_ADDRESS))	return ENUM_CACHE_ADDRESS;
	else if(!strcmp(name, XML_CACHE_PORT))	return ENUM_CACHE_PORT;
	else if(!strcmp(name, XML_ACCESS_KEY))	return ENUM_ACCESS_KEY;

	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_REGIST* xmlRegist, const char *szValue)
{
	if(!szValue) return -1;

	struct XmlRegister *pXmlRegister = xmlRegist->pXmlRegister;
	switch(xmlRegist->Element){
   case ENUM_REGIST_ROOT:
	   break;
   case ENUM_UID:
	   pXmlRegister->uid = atoi(szValue);
	   break;
   case ENUM_CACHE_ADDRESS:
	   strcpy_s(pXmlRegister->cache_address.sin_addr, szValue);
	   break;
   case ENUM_CACHE_PORT:
	   pXmlRegister->cache_address.sin_port = atoi(szValue);
	   break;
   case ENUM_ACCESS_KEY:
	   strcpy_s(pXmlRegister->access_key, szValue);
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
	struct XML_REGIST* xmlRegist = (struct XML_REGIST*) data;

	for( int index=0; attr[index]; index+=2 ){
		xmlRegist->Element = GetID(attr[index]);
		SetElementValue(xmlRegist,attr[index+1]);
	}

	xmlRegist->Element = GetID(el);
	xmlRegist->Depth ++;
}

static void XMLCALL xmlend(void *data, const char *el)
{
	//  ������xml�ļ��Ľ���Ԫ��ʱ������������
	printf("end element:</%s>\n",el);
	((struct XML_REGIST *)data)->Element = ENUM_UNKNOWD;
	((struct XML_REGIST *)data)->Depth --;
}

static void XMLCALL parsedata(void *udata, const XML_Char *stri, int length)
{
	// �������ݣ�ע������stri������\0�����ģ�����ֻȡlength����
	char buf[LINE_LENGTH];

	memset(buf, '\0', sizeof(buf));
	//ConvertFromUTF8(buf, sizeof(buf), stri, length);
	strncpy_s(buf, stri, length);
	printf("data is:[%s]\n", buf);
	SetElementValue((struct XML_REGIST *)udata, buf);
}

DWORD CRegistXml::Initialize(struct XmlRegister *pXmlRegister)
{
	if(!pXmlRegister) return -1;
	m_xmlRegister.pXmlRegister = pXmlRegister;

	//  ����,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return -1;

	//  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);

	//  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
	XML_SetUserData(m_tXmlParser, &m_xmlRegister);

	//  ���õ���������ʱʲô����,
	//  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
	//  ����"hello.gif"������parsedata,����/img������xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�

	return 0;
}

void CRegistXml::Finalize()
{
	m_xmlRegister.pXmlRegister = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CRegistXml::ParseRegistXml(const char *buffer, int length)
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