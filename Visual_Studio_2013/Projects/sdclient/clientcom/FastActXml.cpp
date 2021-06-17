#include "StdAfx.h"

#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"
#include "iattb_type.h"

#include "FastActXml.h"

//
#define IS_DIRE_PATH(STRI) ('/' == *sdstr::strlchr(STRI))

//
/*
<?xml version="1.0" encoding="UTF-8"?>
<inodes_action version="1.2.1">
<inode action_type="A" file_size="343" last_write="05-25-2010 10:55:33 GMT" reserved="ab454">xxxxxxxxxx</inode>
<inode action_type="A" file_size="343" last_write="05-25-2010 10:55:33 GMT" reserved="ab454">xxxxxxxxxx</inode>
</inodes_action>
*/

#define START_IACTI_LABEL	"<inode "
#define END_ATTRIB_XML		'>'
#define END_IACTI_LABEL		"</inode>"

#define GET_ELEMENT_VALUE(ELEMENT_VALUE, ATTRIB_VALUE, PARSE_TOKEN, LABEL_START, LABEL_END) { \
    char *elem_end, *elem_start, *attri_start; \
    if(elem_start = strstr(PARSE_TOKEN, LABEL_START)) { \
		attri_start = elem_start + s_strlen(LABEL_START); \
		if(elem_start = strchr(attri_start, END_ATTRIB_XML)) { \
			int length = elem_start - attri_start; \
			strncpy(ATTRIB_VALUE, attri_start, length); \
			ATTRIB_VALUE[length] = '\0'; \
			elem_start += 1; \
			if (elem_end = strstr(elem_start, LABEL_END)) { \
				length = elem_end - elem_start; \
				strncpy(ELEMENT_VALUE, elem_start, length); \
				ELEMENT_VALUE[length] = '\0'; \
				PARSE_TOKEN = elem_end + s_strlen(LABEL_END); \
			} else PARSE_TOKEN = NULL; \
		} else PARSE_TOKEN = NULL; \
    } else PARSE_TOKEN = NULL; \
}

CFastActXml::CFastActXml(void) {
	m_pFilesVec = NULL;
	parse_buffe = NULL;
	parse_posi = NULL;
	initi_parse = 0x00;
}

CFastActXml::~CFastActXml(void) {
	initi_parse = 0x00;
	if(parse_buffe) {
		free(parse_buffe);
		parse_buffe = NULL;
	}
}

CFastActXml objFastActXml;

DWORD CFastActXml::Initialize(FilesVec *pFilesVec, unsigned int max_len) {
// _LOG_DEBUG(_T("in files attrib xml initial!"));
	if(!pFilesVec) return ((DWORD)-1);
	m_pFilesVec = pFilesVec;
//
	parse_buffe = (char *)malloc(2048 + max_len);
	if(!parse_buffe) return ((DWORD)-1);
	parse_posi = parse_buffe;
	initi_parse = 0x00;
	//
	return 0x00;
}

void CFastActXml::Finalize() {
	initi_parse = 0x00;
	parse_posi = NULL;
	if(parse_buffe) {
		free(parse_buffe);
		parse_buffe = NULL;
	}
//
	m_pFilesVec = NULL;
}

//
#define XML_INODES_ACTI_ROOT		"inodes_action"
#define XML_INODE					"inode"

#define XML_ACTION_TYPE				"action_type"
#define XML_FILE_SIZE				"file_size"
#define XML_LAST_WRITE              "last_write"
#define XML_FILE_RESERVED           "reserved"

enum INODE_ATTRIB {
	ENUM_UNKNOW = 0,
	ENUM_INODES_ROOT,
	ENUM_INODE,
	ENUM_ACTION_TYPE,
	ENUM_FILE_SIZE,
	ENUM_LAST_WRITE,
	ENUM_FILE_RESERVED,
};

static inline int GetID(const char*name) {
	if(!strcmp(name,XML_INODES_ACTI_ROOT))	return ENUM_INODES_ROOT;
	else if(!strcmp(name,XML_INODE))			return ENUM_INODE;
	else if(!strcmp(name,XML_ACTION_TYPE))	return ENUM_ACTION_TYPE;
	else if(!strcmp(name,XML_FILE_SIZE))	return ENUM_FILE_SIZE;
	else if(!strcmp(name,XML_LAST_WRITE))	return ENUM_LAST_WRITE;
	else if(!strcmp(name,XML_FILE_RESERVED))	return ENUM_FILE_RESERVED;
//
	return ENUM_UNKNOW;
}

