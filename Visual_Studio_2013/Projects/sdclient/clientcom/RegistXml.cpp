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
<cache address="123.354.345.345" port="325" />
<access_key>2343ewr544r454rewfa45qwee</access_key>
</register>
 */

#define XML_REGIST_ROOT					"register"
#define XML_UID							"uid"
#define XML_CACHE_ADDR					"address"
#define XML_CACHE_PORT					"port"
#define XML_ACCESS_KEY					"access_key"

enum REGISTER {
	ENUM_UNKNOWD = 0,
	ENUM_REGIST_ROOT,
	ENUM_UID,
	ENUM_CACHE_ADDR,
	ENUM_CACHE_PORT,
	ENUM_ACCESS_KEY
};

CRegistXml::CRegistXml(void) {
	m_tXmlRegister.pXmlRegister = NULL;
	m_tXmlRegister.Depth = 0;
	m_tXmlRegister.Element = ENUM_UNKNOWD;
}

CRegistXml::~CRegistXml(void) {
}

CRegistXml objRegistXml;

static inline int GetID(const char*name) {
	if(!strcmp(name, XML_REGIST_ROOT))	return ENUM_REGIST_ROOT;
	else if(!strcmp(name, XML_UID))	return ENUM_UID;
	else if(!strcmp(name, XML_CACHE_ADDR))	return ENUM_CACHE_ADDR;
	else if(!strcmp(name, XML_CACHE_PORT))	return ENUM_CACHE_PORT;
	else if(!strcmp(name, XML_ACCESS_KEY))	return ENUM_ACCESS_KEY;
//
	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_REGIST* xmlRegist, const char *szValue) {
	if(!szValue) return -1;
//
	struct XmlRegister *pXmlRegister = xmlRegist->pXmlRegister;
	switch(xmlRegist->Element){
   case ENUM_REGIST_ROOT:
	   break;
   case ENUM_UID:
	   pXmlRegister->uid = atoi(szValue);
	   break;
   case ENUM_CACHE_ADDR:
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
//
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
static VOID XMLCALL xmlstart(VOID *data, const char *el, const char **attr) {
	//  ������xmlԪ�صĿ�ʼ��־ʱ������������,���Կ���ӡ��ʾ�Ľ��
	fprintf(stderr, "start element:<%s>\n",el);
	struct XML_REGIST* xmlRegist = (struct XML_REGIST*) data;
//
	for( int inde=0; attr[inde]; inde+=2 ) {
		xmlRegist->Element = GetID(attr[inde]);
		SetElementValue(xmlRegist,attr[inde+1]);
	}
//
	xmlRegist->Element = GetID(el);
	xmlRegist->Depth ++;
}

static VOID XMLCALL xmlend(VOID *data, const char *el) {
	//  ������xml�ļ��Ľ���Ԫ��ʱ������������
	fprintf(stderr, "end element:</%s>\n",el);
	((struct XML_REGIST *)data)->Element = ENUM_UNKNOWD;
	((struct XML_REGIST *)data)->Depth --;
}

static VOID XMLCALL parsedata(VOID *data, const XML_Char *stri, int length) {
	// �������ݣ�ע������stri������\0�����ģ�����ֻȡlength����
	char buffer[LINE_LENGTH];
	// ConvertFromUTF8(buf, sizeof(buf), stri, length);
	strncpy_s(buffer, stri, length);
	buffer[length] = '\0';
	fprintf(stderr, "data is:[%s]\n", buffer);
	SetElementValue((struct XML_REGIST *)data, buffer);
}

DWORD CRegistXml::Initialize(struct XmlRegister *pXmlRegister) {
	if(!pXmlRegister) return ((DWORD)-1);
	m_tXmlRegister.pXmlRegister = pXmlRegister;
//
	//  ����,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return ((DWORD)-1);
//
	//  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);
//
	//  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
	XML_SetUserData(m_tXmlParser, &m_tXmlRegister);
//
	//  ���õ���������ʱʲô����,
	//  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
	//  ����"hello.gif"������parsedata,����/img������xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�
//
	return 0x00;
}

VOID CRegistXml::Finalize() {
	m_tXmlRegister.pXmlRegister = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CRegistXml::ParseRegistXml(const char *buffer, unsigned int length) {
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