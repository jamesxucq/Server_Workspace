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
#define XML_DIRE_NAME			"dir_name"

enum DIRE_NAME {
	ENUM_UNKNOWD = 0,
	ENUM_DIRIES_ROOT,
	ENUM_DIRE_NAME,
};

CDiresXml::CDiresXml(void) {
	m_xmlDiresPath.pDiresVec = NULL;
	m_xmlDiresPath.Depth = 0;
	m_xmlDiresPath.Element = ENUM_UNKNOWD;
}

CDiresXml::~CDiresXml(void) {
}

CDiresXml objDiresXml;

static inline int GetID(const char*name) {
	if(!strcmp(name,XML_DIRIES_ROOT))			return ENUM_DIRIES_ROOT;
	else if(!strcmp(name,XML_DIRE_NAME))				return ENUM_DIRE_NAME;
//
	return ENUM_UNKNOWD;
}

static int SetElementValue(XML_DIRIES_NAME* xmlDiresPath, const char *szValue) {
	if(!szValue) return -1;
//
	TCHAR *dirpath = xmlDiresPath->cure_directory;
	switch(xmlDiresPath->Element){
   case ENUM_DIRIES_ROOT:
	   break;
   case ENUM_DIRE_NAME:
	   striconv::utf8_unicode(dirpath, (char *)szValue);
	   striutil::path_unix_windows(dirpath);
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
	printf("start element:<%s>\n",el);
	struct XML_DIRIES_NAME* xmlDiresPath = (struct XML_DIRIES_NAME*) data;
//
	int iElement = GetID(el);
	if(ENUM_DIRE_NAME == iElement) {
		xmlDiresPath->cure_directory = NDiresVec::AddNewDiresNode(xmlDiresPath->pDiresVec);
	}
	xmlDiresPath->Element = iElement;
	xmlDiresPath->Depth ++;
}

static VOID XMLCALL xmlend(VOID *data, const char *el) {
	//  ������xml�ļ��Ľ���Ԫ��ʱ������������
	printf("end element:</%s>\n",el);
	((struct XML_DIRIES_NAME *)data)->Element = ENUM_UNKNOWD;
	((struct XML_DIRIES_NAME *)data)->Depth --;
}

static VOID XMLCALL parsedata(VOID *data, const XML_Char *stri, int length) {
	// �������ݣ�ע������s������\0�����ģ�����ֻȡlength����
	char buffer[LINE_LENGTH];
	// ConvFromUTF8(buffer, sizeof(buffer), stri, length);
	strncpy_s(buffer, stri, length);
	buffer[length] = '\0';
	printf("data is:[%s]\n", buffer);
	SetElementValue((struct XML_DIRIES_NAME *)data, buffer);
}

DWORD CDiresXml::Initialize(DiresVec *pDiresVec) {
// _LOG_DEBUG(_T("in dire xml initial!"));
	if(!pDiresVec) return ((DWORD)-1);
	m_xmlDiresPath.pDiresVec = pDiresVec;
//
	//  ����,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return ((DWORD)-1);
//
	//  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);
//
	//  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
	XML_SetUserData(m_tXmlParser, &m_xmlDiresPath);
//
	//  ���õ���������ʱʲô����,
	//  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
	//  ����"hello.gif"������parsedata,����/img������xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�
//
	return 0x00;
}

VOID CDiresXml::Finalize() {
	m_xmlDiresPath.pDiresVec = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CDiresXml::ParseDiresPathXml(const char *buffer, unsigned int length) {
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