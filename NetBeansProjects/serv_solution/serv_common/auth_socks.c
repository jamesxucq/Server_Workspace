#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//
#include "logger.h"
#include "string_utility.h"
//
#include "status_xml.h"
#include "auth_socks.h"
#include "auth_cache.h"
//
#define HEAD_TOKEN  "\r\n\r\n"
#define LINE_TOKEN  "\r\n"
#define FIELD_TOKEN  " "
//
serv_addr _cache_saddr_;
int _cache_sockfd_;


//

int auth_socks_create() {
    return 0;
}

int auth_socks_destroy() {
    return 0;
}

#define SDTP_BUFFER_BODY(BODY_TXT, RECEIVE_TXT) \
    BODY_TXT = strstr(RECEIVE_TXT, HEAD_TOKEN); \
    if (BODY_TXT) BODY_TXT += 4;

//

/*
OPTIONS user_id SDTP/1.1\r\n
Command: status\r\n
Validation:xxxx\r\n\r\n
 */
#define BUILD_STATUS_REQUEST(status_send, req_value) { \
    char *line_txt = lsprif(status_send, "OPTIONS %s SDTP/1.1"LINE_TOKEN, req_value.res_identity); \
    line_txt = lscpy(line_txt, "Command: status"LINE_TOKEN); \
    line_txt = lsprif(line_txt, "Params: %s"LINE_TOKEN, req_value.params); \
    sprintf(line_txt, "Validation: %s"HEAD_TOKEN, req_value.req_valid); \
    /* _LOG_INFO("build status send:\n%s", status_send); // disable by james 20130507 */ \
} 

/*
CONTROL user_id SDTP/1.1\r\n
Command: set anchor\r\n
Params: anchor=xxxxx\r\n
Validation:xxxx\r\n\r\n
 */
#define BUILD_SET_ANCHOR_REQUEST(anchor_send, req_value) { \
    char *line_txt = lsprif(anchor_send, "CONTROL %s SDTP/1.1"LINE_TOKEN, req_value.res_identity); \
    line_txt = lscpy(line_txt, "Command: set anchor"LINE_TOKEN); \
    line_txt = lsprif(line_txt, "Params: %s"LINE_TOKEN, req_value.params); \
    sprintf(line_txt, "Validation: %s"HEAD_TOKEN, req_value.req_valid); \
    _LOG_INFO("build set anchor send:\n%s", anchor_send); \
} 

/*
CONTROL user_id SDTP/1.1\r\n
Command: set locked\r\n
Params: locked=true\r\n // false
Validation:xxxx\r\n\r\n
 */
#define BUILD_SET_LOCKEN_REQUEST(locked_send, req_value) { \
    char *line_txt = lsprif(locked_send, "CONTROL %s SDTP/1.1"LINE_TOKEN, req_value.res_identity); \
    line_txt = lscpy(line_txt, "Command: set locked"LINE_TOKEN); \
    line_txt = lsprif(line_txt, "Params: %s"LINE_TOKEN, req_value.params); \
    sprintf(line_txt, "Validation: %s"HEAD_TOKEN, req_value.req_valid); \
    _LOG_INFO("build set locked send:\n%s", locked_send); \
}

/*
KALIVE user_id SDTP/1.1\r\n
Validation:xxxx\r\n\r\n
 */
#define BUILD_KEEP_ALIVE_REQUEST(kalive_send, req_value) { \
    char *line_txt = lsprif(kalive_send, "KALIVE %s SDTP/1.1"LINE_TOKEN, req_value.res_identity); \
    sprintf(line_txt, "Validation: %s"HEAD_TOKEN, req_value.req_valid); \
    _LOG_INFO("build keep alive send:\n%s", kalive_send); \
}
//

/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<access_key>2343ewr544r454rewfa45qwee</access_key>
<cached_anchor>xxxxxx</cached_anchor>
<location>xxxxxxxxx</location>
</query>
 */

/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxx\r\n
Content-Type: text/xml\r\n  // application/octet-stream
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
 */
