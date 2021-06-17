#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_header.h"
#include "sdtp_utility.h"

#define HEAD_TOKEN  "\r\n\r\n"
#define LINE_TOKEN  "\r\n"
#define ONLY_HEADER_SECTION		0

/*
OPTIONS user_name SDTP/1.1\r\n
Command: register\r\n
From: xxxx\r\n
User-Agent: xxxxx\r\n
Authorization: user:password\r\n\r\n
 */
int parse_regis_recv(struct regis_request *req_value, char *regis_recv) {
    char *line_txt = NULL;
    char *value = NULL;

    if (regis_recv[0] == '\0') return -1;
    _LOG_INFO("parse regis recv:\n%s", regis_recv);

    char *splittok;
    TEXT_SPLIT(line_txt, regis_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "OPTIONS")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);

    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "From"))
            strcpy(req_value->from, value);
        else if (!strcmp(line_txt, "User-Agent"))
            strcpy(req_value->user_agent, value);
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
OPTIONS user_name SDTP/1.1\r\n
Command: settings\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
 */
int parse_settings_recv(struct regis_request *req_value, char *settings_recv) {
    char *line_txt = NULL;
    char *value = NULL;

    if (settings_recv[0] == '\0') return -1;
    _LOG_INFO("parse settings recv:\n%s", settings_recv);

    char *splittok;
    TEXT_SPLIT(line_txt, settings_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "OPTIONS")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);

    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "From"))
            strcpy(req_value->from, value);
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
OPTIONS user_name SDTP/1.1\r\n
Command: link\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
 */
int parse_link_recv(struct regis_request *req_value, char *link_recv) {
    char *line_txt = NULL;
    char *value = NULL;

    if (link_recv[0] == '\0') return -1;
    _LOG_INFO("parse link recv:\n%s", link_recv);

    char *splittok;
    TEXT_SPLIT(line_txt, link_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "OPTIONS")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);

    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "From"))
            strcpy(req_value->from, value);
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
OPTIONS user_name SDTP/1.1\r\n
Command: unlink\r\n
From: xxxx\r\n
Authorization: user:password\r\n\r\n
 */
int parse_unlink_recv(struct regis_request *req_value, char *unlink_recv) {
    char *line_txt = NULL;
    char *value = NULL;

    if (unlink_recv[0] == '\0') return -1;
    _LOG_INFO("parse unlink recv:\n%s", unlink_recv);

    char *splittok;
    TEXT_SPLIT(line_txt, unlink_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "OPTIONS")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);

    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "From"))
            strcpy(req_value->from, value);
        else if (!strcmp(line_txt, "Authorization"))
            strcpy(req_value->authorize, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}


