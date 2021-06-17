#include "StdAfx.h"
#include "stdlib.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "AnchorXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<anchor version="1.2.1">
<uid>12345321</uid> 
<last_anchor>xxxxx</last_anchor>
</anchor>
*/

#define XML_ANCHOR_ROOT					"anchor"
#define XML_UID							"uid"
#define XML_LAST_ANCHOR					"last_anchor"

enum ANCHOR
{
	ENUM_UNKNOWD = 0,
	ENUM_ANCHOR_ROOT,
	ENUM_UID,
	ENUM_LAST_ANCHOR,
};

CAnchorXml::CAnchorXml(void)
{
	m_xmlAnchor.pXmlAnchor = NULL;
	m_xmlAnchor.Depth = 0;
	m_xmlAnchor.Element = ENUM_UNKNOWD;
}

CAnchorXml::~CAnchorXml(void)
{
}

CAnchorXml objAnchorXml;


static inline int GetID(const char*name){
	if(!strcmp(name, XML_ANCHOR_ROOT))	return ENUM_ANCHOR_ROOT;
	else if(!strcmp(name, XML_UID))	return ENUM_UID;
	else if(!strcmp(name, XML_LAST_ANCHOR))	return ENUM_LAST_ANCHOR;

	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_ANCHOR* xmlAnchor, const char *szValue)
{
	if(!szValue) return -1;

	struct XmlAnchor *pXmlAnchor = xmlAnchor->pXmlAnchor;
	switch(xmlAnchor->Element){
   case ENUM_ANCHOR_ROOT:
	   break;
   case ENUM_UID:
	   //if( !HasQoute) return -1;
	   pXmlAnchor->uiUID = atoi(szValue);
	   break;
   case ENUM_LAST_ANCHOR:
	   //if( !HasQoute) return -1;
	   pXmlAnchor->last_anchor = atoi(szValue);
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
	struct XML_ANCHOR* xmlAnchor = (struct XML_ANCHOR*) data;

	xmlAnchor->Element = GetID(el);
	xmlAnchor->Depth ++;
}

static void XMLCALL xmlend(void *data, const char *el)
{
	//  ������xml�ļ��Ľ���Ԫ��ʱ������������
	printf("end element:</%s>\n",el);
	((struct XML_ANCHOR *)data)->Element = ENUM_UNKNOWD;
	((struct XML_ANCHOR *)data)->Depth    --;
}

static void XMLCALL parsedata(void *udata, const XML_Char *stri, int length)
{
	// �������ݣ�ע������s������\0�����ģ�����ֻȡlength����
	char buffer[LINE_LENGTH];

	memset(buffer, '\0', sizeof(buffer));
	//ConvertFromUTF8(buffer, sizeof(buffer), s, length);
	strncpy_s(buffer, stri, length);
	printf("data is:[%s]\n", buffer);
	SetElementValue((struct XML_ANCHOR *)udata, buffer);
}

DWORD CAnchorXml::Initialize(struct XmlAnchor *pXmlAnchor)
{
	if(!pXmlAnchor) return -1;
	m_xmlAnchor.pXmlAnchor = pXmlAnchor;

	//  ����,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return -1;

	//  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);

	//  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
	XML_SetUserData(m_tXmlParser, &m_xmlAnchor);

	//  ���õ���������ʱʲô����,
	//  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
	//  ����"hello.gif"������parsedata,����/img������xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�

	return 0;
}

void CAnchorXml::Finalize()
{
	m_xmlAnchor.pXmlAnchor = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CAnchorXml::ParseAnchorXml(const char *buffer, int length)
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