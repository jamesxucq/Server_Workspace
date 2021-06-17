/* 
 * File:   addr_list.h
 * Author: David
 *
 * Created on 2012
 */

#ifndef ADDR_LIST_H
#define	ADDR_LIST_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "common_macro.h"
#include "string_utility.h"

    struct addr_list {
        struct addr_list *next;
        serv_addr saddr;
    };
    typedef struct addr_list** addr_listh;

    //
    serv_addr *add_new_alist(addr_listh alsth);

    //
#define FIND_ADDR_LIST(found, alist, addr) { \
    struct addr_list *_tmp = (alist); \
    found = NULL; \
    while (_tmp) { \
        if (!strcmp(addr, _tmp->saddr.sin_addr)) { \
            found = &_tmp->saddr; \
            break; \
        } \
        _tmp = _tmp->next; \
    } \
}    

    inline int erase_addr_list(addr_listh alsth, char *saddr);
    void dele_addr_list(struct addr_list *alist);

#ifdef	__cplusplus
}
#endif

#endif	/* ADDR_LIST_H */

