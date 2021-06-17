/* 
 * File:   pool_capacity.h
 * Author: Administrator
 *
 * Created on 2012
 */

#ifndef POOL_CAPACITY_H
#define	POOL_CAPACITY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "ope_list.h"    

#define CAPACITY_DEFAULT   "~/capacity.acd"

    struct capacity {
        uint64 used_size;
        uint64 bakup_size;
    };

    int reset_capacity(char *location, struct ope_list *action_list);


#ifdef	__cplusplus
}
#endif

#endif	/* POOL_CAPACITY_H */

