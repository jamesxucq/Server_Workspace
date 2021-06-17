#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server_list.h"

#define SLIST_APPEND(head, add) { \
  (add)->next = head; \
  head = add; \
}

serv_server *add_new_slist(serv_listh slisth) {
    struct server_list *slistmp;
//
    if (!slisth) return NULL;
    slistmp = (struct server_list *) malloc(sizeof (struct server_list));
    if (!slistmp) return NULL;
    memset(slistmp, '\0', sizeof (struct server_list));
//
    SLIST_APPEND(*slisth, slistmp);
    return &slistmp->saddr;
}

//

