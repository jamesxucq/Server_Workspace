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
*  xml解析函数,
*  以下参数说明
*  data是使用XML_SetUserData设置的参数,expat不进行处理,会把它交给用户回调函数处理
*  el是元素名
*  attr是属性-值列表,样子为attr[0]=attr[1],最后一个是NULL
*  
*----------------------------------------------------------------------*/
static VOID XMLCALL xmlstart(VOID *data, const char *el, const char **attr) {
	//  当碰到xml元素的开始标志时会调用这个函数,可以看打印显示的结果
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
	//  当碰到xml文件的结束元素时会调用这个函数
	printf("end element:</%s>\n",el);
	((struct XML_DIRIES_NAME *)data)->Element = ENUM_UNKNOWD;
	((struct XML_DIRIES_NAME *)data)->Depth --;
}

static VOID XMLCALL parsedata(VOID *data, const XML_Char *stri, int length) {
	// 处理数据，注意这里s不是以\0结束的，而是只取length长度
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
	//  解析,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return ((DWORD)-1);
//
	//  设置事件处理函数,当碰到开始元素和结可元素时调用哪个函数来处理
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);
//
	//  设置用户数据，如果需要传给处理函数附加的信息，那就使用它，不然就不用
	XML_SetUserData(m_tXmlParser, &m_xmlDiresPath);
//
	//  设置当碰到数据时什么处理,
	//  如<img> "hello.gif"</img> 那么碰到img时会调用xmlstart,
	//  读完"hello.gif"后会调用parsedata,碰到/img后会调用xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//添加这个,处理不同字符集的xml文件
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