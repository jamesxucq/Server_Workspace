/*
 * sdtp_utility.c
 *
 *  Created on: 2010-4-11
 *      Author: Divad
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_header.h"
#include "sdtp_utility.h"

#define HEAD_TOKEN  "\r\n\r\n"
#define LINE_TOKEN  "\r\n"
#define ONLY_HEADER_SECTION		0

/*
INITIAL user_id SDTP/1.1\r\n
Params: valid=xxxxxxxxxxxx\r\n
Session: xxx\r\n\r\n
 */
int parse_initial_recv(struct serv_request *req_value, char *initial_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*initial_recv == '\0') return -1;
// _LOG_INFO("parse initial recv:\n%s", initial_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, initial_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "INITIAL")) return -1;
    if (value) strcpy(req_value->res_identity, value);
    else return -1;
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
KALIVE user_id SDTP/1.1\r\n
Session: xxx\r\n\r\n
 */
int parse_kalive_recv(struct serv_request *req_value, char *kalive_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*kalive_recv == '\0') return -1;
    _LOG_INFO("parse kalive recv:\n%s", kalive_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, kalive_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "KALIVE")) return -1;
    if (value) strcpy(req_value->res_identity, value);
    else return -1;
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
HEAD filename SDTP/1.1\r\n
Params: subentry=list value=recursion-files\r\n // file // list // anchor-files // recursion-files // recursion-directories // list-directory
Session: xxx\r\n\r\n
 */
int parse_head_recv(struct serv_request *req_value, char *head_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*head_recv == '\0') return -1;
    _LOG_INFO("parse head recv:\n%s", head_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, head_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "HEAD")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);

    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Attach: md5=xxxxxxxxxxxxxxx\r\n
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
 */
void build_head_simple_send(struct evdata *evdat, struct serv_response *res_value) {
    char *header = (char *) evdat->head_buffer;
    //
#define HEAD_SIMPLE_SUCCESS \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value->content_length); \
    header = lsprif(header, "Content-Type: %s"LINE_TOKEN, res_value->content_type); \
    header = lsprif(header, "Attach: %s"LINE_TOKEN, res_value->attach); \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value->seion_id); \
    FINISH_HDBD_BUFFER
    BUILD_STATUS_LINE(header, res_value->status_code, HEAD_SIMPLE_SUCCESS)
            //
_LOG_INFO("build head simple send:\n%s", (char *) evdat->head_buffer); // disable by james 20130507
}

/*
SDTP/1.1 200 OK\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Content-Range: file 4455\8888\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
 */
void build_head_complex_send(struct evdata *evdat, struct serv_response *res_value) {
    char *header = (char *) evdat->head_buffer;
    //
#define HEAD_COMPLEX_SUCCESS \
    header = lsprif(header, "Last-Modified: %s"LINE_TOKEN, res_value->last_write); \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value->content_length); \
    header = lsprif(header, "Content-Type: %s"LINE_TOKEN, res_value->content_type); \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value->seion_id); \
    FINISH_HDBD_BUFFER
    BUILD_STATUS_LINE(header, res_value->status_code, HEAD_COMPLEX_SUCCESS)
            //
_LOG_INFO("build head complex send:\n%s", (char *) evdat->head_buffer); // disable by james 20130507
}

void(*build_head_send[])(struct evdata*, struct serv_response*) = {
    NULL, // ENTRY_INVA
    NULL, // ENTRY_CHKS
    build_head_complex_send, // ENTRY_FILE
    NULL, // ENTRY_ANCHOR
    build_head_simple_send // ENTRY_LIST
};

/*
GET filename SDTP/1.1\r\n
Params: subentry=chks value=md5\r\n // file // list // chks/md5 /adler32&md5 /sha1 //  add by james 2011-09-15   
Range: file=4455\r\n /chunk /block
Session: xxx\r\n\r\n
 */
/*
GET Content SDTP/1.1\r\n
Params: subentry=list value=anchor-files\r\n // file // list // chks // anchor-files // recursion-files // recursion-directories // list-directory
Session: xxx\r\n\r\n
 */
/*
GET user_id SDTP/1.1\r\n
Params: subentry=anchor value=last-anchor\r\n // file // list // chks // anchor
Session: xxx\r\n\r\n
 */

/*
GET filename SDTP/1.1\r\n
Params: subentry=file\r\n // file // list // chks
Range: file=4455\r\n /chunk / block
Cache-Verify: no-cache\r\n //no-cache /cache
Session: xxx\r\n\r\n
 */
