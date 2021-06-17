/* opera_bzl.c
 * Created on: 2010-3-10
 * Author: Divad
 */

#include "ope_list.h"
#include "iattb_type.h"

#include "anchor_cache.h"
#include "opera_bzl.h"
#include "speed_utility.h"
#include "action.h"
#include "anchor.h"

int operabzl_create(struct serv_config *config) {
    /* set anchor file name */
    if (set_anchor_path(config->anchor.anch_idx, config->anchor.anch_acd)) {
        _LOG_WARN("set anchor config failed!");
        return ERR_FAULT;
    }
    /* set action file name */
    /* if (set_action_log(ACTION_LOG_DEFAULT)) {
        _LOG_WARN("set action log failed!");
        return ERR_FAULT;
    } */ // disable by james 20140515
    //
    _anchope_hm_ = create_hashmap(del_actno_ope, config->max_connecting);
    if (!_anchope_hm_) return ERR_FAULT;
    //
    return ERR_SUCCESS; //succ
}

int operabzl_destroy() {
    if (_anchope_hm_) {
        hashm_del(_anchope_hm_);
        _anchope_hm_ = NULL;
    }
    return ERR_SUCCESS; //succ
}

uint32 add_anchor_bzl(char *location, struct actno_ope* aope) {
    if (flush_anch_list(location, aope->action_list))
        return 0;
    uint32 ins_anchor = addi_anchor(location);
    /* if (flush_action_list(location, aope->action_list))
        return 0; */ // disable by james 20140515
    if (reset_capacity(location, aope->action_list))
        return 0;
    //
    // _LOG_DEBUG("add new anchor, delete actno opera."); // disable by james 20120410
    return ins_anchor;
}

int flush_anchor_bzl(char *location, struct actno_ope* aope) {
_LOG_DEBUG("flush anchor bzl."); // disable by james 20120410
    if (flush_anch_list(location, aope->action_list))
        return -1;
    /* if (flush_action_list(location, aope->action_list))
        return -1; */ // disable by james 20140515
    if (reset_capacity(location, aope->action_list))
        return -1;
    //
_LOG_DEBUG("flush new anchor, delete actno opera."); // disable by james 20120410
    return 0;
}

//

void print_p(struct fatsdo *fnod) {
    _LOG_DEBUG("fnod->action_type:%x", fnod->action_type);
    _LOG_DEBUG("fnod->file_name:%s", fnod->file_name);
    //    _LOG_DEBUG("fnod->file_size:%llu", fnod->file_size);
    _LOG_DEBUG("fnod->reserved:%ud", fnod->reserved);
    _LOG_DEBUG("***************");
}

inline struct fsdo_list *find_exist_alist(struct fsdo_list *alist, struct fsdo_list *new_alist) {
    struct fsdo_list *alist_toke;
    //
    ALIST_FOREACH(alist, alist_toke) {
print_p(&alist_toke->fsdo);
        if (alist_toke->fsdo.reserved == new_alist->fsdo.reserved &&
                EXIST & alist_toke->fsdo.action_type)
            break;
    }
    return alist_toke;
}

void print_files_att_list(struct fsdo_list *alist) {
    struct fsdo_list *alistmp;
    if (!alist) {
        _LOG_DEBUG("alist is null");
    } else {
        ALIST_FOREACH(alist, alistmp) {
            _LOG_DEBUG("files->action_type:%x", alistmp->fsdo.action_type);
            _LOG_DEBUG("files->file_name:%s", alistmp->fsdo.file_name);
            // _LOG_DEBUG("files->file_size:%llu", alistmp->fsdo.file_size);
            _LOG_DEBUG("files->reserved:%ud", alistmp->fsdo.reserved);
            _LOG_DEBUG("-----------------");
        }
    }
}

//
#define SAFE_ALIST_DELE(ficon, alsth, spdmap, alist) \
    hand_find_conte(ficon, alist); \
    spdmap_remove(spdmap, alist); \
    alist_delete(alsth, alist); \
    delete_fasd_list(alist);

