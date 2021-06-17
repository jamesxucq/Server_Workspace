#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "logger.h"
#include "string_utility.h"
#include "linked_status.h"

struct value_node {
    const char *text;
    int value;
};

#define TEXT_VALUE(VALUE, TABLE, TEXT) { \
    struct value_node *item; \
    for(item = (struct value_node *)TABLE; item->text; ++item) \
	if(!strcmp(item->text, TEXT)) \
            break; \
    VALUE = item->value; \
}

static const struct value_node bomb_table[] = {
    {"false", 0},
    {"true", 1},
    {"done", 2},
    {NULL, 0}
};

static const struct value_node link_stat_tab[] = {
    {"noauth", LINKED_STATUS_NOAUTH},
    {"linked", LINKED_STATUS_LINKED},
    {"unlink", LINKED_STATUS_UNLINK},
    {NULL, 0}
};

/* <client name,link status,action time,data bomb>
 * <"rfjames","linked",425254,"false"> */

#define GET_LINE_VALUE(LINE_VALUE, LINE_TOKEN) { \
    char *line_end, *line_start; \
    if((line_start = strchr(LINE_TOKEN, '<'))) { \
        if ((line_end = strchr(line_start+1, '>'))) { \
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
    if ((line_end = strpbrk(line_start, ",>"))) { \
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

// fprintf(stderr, "client_id: %s\n", client_id);
// fprintf(stderr, "client_id: %s link_stat: %d time_stamp: %d data_bomb: %d\n", link_stat->client_id, link_stat->link_stat, link_stat->time_stamp, link_stat->data_bomb);

int find_linked_status(struct linked_status *link_stat, char *linked_txt, char *client_id) {
    char line_txt[LINKED_LINE_LEN];
    char value_txt[VALUE_LENGTH];
    int inde, found = 0;
//
    char *line_toke = linked_txt;
    while ('\0' != line_toke[0]) {
        GET_LINE_VALUE(line_txt, line_toke)
                char *value_toke = line_txt;
        for (inde = 0; inde < LINK_STATUS_FIELD; inde++) {
            GET_FIELD_VALUE(value_txt, value_toke)
            switch (inde) {
                case 0:
                    strcpy(link_stat->client_id, value_txt);
                    break;
                case 1:
                    TEXT_VALUE(link_stat->link_stat, link_stat_tab, value_txt)
                    break;
                case 2:
                    link_stat->time_stamp = atol(value_txt);
                    break;
                case 3:
                    TEXT_VALUE(link_stat->data_bomb, bomb_table, value_txt)
                    break;
            }
        }
        if (!strcmp(client_id, link_stat->client_id)) {
            found = 1;
            break;
        }
    }
//
    return found;
}

void insert_linkstat_text(char *linked_txt, char *client_id) {
    char line_txt[LINKED_LINE_LEN];
    sprintf(line_txt, "<\"%s\",\"noauth\",%ld,\"false\">", client_id, time(NULL));
    int link_len = strlen(linked_txt);
    if (LARGE_TEXT_SIZE > strlen(line_txt) + link_len)
        strcpy(linked_txt + link_len, line_txt);
}

void add_linkstat_text(char *linked_txt, char *client_id) {
    char line_txt[LINKED_LINE_LEN];
    sprintf(line_txt, "<\"%s\",\"linked\",%ld,\"false\">", client_id, time(NULL));
    int link_len = strlen(linked_txt);
    if (LARGE_TEXT_SIZE > strlen(line_txt) + link_len)
        strcpy(linked_txt + link_len, line_txt);
}

void erase_linkstat_text(char *linked_txt, char *client_id) {
    char link_con[LARGE_TEXT_SIZE];
    char line_txt[LINKED_LINE_LEN];
    char value_txt[VALUE_LENGTH];
    int found = 0;
//
    char *last = link_con;
    last[0] = '\0';
    char *line_toke = linked_txt;
    while ('\0' != line_toke[0]) {
        GET_LINE_VALUE(line_txt, line_toke)
                char *value_toke = line_txt;
        GET_FIELD_VALUE(value_txt, value_toke)
        if (strcmp(client_id, value_txt)) last = lscpy(last, line_txt);
        else found = 1;
    }
    if (found) strcpy(linked_txt, link_con);
}

static const char *bomb_text_table[] = {
    "false",
    "true",
    "done"
};

static const char *linkstat_text_table[] = {
    "noauth",
    "linked",
    "unlink"
};

#define LINKSTAT_TEXT(line_txt, LINK_STAT) \
    sprintf(line_txt, "<\"%s\",\"%s\",%ld,\"%s\">", \
            LINK_STAT.client_id, \
            linkstat_text_table[LINK_STAT.link_stat], \
            LINK_STAT.time_stamp, \
            bomb_text_table[LINK_STAT.data_bomb]);

int modify_linkstat(char *linked_txt, char *client_id, int linkstat) {
    char link_con[LARGE_TEXT_SIZE];
    char line_txt[LINKED_LINE_LEN];
    char value_txt[VALUE_LENGTH];
    struct linked_status link_stat;
    int inde, found = 0;
//
    char *last = link_con;
    last[0] = '\0';
    char *line_toke = linked_txt;
    while ('\0' != line_toke[0]) {
        GET_LINE_VALUE(line_txt, line_toke)
                char *value_toke = line_txt;
        for (inde = 0; inde < LINK_STATUS_FIELD; inde++) {
            GET_FIELD_VALUE(value_txt, value_toke)
            switch (inde) {
                case 0:
                    strcpy(link_stat.client_id, value_txt);
                    break;
                case 1:
                case 2:
                    break;
                case 3:
                    TEXT_VALUE(link_stat.data_bomb, bomb_table, value_txt)
                    break;
            }
        }
_LOG_DEBUG("client_id: %s\n", client_id);
_LOG_DEBUG("client_id: %s link_stat: %d time_stamp: %ld data_bomb: %d\n", link_stat.client_id, link_stat.link_stat, link_stat.time_stamp, link_stat.data_bomb);
        if (!strcmp(client_id, link_stat.client_id)) {
            link_stat.link_stat = linkstat;
            link_stat.time_stamp = time(NULL);
            LINKSTAT_TEXT(line_txt, link_stat)
            found = 1;
        }
        last = lscpy(last, line_txt);
    }
    if (found) strcpy(linked_txt, link_con);
//
    return found;
}

int modify_databomb(char *linked_txt, char *client_id, int data_bomb) {
    char link_con[LARGE_TEXT_SIZE];
    char line_txt[LINKED_LINE_LEN];
    char value_txt[VALUE_LENGTH];
    struct linked_status link_stat;
    int inde, found = 0;
//
    char *last = link_con;
    last[0] = '\0';
    char *line_toke = linked_txt;
    while ('\0' != line_toke[0]) {
        GET_LINE_VALUE(line_txt, line_toke)
                char *value_toke = line_txt;
        for (inde = 0; inde < LINK_STATUS_FIELD; inde++) {
            GET_FIELD_VALUE(value_txt, value_toke)
            switch (inde) {
                case 0:
                    strcpy(link_stat.client_id, value_txt);
                    break;
                case 1:
                    TEXT_VALUE(link_stat.link_stat, link_stat_tab, value_txt)
                    break;
                case 2:
                case 3:
                    break;
            }
        }
        if (!strcmp(client_id, link_stat.client_id)) {
            link_stat.data_bomb = data_bomb;
            link_stat.time_stamp = time(NULL);
            LINKSTAT_TEXT(line_txt, link_stat)
            found = 1;
        }
        last = lscpy(last, line_txt);
    }
    if (found) strcpy(linked_txt, link_con);
//
    return found;
}

int modify_linkstat_databomb(char *linked_txt, char *client_id, int linkstat, int data_bomb) {
    char link_con[LARGE_TEXT_SIZE];
    char line_txt[LINKED_LINE_LEN];
    char value_txt[VALUE_LENGTH];
    struct linked_status link_stat;
    int found = 0;
//
    char *last = link_con;
    last[0] = '\0';
    char *line_toke = linked_txt;
    while ('\0' != line_toke[0]) {
        GET_LINE_VALUE(line_txt, line_toke)
                char *value_toke = line_txt;
        GET_FIELD_VALUE(value_txt, value_toke)
        if (!strcmp(client_id, value_txt)) {
            strcpy(link_stat.client_id, client_id);
            link_stat.link_stat = linkstat;
            link_stat.time_stamp = time(NULL);
            link_stat.data_bomb = data_bomb;
            LINKSTAT_TEXT(line_txt, link_stat)
            found = 1;
        }
        last = lscpy(last, line_txt);
    }
    if (found) strcpy(linked_txt, link_con);
//
    return found;
}