/* 
 * File:   speed_utility.h
 * Author: Administrator
 */

#ifndef SPEED_UTILITY_H
#define	SPEED_UTILITY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_header.h"
    //
    typedef cachemap speedmap;
    
//
#define SPEED_BACKET_SIZE      1024 
#define create_spdmap() cache_create(lh_strhash, equal_str, SPEED_BACKET_SIZE)
#define spdmap_del(smap) cache_del(smap)
//
    void spdmap_insert(speedmap *smap, struct fsdo_list *alist);
    void spdmap_remove(speedmap *smap, struct fsdo_list *alist);
//
#define spdmap_value(smap, alist) cache_value(alist->fsdo.file_name, smap)
//    
struct find_conte {
    unsigned int passed; // 0:normal 1:passed
    struct fsdo_list *alist_toke;
};

    struct fsdo_list *find_next_value(struct find_conte *ficon, struct fsdo_list *alist);
    void hand_find_conte(struct find_conte *ficon, struct fsdo_list *alist);
    
    //
    
#ifdef	__cplusplus
}
#endif

#endif	/* SPEED_UTILITY_H */

