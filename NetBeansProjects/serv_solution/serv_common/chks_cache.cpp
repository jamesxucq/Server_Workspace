#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rivfs/rivfs.h"
#include "chks_cache.h"

hashmap *_rivlive_hm_;

struct riv_live* new_riv_live(char *location) {
    struct riv_live *plive = (struct riv_live *) malloc(sizeof (struct riv_live));
    if (!plive) return NULL;
    memset(plive, '\0', sizeof (struct riv_live));
    // _LOG_DEBUG("malloc new cope"); // delete by james 20130409
    riv_initial(plive, location);
    //
    return plive;
}

void del_riv_live(void *plive) {
    if (plive) free(plive);
}

void destory_riv_live(struct riv_live *plive) {
    if (plive) riv_final(plive);
    // _LOG_DEBUG("destory actno aope"); // delete by james 20130409
}