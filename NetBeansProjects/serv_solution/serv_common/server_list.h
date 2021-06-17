
/* 
 * File:   server_list.h
 * Author: David
 * Created on 2011                                    
 */

#ifndef SERVERLIST_H
#define	SERVERLIST_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "common_macro.h"

    typedef struct {
        char sin_addr[SERVNAME_LENGTH];
        int sin_port;
        int refer_inde; // reference 
    } serv_server;

    struct server_list {
        struct server_list *next;
        serv_server saddr;
    };
    typedef struct server_list** serv_listh;

    //

#define DELETE_SLIST(slist) { \
    LDECLTYPE(slist) slistmp, _tmp; \
    slistmp = slist; \
    while (slistmp) { \
        _tmp = slistmp; \
        slistmp = slistmp->next; \
        free(_tmp); \
    } \
}
    
    serv_server *add_new_slist(serv_listh slisth);

    //
#define FIND_SERV_LIST(found, alist, addr) { \
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


#ifdef	__cplusplus
}
#endif

#endif	/* SERVERLIST_H */

