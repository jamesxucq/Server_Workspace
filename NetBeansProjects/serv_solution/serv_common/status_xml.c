#include "logger.h"
#include "string_utility.h"

#include "status_xml.h"   

/*
<?xml version="1.0" encoding="UTF-8"?>
<status version="1.2.1">
<uid>12345321</uid>
<cached_anchor>xxxxxx</cached_anchor>
<location>xxxxxxxxx</location>
<session>xxxxxxxxx</session>
</status>
 */

int status_xml_value(char *xml_txt, struct xml_status *cached_status) {
    if (!cached_status || !xml_txt) return -1; //modify by james 20110126
    //
    char *line_txt = lscpy(xml_txt, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    line_txt = lscpy(line_txt, "<status version=\"1.2.1\">");
    line_txt = lsprif(line_txt, "<uid>%u</uid>", cached_status->uid);
    line_txt = lsprif(line_txt, "<cached_anchor>%u</cached_anchor>", cached_status->cached_anchor);
    line_txt = lsprif(line_txt, "<location>%s</location>", cached_status->location);
    line_txt = lsprif(line_txt, "<session>%s</session>", cached_status->cached_seion);
    strcpy(line_txt, "</status>");
    //
    return 0;
}


#define START_UID_LABEL        "<uid>"
#define END_UID_LABEL          "</uid>"
#define START_ANCHOR_LABEL      "<cached_anchor>"
#define END_ANCHOR_LABEL        "</cached_anchor>"
#define START_HOME_LABEL        "<location>"
#define END_HOME_LABEL          "</location>"
#define START_SESSION_LABEL     "<session>"
#define END_SESSION_LABEL       "</session>"

#define GET_ELEMENT_VALUE(ELEMENT_VALUE, PARSE_TOKEN, LABEL_START, LABEL_END) { \
    char *elem_end, *elem_start; \
    if((elem_start = strstr(PARSE_TOKEN, LABEL_START))) { \
        elem_start += s_strlen(LABEL_START); \
        if ((elem_end = strstr(elem_start, LABEL_END))) { \
            int length = elem_end - elem_start; \
            strncpy(ELEMENT_VALUE, elem_start, length); \
            ELEMENT_VALUE[length] = '\0'; \
            PARSE_TOKEN = elem_end + s_strlen(LABEL_END); \
        } else PARSE_TOKEN = NULL; \
    } else PARSE_TOKEN = NULL; \
}

int status_from_xmlfile(struct xml_status *cached_status, char *xml_txt) {
    char element_value[MAX_TEXT_LEN];
    char *parse_toke;
    //
    if (!xml_txt || !cached_status) return -1; //modify by james 20110126    
    //
    parse_toke = xml_txt;
    GET_ELEMENT_VALUE(element_value, parse_toke, START_UID_LABEL, END_UID_LABEL);
    if (!parse_toke) return -1;
    cached_status->uid = atoi(element_value);
    //
    GET_ELEMENT_VALUE(element_value, parse_toke, START_ANCHOR_LABEL, END_ANCHOR_LABEL);
    if (!parse_toke) return -1;
    cached_status->cached_anchor = atoi(element_value);
    //
    GET_ELEMENT_VALUE(element_value, parse_toke, START_HOME_LABEL, END_HOME_LABEL);
    if (!parse_toke) return -1;
    strcpy(cached_status->location, element_value);
    //
    GET_ELEMENT_VALUE(element_value, parse_toke, START_SESSION_LABEL, END_SESSION_LABEL);
    if (!parse_toke) return -1;
    strcpy(cached_status->cached_seion, element_value);
    //
    return 0;
}


