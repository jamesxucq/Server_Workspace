#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "addr_list.h"

#define ADDR_APPEND(head, add) { \
    (add)->next = head; \
    head = add; \
}

serv_addr *add_new_alist(addr_listh alsth) {
    struct addr_list *alistmp;
//
    if (!alsth) return NULL;
    alistmp = (struct addr_list *) malloc(sizeof (struct addr_list));
    if (!alistmp) return NULL;
    memset(alistmp, '\0', sizeof (struct addr_list));
//
    ADDR_APPEND(*alsth, alistmp);
    return &alistmp->saddr;
}

#define DELETE_ALIST(alist) { \
    LDECLTYPE(alist) alistmp, _tmp; \
    alistmp = alist; \
    while (alistmp) { \
        _tmp = alistmp; \
        alistmp = alistmp->next; \
        free(_tmp); \
    } \
}
//

inline int erase_addr_list(addr_listh alsth, char *saddr) { // -1:fail 0:ok 1:exception
    struct addr_list *find_alist = NULL, *prev_alist = NULL;
    //
    if (!alsth || !saddr) return -1;
    find_alist = *alsth;
    int erase_value = 0x01;
    while (find_alist) {
        if (!strcmp(saddr, find_alist->saddr.sin_addr)) {
            {
                if (*alsth == find_alist) *alsth = find_alist->next;
                else prev_alist->next = find_alist->next;
                free(find_alist);
                erase_value = 0;
            }
            break;
        }
        prev_alist = find_alist;
        find_alist = find_alist->next;
    }
    //
    return erase_value;
}

void dele_addr_list(struct addr_list *alist) {
    struct addr_list *alistmp, *dele_alist = NULL; 
    //
    alistmp = alist;
    while (alistmp) {
        dele_alist = alistmp;
        alistmp = alistmp->next;
        free(dele_alist);
    }
}

