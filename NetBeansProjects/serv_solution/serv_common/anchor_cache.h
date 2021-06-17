#ifndef ANCHORCACHE_H
#define	ANCHORCACHE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_macro.h"    
#include "hash_table_ext.h"

    struct actno_ope {
        //
        struct ope_list *action_list;
        cachemap *dire_cache;
        cachemap *file_cache;
        //
        unsigned int refer_inde; // reference     
    };

    struct actno_ope* new_actno_ope();
    void destory_actno_ope(struct actno_ope* aope);
    void del_actno_ope(void *aope);
#define DELE_ACTNO_OPE(AOPE) del_actno_ope(AOPE); AOPE = NULL;

    void add_action_addi(struct actno_ope* aope, unsigned int is_dire, char *file_name, uint64 file_size, time_t act_time);
    void add_action_delete(struct actno_ope* aope, unsigned int is_dire, char *file_name, uint64 file_size, time_t act_time);
    void add_action_modify(struct actno_ope* aope, char *file_name, uint64 file_size, uint64 origin_size, time_t act_time);
    void add_action_move(struct actno_ope* aope, char *exist_name, char *new_name, time_t act_time);
    void add_action_copy(struct actno_ope* aope, char *exist_name, char *new_name, uint64 file_size, time_t act_time);

    extern hashmap *_anchope_hm_;

#ifdef	__cplusplus
}
#endif

#endif	/* ANCHORCACHE_H */

