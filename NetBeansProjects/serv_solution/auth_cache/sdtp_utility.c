/*
 * sdtp_utility.c
 *
 *  Created on: 2010-4-11
 *      Author: David
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_header.h"
#include "sdtp_utility.h"

#define HEAD_TOKEN  "\r\n\r\n"
#define LINE_TOKEN  "\r\n"

/*
OPTIONS user_id SDTP/1.1\r\n
Command: query\r\n
Authorization: uid:access_key\r\n\r\n
 */
int parse_query_recv(struct cache_request *req_value, char *query_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (query_recv[0] == '\0') return -1;
    _LOG_INFO("parse query recv:\n%s", query_recv);
//
    char *splittok;
    TEXT_SPLIT(line_txt, query_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "OPTIONS")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
CONTROL 192.168.1.102:8008 SDTP/1.1\r\n // 192.168.1.102
Command: add\r\n
Params: subtype=admin weight=5\r\n // admin/server 
Authorization: user:password\r\n\r\n
 */
int parse_addi_recv(struct cache_request *req_value, char *addi_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (addi_recv[0] == '\0') return -1;
    _LOG_INFO("parse add recv:\n%s", addi_recv);
//
    char *splittok;
    TEXT_SPLIT(line_txt, addi_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "CONTROL")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
CONTROL user_id SDTP/1.1\r\n // 192.168.1.10
Command: clear\r\n
Params: subtype=all-users\r\n // all-users/single-user/server/admin
Authorization: user:password\r\n\r\n
 */
int parse_clear_recv(struct cache_request *req_value, char *clear_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (clear_recv[0] == '\0') return -1;
    _LOG_INFO("parse clear recv:\n%s", clear_recv);
//
    char *splittok;
    TEXT_SPLIT(line_txt, clear_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "CONTROL")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
CONTROL * SDTP/1.1\r\n
Command: list\r\n
Params: subtype=server\r\n // server/admin
Authorization: user:password\r\n\r\n
 */
int parse_list_recv(struct cache_request *req_value, char *list_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (list_recv[0] == '\0') return -1;
    _LOG_INFO("parse list recv:\n%s", list_recv);
//
    char *splittok;
    TEXT_SPLIT(line_txt, list_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "CONTROL")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
OPTIONS user_id SDTP/1.1\r\n
Command: status\r\n
Params: addr=192.168.1.3 port=8080\r\n
Validation:xxxx\r\n\r\n
 */
int parse_status_recv(struct cache_request *req_value, char *status_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (status_recv[0] == '\0') return -1;
    _LOG_INFO("parse status recv:\n%s", status_recv);
//
    char *splittok;
    TEXT_SPLIT(line_txt, status_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "OPTIONS")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Validation"))
            strcpy(req_value->req_valid, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
CONTROL user_id SDTP/1.1\r\n
Command: set anchor\r\n
Params: anchor=xxxxx\r\n
Validation:xxxx\r\n\r\n
 */
int parse_set_anchor_recv(struct cache_request *req_value, char *anchor_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (anchor_recv[0] == '\0') return -1;
    _LOG_INFO("parse set anchor recv:\n%s", anchor_recv);
    // res_value->cached_anchor = (int) NULL;
//
    char *splittok;
    TEXT_SPLIT(line_txt, anchor_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "CONTROL")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Validation"))
            strcpy(req_value->req_valid, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
CONTROL user_id SDTP/1.1\r\n
Command: set locked\r\n
Params: locked=true\r\n // false
Validation:xxxx\r\n\r\n
 */
int parse_set_locked_recv(struct cache_request *req_value, char *locked_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (locked_recv[0] == '\0') return -1;
    _LOG_INFO("parse set locked recv:\n%s", locked_recv);
//
    char *splittok;
    TEXT_SPLIT(line_txt, locked_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "CONTROL")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Validation"))
            strcpy(req_value->req_valid, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
KALIVE user_id SDTP/1.1\r\n
Validation:xxxx\r\n\r\n
 */
int parse_keep_alive_recv(struct cache_request *req_value, char *kalive_recv) {
    char *line_txt = NULL;
    char *value = NULL;
//
    if (kalive_recv[0] == '\0') return -1;
    _LOG_INFO("parse keep alive recv:\n%s", kalive_recv);
//
    char *splittok;
    TEXT_SPLIT(line_txt, kalive_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "KALIVE")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
//
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Validation"))
            strcpy(req_value->req_valid, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

