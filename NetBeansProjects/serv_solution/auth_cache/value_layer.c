#include "value_layer.h"

//

void split_ipaddr(cache_address *caddr, char *addr_txt) {
    char *toke = NULL;
    memset(caddr, '\0', sizeof (cache_address));
    toke = strchr(addr_txt, ':');
    if (toke) {
        strncpy(caddr->sin_addr, addr_txt, toke - addr_txt);
        caddr->sin_port = atoi(++toke);
    } else strcpy(caddr->sin_addr, addr_txt);
}

//
const char *content_table[] = {
    "invalid",
    "text/xml",
    "application/octet-stream",
    "text/plain"
};

//
const char *execute_text_table[] = {
    "exception",
    "successful"
};