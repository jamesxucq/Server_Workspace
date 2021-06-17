#include "StdAfx.h"
#include "stdlib.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "SettingsXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<settings version="1.2.1">
<client_version>1.12</client_version>
<uid>12345321</uid>
<pool_size>1</pool_size>
</settings>
*/

#define XML_SETTINGS_ROOT		"settings"
#define XML_AGENT_VERSION		"client_version"
#define XML_UID					"uid"
#define XML_SPACE_SIZE			"pool_size"

enum SETTINGS
{
	ENUM_UNKNOWD = 0,
	ENUM_SETTINGS_ROOT,
	ENUM_AGENT_VERSION,
	ENUM_UID,
	ENUM_SPACE_SIZE
};

CSettingsXml::CSettingsXml(void)
{
	m_xmlSettingsXml.pXmlSettings = NULL;
	m_xmlSettingsXml.Depth = 0;
	m_xmlSettingsXml.Element = ENUM_UNKNOWD;
}

CSettingsXml::~CSettingsXml(void)
{
}

CSettingsXml objSettingsXml;

static inline int GetID(const char*name){
	if(!strcmp(name, XML_SETTINGS_ROOT))	return ENUM_SETTINGS_ROOT;
	else if(!strcmp(name, XML_AGENT_VERSION))	return ENUM_AGENT_VERSION;
	else if(!strcmp(name, XML_UID))	return ENUM_UID;
	else if(!strcmp(name, XML_SPACE_SIZE))	return ENUM_SPACE_SIZE;

	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_SETTINGS* xmlSettings, const char *szValue)
{
	if(!szValue) return -1;

	struct XmlSettings *pXmlSettings = xmlSettings->pXmlSettings;
	switch(xmlSettings->Element){
   case ENUM_SETTINGS_ROOT:
	   break;
   case ENUM_AGENT_VERSION:
	   strcpy_s(pXmlSettings->client_version, szValue);
	   break;
   case ENUM_UID:
	   pXmlSettings->uid = atoi(szValue);
	   break;
   case ENUM_SPACE_SIZE:
	   pXmlSettings->pool_size = atoi(szValue);
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
	struct XML_SETTINGS* xmlSettings = (struct XML_SETTINGS*) data;

	xmlSettings->Element = GetID(el);
	xmlSettings->Depth ++;
}

static void XMLCALL xmlend(void *data, const char *el)
{
	//  ������xml�ļ��Ľ���Ԫ��ʱ������������
	printf("end element:</%s>\n",el);
	((struct XML_SETTINGS *)data)->Element = ENUM_UNKNOWD;
	((struct XML_SETTINGS *)data)->Depth    --;
}

static void XMLCALL parsedata(void *udata, const XML_Char *stri, int length)
{
	// �������ݣ�ע������stri������\0�����ģ�����ֻȡlength����
	char buffer[LINE_LENGTH];

	memset(buffer, '\0', sizeof(buffer));
	//ConvFromUTF8(buffer, sizeof(buffer), stri, length);
	strncpy_s(buffer, stri, length);
	printf("data is:[%s]\n", buffer);
	SetElementValue((struct XML_SETTINGS *)udata, buffer);
}

DWORD CSettingsXml::Initialize(struct XmlSettings *pXmlSettings)
{
	if(!pXmlSettings) return -1;
	m_xmlSettingsXml.pXmlSettings = pXmlSettings;

	//  ����,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return -1;

	//  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);

	//  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
	XML_SetUserData(m_tXmlParser, &m_xmlSettingsXml);

	//  ���õ���������ʱʲô����,
	//  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
	//  ����"hello.gif"������parsedata,����/img������xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�

	return 0;
}

void CSettingsXml::Finalize()
{
	m_xmlSettingsXml.pXmlSettings = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CSettingsXml::ParseSettingsXml(const char *buffer, int length)
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