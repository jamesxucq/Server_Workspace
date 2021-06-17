#include "ope_list.h"
#include "file_utility.h"
#include "iattb_type.h"
#include "logger.h"

#include "action.h"
#include "anchor_cache.h"

hashmap *_anchope_hm_;
#define DIRE_BACKET_SIZE      256 
#define FILE_BACKET_SIZE      1024 

struct actno_ope* new_actno_ope() {
    struct actno_ope *aope = (struct actno_ope *) malloc(sizeof (struct actno_ope));
    if (!aope) return NULL;
    memset(aope, '\0', sizeof (struct actno_ope));
    // _LOG_DEBUG("malloc new actno aope"); // delete by james 20130409
    aope->dire_cache = create_cache(DIRE_BACKET_SIZE);
    aope->file_cache = create_cache(FILE_BACKET_SIZE);
    //
    return aope;
}

void del_actno_ope(void *aope) {
    if (aope) free(aope);
}

void destory_actno_ope(struct actno_ope* aope) {
    if (aope) {
        if (aope->file_cache) {
            cache_del(aope->file_cache);
            aope->file_cache = NULL;
        }
        if (aope->dire_cache) {
            cache_del(aope->dire_cache);
            aope->dire_cache = NULL;
        }
        if (aope->action_list) {
            DELETE_OPE_LIST(aope->action_list)
            aope->action_list = NULL;
        }
    }
    // _LOG_DEBUG("destory actno aope"); // delete by james 20130409
}

void add_action_addi(struct actno_ope* aope, unsigned int is_dire, char *file_name, uint64 file_size, time_t act_time) {
    struct ope_list *olist;
    struct openod *opesdo;
    // 
    if (is_dire) {
        if ((olist = cache_value(file_name, aope->dire_cache))) {
            switch (olist->opesdo.action_type) {
                case ADDI:
                    break;
                case DELE:
                    olist_delete(&aope->action_list, olist);
                    cache_remove(file_name, aope->dire_cache);
                    delete_ope_list(olist);
                    break;
            }
        } else {
            olist = append_new_olist(&aope->action_list);
            opesdo = &olist->opesdo;
            //
            opesdo->action_type = ADDI | DIRECTORY;
            opesdo->file_name = strdup(file_name);
            opesdo->file_size = file_size;
            opesdo->act_time = act_time;
            cache_insert(opesdo->file_name, olist, aope->dire_cache);
        }
    } else {
        if ((olist = cache_value(file_name, aope->file_cache))) {
            switch (olist->opesdo.action_type) {
                case ADDI:
                case MODIFY:
                    break;
                case DELE:
                    olist->opesdo.action_type = MODIFY;
                    olist->opesdo.act_time = act_time;
                    break;
                case COPY:
                case MOVE:
                    break;
            }
        } else {
            olist = append_new_olist(&aope->action_list);
            opesdo = &olist->opesdo;
            //
            opesdo->action_type = ADDI;
            opesdo->file_name = strdup(file_name);
            opesdo->file_size = file_size;
            opesdo->act_time = act_time;
            cache_insert(opesdo->file_name, olist, aope->file_cache);
        }
    }
    //
}

//
#define hashinde(key, map) ((map->gethash)(key) % (map->hashsize))

struct ope_list *cache_file_value(void *key, cachemap *map) {
    struct cache_node *posi;
    struct openod *opesdo;
    unsigned int inde = hashinde(key, map);
    //
    for (posi = map->hmaplist[inde]; NULL != posi; posi = posi->next) {
        opesdo = &((struct ope_list *) posi->data)->opesdo;
        if (map->compare(key, posi->key) && !(DIRECTORY & opesdo->action_type)) {
            return (posi->data);
        }
    }
    return NULL;
}

void add_action_delete(struct actno_ope* aope, unsigned int is_dire, char *file_name, uint64 file_size, time_t act_time) {
    struct ope_list *olist, *exis_olist = NULL;
    struct openod *opesdo;
    //     
    if (is_dire) {
        if ((olist = cache_value(file_name, aope->dire_cache))) {
            switch (olist->opesdo.action_type) {
                case ADDI:
                    olist_delete(&aope->action_list, olist);
                    cache_remove(file_name, aope->dire_cache);
                    delete_ope_list(olist);
                    break;
                case DELE:
                    break;
            }
        } else {
            olist = append_new_olist(&aope->action_list);
            opesdo = &olist->opesdo;
            //
            opesdo->action_type = DELE | DIRECTORY;
            opesdo->file_name = strdup(file_name);
            opesdo->file_size = file_size;
            opesdo->act_time = act_time;
            cache_insert(opesdo->file_name, olist, aope->dire_cache);
        }
    } else {
        if ((olist = cache_value(file_name, aope->file_cache))) {
            switch (olist->opesdo.action_type) {
                case ADDI:
                    olist_delete(&aope->action_list, olist);
                    cache_remove(file_name, aope->file_cache);
                    delete_ope_list(olist);
                    break;
                case MODIFY:
                    olist->opesdo.action_type = DELE;
                    olist->opesdo.act_time = act_time;
                    break;
                case DELE:
                    break;
                case COPY:
                    exis_olist = find_olist_prev(aope->action_list, olist);
                    olist_delete(&aope->action_list, olist);
                    cache_remove(file_name, aope->file_cache);
                    delete_ope_list(olist);
                    olist_delete(&aope->action_list, exis_olist);
                    delete_ope_list(exis_olist);
                    break;
                case MOVE:
                    exis_olist = find_olist_prev(aope->action_list, olist);
                    olist_delete(&aope->action_list, olist);
                    cache_remove(file_name, aope->file_cache);
                    delete_ope_list(olist);
                    olist_delete(&aope->action_list, exis_olist);
                    delete_ope_list(exis_olist);
                    break;
            }
        } else {
            olist = append_new_olist(&aope->action_list);
            opesdo = &olist->opesdo;
            //
            opesdo->action_type = DELE;
            opesdo->file_name = strdup(file_name);
            opesdo->file_size = file_size;
            opesdo->act_time = act_time;
            cache_insert(opesdo->file_name, olist, aope->file_cache);
        }
        //
    }
}