#define SAFE_OLIST_DELE(olsth, spdmap, olist) \
    spdmap_remove(spdmap, olist); \
    alist_delete(olsth, olist); \
    delete_fasd_list(olist);

//

static void handle_active_directory(fsdo_lsth activ_alsth, struct fatsdo *inde_nod, speedmap *dire_spdm, struct find_conte *dicon, struct fsdo_list *found_alist) {
    struct fatsdo *found_nod = NULL;
    //
    while (found_alist) {
        // _LOG_DEBUG("--- found node:%s", found_alist->fsdo.file_name);
        found_nod = &found_alist->fsdo;
        // print_fatt(found_nod);
        // if(ADDI&found_nod->stat && ADDI&inde_nod->stat){ } 
        if (ADDI & found_nod->action_type && DELE & inde_nod->action_type) {
            SAFE_ALIST_DELE(dicon, activ_alsth, dire_spdm, found_alist)
        } else if (DELE & found_nod->action_type && ADDI & inde_nod->action_type) {
            SAFE_ALIST_DELE(dicon, activ_alsth, dire_spdm, found_alist)
        } else if (DELE & found_nod->action_type && DELE & inde_nod->action_type) {
            move_alist_last(activ_alsth, found_alist);
        } 
        // _LOG_DEBUG("#############################");
        // print_files_att_list(*activ_alsth);
        found_alist = find_next_value(dicon, NULL);
    }
    //
}

