#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "speed_utility.h"
#include "fasd_list.h"

//
void spdmap_insert(speedmap *smap, struct fsdo_list *alist) {
     struct fsdo_list *alistmp;
    //
    alistmp = cache_value(alist->fsdo.file_name, smap);
    if(alistmp){
        while(alistmp->ison_next) alistmp = alistmp->ison_next;
        alistmp->ison_next = alist;
    } else cache_insert(alist->fsdo.file_name, alist, smap);
}

void spdmap_remove(speedmap *smap, struct fsdo_list *alist) {
     struct fsdo_list *alistmp;
    //
    alistmp = cache_value(alist->fsdo.file_name, smap);
    if(alistmp) {
        if(alist == alistmp) {
            cache_remove(alist->fsdo.file_name, smap);
        } else {
            while(alistmp->ison_next && (alist!=alistmp->ison_next)) 
                alistmp = alistmp->ison_next;
            if(alistmp->ison_next) alistmp->ison_next = alist->ison_next;
        }
    }
}
//

struct fsdo_list *find_next_value(struct find_conte *ficon, struct fsdo_list *alist) {
    if(alist) {
        ficon->alist_toke = alist;
        ficon->passed = 0x00;
    } else {
        if(ficon->passed) ficon->passed = 0x00;
        else ficon->alist_toke = ficon->alist_toke->ison_next;
    }
    //
    return ficon->alist_toke;
}

void hand_find_conte(struct find_conte *ficon, struct fsdo_list *alist) {
    if(alist == ficon->alist_toke) {
        ficon->alist_toke = alist->ison_next;
        ficon->passed = 0x01;
    }
}

