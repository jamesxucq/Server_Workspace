/* 
 * File:   auth_cache.h
 * Author: David
 *
 * Created on: 2010-3-10
 */

#ifndef AUTH_CACHE_H
#define	AUTH_CACHE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "common_macro.h"
#include "hash_table_ext.h"
#include "string_utility.h"

    //

    struct auth_data { // login
        unsigned int uid;
        char req_valid[VALID_LENGTH];
        char location[MAX_PATH * 3];
        char seion_id[SESSION_LENGTH];
        //
        struct {
            char sin_addr[SERVNAME_LENGTH];
            int sin_port;
        } saddr;
        unsigned int cached_anchor;
        char _author_chks_[MD5_TEXT_LENGTH];
        //
        BYTE serv_locked;
        long last_active; // last active time
    };

    struct list_data {
        unsigned int uid;
        char req_valid[VALID_LENGTH];
        char location[MAX_PATH * 3];
        char seion_id[SESSION_LENGTH];
        unsigned int last_anchor;
        //
        int anchor_cached; // 0 not cached, 1 cached
        //
        unsigned int refer_inde; // reference
        unsigned int list_cache; // 
        long last_active; // last active time
    };

    struct list_data* new_ldata();
#define DELETE_LDATA(LDATA)     { if (ldata) free(ldata); LDATA = NULL; }
    void free_ldata(void* ldata);
#define free_adata      free_ldata

    //
    // extern hashmap *_auth_cache_hm_;
    extern hashmap *_ldata_hm_;
#define _adata_hm_     _ldata_hm_

#ifdef	__cplusplus
}
#endif

#endif	/* AUTH_CACHE_H */