//
static void comp_active_file(fsdo_lsth appe_alsth, fsdo_lsth activ_alsth, struct fatsdo *inde_nod, struct fatsdo *nexis_nod, speedmap *file_spdm, struct find_conte *ficon, struct fsdo_list *found_alist) {
    struct fsdo_list *oexis_alist;
    struct fatsdo *found_nod = NULL;
    // print_fatt(inde_nod);
    while (found_alist) {
_LOG_DEBUG("-----------------");
_LOG_DEBUG("--- found_alist:%08x :%08x :%08x :%08x", found_alist, &found_alist->list_next, &found_alist->ison_next, &found_alist->fsdo);
        found_nod = &found_alist->fsdo;
_LOG_DEBUG("--- inde node:%s :%08x", inde_nod->file_name, inde_nod->action_type);
_LOG_DEBUG("--- found_nod:%s :%08x", found_nod->file_name, found_nod->action_type);
        // print_fatt(found_nod);
        // if(ADDI&found_nod->stat && ADDI&inde_nod->stat){ } 
        // else if (ADDI&found_nod->action_type && MODIFY&inde_nod->action_type) { }
        if (ADDI & found_nod->action_type && DELE & inde_nod->action_type) {
            SAFE_ALIST_DELE(ficon, activ_alsth, file_spdm, found_alist)
        } else if (ADDI & found_nod->action_type && EXIST & inde_nod->action_type) {
            // _LOG_DEBUG("add exist inde:%s", inde_nod->file_name);
            if (COPY & nexis_nod->action_type) {
                // _LOG_DEBUG("add exist next:%s", nexis_alist->fsdo.file_name);
                if(comp_fsdo_ext(*appe_alsth, inde_nod, nexis_nod)) {
                    append_alist_newsdo(appe_alsth, inde_nod);
                    append_alist_newsdo(appe_alsth, nexis_nod);                
                }
            } else if (MOVE & nexis_nod->action_type) {
                stredup(found_nod->file_name, nexis_nod->file_name);
            }
        } else if (MODIFY & found_nod->action_type && ADDI & inde_nod->action_type) {
            found_nod->action_type = ADDI;
        } else if (MODIFY & found_nod->action_type && MODIFY & inde_nod->action_type) {
            found_nod->file_size = inde_nod->file_size;
        } else if (MODIFY & found_nod->action_type && DELE & inde_nod->action_type) {
            found_nod->action_type = DELE;
            move_alist_last(activ_alsth, found_alist);
        } else if (MODIFY & found_nod->action_type && EXIST & inde_nod->action_type) {
            insert_alist_newsdo(appe_alsth, inde_nod, found_alist);
            insert_alist_newsdo(appe_alsth, nexis_nod, found_alist);
            stredup(found_nod->file_name, inde_nod->file_name);
        } else if (DELE & found_nod->action_type && ADDI & inde_nod->action_type) {
            found_nod->action_type = MODIFY;
            move_alist_last(activ_alsth, found_alist);
        } // else if(DELE&found_nod->action_type && MODIFY&inde_nod->action_type) { // }
        else if (DELE & found_nod->action_type && DELE & inde_nod->action_type) {
            move_alist_last(activ_alsth, found_alist);
        } else if (DELE & found_nod->action_type && EXIST & inde_nod->action_type) {
            SAFE_ALIST_DELE(ficon, activ_alsth, file_spdm, found_alist)
            if(comp_fsdo_ext(*appe_alsth, inde_nod, nexis_nod)) {
                append_alist_newsdo(appe_alsth, inde_nod);
                append_alist_newsdo(appe_alsth, nexis_nod);
            }
        } else if (EXIST & found_nod->action_type && ADDI & inde_nod->action_type) {
            if(comp_fsdo(*appe_alsth, inde_nod)) {
                append_alist_newsdo(appe_alsth, inde_nod);
            }
        } // else if(EXIST&found_nod->action_type && MODIFY&inde_nod->action_type) { // }
          // else if(EXIST&found_nod->action_type && DELE&inde_nod->action_type){ // }
        else if (EXIST & found_nod->action_type && EXIST & inde_nod->action_type) {
            if(comp_fsdo_ext(*appe_alsth, inde_nod, nexis_nod)) {
                append_alist_newsdo(appe_alsth, inde_nod);
                append_alist_newsdo(appe_alsth, nexis_nod);
            }
        } // else if(COPY&found_nod->action_type && ADDI&inde_nod->action_type){ // }
        else if (COPY & found_nod->action_type && MODIFY & inde_nod->action_type) {
            if(comp_fsdo(*appe_alsth, inde_nod)) {
                append_alist_newsdo(appe_alsth, inde_nod);
            }
        } else if (COPY & found_nod->action_type && DELE & inde_nod->action_type) {
            oexis_alist = find_exist_alist(*activ_alsth, found_alist);
            SAFE_ALIST_DELE(ficon, activ_alsth, file_spdm, found_alist)
            SAFE_OLIST_DELE(activ_alsth, file_spdm, oexis_alist)
        } else if (COPY & found_nod->action_type && EXIST & inde_nod->action_type) {
            // _LOG_DEBUG("copy exist inde:%s", inde_nod->file_name);
            if (COPY & nexis_nod->action_type) {
                // _LOG_DEBUG("copy exist next:%s", nexis_alist->fsdo.file_name);
                if(comp_fsdo_ext(*appe_alsth, inde_nod, nexis_nod)) {
                    append_alist_newsdo(appe_alsth, inde_nod);
                    append_alist_newsdo(appe_alsth, nexis_nod);
                }
            } else if (MOVE & nexis_nod->action_type) {
                stredup(found_nod->file_name, nexis_nod->file_name);
            }
        } //
          // else if(MOVE&found_nod->action_type && ADDI&inde_nod->action_type){ // }
        else if (MOVE & found_nod->action_type && MODIFY & inde_nod->action_type) {
            if(comp_fsdo(*appe_alsth, inde_nod)) {
                append_alist_newsdo(appe_alsth, inde_nod);
            }
        } else if (MOVE & found_nod->action_type && DELE & inde_nod->action_type) {
            // _LOG_DEBUG("#############################");
            // print_files_att_list(*inde_alsth);
            oexis_alist = find_exist_alist(*activ_alsth, found_alist);
            // _LOG_DEBUG("oexis_alist:%d", oexis_alist);
            // print_fatt(&oexis_alist->fsdo);
            oexis_alist->fsdo.action_type = DELE;
            oexis_alist->fsdo.reserved = 0;
            SAFE_ALIST_DELE(ficon, activ_alsth, file_spdm, found_alist)
        } else if (MOVE & found_nod->action_type && EXIST & inde_nod->action_type) {
            if (COPY & nexis_nod->action_type) {
// _LOG_DEBUG("--- 1");
                insert_alist_newsdo(appe_alsth, inde_nod, found_alist);
                insert_alist_newsdo(appe_alsth, nexis_nod, found_alist);
            } else if (MOVE & nexis_nod->action_type) {
// _LOG_DEBUG("--- 2");
                oexis_alist = find_exist_alist(*activ_alsth, found_alist);
                if (!strcmp(nexis_nod->file_name, oexis_alist->fsdo.file_name)) {
                    SAFE_ALIST_DELE(ficon, activ_alsth, file_spdm, found_alist)
                    SAFE_OLIST_DELE(activ_alsth, file_spdm, oexis_alist)
                } else stredup(found_nod->file_name, nexis_nod->file_name);
            }
        }
        // _LOG_DEBUG("#############################");
        // print_files_att_list(*activ_alsth);
        found_alist = find_next_value(ficon, NULL);
    }
    //
}

