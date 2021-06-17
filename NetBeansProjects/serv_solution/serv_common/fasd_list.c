#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "logger.h"
#include "file_utility.h"
#include "string_utility.h"
#include "iattb_type.h"

#include "fasd_list.h"

inline void delete_fasd_list(struct fsdo_list *alist) {
    if (alist) {
        free(alist->fsdo.file_name);
        free(alist);
    }
}

void delete_alist(struct fsdo_list *alist) {
    struct fsdo_list *alistmp = NULL, *del_alist = NULL;
    //
    alistmp = alist;
    while (alistmp) {
        del_alist = alistmp;
        alistmp = alistmp->list_next;
        DELETE_FALIST(del_alist);
    }
}

#define ALIST_PREPEND(head, add) { \
    add->next = head; \
    head = add; \
}

inline void alist_apend(fsdo_lsth alsth, struct fsdo_list *alist) {
    struct fsdo_list *alistmp;
    //
    alist->list_next = NULL;
    if (*alsth) {
        alistmp = *alsth;
        while (alistmp->list_next)
            alistmp = alistmp->list_next;
        alistmp->list_next = alist;
    } else *alsth = alist;
}

#define ALIST_APPEND(head, add) { \
    LDECLTYPE(head) _tmp; \
    add->list_next = NULL; \
    if (head) { \
        _tmp = head; \
        while (_tmp->list_next) _tmp = _tmp->list_next; \
        _tmp->list_next = add; \
    } else head = add; \
}

inline void alist_delete(fsdo_lsth alsth, struct fsdo_list *alist) {
    struct fsdo_list *alistmp;
    //
    if (*alsth == alist) *alsth = (*alsth)->list_next;
    else {
        alistmp = *alsth;
        while (alistmp->list_next && alist != alistmp->list_next)
            alistmp = alistmp->list_next;
        if (alistmp->list_next) alistmp->list_next = alist->list_next;
    }
}

#define ALIST_INSERT(head, ins, toke) { \
    LDECLTYPE(head) _tmp; \
    if (head == toke) { \
        ins->list_next = head; \
        head = ins; \
    } else { \
        _tmp = head; \
        while (_tmp->list_next && _tmp->list_next != toke) \
            _tmp = _tmp->list_next; \
        if (_tmp->list_next) { \
            ins->list_next = _tmp->list_next; \
            _tmp->list_next = ins; \
        } \
    } \
}

struct fsdo_list *insert_alist_newsdo(fsdo_lsth alsth, struct fatsdo *fsdo, struct fsdo_list *toke_alist) {
    struct fsdo_list *alistmp = (struct fsdo_list *) malloc(sizeof (struct fsdo_list));
    if (!alistmp) return NULL;
    memset(alistmp, '\0', sizeof (struct fsdo_list));
    memcpy(&alistmp->fsdo, fsdo, sizeof (struct fatsdo));
    alistmp->fsdo.file_name = strdup(fsdo->file_name);
    //
    ALIST_INSERT(*alsth, alistmp, toke_alist);
    return alistmp;
}

struct fsdo_list *append_alist_newsdo(fsdo_lsth alsth, struct fatsdo *fsdo) {
    struct fsdo_list *alistmp = (struct fsdo_list *) malloc(sizeof (struct fsdo_list));
    if (!alistmp) return NULL;
    memset(alistmp, '\0', sizeof (struct fsdo_list));
    memcpy(&alistmp->fsdo, fsdo, sizeof (struct fatsdo));
    alistmp->fsdo.file_name = strdup(fsdo->file_name);
    //
    ALIST_APPEND(*alsth, alistmp);
    return alistmp;
}

#define FSDO_ENT_SDO(SDO, ENT) { \
    SDO->action_type = ENT->action_type; \
    SDO->file_name = strdup(ENT->file_name); \
    SDO->reserved = ENT->reserved; \
}