void add_action_modify(struct actno_ope* aope, char *file_name, uint64 file_size, uint64 origin_size, time_t act_time) {
    struct ope_list *olist;
    struct openod *opesdo;
    //     
    if ((olist = cache_value(file_name, aope->file_cache))) {
        if ((ADDI | MODIFY | DELE) & olist->opesdo.action_type)
            return;
    }
    //
    olist = append_new_olist(&aope->action_list);
    opesdo = &olist->opesdo;
    //
    opesdo->action_type = MODIFY;
    opesdo->file_name = strdup(file_name);
    opesdo->file_size = file_size;
    opesdo->reserved = origin_size;
    opesdo->act_time = act_time;
    cache_insert(opesdo->file_name, olist, aope->file_cache);
}

void add_action_copy(struct actno_ope* aope, char *exist_name, char *new_name, uint64 file_size, time_t act_time) {
    struct openod *opesdo;
    // 
    struct ope_list *olist = append_new_olist(&aope->action_list);
    // cache_insert(exist_name, olist, aope->cache_map);
    opesdo = &olist->opesdo;
    //
    unsigned int coplue_id = GetTickCount;
    opesdo->action_type = EXIST;
    opesdo->file_name = strdup(exist_name);
    opesdo->reserved = coplue_id;
    opesdo->act_time = act_time;
    //
    olist = append_new_olist(&aope->action_list);
    opesdo = &olist->opesdo;
    //
    opesdo->action_type = COPY;
    opesdo->file_name = strdup(new_name);
    opesdo->file_size = file_size;
    opesdo->reserved = coplue_id;
    opesdo->act_time = act_time;
    cache_insert(opesdo->file_name, olist, aope->file_cache);
}

void add_action_move(struct actno_ope* aope, char *exist_name, char *new_name, time_t act_time) {
    struct ope_list *olist, *new_olist;
    struct openod *opesdo;
    //    
    if ((olist = cache_value(exist_name, aope->file_cache))) {
        memset(&opesdo, '\0', sizeof (struct openod));
        switch (olist->opesdo.action_type) {
            case ADDI:
                stredup(olist->opesdo.file_name, new_name);
                olist->opesdo.act_time = act_time;
                break;
            case MODIFY:
                new_olist = insert_new_olist(&aope->action_list, olist);
                // cache_insert(exist_name, olist, aope->cache_map);
                opesdo = &new_olist->opesdo;
                //
                unsigned int coplue_id = GetTickCount;
                opesdo->file_name = strdup(exist_name);
                opesdo->action_type = EXIST;
                opesdo->reserved = coplue_id;
                opesdo->act_time = act_time;

                new_olist = insert_new_olist(&aope->action_list, new_olist);
                opesdo = &new_olist->opesdo;
                //
                opesdo->file_name = strdup(new_name);
                opesdo->action_type = MOVE;
                opesdo->reserved = coplue_id;
                opesdo->act_time = act_time;
                cache_insert(opesdo->file_name, new_olist, aope->file_cache);

                stredup(olist->opesdo.file_name, new_name);
                break;
            case DELE:
                break;
            case COPY:
            case MOVE:
                stredup(olist->opesdo.file_name, new_name);
                olist->opesdo.act_time = act_time;
                break;
        }
    } else {
        olist = append_new_olist(&aope->action_list);
        // cache_insert(exist_name, olist, aope->cache_map);
        opesdo = &olist->opesdo;
        //
        unsigned int coplue_id = GetTickCount;
        opesdo->action_type = EXIST;
        opesdo->file_name = strdup(exist_name);
        opesdo->reserved = coplue_id;
        opesdo->act_time = act_time;
        //
        olist = append_new_olist(&aope->action_list);
        opesdo = &olist->opesdo;
        //
        opesdo->action_type = MOVE;
        opesdo->file_name = strdup(new_name);
        opesdo->reserved = coplue_id;
        opesdo->act_time = act_time;
        cache_insert(opesdo->file_name, olist, aope->file_cache);
    }
}
