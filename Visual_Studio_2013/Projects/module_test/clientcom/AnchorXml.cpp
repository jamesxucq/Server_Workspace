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
*  xml解析函数,
*  以下参数说明
*  data是使用XML_SetUserData设置的参数,expat不进行处理,会把它交给用户回调函数处理
*  el是元素名
*  attr是属性-值列表,样子为attr[0]=attr[1],最后一个是NULL
*  
*----------------------------------------------------------------------*/
static void XMLCALL xmlstart(void *data, const char *el, const char **attr)
{
	//  当碰到xml元素的开始标志时会调用这个函数,可以看打印显示的结果
	printf("start element:<%s>\n",el);
	struct XML_ANCHOR* xmlAnchor = (struct XML_ANCHOR*) data;

	xmlAnchor->Element = GetID(el);
	xmlAnchor->Depth ++;
}

static void XMLCALL xmlend(void *data, const char *el)
{
	//  当碰到xml文件的结束元素时会调用这个函数
	printf("end element:</%s>\n",el);
	((struct XML_ANCHOR *)data)->Element = ENUM_UNKNOWD;
	((struct XML_ANCHOR *)data)->Depth    --;
}

static void XMLCALL parsedata(void *udata, const XML_Char *stri, int length)
{
	// 处理数据，注意这里s不是以\0结束的，而是只取length长度
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

	//  解析,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return -1;

	//  设置事件处理函数,当碰到开始元素和结可元素时调用哪个函数来处理
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);

	//  设置用户数据，如果需要传给处理函数附加的信息，那就使用它，不然就不用
	XML_SetUserData(m_tXmlParser, &m_xmlAnchor);

	//  设置当碰到数据时什么处理,
	//  如<img> "hello.gif"</img> 那么碰到img时会调用xmlstart,
	//  读完"hello.gif"后会调用parsedata,碰到/img后会调用xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//添加这个,处理不同字符集的xml文件

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