struct fsdo_list *append_alist_entry(fsdo_lsth alsth, struct fatent *faen) {
    struct fsdo_list *alistmp = (struct fsdo_list *) malloc(sizeof (struct fsdo_list));
    if (!alistmp) return NULL;
    memset(alistmp, '\0', sizeof (struct fsdo_list));
    struct fatsdo *new_fsdo = &alistmp->fsdo;
    FSDO_ENT_SDO(new_fsdo, faen)
    // _LOG_DEBUG("sdo action_type:%x file_name:%s file_size:%llu", new_iden->action_type, new_iden->file_name, new_iden->file_size);
    //
    ALIST_APPEND(*alsth, alistmp);
    return alistmp;
}

struct fsdo_list *move_alist_last(fsdo_lsth alsth, struct fsdo_list *alist) {
    struct fsdo_list *alistmp;
    //
    if (*alsth == alist) *alsth = (*alsth)->list_next;
    else {
        alistmp = *alsth;
        while (alistmp->list_next && alist != alistmp->list_next)
            alistmp = alistmp->list_next;
        if (alistmp->list_next) alistmp->list_next = alist->list_next;
    }
    //
    ALIST_APPEND(*alsth, alist);
    return alist;
}

void move_alist_list(fsdo_lsth alsth, struct fsdo_list *alist) {
    struct fsdo_list *alistmp;
    if (*alsth) {
        alistmp = *alsth;
        while (alistmp->list_next) alistmp = alistmp->list_next;
        alistmp->list_next = alist;
    } else *alsth = alist;
}

int comp_fsdo(struct fsdo_list *alist, struct fatsdo *fsdo) {
    struct fsdo_list *alistmp;
    struct fatsdo *fsdo_nod;
    int comp_value = -1;
    //
    for(alistmp = alist; alistmp; alistmp = alistmp->list_next) {
        fsdo_nod = &alistmp->fsdo;
        if((fsdo->action_type==fsdo_nod->action_type) && !strcmp(fsdo->file_name, fsdo_nod->file_name)) {
            comp_value = 0x00;
            break;
        }
    }
    return comp_value;
}

int comp_fsdo_ext(struct fsdo_list *alist, struct fatsdo *exis, struct fatsdo *neis) {
    struct fsdo_list *alistmp;
    struct fatsdo *exis_nod, *neis_nod;
    int comp_value = -1;
//
    for(alistmp = alist; alistmp; alistmp = alistmp->list_next) {
        exis_nod = &alistmp->fsdo;
        if(EXIST & exis_nod->action_type) {
            alistmp = alistmp->list_next;
            neis_nod = &alistmp->fsdo;
            if((exis->action_type==exis_nod->action_type) && (neis->action_type==neis_nod->action_type)
                && !strcmp(exis->file_name, exis_nod->file_name) &&  !strcmp(neis->file_name, neis_nod->file_name)) {
                comp_value = 0x00;
                break;
            }
        }
    }
    return comp_value;
}

//
int get_fattrib_tatol(struct fsdo_list *alist) {
    struct fsdo_list *alistmp;
    int fsdo_tatol = 0;
    //
    if (!alist) return 0;
    //#define ALIST_FOREACH(alsth,el) for(el=alsth;el;el=el->next)
    ALIST_FOREACH(alist, alistmp)
    fsdo_tatol++;
    //
    return fsdo_tatol;
}

inline struct fatsdo *get_attsdo_by_num(struct fsdo_list *alist, int section, int inde) {
    struct fsdo_list *alistmp;
    int idx = section + inde;
    int alist_tatol = -1;
    //
    if (!alist) return 0;
    //#define ALIST_FOREACH(alsth,el) for(el=alsth;el;el=el->next)
    //

    ALIST_FOREACH(alist, alistmp) {
        alist_tatol++;
        if (idx == alist_tatol)
            break;
    }
    if (!alistmp) return NULL;
    return &alistmp->fsdo;
}

inline struct fatsdo *read_attsdo_next(struct fsdo_list *alist, int section) {
    struct fsdo_list *alistmp;
    static struct fsdo_list *alist_toke;
    //
    if (alist) {
        int alist_tatol = -1;
        //

        ALIST_FOREACH(alist, alistmp) {
            alist_tatol++;
            if (section == alist_tatol) break;
        }
        alist_toke = alistmp;
    } else alist_toke = alist_toke->list_next;
    if (!alist_toke) return NULL;
    //
    return &alist_toke->fsdo;
}

