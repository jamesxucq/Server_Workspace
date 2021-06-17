#include "StdAfx.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"
#include "fattb_type.h"
#include "logger_ansi.h"

#include "SlowAttXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<files_attrib version="1.2.1">
<file action_type="A" file_size="343" last_write="05-25-2010 10:55:33 GMT" reserved="ab454">xxxxxxxxxx</file>
<file action_type="A" file_size="343" last_write="05-25-2010 10:55:33 GMT" reserved="ab454">xxxxxxxxxx</file>
</files_attrib>
*/

#define XML_FILES_ATTRIB_ROOT		"files_attrib"
#define XML_FILE					"file"

#define XML_MODIFY_TYPE				"action_type"
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

CSlowAttXml::CSlowAttXml(void)
{
    m_xmlFilesAttrib.hFilesAtt = INVALID_HANDLE_VALUE;
    m_xmlFilesAttrib.Depth = 0;
    m_xmlFilesAttrib.Element = ENUM_UNKNOW;
}

CSlowAttXml::~CSlowAttXml(void)
{
}

CSlowAttXml objSlowAttXml;


static inline int GetID(const char*name)
{
    if(!strcmp(name,XML_FILES_ATTRIB_ROOT))		return ENUM_FILES_ROOT;
    else if(!strcmp(name,XML_FILE))					return ENUM_FILE;
    else if(!strcmp(name,XML_MODIFY_TYPE))			return ENUM_MODIFY_TYPE;
    else if(!strcmp(name,XML_FILE_SIZE))				return ENUM_FILE_SIZE;
    else if(!strcmp(name,XML_MODIFY_TIME))			return ENUM_MODIFY_TIME;
    else if(!strcmp(name,XML_FILE_RESERVED))			return ENUM_FILE_RESERVED;
//
    return ENUM_UNKNOW;
}

inline DWORD ActionValueType(const char *acti_text)
{
    if ('\0' == acti_text[0]) return INVA_ATTR;
    else if (!strcmp("add", acti_text)) return ADD;
    else if (!strcmp("mod", acti_text)) return MODIFY;
    else if (!strcmp("del", acti_text)) return DEL;
    else if (!strcmp("exis", acti_text)) return EXISTS;
    else if (!strcmp("copy", acti_text)) return COPY;
    else if (!strcmp("move", acti_text)) return MOVE;
    else if (!strcmp("recu", acti_text)) return RECURSIVE;
    else if (!strcmp("list", acti_text)) return LIST;
//
    return INVA_ATTR;
}

static int SetElementValue(XML_SLOW_ATTRIB* files_attrib, const char *szValue)
{
    if(!szValue) return -1;

    struct attent *atte = &files_attrib->cure_atte;
    switch(files_attrib->Element) {
    case ENUM_FILES_ROOT:
        break;
    case ENUM_FILE:
        striconv::utf8_unicode(atte->file_name, (char *)szValue);
        striutil::path_unix_windows(atte->file_name);
_LOG_DEBUG(_T("file name:%s"), atte->file_name); // disable by james 20130410
        break;
    case ENUM_MODIFY_TYPE:
        atte->action_type = ActionValueType(szValue);
        break;
    case ENUM_FILE_SIZE:
        atte->file_size = _atoi64(szValue);
_LOG_DEBUG(_T("file size:%lld"), atte->file_size); // disable by james 20130410
        break;
    case ENUM_MODIFY_TIME:
        ntimeconv::ansi_filetime(&(atte)->last_write, (char *)szValue);
_LOG_DEBUG(_T("last_write:%llu"), atte->last_write); // disable by james 20130410
        break;
    case ENUM_FILE_RESERVED:
        atte->reserved = atol(szValue);
_LOG_DEBUG(_T("reserved:%d"), atte->reserved); // disable by james 20130410
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
static VOID XMLCALL xmlstart(VOID *data, const char *el, const char **attr)
{
    //  ������xmlԪ�صĿ�ʼ��־ʱ������������,���Կ���ӡ��ʾ�Ľ��
    printf("start element:<%s>\n",el);
    struct XML_SLOW_ATTRIB* xmlFilesAttrib = (struct XML_SLOW_ATTRIB*) data;
//
    int iElement = GetID(el);
    if(ENUM_FILE == iElement) {
        for( int inde=0; attr[inde]; inde+=2) {
            xmlFilesAttrib->Element = GetID(attr[inde]);
            SetElementValue(xmlFilesAttrib, attr[inde+1]);
        }
    }
    xmlFilesAttrib->Element = iElement;
    xmlFilesAttrib->Depth ++;
}

static VOID XMLCALL xmlend(VOID *data, const char *el)
{
    //  ������xml�ļ��Ľ���Ԫ��ʱ������������
    struct XML_SLOW_ATTRIB* xmlFilesAttrib = (struct XML_SLOW_ATTRIB*) data;
    if(ENUM_FILE == xmlFilesAttrib->Element)
        NFilesVec::WriteNode(xmlFilesAttrib->hFilesAtt, &xmlFilesAttrib->cure_atte);
    printf("end element:</%s>\n",el);
    ((struct XML_SLOW_ATTRIB *)data)->Element = ENUM_UNKNOW;
    ((struct XML_SLOW_ATTRIB *)data)->Depth --;
}

static VOID XMLCALL parsedata(VOID *data, const XML_Char *stri, int length)
{
    // �������ݣ�ע������s������\0�����ģ�����ֻȡlength����
    char buffer[LINE_LENGTH];
    // ConvFromUTF8(buffer, sizeof(buffer), stri, length);
    strncpy_s(buffer, stri, length);
	buffer[length] = '\0';
    printf("data is:[%s]\n", buffer);
    SetElementValue((struct XML_SLOW_ATTRIB *)data, buffer);
}

DWORD CSlowAttXml::Initialize(HANDLE hFilesAtt)
{
// _LOG_DEBUG(_T("in files attrib xml initial!"));
    if(INVALID_HANDLE_VALUE == hFilesAtt) return ((DWORD)-1);
    DWORD dwResult = SetFilePointer(hFilesAtt, 0, NULL, FILE_BEGIN);
    if(INVALID_SET_FILE_POINTER==dwResult && NO_ERROR!=GetLastError())
        return ((DWORD)-1);
    m_xmlFilesAttrib.hFilesAtt = hFilesAtt;
//
    //  ����,
    m_tXmlParser = XML_ParserCreate(NULL);
    if( !m_tXmlParser ) return ((DWORD)-1);
//
    //  �����¼�������,��������ʼԪ�غͽ��Ԫ��ʱ�����ĸ�����������
    XML_SetElementHandler(m_tXmlParser, xmlstart, xmlend);
//
    //  �����û����ݣ������Ҫ�������������ӵ���Ϣ���Ǿ�ʹ��������Ȼ�Ͳ���
    XML_SetUserData(m_tXmlParser, &m_xmlFilesAttrib);
//
    //  ���õ���������ʱʲô����,
    //  ��<img> "hello.gif"</img> ��ô����imgʱ�����xmlstart,
    //  ����"hello.gif"������parsedata,����/img������xmlend
    XML_SetCharacterDataHandler(m_tXmlParser, parsedata);//������,����ͬ�ַ�����xml�ļ�
//
    return 0x00;
}

void CSlowAttXml::Finalize()
{
    m_xmlFilesAttrib.hFilesAtt = INVALID_HANDLE_VALUE;
    XML_ParserFree(m_tXmlParser);
}

DWORD CSlowAttXml::ParseFilesAttXml(const char *buffer, unsigned int length)
{
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