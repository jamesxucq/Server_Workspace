#ifndef FASD_LIST_H_
#define FASD_LIST_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
//#include "utility/utlist.h"
#include "common_macro.h"

//

struct fatent {
    unsigned int action_type; /* add mod del list recu */
    char file_name[MAX_PATH * 3];
    unsigned int reserved; /* reserved */
};

struct fatsdo {
    unsigned int action_type; /* add mod del list recu */
    char *file_name;
    uint64 file_size;
    time_t last_write; /* When the item was last modified */
    unsigned int reserved; /* reserved */
};

struct fsdo_list {
    struct fsdo_list *ison_next;
    struct fsdo_list *list_next;
    struct fatsdo fsdo;
};
typedef struct fsdo_list** fsdo_lsth;
#define stredup(str1, str2) free(str1); str1 = strdup(str2);

//
inline void delete_fasd_list(struct fsdo_list *alist);
#define DELETE_FALIST(alist) { \
    if (alist) { \
        free(alist->fsdo.file_name); \
        free(alist); \
    } \
}
void delete_alist(struct fsdo_list *alist);
#define DELETE_ALIST(alist) { \
    LDECLTYPE(alist) alistmp, _tmp; \
    alistmp = alist; \
    while (alistmp) { \
        _tmp = alistmp; \
        alistmp = alistmp->list_next; \
        DELETE_FALIST(_tmp); \
    } \
}

#define DELETE_FSDO_LIST(FSDO_LIST) { \
    DELETE_ALIST((struct fsdo_list *)(FSDO_LIST)); \
    (FSDO_LIST) = NULL; \
}

inline void alist_apend(fsdo_lsth alsth, struct fsdo_list *alist);
inline void alist_delete(fsdo_lsth alsth, struct fsdo_list *alist);
#define ALIST_FOREACH(head, el) \
    for(el=head;el;el=el->list_next)

//
struct fsdo_list *insert_alist_newsdo(fsdo_lsth alsth, struct fatsdo *fsdo, struct fsdo_list *toke_alist);
struct fsdo_list *append_alist_newsdo(fsdo_lsth alsth, struct fatsdo *fsdo);
struct fsdo_list *append_alist_entry(fsdo_lsth alsth, struct fatent *faen);
struct fsdo_list *move_alist_last(fsdo_lsth alsth, struct fsdo_list *alist);
void move_alist_list(fsdo_lsth alsth, struct fsdo_list *alist);
int comp_fsdo(struct fsdo_list *alist, struct fatsdo *fsdo);
int comp_fsdo_ext(struct fsdo_list *alist, struct fatsdo *exis, struct fatsdo *neis);
//
int get_fattrib_tatol(struct fsdo_list *alist);
inline struct fatsdo *get_attsdo_by_num(struct fsdo_list *alist, int section, int inde);
inline struct fatsdo *read_attsdo_next(struct fsdo_list *alist, int section);

#ifdef	__cplusplus
}
#endif

#endif /* FASD_LIST_H_ */

