
#include"ope_list.h"

inline void delete_ope_list(struct ope_list *olist) {
    if (!olist) return;
    free(olist->opesdo.file_name);
    free(olist);
}

#define OLIST_APPEND(head, add) { \
  LDECLTYPE(head) _tmp; \
  (add)->next=NULL; \
  if (head) { \
    _tmp = head; \
    while (_tmp->next) { _tmp = _tmp->next; } \
    _tmp->next=(add); \
  } else { (head)=(add); } \
}

inline void olist_delete(ope_listh head, struct ope_list *del) {
    struct ope_list *olistmp;

    if (*head == del) *head = (*head)->next;
    else {
        olistmp = *head;
        while (olistmp->next && olistmp->next != del)
            olistmp = olistmp->next;
        if (olistmp->next) olistmp->next = del->next;
    }
}

#define OLIST_INSERT(head, ins, toke) { \
    LDECLTYPE(head) _tmp; \
    if (head == toke) { \
        ins->next = head; \
        head = ins; \
    } else { \
        _tmp = head; \
        while (_tmp->next && _tmp->next != toke) \
            _tmp = _tmp->next; \
        if (_tmp->next) { \
            ins->next = _tmp->next; \
            _tmp->next = ins; \
        } \
    } \
}

struct ope_list *insert_new_olist(ope_listh olisth, struct ope_list *toke_olist) {
    struct ope_list *olist = (struct ope_list *) malloc(sizeof (struct ope_list));
    if (!olist) return NULL;
    memset(olist, '\0', sizeof (struct ope_list));
    OLIST_INSERT(*olisth, olist, toke_olist);
//
    return olist;
}

struct ope_list *append_new_olist(ope_listh olisth) {
    struct ope_list *new_olist = (struct ope_list *) malloc(sizeof (struct ope_list));
    if (!new_olist) return NULL;
    memset(new_olist, '\0', sizeof (struct ope_list));
    OLIST_APPEND(*olisth, new_olist);
//
    return new_olist;
}

struct ope_list *find_olist_prev(struct ope_list *olist, struct ope_list *toke_olist) {
    struct ope_list *prev_olist = NULL, *olistmp;
//
    OLIST_FOREACH(olist, olistmp) {
        if (toke_olist == olistmp) break;
        prev_olist = olistmp;
    }
//
    return prev_olist;
}
