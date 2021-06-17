#include "StdAfx.h"
#include "UpdateXml.h"

#include "clientcom/lupdatecom.h"
#include "third_party.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<live_update version="1.21.2">
<item cmd="explorer" pid="0">
<uri></uri><noa></noa><nob></nob>
</item>
<item cmd="replace" pid="1">
<uri></uri><noa></noa><nob></nob>
</item>
</live_update>
*/

#define XML_LIVE_ROOT				"live_update"
#define XML_VERSION					"version"
#define XML_ITEM					"item"
#define XML_COMMAND					"cmd"
#define XML_PID						"pid"
#define XML_NOA						"noa"
#define XML_NOB						"nob"
#define XML_URI						"uri"

enum ELIVE_UPDATE {
    ENUM_UNKNOWD = 0,
    ENUM_LIVE_ROOT,
    ENUM_VERSION,
    ENUM_ITEM,
    ENUM_COMMAND,
    ENUM_PID,
    ENUM_NOA,
    ENUM_NOB,
    ENUM_URI,
};

CUpdateXml::CUpdateXml(void) {
    m_tLdataName.pLiveVec = NULL;
    m_tLdataName.Depth = 0;
    m_tLdataName.Element = ENUM_UNKNOWD;
}

CUpdateXml::~CUpdateXml(void) {
}

CUpdateXml objUpdateXml;

static inline int GetID(const char*name) {
    if(!strcmp(name,XML_LIVE_ROOT))				return ENUM_LIVE_ROOT;
    else if(!strcmp(name,XML_VERSION))			return ENUM_VERSION;
    else if(!strcmp(name,XML_ITEM))				return ENUM_ITEM;
    else if(!strcmp(name,XML_COMMAND))			return ENUM_COMMAND;
    else if(!strcmp(name,XML_PID))				return ENUM_PID;
    else if(!strcmp(name,XML_NOA))				return ENUM_NOA;
    else if(!strcmp(name,XML_NOB))				return ENUM_NOB;
    else if(!strcmp(name,XML_URI))				return ENUM_URI;
//
    return ENUM_UNKNOWD;
}

static int SetElementValue(XML_LDATA_NAME* xmlLdataName, const char *szValue) {
    if(!szValue) return -1;
//
    struct LiveData *ldata = xmlLdataName->ldata;
    switch(xmlLdataName->Element) {
    case ENUM_LIVE_ROOT:
        break;
    case ENUM_ITEM:
        break;
    case ENUM_VERSION:
        strcon::utf8_ustr(xmlLdataName->szVersion, (char *)szValue);
        break;
    case ENUM_COMMAND:
        ldata->command = GetLiveCmd((char *)szValue);
        break;
    case ENUM_PID:
        ldata->pid = atoi(szValue);
        break;
    case ENUM_URI:
        strcpy(ldata->szReqURI, (char *)szValue);
        break;
    case ENUM_NOA:
        strcon::utf8_ustr(ldata->szNoA, (char *)szValue);
        struti::path_unix_wind(ldata->szNoA);
        break;
    case ENUM_NOB:
        strcon::utf8_ustr(ldata->szNoB, (char *)szValue);
        struti::path_unix_wind(ldata->szNoB);
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
    fprintf(stderr, "start element:<%s>\n",el);
    struct XML_LDATA_NAME* xmlLdataName = (struct XML_LDATA_NAME*) data;
//
    int iElement = GetID(el);
    if(ENUM_LIVE_ROOT == iElement) {
        for( int inde=0; attr[inde]; inde+=2) {
            xmlLdataName->Element = GetID(attr[inde]);
            SetElementValue(xmlLdataName, attr[inde+1]);
        }
    } else if(ENUM_ITEM == iElement) {
        xmlLdataName->ldata = NLiveVec::AddNewLiveData(xmlLdataName->pLiveVec);
        for( int inde=0; attr[inde]; inde+=2) {
            xmlLdataName->Element = GetID(attr[inde]);
            SetElementValue(xmlLdataName, attr[inde+1]);
        }
    }
//
    xmlLdataName->Element = iElement;
    xmlLdataName->Depth ++;
}

static VOID XMLCALL xmlend(VOID *data, const char *el) {
    //  当碰到xml文件的结束元素时会调用这个函数
    fprintf(stderr, "end element:</%s>\n",el);
    ((struct XML_LDATA_NAME *)data)->Element = ENUM_UNKNOWD;
    ((struct XML_LDATA_NAME *)data)->Depth --;
}

static VOID XMLCALL parsedata(VOID *data, const XML_Char *stri, int length) {
    // 处理数据，注意这里s不是以\0结束的，而是只取length长度
    char buffer[LINE_LENGTH];
    // ConvFromUTF8(buffer, sizeof(buffer), stri, length);
    strncpy_s(buffer, stri, length);
	buffer[length] = '\0';
    fprintf(stderr, "data is:[%s]\n", buffer);
    SetElementValue((struct XML_LDATA_NAME *)data, buffer);
}

DWORD CUpdateXml::Initialize(TCHAR *szVersion, LiveVec *pLiveVec) {
    if(!pLiveVec || !szVersion) return ((DWORD)-1);
    m_tLdataName.szVersion = szVersion;
    m_tLdataName.pLiveVec = pLiveVec;
    //  解析,
    m_tXmlParser = XML_ParserCreate(NULL);
    if( !m_tXmlParser ) return ((DWORD)-1);
    //  设置事件处理函数,当碰到开始元素和结可元素时调用哪个函数来处理
    XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);
    //  设置用户数据，如果需要传给处理函数附加的信息，那就使用它，不然就不用
    XML_SetUserData(m_tXmlParser, &m_tLdataName);
    //  设置当碰到数据时什么处理,
    //  如<img> "hello.gif"</img> 那么碰到img时会调用xmlstart,
    //  读完"hello.gif"后会调用parsedata,碰到/img后会调用xmlend
    XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//添加这个,处理不同字符集的xml文件
//
    return 0x00;
}

VOID CUpdateXml::Finalize() {
    m_tLdataName.pLiveVec = NULL;
    XML_ParserFree(m_tXmlParser);
}

DWORD CUpdateXml::ParseUpdateXml(const char *buffer, int length) {
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
