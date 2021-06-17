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
    //  ������xml�ļ��Ľ���Ԫ��ʱ������������
    fprintf(stderr, "end element:</%s>\n",el);
    ((struct XML_LDATA_NAME *)data)->Element = ENUM_UNKNOWD;
    ((struct XML_LDATA_NAME *)data)->Depth --;
}

static VOID XMLCALL parsedata(VOID *data, const XML_Char *stri, int length) {
    // �������ݣ�ע������s������\0�����ģ�����ֻȡlength����
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
    //  ����,
    m_tXmlParser = XML_ParserCreate(NULL);
    if( !m_tXmlParser ) return ((DWORD)-1);
    //  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
    XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);
    //  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
    XML_SetUserData(m_tXmlParser, &m_tLdataName);
    //  ���õ���������ʱʲô����,
    //  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
    //  ����"hello.gif"������parsedata,����/img������xmlend
    XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�
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
