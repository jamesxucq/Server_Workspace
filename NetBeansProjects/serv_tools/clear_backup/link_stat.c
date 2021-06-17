#include <stdlib.h>
#include <string.h>

#include "link_stat.h"
#include "clear.h"

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

static const struct value_entry bomb_table[] = {
    {"false", 0},
    {"true", 1},
    {"done", 2},
    {NULL, 0}
};

static const struct value_entry link_stat_tab[] = {
    {"initial", LINKED_STATUS_INITIAL},
    {"linked", LINKED_STATUS_LINKED},
    {"unlink", LINKED_STATUS_UNLINK},
    {NULL, 0}
};

/* <client name,link status,action time,data bomb>
 * <"rfjames","linked",425254,"false"> */

#define GET_LINE_VALUE(LINE_VALUE, LINE_TOKEN) { \
    char *line_end, *line_start; \
    if(line_start = strchr(LINE_TOKEN, '<')) { \
        if (line_end = strchr(line_start+1, '>')) { \
            int length = (++line_end)-(line_start); \
            strncpy(LINE_VALUE, line_start, length); \
            LINE_VALUE[length] = '\0'; \
            LINE_TOKEN = line_end; \
        } \
    }\
}

#define GET_FIELD_VALUE(VALUE_TEXT, VALUE_TOKEN) { \
    char *line_end, *line_start; \
    if('<' != VALUE_TOKEN[0]) line_start = VALUE_TOKEN; \
    else line_start = VALUE_TOKEN + 1; \
    if (line_end = strpbrk(line_start, ",>")) { \
        int length = line_end - line_start; \
        strncpy(VALUE_TEXT, line_start, length); \
        VALUE_TEXT[length] = '\0'; \
    } \
    VALUE_TOKEN = line_end + 1; \
    if('"' == VALUE_TEXT[0]) { \
        line_start = line_end = VALUE_TEXT; \
        while ('"' != *(++line_end)) { \
            *line_start = *line_end; \
            line_start++; \
        } \
        *line_start = '\0'; \
    } \
}

#define LINK_STATUS_FIELD     4
#define VALUE_LENGTH          64

int clear_linked_status(char *linked_txt, struct erase_link *eralink) {
    char linked_status[LARGE_TEXT_SIZE];
    char line_txt[LINKED_LINE_LEN];
    char value_txt[VALUE_LENGTH];
    struct linked_status link_stat;
    int index, clear = 0;

    int time_stamp = (time(NULL) - link_stat.time_stamp) / SECONDS_ONEDAY;
    linked_status[0] = '\0';
    char *line_token = linked_txt;
    while ('\0' != line_token[0]) {
        GET_LINE_VALUE(line_txt, line_token)
                char *value_token = line_txt;
        for (index = 0; index < LINK_STATUS_FIELD; index++) {
            GET_FIELD_VALUE(value_txt, value_token)
            switch (index) {
                case 0:
                    strcpy(link_stat.client_id, value_txt);
                    break;
                case 1:
                    TEXT_VALUE(link_stat.link_stat, link_stat_tab, value_txt)
                    break;
                case 2:
                    link_stat.time_stamp = atol(value_txt);
                    break;
                case 3:
                    TEXT_VALUE(link_stat.data_bomb, bomb_table, value_txt)
                    break;
            }
        }
        if (((LINKED_STATUS_UNLINK == link_stat.link_stat) && (time_stamp > eralink->unlink_day))
                || ((BOMB_STATUS_DONE == link_stat.data_bomb) && (time_stamp > eralink->bomb_day)))
            clear = 1;
        else strcat(linked_status, line_txt);
        ////////////////////////////////////////////////////////////////////////
    }
    if (clear) strcpy(linked_txt, linked_status);

    return clear;
}

