
#include "logger.h"
#include "query_xml.h"

/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<uid>2</uid>
<validation>VB48uIcNsS9JvOaY0btu6Kaqmo1wLGpY</validation>
<worker address="192.168.1.103" port="8090" locked="true" />
<cached_anchor>0</cached_anchor>
<session_id>9209-1348697447-2</session_id>
</query>
 */

#define UID_START_LABEL        "<uid>"
#define UID_END_LABEL          "</uid>"
#define VALID_START_LABEL     "<validation>"
#define VALID_END_LABEL       "</validation>"
#define LOCKED_START_LABEL      "locked=\""
#define LOCKED_END_LABEL        "\""

#define GET_ELEMENT_VALUE(ELEMENT_VALUE, PARSE_TOKEN, LABEL_START, LABEL_END) { \
    char *value_end, *value_start; \
    if(value_start = strstr(PARSE_TOKEN, LABEL_START)) { \
        int length = strlen(LABEL_START); \
        if (value_end = strstr(value_start+length, LABEL_END)) { \
            value_start += length; \
            length = value_end - value_start; \
            strncpy(ELEMENT_VALUE, value_start, length); \
            ELEMENT_VALUE[length] = '\0'; \
            PARSE_TOKEN = value_end + strlen(LABEL_END); \
        } \
    }\
}

struct value_entry {
    const char *text;
    int value;
};

#define TEXT_VALUE(VALUE, TABLE, TEXT) { \
    struct value_entry *entry; \
    for(entry = (struct value_entry *)TABLE; entry->text; ++entry) \
	if(!strcmp(entry->text, TEXT)) \
            break; \
    VALUE = entry->value; \
}
static const struct value_entry bool_value_table[] = {
    {"false", 0},
    {"true", 1},
    {NULL, 0}
};

int query_from_xmlfile(struct xml_query *cached_query, char *xml_txt) {
    char element_value[MAX_TEXT_LEN];
    char *parse_token;

    if (!xml_txt || !cached_query) return -1; //modify by james 20110126    

    parse_token = xml_txt;
    GET_ELEMENT_VALUE(element_value, parse_token, UID_START_LABEL, UID_END_LABEL);
    if (!parse_token) return -1;
    cached_query->uid = atoi(element_value);

    GET_ELEMENT_VALUE(element_value, parse_token, VALID_START_LABEL, VALID_END_LABEL);
    if (!parse_token) return -1;
    strcpy(cached_query->validation, element_value);

    GET_ELEMENT_VALUE(element_value, parse_token, LOCKED_START_LABEL, LOCKED_END_LABEL);
    if (!parse_token) return -1;
    TEXT_VALUE(cached_query->locked, bool_value_table, element_value)

    return 0;
}


