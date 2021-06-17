#include "StdAfx.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "DiresXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<directories version="1.2.1">
<dir_name>xxxxxxxxxx</dir_name>
<dir_name>xxxxxxxxxx</dir_name>
</directories>
*/

#define XML_DIRIES_ROOT			"directories"
#define XML_DIR_NAME			"dir_name"

enum DIR_NAME
{
	ENUM_UNKNOWD = 0,
	ENUM_DIRIES_ROOT,
	ENUM_DIR_NAME,
};

CDiresXml::CDiresXml(void)
{
	m_xmlDiresName.pDiresVec = NULL;
	m_xmlDiresName.Depth = 0;
	m_xmlDiresName.Element = ENUM_UNKNOWD;
}

CDiresXml::~CDiresXml(void)
{
}

CDiresXml objDiresXml;



static inline int GetID(const char*name){
	if(!strcmp(name,XML_DIRIES_ROOT))			return ENUM_DIRIES_ROOT;
	else if(!strcmp(name,XML_DIR_NAME))				return ENUM_DIR_NAME;

	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_DIRIES_NAME* xmlDiresName, const char *szValue)
{
	if(!szValue) return -1;

	TCHAR *dirname = xmlDiresName->current_directory;
	switch(xmlDiresName->Element){
   case ENUM_DIRIES_ROOT:
	   break;
   case ENUM_DIR_NAME:
	   nstriconv::utf8_unicode(dirname, (char *)szValue);
	   nstriutility::path_unix_windows(dirname);
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
	struct XML_DIRIES_NAME* xmlDiresName = (struct XML_DIRIES_NAME*) data;

	int iElement = GetID(el);
	if(ENUM_DIR_NAME == iElement) {
		xmlDiresName->current_directory = NDiresVec::AddNewDiresNode(*(xmlDiresName->pDiresVec));
	}
	xmlDiresName->Element = iElement;
	xmlDiresName->Depth ++;
}

static void XMLCALL xmlend(void *data, const char *el)
{
	//  ������xml�ļ��Ľ���Ԫ��ʱ������������
	printf("end element:</%s>\n",el);
	((struct XML_DIRIES_NAME *)data)->Element = ENUM_UNKNOWD;
	((struct XML_DIRIES_NAME *)data)->Depth    --;
}

static void XMLCALL parsedata(void *udata, const XML_Char *stri, int length)
{
	// �������ݣ�ע������s������\0�����ģ�����ֻȡlength����
	char buffer[LINE_LENGTH];

	memset(buffer, '\0', sizeof(buffer));
	//ConvFromUTF8(buffer, sizeof(buffer), stri, length);
	strncpy_s(buffer, stri, length);
	printf("data is:[%s]\n", buffer);
	SetElementValue((struct XML_DIRIES_NAME *)udata, buffer);
}

DWORD CDiresXml::Initialize(DiresVec *pDiresVec)
{
	if(!pDiresVec) return -1;
	m_xmlDiresName.pDiresVec = pDiresVec;

	//  ����,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return -1;

	//  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);

	//  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
	XML_SetUserData(m_tXmlParser, &m_xmlDiresName);

	//  ���õ���������ʱʲô����,
	//  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
	//  ����"hello.gif"������parsedata,����/img������xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�

	return 0;
}

void CDiresXml::Finalize()
{
	m_xmlDiresName.pDiresVec = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CDiresXml::ParseDiresNameXml(const char *buffer, int length)
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