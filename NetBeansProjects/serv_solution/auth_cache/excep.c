#include "aserv_bzl.h"
#include "auth_bzl.h"

#include "excep.h"

//

void unused_excep(struct session *seion) {
}

void query_excep(struct session *seion) {
    struct seion_ctrl *sctrl = &seion->sctrl;
    CLOSE_QUERY_SE(sctrl);
}

void add_excep(struct session *seion) {
    CLOSE_ADDI_SE(&seion->sctrl);
}

void clear_excep(struct session *seion) {
    CLOSE_CLEAR_SE(&seion->sctrl);
}

void list_excep(struct session *seion) {
    struct seion_ctrl *sctrl = &seion->sctrl;
    CLOSE_LIST_SE(sctrl);
}

void status_excep(struct session *seion) {
    struct seion_ctrl *sctrl = &seion->sctrl;
    struct auth_data *adata = get_adata_bzl(seion->ival.uid);
    SERVE_REFEREN_DECREA(adata->saddr.sin_addr, adata->saddr.sin_port);
    CLOSE_STATUS_SE(sctrl);
}

void sanchor_excep(struct session *seion) {
    struct auth_data *adata = get_adata_bzl(seion->ival.uid);
    SERVE_REFEREN_DECREA(adata->saddr.sin_addr, adata->saddr.sin_port);
    CLOSE_SET_ANCHOR_SE(&seion->sctrl);
}

void slocked_excep(struct session *seion) {
    struct auth_data *adata = get_adata_bzl(seion->ival.uid);
    SERVE_REFEREN_DECREA(adata->saddr.sin_addr, adata->saddr.sin_port);
    CLOSE_SET_LOCKED_SE(&seion->sctrl);
}

void kalive_excep(struct session *seion) {
    CLOSE_KEEP_ALIVE_SE(&seion->sctrl);
}

//

void(*excep_null_code[])(struct session*) = {
    unused_excep
};

void(*excep_options_code[])(struct session*) = {
    unused_excep,
    query_excep,
    status_excep
};

void(*excep_control_code[])(struct session*) = {
    unused_excep,
    sanchor_excep,
    slocked_excep,
    add_excep,
    clear_excep,
    list_excep
};

void(*excep_kalive_code[])(struct session*) = {
    kalive_excep
};

void(**exception_tab[])(struct session*) = {
    excep_null_code,
    excep_options_code,
    excep_control_code,
    excep_kalive_code
};


