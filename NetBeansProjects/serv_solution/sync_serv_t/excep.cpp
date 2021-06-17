#include "excep.h"
#include "chks_bzl.h"
#include "opera_bzl.h"
#include "auth_bzl.h"
#include "list_cache.h"
#include "auth_cache.h"

#define OPERA_AUTH_EXCEP(SEION) \
    struct list_data *ldata = SEION->ldata; \
    struct actno_ope *aope = SEION->aope; \
    /* // flush cached anchor if the last exception */ \
    if (0x0001 == valid_opera_referen(ldata->uid)) { \
        clear_list_cache(ldata->list_cache, ldata->location); \
        flush_anchor_bzl(ldata->location, aope); \
    } \
    erase_chks_cache_bzl(ldata->uid); \
    opera_epoll_exception(ldata->uid); \
    auth_epoll_exception(ldata->uid, ldata->last_anchor, ldata->req_valid, ldata->anchor_cached);
//

void unused_excep(struct session *seion) {
}

void initial_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
    CLOSE_INITIAL_SE(&seion->sctrl);
}

void kalive_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
    CLOSE_KALIVE_SE(&seion->sctrl);
}

void head_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
    close_head_se[seion->ival.sub_entry](&seion->sctrl);
}

void get_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
            //
            struct inte_value *ival = &seion->ival;
    struct seion_ctrl *sctrl = &seion->sctrl;
    close_get_se[ival->sub_entry][ival->svalue](sctrl);
}

void post_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
            struct seion_ctrl *sctrl = &seion->sctrl;
    CLOSE_POST_SE(sctrl);
}

void put_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
            //
            struct inte_value *ival = &seion->ival;
    struct seion_ctrl *sctrl = &seion->sctrl;
    switch (ival->sub_entry) {
        case ENTRY_CHKS:
            CLOSE_PUT_CHKS_SE(sctrl)
            break;
        case ENTRY_FILE:
            CLOSE_PUT_FILE_SE(aope, sctrl, ldata, ival)
            break;
    }
}

void copy_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
    CLOSE_COPY_SE(&seion->sctrl);
}

void move_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
    CLOSE_MOVE_SE(&seion->sctrl);
}

void delete_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
    CLOSE_DELETE_SE(&seion->sctrl);
}

void final_excep(struct session *seion) {
    OPERA_AUTH_EXCEP(seion)
    CLOSE_FINAL_SE(&seion->sctrl);
}

//
void (*exception_tab[])(struct session*) = {
    unused_excep,
    initial_excep,
    kalive_excep,
    head_excep,
    get_excep,
    post_excep,
    put_excep,
    copy_excep,
    move_excep,
    delete_excep,
    final_excep
};
