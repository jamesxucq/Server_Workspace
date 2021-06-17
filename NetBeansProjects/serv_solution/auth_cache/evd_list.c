#include "evd_list.h"

int init_evd_list(struct evd_list *elist, struct event_data *evds, int events_tatol) {
    int inde;
    struct levd_list *elistmp;
//
_LOG_DEBUG("init evd list");
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
        LL_APPEND(elist->sleep, elistmp);
    }
//
    return ERR_SUCCESS;
}

inline struct levd_list *get_levd_list(struct evd_list *elist) {
    struct levd_list *elistmp = elist->sleep;
//
_LOG_DEBUG("get levd list");
    if (!elist->sleep) return NULL;
    LL_DELETE(elist->sleep, elistmp);
    LL_APPEND(elist->active, elistmp);
//
    return elistmp;
}

inline struct levd_list *get_listen_elist(struct evd_list *elist) {
    struct levd_list *elistmp = elist->sleep;
//
_LOG_DEBUG("get listen elist");
    if (!elist->sleep) return NULL;
    LL_DELETE(elist->sleep, elistmp);
    LL_APPEND(elist->listen, elistmp);
//
    return elistmp;
}

inline struct levd_list *set_active_elist(struct evd_list *elist, struct event_data *evd) {
    struct levd_list *elistmp;
//
    if (!elist->active) return NULL;
    // for(el=head;el;el=el->next)
    LL_FOREACH(elist->active, elistmp)
        if (elistmp->evd == evd) break;
    if (!elistmp) return NULL;
//
_LOG_DEBUG("set active elist");
    LL_DELETE(elist->active, elistmp);
    LL_APPEND(elist->sleep, elistmp);
//
    return elistmp;
}

inline struct levd_list *set_listen_elist(struct evd_list *elist, struct event_data *evd) {
    struct levd_list *elistmp;
//
    if (!elist->listen) return NULL;
    //for(el=head;el;el= (el->next==head ? 0L : el->next))
    LL_FOREACH(elist->listen, elistmp)
        if (elistmp->evd == evd) break;
    if (!elistmp) return NULL;
//
_LOG_DEBUG("set listen elist");
    LL_DELETE(elist->listen, elistmp);
    LL_APPEND(elist->sleep, elistmp);
//
    return elistmp;
}

inline struct levd_list *set_levd_list(struct evd_list *elist, struct event_data *evd) {
    struct levd_list *elistmp;
//
_LOG_DEBUG("set levd elist");
    if (!elist || !evd) return NULL;
    if (!(elistmp = set_active_elist(elist, evd)))
        elistmp = set_listen_elist(elist, evd);
//
    return elistmp;
}

void dele_evd_list(struct evd_list *elist) {
//
_LOG_DEBUG("dele evd list");
}