static inline DWORD ActionValueType(const char *acti_text) {
	if ('\0' == acti_text[0]) return INVA_ATTR;
	else if (!strcmp("add", acti_text)) return ADDI;
	else if (!strcmp("mod", acti_text)) return MODIFY;
	else if (!strcmp("del", acti_text)) return DELE;
	else if (!strcmp("exis", acti_text)) return EXIST;
	else if (!strcmp("copy", acti_text)) return COPY;
	else if (!strcmp("move", acti_text)) return MOVE;
    else if (!strcmp("inva", acti_text)) return INVA_ATTR;
//
	return INVA_ATTR;
}

#define PATH_ATTRIB(ACTI_TYPE, PATH) \
	if(IS_DIRE_PATH(PATH)) ACTI_TYPE |= DIRECTORY;

static int SetElementValue(struct attent *atte, int element, const char *szValue) {
	if(!szValue) return -1;
//
	switch(element) {
   case ENUM_INODES_ROOT:
	   break;
   case ENUM_INODE:
	   PATH_ATTRIB(atte->action_type, szValue)
	   strcon::utf8_ustr(atte->file_name, (char *)szValue);
	   struti::path_unix_wind(atte->file_name);
// _LOG_DEBUG(_T("file name:%s"), atte->file_name);
	   break;
   case ENUM_ACTION_TYPE:
	   atte->action_type |= ActionValueType(szValue);
// _LOG_DEBUG(_T("action type:%08X"), atte->action_type);
	   break;
   case ENUM_FILE_SIZE:
	   atte->file_size = _atoi64(szValue);
// _LOG_DEBUG(_T("file size:%lld"), atte->file_size);
	   break;
   case ENUM_LAST_WRITE:
	   timcon::ansi_ftim(&(atte)->last_write, (char *)szValue);
// _LOG_DEBUG(_T("last_write:%llu"), atte->last_write);
	   break;
   case ENUM_FILE_RESERVED:
	   atte->reserved = atol(szValue);
// _LOG_DEBUG(_T("reserved:%d"), atte->reserved);
	   break;
   default:    
	   break;
	}
//
	return 0;
}

//
#define ITEM_TOKEN  ' '
#define ATT_VALUE_LEN  128
#define ATT_LINE_LEN  512

static DWORD ParseAttribXml(struct attent *catte, const char *buffe) {
	char *name, *value;
	char quote_value[ATT_VALUE_LEN];
	//
	value = sdstr::name_value(&name, (char *)buffe, ITEM_TOKEN);
	if(!value) return ((DWORD)-1);
	sdstr::no_quote(quote_value, value);
	SetElementValue(catte, GetID(name), quote_value);
	for(;;) {
		value = sdstr::name_value(&name, NULL, ITEM_TOKEN);
		if(!value) break;
		sdstr::no_quote(quote_value, value);
		SetElementValue(catte, GetID(name), quote_value);
	}
//
	return 0x00;
}

static DWORD ParseElementXml(FilesVec *pFilesVec, const char *buffe) {
	char element_value[MAX_PATH];
	char attrib_value[ATT_LINE_LEN];
	char *parse_toke;
	struct attent *catte;
//
	parse_toke = (char *)buffe;
	for(;;) {
		GET_ELEMENT_VALUE(element_value, attrib_value, parse_toke, START_IACTI_LABEL, END_IACTI_LABEL);
		if (!parse_toke) break;
		catte = NFilesVec::AppNewNode(pFilesVec);
		SetElementValue(catte, ENUM_INODE, element_value);
		//
		ParseAttribXml(catte, attrib_value);
	}
//
	return 0x00;
}

//
DWORD CFastActXml::ParseFilesActXml(const char *buffe, unsigned int length) {
	char *line_start, *line_end;
	unsigned int size;
//
	if(initi_parse) {
		line_end = sdstr::strrstr(buffe, END_IACTI_LABEL);
		if(!line_end) return ((DWORD)-1);
		line_end += s_strlen(END_IACTI_LABEL);
//
		size = line_end - buffe;
		strncpy(parse_posi, buffe, size);
	} else {
		line_start = strstr((char *)buffe, START_IACTI_LABEL);
		if(!line_start) return ((DWORD)-1);
		line_end = sdstr::strrstr(line_start, END_IACTI_LABEL);
		if(!line_end) return ((DWORD)-1);
		line_end += s_strlen(END_IACTI_LABEL);
//
		size = line_end - line_start;
		strncpy(parse_posi, line_start, size);
		initi_parse++;
	}
	parse_posi[size] = '\0';
	ParseElementXml(m_pFilesVec, parse_buffe);
	//
	if((buffe + length) != line_end) {
		strcpy(parse_buffe, line_end);
		parse_posi = parse_buffe + strlen(parse_buffe);
	} else parse_posi = parse_buffe;
//
	return 0x00;
}