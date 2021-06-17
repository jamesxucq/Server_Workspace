#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auth_cache.h"

//
hashmap *_ldata_hm_;

//
struct list_data* new_ldata() {
    struct list_data *ldata = (struct list_data *) malloc(sizeof (struct list_data));
    if (!ldata) return NULL;
    memset(ldata, '\0', sizeof (struct list_data));
//
    return ldata;
}

void free_ldata(void* ldata) {
    if (ldata) free(ldata);
}