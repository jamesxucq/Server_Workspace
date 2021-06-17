#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "utility.h"
#include "logger.h"
#include "lock_pool.h"
#include "auth_ctrl.h"
#include "query_xml.h"

#define HEAD_TOKEN  "\r\n\r\n"
#define LINE_TOKEN  "\r\n"
#define FIELD_TOKEN  " "

int send_receive_cache(char *recv_buffer, char *send_buffer, int slen, int cache_sockfd) {
    int bytes_sent, bytes_recv;
    ////////////////////////////////////////////////////////////////////////////
    bytes_sent = send(cache_sockfd, send_buffer, slen, 0);
    if (bytes_sent != slen) { // reconnect to server
        LOG_INFO("send auth cache failed!");
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////    
    bytes_recv = recv(cache_sockfd, recv_buffer, AUTH_SOCKS_DATSIZ, 0);
    if (0 < bytes_recv) recv_buffer[bytes_recv] = '\0';
    else { // bytes_recv = 0; bytes_recv < 0;
        LOG_INFO("recv auth cache failed!");
        return -1;
    }
    return bytes_recv;
}

////////////////////////////////////////////////////////////////////////////////

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

static const struct value_entry execute_value_table[] = {
    {"successful", 0},
    {"exception", 1},
    {NULL, 0}
};

static const char *bool_text_table[] = {
    "false",
    "true"
};

////////////////////////////////////////////////////////////////////////////////
#define SET_LOCKED_REQUEST(REQ_VALUE, UID, SYNC_LOCKED, VALID) { \
    sprintf(REQ_VALUE.resource_identi, "%lu", UID); \
    /* //////////////////////////////////////////////////////////////////////////// */ \
    sprintf(REQ_VALUE.params, "locked=%s", bool_text_table[SYNC_LOCKED]); \
    strcpy(REQ_VALUE.validation, VALID); \
}

#define SET_LOCKED_RESPONSE(SET_STATUS, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(SET_STATUS, execute_value_table, RES_VALUE.execute_status) \
    } \
}

/*
CONTROL user_id SDTP/1.1\r\n
Command: set locked\r\n
Params: locked=true\r\n // false
Validation:xxxx\r\n\r\n
 */
void build_set_locked_request(char *locked_send, struct socks_request *req_value) {
    char line_txt[AUTH_LINE_LENGTH];

    sprintf(line_txt, "CONTROL %s SDTP/1.1"LINE_TOKEN, req_value->resource_identi);
    strcpy(locked_send, line_txt);
    strcat(locked_send, "Command: set locked"LINE_TOKEN);
    sprintf(line_txt, "Params: %s"LINE_TOKEN, req_value->params);
    strcat(locked_send, line_txt);
    sprintf(line_txt, "Validation: %s"HEAD_TOKEN, req_value->validation);
    strcat(locked_send, line_txt);

    // LOG_INFO("build set locked send:\n%s", locked_send);
}

/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
// SDTP/1.1 409 Conflict
 */
int parse_set_locked_response(struct socks_response *res_value, char *locked_recv) {
    char *line_txt = NULL;
    char *value = NULL;

    if (*locked_recv == '\0') return -1;
    // LOG_INFO("parse set locked recv:\n%s", locked_recv);

    line_txt = strsplit(locked_recv, LINE_TOKEN);
    ////////////////////////////////////////////////////////////////////////////
    value = split_value(line_txt);
    if (strcmp(line_txt, "SDTP/1.1")) return -1;
    if (!value) return -1;
    else res_value->status_code = atol(value);
    if (200 != res_value->status_code) return -1;
    ////////////////////////////////////////////////////////////////////////////
    while (line_txt = strsplit(NULL, LINE_TOKEN)) {
        value = split_line(line_txt);

        if (!value) continue;
        else if (!strcmp(line_txt, "Execute"))
            strcpy(res_value->execute_status, value);
    }
    return 0;
}

int set_cache_locked(int locked, unsigned int uid, char *validation, int cache_sockfd) {
    char reply_buffer[AUTH_TEXT_LENGTH];
    int set_status;
    //////////////////////////////////////////////////////////////////////////// 
    struct socks_request req_value;
    struct socks_response res_value;

    memset(reply_buffer, '\0', AUTH_TEXT_LENGTH);
    SET_LOCKED_REQUEST(req_value, uid, locked, validation);
    build_set_locked_request(reply_buffer, &req_value);
    if (send_receive_cache(reply_buffer, reply_buffer, strlen(reply_buffer), cache_sockfd) == -1) {
        LOG_INFO("send recv cache error");
        return -1;
    }
    if (parse_set_locked_response(&res_value, reply_buffer)) {
        LOG_INFO("parse unlocked response error");
        return -1;
    }
    SET_LOCKED_RESPONSE(set_status, res_value);

    return set_status;
}

////////////////////////////////////////////////////////////////////////////////
#define QUERY_REQUEST(REQ_VALUE, UID, ACCESS_KEY) { \
    sprintf(REQ_VALUE.resource_identi, "%lu", UID); \
    /* //////////////////////////////////////////////////////////////////////////// */ \
    trans_auth_csum(REQ_VALUE.authorization, UID, ACCESS_KEY); \
}

static const struct value_entry content_value_table[] = {
    {"text/xml", CONTENT_TYPE_XML},
    {"application/octet-stream", CONTENT_TYPE_OCTET},
    {"text/plain", CONTENT_TYPE_PLAIN},
    {NULL, 0}
};

#define QUERY_RESPONSE(CONTENT_TYPE, CONTENT_LEN, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(CONTENT_TYPE, content_value_table, RES_VALUE.content_type) \
        CONTENT_LEN = atoi(RES_VALUE.content_length); \
    } \
}

