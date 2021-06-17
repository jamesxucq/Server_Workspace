#include "excep.h"

//

void unused_excep(struct session *seion) {
}

void regis_excep(struct session *seion) {
    struct seion_ctrl *sctrl = &seion->sctrl;
    CLOSE_REGIS_SE(sctrl);
}

void settings_excep(struct session *seion) {
    struct seion_ctrl *sctrl = &seion->sctrl;
    CLOSE_SETTINGS_SE(sctrl);
}

void link_excep(struct session *seion) {
    CLOSE_LINK_SE(&seion->sctrl);
}

void unlink_excep(struct session *seion) {
    CLOSE_UNLINK_SE(&seion->sctrl);
}

//
void(*excep_null_code[])(struct session*) = {
    unused_excep
};

void(*excep_options_code[])(struct session*) = {
    unused_excep,
    regis_excep,
    settings_excep,
    link_excep,
    unlink_excep
};

void(**exception_tab[])(struct session*) = {
    excep_null_code,
    excep_options_code
};