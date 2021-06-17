
#include "text_value.h"
#include "sdtproto.h"

#define HEAD_LINE_LEN                   256

//
static const struct value_node regis_method_table[] = {
    {"OPTIONS", RGTP_OPTIONS},
    {NULL, 0}
};

static const struct value_node regis_null_code[] = {
    {NULL, 0}
};
static const struct value_node regis_options_code[] = {
    {"register", ROP_REGIS},
    {"settings", ROP_SETTINGS},
    {"link", ROP_LINK},
    {"unlink", ROP_UNLINK},
    {NULL, 0}
};
static const struct value_node *regis_command_table[] = {
    regis_null_code,
    regis_options_code
};

enum REGIS_METHOD regis_command(enum REGIS_OPTIONS *command, char *header, int len) {
    char line_txt[HEAD_LINE_LEN];
    enum REGIS_METHOD method;
    char *toksp = NULL, *tokep = NULL;
    //
    if (header[0] == '\0') return RGTP_INVA;
    tokep = strchr(header, ' ');
    MKZERO(line_txt);
    strncpy(line_txt, header, tokep - header);
    TEXT_VALUE(method, regis_method_table, line_txt)
            //
    if (RGTP_OPTIONS == method) {
        toksp = strstr(tokep, ": ");
        tokep = strstr(toksp, "\r\n");
        MKZERO(line_txt);
        strncpy(line_txt, toksp + 2, tokep - toksp - 2);
        TEXT_VALUE(*command, regis_command_table[method], line_txt)
    }
    //
    return method;
}

//
static const struct value_node cache_method_table[] = {
    {"OPTIONS", CATP_OPTIONS},
    {"CONTROL", INTE_CONTROL},
    {"KALIVE", CATP_KALIVE},
    {NULL, 0}
};

//
static const struct value_node cache_null_code[] = {
    {NULL, 0}
};

static const struct value_node cache_options_code[] = {
    {"query", COP_QUERY},
    {"status", COP_STATUS},
    {NULL, 0}
};

static const struct value_node cache_control_code[] = {
    {"set anchor", CTRL_SANCHOR},
    {"set locked", CTRL_SLOCKED},
    {"add", CTRL_ADD},
    {"clear", CTRL_CLEAR},
    {"list", CTRL_LIST},
    {NULL, 0}
};

static const struct value_node *cache_command_table[] = {
    cache_null_code,
    cache_options_code,
    cache_control_code,
    cache_null_code
};

enum CACHE_METHOD cache_command(int *command, char *header, int len) {
    char line_txt[HEAD_LINE_LEN];
    enum CACHE_METHOD method;
    char *toksp = NULL, *tokep = NULL;
    //
    if (header[0] == '\0') return CATP_INVA;
    tokep = strchr(header, ' ');
    MKZERO(line_txt);
    strncpy(line_txt, header, tokep - header);
    TEXT_VALUE(method, cache_method_table, line_txt)
            //
    if (CATP_OPTIONS == method || INTE_CONTROL == method) {
        toksp = strstr(tokep, ": ");
        tokep = strstr(toksp, "\r\n");
        MKZERO(line_txt);
        strncpy(line_txt, toksp + 2, tokep - toksp - 2);
        TEXT_VALUE(*command, cache_command_table[method], line_txt)
    }
    //
    return method;
}

//
// {"ATTRIB", SDTP_ATTRIB},
static const struct value_node serv_method_table[] = {
    {"INITIAL", SVTP_INITIAL},
    {"KALIVE", SVTP_KALIVE},
    {"HEAD", SVTP_HEAD},
    {"GET", SVTP_GET},
    {"POST", SVTP_POST},
    {"PUT", SVTP_PUT},
    {"COPY", SVTP_COPY},
    {"MOVE", SVTP_MOVE},
    {"DELETE", SVTP_DELETE},
    {"FINAL", SVTP_FINAL},
    {NULL, 0}
};

enum SERVER_METHOD proto_method(char *header, int len) {
    char line_txt[HEAD_LINE_LEN];
    enum SERVER_METHOD method;
    char *tokep = NULL;
    //
    if (header[0] == '\0') return SVTP_INVA;
    tokep = strchr(header, ' ');
    MKZERO(line_txt);
    strncpy(line_txt, header, tokep - header);
    TEXT_VALUE(method, serv_method_table, line_txt)
            //
    return method;
}

//

int get_header_value(char **head_buffer, char *receive_txt) {
    char *header_end = strstr(receive_txt, "\r\n\r\n");
    if (!header_end) return -1;
    header_end += 2;
    *header_end++ = '\0';
    *header_end++ = '\0';
    //
    *head_buffer = receive_txt;
    return header_end - receive_txt;
}

int get_body_value(char **body_buffer, char *receive_txt, int len) {
    char *body_start = receive_txt + strlen(receive_txt) + 2;
    *body_buffer = body_start;
    //
    return len - (body_start - receive_txt);
}