int parse_get_recv(struct serv_request *req_value, char *get_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*get_recv == '\0') return -1;
    _LOG_INFO("parse get recv:\n%s", get_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, get_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "GET")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Range"))
            strcpy(req_value->content_range, value);
        else if (!strcmp(line_txt, "Cache-Verify"))
            strcpy(req_value->cache_verify, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
<?xml version="1.0" encoding="UTF-8"?>
<anchor version="1.2.1">
<uid>12345321</uid> 
<last_anchor>xxxxx</last_anchor>
</anchor>
 */
/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxx\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Session: xxx\r\n \r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 404 Not Found
//SDTP/1.1 500 Internal Server Error
 */
/*
<?xml version="1.0" encoding="UTF-8"?>
<directories version="1.2.1">
<dire_name>xxxxxxxxxx</dire_name>
<dire_name>xxxxxxxxxx</dire_name>
</directories>
 */
/*
<?xml version="1.0" encoding="UTF-8"?>
<files_attrib version="1.2.1">
<file action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</file>
<file action_type="A" file_size="343" last_write="Wed, 15 Nov 1995 04:58:08 GMT" reserved="ab454">xxxxxxxxxx</file>
</files_attrib>
 */
/*
SDTP/1.1 200 OK\r\n
Content-Type: text/xml\r\n  // application/octet-stream
Content-Length: xxxxx\r\n
Session: xxx\r\n \r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 404 Not Found
//SDTP/1.1 500 Internal Server Error
 */

/*
SDTP/1.1 200 OK\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
 */
void build_get_simple_send(struct evdata *evdat, struct serv_response *res_value) {
    char *header = (char *) evdat->head_buffer;
    //
#define GET_SIMPLE_SUCCESS \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value->content_length); \
    header = lsprif(header, "Content-Type: %s"LINE_TOKEN, res_value->content_type); \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value->seion_id); \
    FINISH_HDBD_BUFFER
    BUILD_STATUS_LINE(header, res_value->status_code, GET_SIMPLE_SUCCESS)
    //
    _LOG_INFO("build get simple send:\n%s", (char *) evdat->head_buffer); // disable by james 20130507
}

/*
SDTP/1.1 200 OK\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Content-Range: file 4455\8888\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
 */
void build_get_complex_send(struct evdata *evdat, struct serv_response *res_value) {
    char *header = (char *) evdat->head_buffer;
    //
#define GET_COMPLEX_SUCCESS \
    header = lsprif(header, "Last-Modified: %s"LINE_TOKEN, res_value->last_write); \
    header = lsprif(header, "Content-Range: %s"LINE_TOKEN, res_value->content_range); \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value->content_length); \
    header = lsprif(header, "Content-Type: %s"LINE_TOKEN, res_value->content_type); \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value->seion_id); \
    FINISH_HDBD_BUFFER
    BUILD_STATUS_LINE(header, res_value->status_code, GET_COMPLEX_SUCCESS)
    //
    _LOG_INFO("build get complex send:\n%s", (char *) evdat->head_buffer); // disable by james 20130507
}

void(*build_get_send[])(struct evdata*, struct serv_response*) = {
    NULL,
    build_get_simple_send,
    build_get_complex_send,
    build_get_simple_send,
    build_get_simple_send
};

/*
POST FileName SDTP/1.1\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n			// add by james 2010-05-25
Content-Range: file 4455/455255\r\n /chunk
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
 */