static void final_active_file(fsdo_lsth activ_alsth, speedmap *file_spdm, struct fsdo_list *appe_alist) {
    struct fsdo_list *inde_alist;
    for (inde_alist = appe_alist; inde_alist; inde_alist = inde_alist->list_next) {    
        spdmap_insert(file_spdm, inde_alist);
    }
    move_alist_list(activ_alsth, appe_alist);
}

static void handle_active_file(fsdo_lsth activ_alsth, struct fatsdo *inde_nod, struct fatsdo *nexis_nod, speedmap *file_spdm, struct find_conte *ficon, struct fsdo_list *found_alist) {
    struct fsdo_list *appe_alist = NULL;
    comp_active_file(&appe_alist, activ_alsth, inde_nod, nexis_nod, file_spdm, ficon, found_alist);
    final_active_file(activ_alsth, file_spdm, appe_alist);
}

//
static void handle_nexis_file(fsdo_lsth activ_alsth, struct fatsdo *inde_nod, struct fatsdo *nexis_nod, speedmap *file_spdm, struct find_conte *necon, struct fsdo_list *fnexi_alist) {
    struct fsdo_list *new_nod;
    struct fatsdo *found_nod = NULL;
    // _LOG_DEBUG("--- inde node:%s", inde_nod->file_name);
    // print_fatt(inde_nod);
    while (fnexi_alist) {
        // _LOG_DEBUG("--- found node:%s", found_alist->fsdo.file_name);
        found_nod = &fnexi_alist->fsdo;
        // print_fatt(found_nod);
        if (DELE & found_nod->action_type && COPY & nexis_nod->action_type) {
            found_nod->action_type = MODIFY;
            move_alist_last(activ_alsth, fnexi_alist);
        } else if (DELE & found_nod->action_type && MOVE & nexis_nod->action_type) {
            new_nod = append_alist_newsdo(activ_alsth, inde_nod);
            spdmap_insert(file_spdm, new_nod);
            //
            found_nod->action_type = MOVE;
            move_alist_last(activ_alsth, fnexi_alist);
        }
        // _LOG_DEBUG("#############################");
        // print_files_att_list(*activ_alsth);
        fnexi_alist = find_next_value(necon, NULL);
    }
    //
}

//
void print_fatt(struct fatsdo *fnod) {
    _LOG_DEBUG("fnod->action_type:%x", fnod->action_type);
    _LOG_DEBUG("fnod->file_name:%s", fnod->file_name);
    _LOG_DEBUG("fnod->reserved:%ud", fnod->reserved);
    _LOG_DEBUG("++++++++++++++++++");
}

//
#define AlistNextNode(exist_alist, next_alist) \
{ exist_alist = next_alist; next_alist = next_alist->list_next; }


