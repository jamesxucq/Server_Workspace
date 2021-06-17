#include "StdAfx.h"

#include <string.h>
#include "StringUtility.h"
#include "FileUtility.h"
#include "third_party.h"

#include "DiresXml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<inodes_dire version="1.2.1">
<inode>xxxxxxxxxx</inode>
<inode>xxxxxxxxxx</inode>
</inodes_dire>
*/

#define START_IDIRE_LABEL	"<inode>"
#define END_IDIRE_LABEL		"</inode>"

#define GET_ELEMENT_VALUE(ELEMENT_VALUE, PARSE_TOKEN, LABEL_START, LABEL_END) { \
    char *elem_end, *elem_start; \
    if(elem_start = strstr(PARSE_TOKEN, LABEL_START)) { \
		elem_start += s_strlen(LABEL_START); \
        if (elem_end = strstr(elem_start, LABEL_END)) { \
            int length = elem_end - elem_start; \
            strncpy(ELEMENT_VALUE, elem_start, length); \
            ELEMENT_VALUE[length] = '\0'; \
            PARSE_TOKEN = elem_end + s_strlen(LABEL_END); \
        } else PARSE_TOKEN = NULL; \
    } else PARSE_TOKEN = NULL; \
}

CDiresXml::CDiresXml(void) {
	m_pDiresVec = NULL;
	parse_buffe = NULL;
	parse_posi = NULL;
	initi_parse = 0x00;
}

CDiresXml::~CDiresXml(void) {
	initi_parse = 0x00;
	if(parse_buffe) {
		free(parse_buffe);
		parse_buffe = NULL;
	}
}

CDiresXml objDiresXml;

DWORD CDiresXml::Initialize(DiresVec *pListVec, unsigned int max_len) {
// _LOG_DEBUG(_T("in dire xml initial!"));
	if(!pListVec) return ((DWORD)-1);
	m_pDiresVec = pListVec;
//
	parse_buffe = (char *)malloc(2048 + max_len);
	if(!parse_buffe) return ((DWORD)-1);
	parse_posi = parse_buffe;
	initi_parse = 0x00;
	//
	return 0x00;
}

VOID CDiresXml::Finalize() {
	initi_parse = 0x00;
	parse_posi = NULL;
	if(parse_buffe) {
		free(parse_buffe);
		parse_buffe = NULL;
	}
//
	m_pDiresVec = NULL;
}

//
enum DIRE_NAME {
	ENUM_UNKNOWD = 0,
	ENUM_IDIRES_ROOT,
	ENUM_IDIRE,
};

static int SetElementValue(TCHAR *cdire, int element, const char *szValue) {
	if(!szValue) return -1;
//
	switch(element){
	case ENUM_IDIRES_ROOT:
		break;
	case ENUM_IDIRE:
		strcon::utf8_ustr(cdire, (char *)szValue);
		struti::path_unix_wind(cdire);
// _LOG_DEBUG(_T("dire name:%s"), cdire);
		break;
	default:    
		break;
	}
//
	return 0;
}

//
static DWORD ParseElementXml(DiresVec *pListVec, const char *buffe) {
	char element_value[MAX_PATH];
	char *parse_toke;
//
	parse_toke = (char *)buffe;
	for(;;) {
		GET_ELEMENT_VALUE(element_value, parse_toke, START_IDIRE_LABEL, END_IDIRE_LABEL);
		if (!parse_toke) break;
		TCHAR *cdire = NDiresVec::AddNewDiresNode(pListVec);
		SetElementValue(cdire, ENUM_IDIRE, element_value);
	}
//
	return 0x00;
}

DWORD CDiresXml::ParseDiresPathXml(const char *buffe, unsigned int length) {
	char *line_start, *line_end;
	unsigned int size;
//
	if(initi_parse) {
		line_end = sdstr::strrstr(buffe, END_IDIRE_LABEL);
		if(!line_end) return ((DWORD)-1);
		line_end += s_strlen(END_IDIRE_LABEL);

		size = line_end - buffe;
		strncpy(parse_posi, buffe, size);
	} else {
		line_start = strstr((char *)buffe, START_IDIRE_LABEL);
		if(!line_start) return ((DWORD)-1);
		line_end = sdstr::strrstr(line_start, END_IDIRE_LABEL);
		if(!line_end) return ((DWORD)-1);
		line_end += s_strlen(END_IDIRE_LABEL);

		size = line_end - line_start;
		strncpy(parse_posi, line_start, size);
		initi_parse++;
	}
	parse_posi[size] = '\0';
	ParseElementXml(m_pDiresVec, parse_buffe);
	//
	if((buffe + length) != line_end) {
		strcpy(parse_buffe, line_end);
		parse_posi = parse_buffe + strlen(parse_buffe);
	} else parse_posi = parse_buffe;
//
	return 0x00;
}