int parse_status_response(struct socks_response *res_value, char *status_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*status_recv == '\0') return -1;
    // _LOG_INFO("parse status recv:\n%s", status_recv); // disable by james 20130507
    char *splittok = NULL;
    TEXT_SPLIT(line_txt, status_recv, LINE_TOKEN, splittok)
    //
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "SDTP/1.1")) return -1;
    if (!value) return -1;
    else res_value->status_code = atol(value);
    if (200 != res_value->status_code) return -1;
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Content-Type"))
            strcpy(res_value->content_type, value);
        else if (!strcmp(line_txt, "Content-Length"))
            strcpy(res_value->content_length, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
// SDTP/1.1 409 Conflict
 */
int parse_set_anchor_response(struct socks_response *res_value, char *anchor_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*anchor_recv == '\0') return -1;
    _LOG_INFO("parse set anchor recv:\n%s", anchor_recv);
    char *splittok = NULL;
    TEXT_SPLIT(line_txt, anchor_recv, LINE_TOKEN, splittok)
    //
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "SDTP/1.1")) return -1;
    if (!value) return -1;
    else res_value->status_code = atol(value);
    if (200 != res_value->status_code) return -1;
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Execute"))
            strcpy(res_value->execute_status, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
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
    //
    if (*locked_recv == '\0') return -1;
    _LOG_INFO("parse set locked recv:\n%s", locked_recv);
    char *splittok = NULL;
    TEXT_SPLIT(line_txt, locked_recv, LINE_TOKEN, splittok)
    //
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "SDTP/1.1")) return -1;
    if (!value) return -1;
    else res_value->status_code = atol(value);
    if (200 != res_value->status_code) return -1;
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Execute"))
            strcpy(res_value->execute_status, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
SDTP/1.1 200 OK\r\n
Execute: successful\r\n // exception
//SDTP/1.1 400 Bad Request\r\n\r\n
// SDTP/1.1 404 Not Found
 */
int parse_keep_alive_response(struct socks_response *res_value, char *kalive_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*kalive_recv == '\0') return -1;
    _LOG_INFO("parse keep alive recv:\n%s", kalive_recv);
    char *splittok = NULL;
    TEXT_SPLIT(line_txt, kalive_recv, LINE_TOKEN, splittok)
    //
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "SDTP/1.1")) return -1;
    if (!value) return -1;
    else res_value->status_code = atol(value);
    if (200 != res_value->status_code) return -1;
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Execute"))
            strcpy(res_value->execute_status, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

int hand_status_recv(struct list_data *ldata, char *status_recv) {
    struct xml_status status;
    //
    if (!ldata || !status_recv) return -1;
    if (status_from_xmlfile(&status, status_recv))
        return -1;
    // _LOG_DEBUG("cached_anchor:%lu location:%s cached_session:%s", status.cached_anchor, status.location, status.cached_seion);
    ldata->uid = status.uid;
    ldata->last_anchor = status.cached_anchor;
    strcpy(ldata->location, status.location);
    strcpy(ldata->seion_id, status.cached_seion);
    //
    return 0;
}
//

struct list_data *get_ldata_by_uid(struct list_data *ldata, unsigned int uid, char *saddr, int port, char *req_valid, int cache_sockfd) {
    char reply_buffer[AUTH_TEXT_LEN];
    char *xml_text;
    //
    struct socks_request req_value;
    struct socks_response res_value;
    //
    MKZERO(reply_buffer);
_LOG_DEBUG("status req_valid:%s", req_valid);
    STATUS_REQUEST(req_value, uid, saddr, port, req_valid);
    BUILD_STATUS_REQUEST(reply_buffer, req_value);
    if (send_receive_cache(reply_buffer, reply_buffer, strlen(reply_buffer), cache_sockfd) == -1) {
        _LOG_WARN("send recv cache exception!");
        return NULL;
    }
    SDTP_BUFFER_BODY(xml_text, reply_buffer);
    if (parse_status_response(&res_value, reply_buffer)) {
        // _LOG_WARN("parse query response exception!"); // disable by james 20130507
        return NULL;
    }
    //
    uint32 content_length = 0;
    unsigned int content_type = CONTENT_TYPE_INVA;
    STATUS_RESPONSE(content_type, content_length, res_value);
    if (!content_length || CONTENT_TYPE_XML != content_type)
        return NULL;
    if (hand_status_recv(ldata, xml_text))
        return NULL;
    //
    return ldata;
}

int set_cache_anchor(unsigned int uid, unsigned int last_anchor, char *req_valid, int cache_sockfd) {
    char reply_buffer[AUTH_TEXT_LEN];
    //  
    struct socks_request req_value;
    struct socks_response res_value;
    //
    MKZERO(reply_buffer);
    SET_ANCHOR_REQUEST(req_value, uid, last_anchor, req_valid);
    BUILD_SET_ANCHOR_REQUEST(reply_buffer, req_value);
    if (send_receive_cache(reply_buffer, reply_buffer, strlen(reply_buffer), cache_sockfd) == -1) {
        _LOG_WARN("send recv cache exception!");
        return -1;
    }
    if (parse_set_anchor_response(&res_value, reply_buffer)) {
        _LOG_WARN("parse set anchor response exception!");
        return -1;
    }
    int set_status = 0x00;
    SET_ANCHOR_RESPONSE(set_status, res_value);
    //
    return set_status;
}

int set_cache_unlocked(unsigned int uid, char *req_valid, int cache_sockfd) {
    char reply_buffer[AUTH_TEXT_LEN];
    // 
    struct socks_request req_value;
    struct socks_response res_value;
    //
    MKZERO(reply_buffer);
    SET_LOCKED_REQUEST(req_value, uid, FALSE, req_valid);
    BUILD_SET_LOCKEN_REQUEST(reply_buffer, req_value);
    if (send_receive_cache(reply_buffer, reply_buffer, strlen(reply_buffer), cache_sockfd) == -1) {
        _LOG_WARN("send recv cache exception!");
        return -1;
    }
    if (parse_set_locked_response(&res_value, reply_buffer)) {
        _LOG_WARN("parse unlocked response exception!");
        return -1;
    }
    int set_status = 0x00;
    SET_LOCKED_RESPONSE(set_status, res_value);
    //
    return set_status;
}

int keep_alive_cache(unsigned int uid, char *req_valid, int cache_sockfd) {
    char reply_buffer[AUTH_TEXT_LEN];
    //  
    struct socks_request req_value;
    struct socks_response res_value;
    //
    MKZERO(reply_buffer);
    KEEP_ALIVE_REQUEST(req_value, uid, req_valid);
    BUILD_KEEP_ALIVE_REQUEST(reply_buffer, req_value);
    if (send_receive_cache(reply_buffer, reply_buffer, strlen(reply_buffer), cache_sockfd) == -1) {
        _LOG_WARN("send recv cache exception!");
        return -1;
    }
    if (parse_keep_alive_response(&res_value, reply_buffer)) {
        _LOG_WARN("parse keep alive response exception!");
        return -1;
    }
    int keep_status = 0x00;
    KEEP_ALIVE_RESPONSE(keep_status, res_value);
    //
    return keep_status;
}

//

int ping_auth_cache(serv_addr *cache_saddr) {
    return 0;
}

int init_cache_sockfd(serv_addr *cache_saddr) {
    int cache_sockfd;
    if ((cache_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        _LOG_WARN("auth cache socket create error!");
        return -1;
    }
_LOG_DEBUG("auth cache addr:%s port:%d", cache_saddr->sin_addr, cache_saddr->sin_port); 
    //
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(cache_saddr->sin_port);
    serv_addr.sin_addr.s_addr = inet_addr(cache_saddr->sin_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if (connect(cache_sockfd, (struct sockaddr *) &serv_addr, sizeof (struct sockaddr)) == -1) {
        _LOG_WARN("auth cache socket connect error!");
        _LOG_WARN("sin_addr:%s sin_port:%d", cache_saddr->sin_addr, cache_saddr->sin_port);
        return -1;
    }
    _LOG_INFO("auth cache socket connect ok!");
    //
    return cache_sockfd;
}

inline void close_cache_sockfd(int cache_sockfd) {
    close(cache_sockfd);
}

#define RETRY_CACHE_TIMES       5
#define RETRY_CONNECT_DELAY     3 // 3s

int update_cache_server() {
    int retry_times = 0;
    //
    if (0 < _cache_sockfd_) {
        close(_cache_sockfd_);
        _cache_sockfd_ = -1;
    }
    while ((0 >= _cache_sockfd_) && (RETRY_CACHE_TIMES > retry_times++)) {
        _LOG_INFO("connect to cache server.");
        _cache_sockfd_ = init_cache_sockfd(&_cache_saddr_);
        if (-1 == _cache_sockfd_) {
            _LOG_WARN("auth cache socket reconnect error!");
            sleep(RETRY_CONNECT_DELAY);
        }
    }
    return _cache_sockfd_;
}

int send_receive_cache(char *recv_buffer, char *send_buffer, int slen, int cache_sockfd) {
    int bytes_sent, bytes_recv;
    //
    bytes_sent = send(cache_sockfd, send_buffer, slen, 0);
    if (bytes_sent != slen) { // reconnect to server
        _LOG_INFO("send auth cache failed!");
        cache_sockfd = update_cache_server();
        if (-1 == cache_sockfd) return -1;
        _LOG_INFO("auth cache socket reconnect ok!");

        bytes_sent = send(cache_sockfd, send_buffer, slen, 0);
        if (bytes_sent != slen) {
            _LOG_WARN("resend auth cache failed!");
            return -1;
        }
    }
    //    
    bytes_recv = recv(cache_sockfd, recv_buffer, AUTH_SOCKS_DATSIZ, 0);
    if (0 < bytes_recv) recv_buffer[bytes_recv] = '\0';
    else { // bytes_recv = 0; bytes_recv < 0;
        _LOG_INFO("recv auth cache failed!");
        cache_sockfd = update_cache_server();
        if (-1 == cache_sockfd) return -1;
        _LOG_INFO("auth cache socket reconnect ok!");

        bytes_sent = send(cache_sockfd, send_buffer, slen, 0);
        if (bytes_sent != slen) {
            _LOG_WARN("resend auth cache failed!");
            return -1;
        }
        bytes_recv = recv(cache_sockfd, recv_buffer, AUTH_SOCKS_DATSIZ, 0);
        if (0 < bytes_recv) recv_buffer[bytes_recv] = '\0';
        else {
            _LOG_WARN("rerecv auth cache failed!");
            return -1;
        }
    }
    return bytes_recv;
}