int creat_active_alist(fsdo_lsth activ_alsth, fsdo_lsth inde_alsth) {
    speedmap *dire_spdm, *file_spdm;
    struct fsdo_list *inde_alist, *nexis_alist, *next_alist, *found_alist, *fnexi_alist;
    struct fatsdo *inde_nod = NULL;
    struct find_conte ficon, necon, dicon;
    //
    if (!(dire_spdm = create_spdmap())) return -1;
    if (!(file_spdm = create_spdmap())) return -1;
    //
    for (inde_alist = (*inde_alsth); inde_alist; inde_alist = next_alist) {
        next_alist = inde_alist->list_next;
        inde_nod = &inde_alist->fsdo;
        //
        if (DIRECTORY & inde_nod->action_type) {
            found_alist = spdmap_value(dire_spdm, inde_alist);
            if (found_alist) {
                find_next_value(&dicon, found_alist);
                handle_active_directory(activ_alsth, inde_nod, dire_spdm, &dicon, found_alist);
            } else { // handle directory
                alist_delete(inde_alsth, inde_alist);
                spdmap_insert(dire_spdm, inde_alist);
                alist_apend(activ_alsth, inde_alist);
            }
        } else {
            found_alist = spdmap_value(file_spdm, inde_alist);
            if (EXIST & inde_nod->action_type) {
                AlistNextNode(nexis_alist, next_alist)
                if (!((COPY|MOVE) & nexis_alist->fsdo.action_type)) {
                    // _LOG_DEBUG("+++ ins node:%s", nexis_alist->fsdo.file_name);
                    continue;
                }
                // HANDLE_FILE_NFOUND(nexis_alist)
                fnexi_alist = spdmap_value(file_spdm, nexis_alist);
                ////////////////////////////////////////////////////////////////
                if(fnexi_alist) {
                    find_next_value(&necon, fnexi_alist);
                    handle_nexis_file(activ_alsth, inde_nod, &nexis_alist->fsdo, file_spdm, &necon, fnexi_alist);
                } else if(found_alist) {
                    find_next_value(&ficon, found_alist);
                    handle_active_file(activ_alsth, inde_nod, &nexis_alist->fsdo, file_spdm, &ficon, found_alist);
                } else {
                    alist_delete(inde_alsth, inde_alist);
                    spdmap_insert(file_spdm, inde_alist);
                    alist_apend(activ_alsth, inde_alist);
                    //
                    alist_delete(inde_alsth, nexis_alist);
                    spdmap_insert(file_spdm, nexis_alist);
                    alist_apend(activ_alsth, nexis_alist);
                }        
            } else {
                if (found_alist) {
                    find_next_value(&ficon, found_alist);
                    handle_active_file(activ_alsth, inde_nod, NULL, file_spdm, &ficon, found_alist);
                } else { // handle file
                    // _LOG_DEBUG("--- ins node:%s", inde_nod->file_name);
                    // print_fatt(&inde_alist->fsdo);
                    alist_delete(inde_alsth, inde_alist);
                    spdmap_insert(file_spdm, inde_alist);
                    alist_apend(activ_alsth, inde_alist);
                }
            }
            //
        }
    }
    //
    spdmap_del(file_spdm);
    spdmap_del(dire_spdm);
    return 0;
}

int fill_active_alist(fsdo_lsth activ_alsth, char *location) {
    struct fatsdo *exis_nod = NULL, *inde_nod;
    struct fsdo_list *inde_alist;
    char abso_path[MAX_PATH * 3];
    //
    for (inde_alist = (*activ_alsth); inde_alist; inde_alist = inde_alist->list_next) {
        inde_nod = &inde_alist->fsdo;
        if (!(DELE & inde_nod->action_type)) {
            if(!(EXIST & inde_nod->action_type)){
                POOL_PATH(abso_path, location, inde_nod->file_name);
// _LOG_DEBUG("action_type:%x, active path:%s", inde_nod->action_type , inde_nod->file_name);
                FILE_SIZE_TIME(inde_nod->file_size, inde_nod->last_write, abso_path)
                if(exis_nod && (COPY|MOVE)&inde_nod->action_type) {
                    exis_nod->file_size = inde_nod->file_size;
                    exis_nod->last_write = inde_nod->last_write;
                    exis_nod = NULL;
                }
            } else exis_nod = inde_nod;
        }
    }
    //
    return 0;
}