/*
OPTIONS user_id SDTP/1.1\r\n
Command: query\r\n
Authorization: uid:access_key\r\n\r\n
 */
void build_query_request(char *query_send, struct socks_request *req_value) {
    char line_txt[AUTH_LINE_LENGTH];

    sprintf(line_txt, "OPTIONS %s SDTP/1.1"LINE_TOKEN, req_value->resource_identi);
    strcpy(query_send, line_txt);
    strcat(query_send, "Command: query"LINE_TOKEN);
    sprintf(line_txt, "Authorization: %s"HEAD_TOKEN, req_value->authorization);
    strcat(query_send, line_txt);

    // LOG_INFO("build query send:\n%s", query_send);
}

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

/*
SDTP/1.1 200 OK\r\n 
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxxxxxxx\r\n\r\n 
//SDTP/1.1 400 Bad Request\r\n\r\n 
// SDTP/1.1 404 Not Found
 */
int parse_query_response(struct socks_response *res_value, char *query_recv) {
    char *line_txt = NULL;
    char *value = NULL;

    if (*query_recv == '\0') return -1;
    // LOG_INFO("parse query recv:\n%s", query_recv);

    line_txt = strsplit(query_recv, LINE_TOKEN);
    ////////////////////////////////////////////////////////////////////////////
    value = split_value(line_txt);
    if (strcmp(line_txt, "SDTP/1.1")) return -1;
    if (!value) return -1;
    else res_value->status_code = atol(value);
    if (200 != res_value->status_code) return -1;
    ////////////////////////////////////////////////////////////////////////////
    while (line_txt = strsplit(NULL, LINE_TOKEN)) {
        value = split_line(line_txt);

        if (!value) continue;
        else if (!strcmp(line_txt, "Content-Type"))
            strcpy(res_value->content_type, value);
        else if (!strcmp(line_txt, "Content-Length"))
            strcpy(res_value->content_length, value);
    }
    return 0;
}

#define SDTP_BUFFER_BODY(BODY_TXT, RECEIVE_TXT) \
    BODY_TXT = strstr(RECEIVE_TXT, HEAD_TOKEN); \
    if (BODY_TXT) BODY_TXT += 4;

struct xml_query *query_cached(struct xml_query *cached_query, unsigned int uid, char *access_key, int cache_sockfd) {
    char reply_buffer[AUTH_TEXT_LENGTH];
    char *xml_text;
    ////////////////////////////////////////////////////////////////////////////
    struct socks_request req_value;
    struct socks_response res_value;

    memset(reply_buffer, '\0', AUTH_TEXT_LENGTH);
    QUERY_REQUEST(req_value, uid, access_key);
    build_query_request(reply_buffer, &req_value);
    if (send_receive_cache(reply_buffer, reply_buffer, strlen(reply_buffer), cache_sockfd) == -1) {
        LOG_INFO("send recv cache error");
        return NULL;
    }
    SDTP_BUFFER_BODY(xml_text, reply_buffer);
    if (parse_query_response(&res_value, reply_buffer)) {
        LOG_INFO("parse query response error");
        return NULL;
    }

    off_t content_length;
    unsigned int content_type;
    QUERY_RESPONSE(content_type, content_length, res_value);
    if (!content_length || CONTENT_TYPE_XML != content_type)
        return NULL;

    if (query_from_xmlfile(cached_query, xml_text))
        return NULL;

    return cached_query;
}

////////////////////////////////////////////////////////////////////////////////

int init_cache_sockfd(serv_addr *cache_saddr) {
    int cache_sockfd;
    if ((cache_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        LOG_INFO("auth cache socket create error !!!");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(cache_saddr->sin_port);
    serv_addr.sin_addr.s_addr = inet_addr(cache_saddr->sin_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(cache_sockfd, (struct sockaddr *) &serv_addr, sizeof (struct sockaddr)) == -1) {
        LOG_INFO("auth cache socket connect error !!!");
        return -1;
    }
    // LOG_INFO("auth cache socket connect ok !!!");

    return cache_sockfd;
}

inline void close_cache_sockfd(int cache_sockfd) {
    close(cache_sockfd);
}

////////////////////////////////////////////////////////////////////////////////

int valid_cache_locked(char *validation, struct auth_value *avalue) {
    int ret_value = 0;

    int cache_sockfd = init_cache_sockfd(&avalue->saddr);
    if (-1 == cache_sockfd) {
        LOG_INFO("connect auth cache failed!");
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////
    struct xml_query cached_query;
    if (!query_cached(&cached_query, avalue->uid, avalue->access_key, cache_sockfd)) {
        LOG_INFO("query auth cache failed!");
        close_cache_sockfd(cache_sockfd);
        return -1;
    }
    if (!cached_query.locked) {
        if (set_cache_locked(TRUE, avalue->uid, cached_query.validation, cache_sockfd)) {
            LOG_INFO("locked auth cache failed!");
            ret_value = -1;
        }
        LOG_INFO("lock auth server OK.");
        strcpy(validation, cached_query.validation);
    } else {
        LOG_INFO("other operation locked auth server!");
        ret_value = 1;
    }
    ////////////////////////////////////////////////////////////////////////////
    close_cache_sockfd(cache_sockfd);
    return ret_value;
}

int unlocked_cache(struct auth_value *avalue, char *validation) {
    int cache_sockfd = init_cache_sockfd(&avalue->saddr);
    if (-1 == cache_sockfd) {
        LOG_INFO("connect auth cache failed!");
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////
    if (set_cache_locked(FALSE, avalue->uid, validation, cache_sockfd)) {
        LOG_INFO("locked auth cache failed!");
        close_cache_sockfd(cache_sockfd);
        return -1;
    }
    ////////////////////////////////////////////////////////////////////////////
    close_cache_sockfd(cache_sockfd);
    return 0;
}