int parse_post_recv(struct serv_request *req_value, char *post_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*post_recv == '\0') return -1;
    _LOG_INFO("parse post recv:\n%s", post_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, post_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "POST")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Last-Modified"))
            strcpy(req_value->last_write, value);
        else if (!strcmp(line_txt, "Content-Range"))
            strcpy(req_value->content_range, value);
        else if (!strcmp(line_txt, "Content-Type"))
            strcpy(req_value->content_type, value);
        else if (!strcmp(line_txt, "Content-Length"))
            strcpy(req_value->content_length, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
PUT FileName SDTP/1.1\r\n
Params: subentry=chks value=md5\r\n // file // chks/adler32 /adler32&md5 //  add by james 2011-09-15
Range: file=4455\r\n /chunk /block
Content-Type: application/octet-stream\r\n  // text/xml
Content-Length: xxxxx\r\n
Session: xxx\r\n\r\n
 */

/*
PUT FileName SDTP/1.1\r\n
Params: subentry=file\r\n // file // chks					// add by james 2011-10-28
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n			// add by james 2010-05-25
Content-Range: file 4455/55244\r\n /chunk
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
 */
int parse_put_recv(struct serv_request *req_value, char *put_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*put_recv == '\0') return -1;
    _LOG_INFO("parse put recv:\n%s", put_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, put_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "PUT")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Last-Modified"))
            strcpy(req_value->last_write, value);
        else if (!strcmp(line_txt, "Range") || !strcmp(line_txt, "Content-Range"))
            strcpy(req_value->content_range, value);
        else if (!strcmp(line_txt, "Content-Length"))
            strcpy(req_value->content_length, value);
        else if (!strcmp(line_txt, "Content-Type"))
            strcpy(req_value->content_type, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
SDTP/1.1 200 OK\r\n
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
 */
void build_put_simple_send(struct evdata *evdat, struct serv_response *res_value) {
    char *header = (char *) evdat->head_buffer;
    //
#define PUT_SIMPLE_SUCCESS \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value->seion_id); \
    FINISH_HEADER_BUFFER
    BUILD_STATUS_LINE(header, res_value->status_code, PUT_SIMPLE_SUCCESS)
            //
_LOG_INFO("build put simple send:\n%s", header); // disable by james 20130507
}

/*
SDTP/1.1 200 OK\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Content-Range: file 4455/8000\r\n
Content-Length: xxxxx\r\n
Content-Type: application/octet-stream\r\n  // text/xml
Session: xxx\r\n\r\n
//SDTP/1.1 400 Bad Request\r\n\r\n
//SDTP/1.1 401 Not Authorized
//SDTP/1.1 500 Internal Server Error
 */
void build_put_complex_send(struct evdata *evdat, struct serv_response *res_value) {
    char *header = (char *) evdat->head_buffer;
    //
#define PUT_COMPLEX_SUCCESS \
    header = lsprif(header, "Last-Modified: %s"LINE_TOKEN, res_value->last_write); \
    header = lsprif(header, "Content-Range: %s"LINE_TOKEN, res_value->content_range); \
    header = lsprif(header, "Content-Length: %s"LINE_TOKEN, res_value->content_length); \
    header = lsprif(header, "Content-Type: %s"LINE_TOKEN, res_value->content_type); \
    sprintf(header, "Session: %s"HEAD_TOKEN, res_value->seion_id); \
    FINISH_HDBD_BUFFER
    BUILD_STATUS_LINE(header, res_value->status_code, PUT_COMPLEX_SUCCESS)
            //
_LOG_INFO("build put complex send:\n%s", (char *) evdat->head_buffer); // disable by james 20130507
}

void(*build_put_send[])(struct evdata*, struct serv_response*) = {
    NULL,
    build_put_complex_send,
    build_put_simple_send
};

/*
COPY FileName SDTP/1.1\r\n
Params: desti=xxxxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Session: xxx\r\n\r\n
 */
int parse_copy_recv(struct serv_request *req_value, char *copy_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*copy_recv == '\0') return -1;
    _LOG_INFO("parse copy recv:\n%s", copy_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, copy_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "COPY")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Last-Modified"))
            strcpy(req_value->last_write, value);
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
MOVE FileName SDTP/1.1\r\n
Params: desti=xxxxxxx\r\n
Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n
Session: xxx\r\n\r\n
 */
int parse_move_recv(struct serv_request *req_value, char *move_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*move_recv == '\0') return -1;
    _LOG_INFO("parse move recv:\n%s", move_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, move_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "MOVE")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Last-Modified"))
            strcpy(req_value->last_write, value);
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
DELETE FileName SDTP/1.1\r\n
Session: xxx\r\n\r\n
 */
int parse_delete_recv(struct serv_request *req_value, char *delete_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*delete_recv == '\0') return -1;
    _LOG_INFO("parse delete recv:\n%s", delete_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, delete_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "DELETE")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}

/*
FINAL user_id SDTP/1.1\r\n
Params: subtype=end\r\n //successful //finish
Session: xxx\r\n\r\n
 */
int parse_final_recv(struct serv_request *req_value, char *final_recv) {
    char *line_txt = NULL;
    char *value = NULL;
    //
    if (*final_recv == '\0') return -1;
// _LOG_INFO("parse final recv:\n%s", final_recv); // disable by james 20130507
    //
    char *splittok;
    TEXT_SPLIT(line_txt, final_recv, LINE_TOKEN, splittok)
    SPLIT_VALUE(value, line_txt)
    if (strcmp(line_txt, "FINAL")) return -1;
    if (!value) return -1;
    else strcpy(req_value->res_identity, value);
    //
    SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    while (line_txt) {
        SPLIT_LINE(value, line_txt)
        if (!value) continue;
        else if (!strcmp(line_txt, "Params"))
            strcpy(req_value->params, value);
        else if (!strcmp(line_txt, "Session"))
            strcpy(req_value->seion_id, value);
        SPLIT_TOKEN(line_txt, LINE_TOKEN, splittok)
    }
    return 0;
}
