#include "StdAfx.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "FilesAttribXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<files_attrib version="1.2.1">
<file modify_type="A" file_size="343" last_write="05-25-2010 10:55:33 GMT" reserved="ab454">xxxxxxxxxx</file>
<file modify_type="A" file_size="343" last_write="05-25-2010 10:55:33 GMT" reserved="ab454">xxxxxxxxxx</file>
</files_attrib>
*/

#define XML_FILES_ATTRIB_ROOT		"files_attrib"
#define XML_FILE					"file"

#define XML_MODIFY_TYPE				"modify_type"
#define XML_FILE_SIZE				"file_size"
#define XML_MODIFY_TIME             "last_write"
#define XML_FILE_RESERVED           "reserved"

enum FILE_ATTRIB {
	ENUM_UNKNOW = 0,
	ENUM_FILES_ROOT,
	ENUM_FILE,
	ENUM_MODIFY_TYPE,
	ENUM_FILE_SIZE,
	ENUM_MODIFY_TIME,
	ENUM_FILE_RESERVED,
};

CFilesAttribXml::CFilesAttribXml(void) {
	m_xmlFilesAttrib.pFilesVec = NULL;
	m_xmlFilesAttrib.Depth = 0;
	m_xmlFilesAttrib.Element = ENUM_UNKNOW;
}

CFilesAttribXml::~CFilesAttribXml(void) {
}

CFilesAttribXml objFilesAttribXml;


static inline int GetID(const char*name) {
	if(!strcmp(name,XML_FILES_ATTRIB_ROOT))		return ENUM_FILES_ROOT;
	else if(!strcmp(name,XML_FILE))					return ENUM_FILE;
	else if(!strcmp(name,XML_MODIFY_TYPE))			return ENUM_MODIFY_TYPE;
	else if(!strcmp(name,XML_FILE_SIZE))				return ENUM_FILE_SIZE;
	else if(!strcmp(name,XML_MODIFY_TIME))			return ENUM_MODIFY_TIME;
	else if(!strcmp(name,XML_FILE_RESERVED))			return ENUM_FILE_RESERVED;

	return ENUM_UNKNOW;
}

inline DWORD ModifyValueType(const char *modify_text) {
	if (*modify_text == '\0') return UNKNOWN;
	else if (!strcmp("add", modify_text)) return ADD;
	else if (!strcmp("mod", modify_text)) return MODIFY;
	else if (!strcmp("del", modify_text)) return DEL;
	else if (!strcmp("exis", modify_text)) return EXISTS;
	else if (!strcmp("copy", modify_text)) return COPY;
	else if (!strcmp("move", modify_text)) return MOVE;
	else if (!strcmp("recu", modify_text)) return RECURSIVE;
	else if (!strcmp("list", modify_text)) return LIST;

	return UNKNOWN;
}

static int SetElementValue(XML_FILES_ATTRIB* files_attrib, const char *szValue) {
	if(!szValue) return -1;

	struct file_attrib *fast = files_attrib->current_fast;
	switch(files_attrib->Element) {
   case ENUM_FILES_ROOT:
	   break;
   case ENUM_FILE:
	   nstriconv::utf8_unicode(fast->file_name, (char *)szValue);
	   nstriutility::path_unix_windows(fast->file_name);
	   LOG_DEBUG(_T("file name:%s"), fast->file_name);
	   break;
   case ENUM_MODIFY_TYPE:
	   fast->modify_type = ModifyValueType(szValue);
	   break;
   case ENUM_FILE_SIZE:
	   fast->file_size = atol(szValue);
	   LOG_DEBUG(_T("file size:%d"), fast->file_size);
	   break;
   case ENUM_MODIFY_TIME:
	   ntimeconv::ansi_filetime(&(fast)->last_write, (char *)szValue);
	   break;
   case ENUM_FILE_RESERVED:
	   fast->reserved = atol(szValue);
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
static void XMLCALL xmlstart(void *data, const char *el, const char **attr) {
	//  当碰到xml元素的开始标志时会调用这个函数,可以看打印显示的结果
	printf("start element:<%s>\n",el);
	struct XML_FILES_ATTRIB* xmlFilesAttrib = (struct XML_FILES_ATTRIB*) data;

	int iElement = GetID(el);
	if(ENUM_FILE == iElement) {
		xmlFilesAttrib->current_fast = NFilesVec::AppendNewNode(xmlFilesAttrib->pFilesVec);
		for( int index=0; attr[index]; index+=2){
			xmlFilesAttrib->Element    = GetID(attr[index]);
			SetElementValue(xmlFilesAttrib, attr[index+1]);
		}
	}
	xmlFilesAttrib->Element = iElement;
	xmlFilesAttrib->Depth ++;
}

static void XMLCALL xmlend(void *data, const char *el) {
	//  当碰到xml文件的结束元素时会调用这个函数
	printf("end element:</%s>\n",el);
	((struct XML_FILES_ATTRIB *)data)->Element = ENUM_UNKNOW;
	((struct XML_FILES_ATTRIB *)data)->Depth    --;
}

static void XMLCALL parsedata(void *udata, const XML_Char *stri, int length) {
	// 处理数据，注意这里s不是以\0结束的，而是只取length长度
	char buffer[LINE_LENGTH];

	memset(buffer, '\0', sizeof(buffer));
	//ConvFromUTF8(buffer, sizeof(buffer), stri, length);
	strncpy_s(buffer, stri, length);
	printf("data is:[%s]\n", buffer);
	SetElementValue((struct XML_FILES_ATTRIB *)udata, buffer);
}

DWORD CFilesAttribXml::Initialize(FilesVec *pFilesVec) {
	if(!pFilesVec) return -1;
	m_xmlFilesAttrib.pFilesVec = pFilesVec;

	//  解析,
	m_tXmlParser = XML_ParserCreate(NULL);
	if( !m_tXmlParser ) return -1;

	//  设置事件处理函数,当碰到开始元素和结可元素时调用哪个函数来处理
	XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);

	//  设置用户数据，如果需要传给处理函数附加的信息，那就使用它，不然就不用
	XML_SetUserData(m_tXmlParser, &m_xmlFilesAttrib);

	//  设置当碰到数据时什么处理,
	//  如<img> "hello.gif"</img> 那么碰到img时会调用xmlstart,
	//  读完"hello.gif"后会调用parsedata,碰到/img后会调用xmlend
	XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//添加这个,处理不同字符集的xml文件

	return 0;
}

void CFilesAttribXml::Finalize() {
	m_xmlFilesAttrib.pFilesVec = NULL;
	XML_ParserFree(m_tXmlParser);
}

DWORD CFilesAttribXml::ParseFilesAttXml(const char *buffer, int length) {
	int done = 0;
	if (XML_Parse(m_tXmlParser, buffer, length, done) == XML_STATUS_ERROR) {
		fprintf(stderr, "%s at line %u\n",
			XML_ErrorString(XML_GetErrorCode(m_tXmlParser)),
			XML_GetCurrentLineNumber(m_tXmlParser));
		return -1;
	}

	return 0;
}