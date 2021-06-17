#ifndef DIRLIST_H_
#define DIRLIST_H_

#ifdef	__cplusplus
extern "C" {
#endif
//
////#include "utility/utlist.h"
//#include "common_macro.h"
//
//struct dires_list {
//    struct dires_list *next;
//    char dir_name[MAX_PATH * 3];
//};
//typedef struct dires_list** dires_listh;
//
//#define DLIST_FOREACH(head,el) \
//    for(el=head;el;el=el->next)
//
////
//#define DELETE_DLIST(dlist) { \
//    LDECLTYPE(dlist) dlisttmp, _tmp; \
//    dlisttmp = dlist; \
//    while (dlisttmp) { \
//        _tmp = dlisttmp; \
//        dlisttmp = dlisttmp->next; \
//        free(_tmp); \
//    } \
//}
//
//#define DELETE_DIRES_LIST(DLIST) \
//    DELETE_DLIST((struct dires_list *)(DLIST)); \
//    (DLIST) = NULL;
//struct dires_list *add_new_dlist(dires_listh dlisth, char *dirname);

#ifdef	__cplusplus
}
#endif

#endif /* DIRLIST_H_ */

