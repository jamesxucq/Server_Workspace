/* 
 * File:   action_cache.h
 * Author: Administrator
 *
 * Created on 2012
 */

#ifndef ACTION_CACHE_H
#define	ACTION_CACHE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"

    struct openod {
        unsigned int action_type; /* add mod del list recu */
        char *file_name;
        uint64 file_size;
        time_t last_write; /* When the item was last modified */
        uint64 reserved; /* filehigh */
        time_t act_time;
    };

    struct ope_list {
        struct ope_list *next;
        struct openod opesdo;
    };
    typedef struct ope_list** ope_listh;

    //
    inline void delete_ope_list(struct ope_list *olist);
#define DELETE_OPLIST(head) { \
    if (head) { \
        free(head->opesdo.file_name); \
        free(head); \
    } \
}   
#define DELETE_OLIST(olist) { \
    LDECLTYPE(olist) olistmp, _tmp; \
    olistmp = olist; \
    while (olistmp) { \
        _tmp = olistmp; \
        olistmp = olistmp->next; \
        DELETE_OPLIST(_tmp); \
    } \
}
#define DELETE_OPE_LIST(OLIST) { \
    DELETE_OLIST((struct ope_list *)(OLIST)) \
    (OLIST) = NULL; \
}

    inline void olist_delete(ope_listh head, struct ope_list *del);
#define OLIST_FOREACH(head, el) \
    for(el=head;el;el=el->next)

    //
    struct ope_list *insert_new_olist(ope_listh olisth, struct ope_list *toke_olist);
    struct ope_list *append_new_olist(ope_listh olisth);

    //
    struct ope_list *find_olist_prev(struct ope_list *olist, struct ope_list *toke_olist);

#ifdef	__cplusplus
}
#endif

#endif	/* ACTION_CACHE_H */

