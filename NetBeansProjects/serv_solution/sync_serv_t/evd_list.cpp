#include "evd_list.h"

int init_evd_list(struct evd_list *elist, struct event_data *evds, int events_tatol) {
    int inde;
    struct levd_list *elistmp;
//
    if (!elist || !evds) return ERR_FAULT;
    memset(elist, '\0', sizeof (struct evd_list));
//
    for (inde = 0; inde < events_tatol; inde++) {
        elistmp = (struct levd_list *) malloc(sizeof (struct levd_list));
        if (!elistmp) {
            _LOG_WARN("new elist item failed");
            return ERR_FAULT;
        }
        memset(elistmp, '\0', sizeof (struct levd_list));
        elistmp->evd = &evds[inde];
        CDL_APPEND(elist->sleep, elistmp);
    }
//
    return ERR_SUCCESS;
}

struct levd_list *get_levd_list(struct evd_list *elist) {
    struct levd_list *elistmp = elist->sleep;
//
    if (!elist->sleep) return NULL;
    CDL_DELETE(elist->sleep, elistmp);
    CDL_APPEND(elist->active, elistmp);
    if (!elist->chkpos)
        elist->chkpos = elist->active;
//
    return elistmp;
}

struct levd_list *set_levd_list(struct evd_list *elist, struct event_data *evd) {
    struct levd_list *elistmp;
//
    if (!elist->active) return NULL;
    // for(el=head; el; el=(el->next==head?0L:el->next)) 
    CDL_FOREACH(elist->active, elistmp)
    if (elistmp->evd == evd) break;
    if (!elistmp) return NULL;
//
    if (elistmp == elist->chkpos) {
        if (elistmp->next == elistmp) elist->chkpos = NULL;
        else elist->chkpos = elistmp->next;
    }
    CDL_DELETE(elist->active, elistmp);
    CDL_APPEND(elist->sleep, elistmp);
//
    return elistmp;
}

void dele_evd_list(struct evd_list *elist) {
//
}
