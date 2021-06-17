#include "evd_list.h"

int init_evd_list(struct evd_list *elist, struct event_data *evds, int events_tatol) {
    int inde;
    struct levd_list *elistmp;

    if (!elist || !evds) return ERR_FAULT;
    memset(elist, '\0', sizeof (struct evd_list));

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

    return ERR_SUCCESS;
}

inline struct levd_list *get_levd_list(struct evd_list *elist) {
    struct levd_list *elistmp = elist->sleep;

    if (!elist->sleep) return NULL;
    LL_DELETE(elist->sleep, elistmp);
    LL_APPEND(elist->active, elistmp);

    return elistmp;
}

inline struct levd_list *set_levd_list(struct evd_list *elist, struct event_data *evd) {
    struct levd_list *elistmp;

    if (!elist->active) return NULL;
    // for(el=head;el;el=el->next)
    LL_FOREACH(elist->active, elistmp)
    if (elistmp->evd == evd) break;
    if (!elistmp) return NULL;

    LL_DELETE(elist->active, elistmp);
    LL_APPEND(elist->sleep, elistmp);

    return elistmp;
}

void dele_evd_list(struct evd_list *elist) {

}