int get_alist_by_anchor(fsdo_lsth alsth, char *location, uint32 anchor) {
    struct idxent ndxent;
    char anch_idx[MAX_PATH * 3], anch_acd[MAX_PATH * 3];
    struct fsdo_list *inde_alist = NULL;
    //
    memset(&ndxent, '\0', sizeof (struct idxent));
    APPEND_PATH(anch_idx, location, _anchor_files_.anch_idx);
    APPEND_PATH(anch_acd, location, _anchor_files_.anch_acd);
    if (search_anchor(&ndxent, anch_idx, anchor)) {
_LOG_DEBUG("get alist not found! anchor:%u", anchor);
        return ANCHOR_NOFOND;
    }
    // _LOG_DEBUG("ndxent anchor_number:%u offset:%llu time_stamp:%d", ndxent.anchor_number, ndxent.data_oset, ndxent.time_stamp);
    if (get_alist_by_idxent(&inde_alist, anch_acd, &ndxent)) {
_LOG_DEBUG("get alist search anchor!");
        return SEARCH_ANCHOR;
    }
    // _LOG_DEBUG("--------------------------------------------------");
    // print_files_att_list(inde_alist); // disable by james 20150213
    if (inde_alist) {
        if (creat_active_alist(alsth, &inde_alist)) {
            delete_alist(inde_alist);
_LOG_DEBUG("get alist fault!");
            return ERR_FAULT;
        }
        delete_alist(inde_alist);
        // _LOG_DEBUG("--------------------------------------------------");
        // print_files_att_list(*alsth); // disable by james 20150213
    }
    // _LOG_DEBUG("--------------------------------------------------");
    if (fill_active_alist(alsth, location)) {
_LOG_DEBUG("active alist error!");
        return ALIST_ERROR;
    }
    //
    return ERR_SUCCESS;
}

void clear_anchor_file(char *location) {

}

//

struct actno_ope * get_opera_cache_bzl(int uid) {
    struct actno_ope *aope;
    if ((aope = hashm_value(uid, _anchope_hm_))) aope->refer_inde++;
    else {
        if ((aope = new_actno_ope())) {
            aope->refer_inde = 1;
            hashm_insert(uid, aope, _anchope_hm_);
        }
    }
    //
    return aope;
}

int valid_opera_referen(int uid) {
    struct actno_ope *aope;
    int referen_value = 0;
    if ((aope = hashm_value(uid, _anchope_hm_))) {
        referen_value = aope->refer_inde;
    }
    return referen_value;
}

void opera_epoll_exception(int uid) {
    struct actno_ope *aope;
    if ((aope = hashm_value(uid, _anchope_hm_))) {
        aope->refer_inde--;
        if (!aope->refer_inde) {
            _LOG_INFO("timeout delete anchor aope reference:%d", aope->refer_inde);
            destory_actno_ope(aope);
            hashm_remove(uid, _anchope_hm_);
        }
    }
    // _LOG_INFO("timeout decrease anchor cache reference:%d", aope->refer_inde); // disable by james 20120410
}

void opera_reconn_exception(int uid) {
    struct actno_ope *aope;
    if ((aope = hashm_value(uid, _anchope_hm_))) {
        _LOG_INFO("reconn delete anchor aope reference:%d", aope->refer_inde);
        destory_actno_ope(aope);
        hashm_remove(uid, _anchope_hm_);
    }
}

int del_opera_cache_end(int uid) {
    struct actno_ope *aope;
    //
    if ((aope = hashm_value(uid, _anchope_hm_))) aope->refer_inde--;
    else return ERR_FAULT;
    // _LOG_INFO("decrease anchor cache reference:%d", aope->refer_inde); // disable by james 20120410
    //
    return ERR_SUCCESS;
}

int del_opera_cache_finish(int uid) {
    struct actno_ope *aope;
    //
    if ((aope = hashm_value(uid, _anchope_hm_))) { // close_anch_data(aope->anchor_datfp);
        destory_actno_ope(aope);
        hashm_remove(uid, _anchope_hm_);
    } else return ERR_FAULT;
    //
    return ERR_SUCCESS;
}
