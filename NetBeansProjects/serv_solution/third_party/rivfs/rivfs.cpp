
#include <sys/stat.h>

#include "river_utility.h"
#include "river_cache.h"
#include "river_entry.h"
#include "creat_river.h"

#include "rivfs.h"
//

uint32 riv_initial(struct riv_live *plive, char *location) {
    strcpy(plive->location, location);
    if (build_river_enviro(plive->location)) return 0x01;
    //
    if (cache_initial(plive)) return 0x01;
    if (entry_initial(plive)) return 0x01;
// fprintf(stderr, "rivfs initial ok!\n");
    return 0x00;
}

uint32 riv_final(struct riv_live *plive) {
    entry_final(plive);
    cache_final(plive);
// fprintf(stderr, "rivfs final ok!\n");
    return 0x00;
}

//
uint32 _river_insert_(struct cache_inde *pcval, char *file_name, struct riv_live * plive);
//
#define make_cache_exist(cvalue, path, plive, file_name, urval) \
    POOL_PATH(path, plive->location, file_name); \
    struct stat64 statb; \
    if (stat64(path, &statb) < 0) return urval; \
    if (-1 == cache_find(cvalue, plive->rivdb, file_name)) \
        _river_insert_(&cvalue, file_name, plive);

#define cchks_length(new_size) \
        uint32 new_clen = new_size >> 22; /* new_size / CHUNK_SIZE; */ \
        if (POW2N_MOD(new_size, CHUNK_SIZE)) new_clen++;

// for test
void chks_sha1(char *label, unsigned char *sha1) {
    char chksum_str[64];
    for (int i = 0; i < SHA1_DIGEST_LEN; i++)
        sprintf(chksum_str + i * 2, "%02x", sha1[i]);
    chksum_str[40] = '\0';
fprintf(stderr, "%s:%s\n", label, chksum_str);
}

// 0:pass 0x01:no content 0x02:not found 0x03:processing 0x04:server error
uint32 riv_file_sha1(FILE *fpriv, char *file_name, struct riv_live *plive) {
    struct cache_inde cvalue;
    char path[MAX_PATH * 3];
    struct file_id ident;
    //
fprintf(stderr, "rivfs file sha1:%s\n", file_name);
    make_cache_exist(cvalue, path, plive, file_name, 0x02)
    if(INVA_INDE_VALU == identi_read(&ident, plive->fpiden, cvalue.ainde))
        return 0x02;
    cchks_length(statb.st_size)
    if (new_clen != ident.chklen) {
fprintf(stderr, "reset ckchks length, new_clen:%u old_clen:%u\n", new_clen, ident.chklen);
        uint64 ind_pos = reset_ckchks_length(plive->fpchks, cvalue.chks_hind, ident.chklen, new_clen);
        identi_clen_update(plive, ident, cvalue.ainde, new_clen)
        if (ind_pos != cvalue.chks_hind) {
            cvalue.chks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    uint32 uval = 0x00;
    if (statb.st_mtime != ident.time_stamp) {
        uval = file_chks_update(plive->fpchks, path, cvalue.chks_hind, &statb);
        if (!uval) {
            file_sha1_chks(ident.sha1_chks, plive->fpchks, cvalue.chks_hind);
            identi_stamp_update(plive, ident, cvalue.ainde, statb.st_mtime)
            if (file_rive_sha1(fpriv, ident.sha1_chks)) uval = 0x04;
        }
    } else {
        if(ident.chklen) {
            if (file_rive_sha1(fpriv, ident.sha1_chks)) uval = 0x04;        
        } else  uval = 0x01;
    }
// chks_sha1("rivfs file sha1", ident.sha1_chks); // for test
// fprintf(stderr, "leave rivfs sha1\n");
    return uval;
}

// 0:pass 0x01:no content 0x02:not found 0x03:processing 0x04:server error
uint32 ren_file_sha1(FILE *fpriv, char *file_name, struct riv_live *plive) {
    struct cache_inde cvalue;
    char path[MAX_PATH * 3];
    struct file_id ident;
    //
fprintf(stderr, "renew file sha1:%s\n", file_name);
    make_cache_exist(cvalue, path, plive, file_name, 0x02)
    if(INVA_INDE_VALU == identi_read(&ident, plive->fpiden, cvalue.ainde))
        return 0x02;
    cchks_length(statb.st_size)
    if (new_clen != ident.chklen) {
fprintf(stderr, "reset ckchks length, new_clen:%u old_clen:%u\n", new_clen, ident.chklen);
        uint64 ind_pos = reset_ckchks_length(plive->fpchks, cvalue.chks_hind, ident.chklen, new_clen);
        identi_clen_update(plive, ident, cvalue.ainde, new_clen)
        if (ind_pos != cvalue.chks_hind) {
            cvalue.chks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    uint32 uval = file_chks_verify(plive->fpchks, path, cvalue.chks_hind, &statb);
    if (!uval) {
        file_sha1_chks(ident.sha1_chks, plive->fpchks, cvalue.chks_hind);
        identi_stamp_update(plive, ident, cvalue.ainde, statb.st_mtime)
        if (file_rive_sha1(fpriv, ident.sha1_chks)) uval = 0x04;
    }
// chks_sha1("renew file sha1", ident.sha1_chks);
// fprintf(stderr, "leave renew sha1\n");
    return uval;
}

// 0:pass 0x01:no content 0x02:not found 0x03:processing 0x04:server error
uint32 riv_file_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive) {
    struct cache_inde cvalue;
    char path[MAX_PATH * 3];
    struct file_id ident;
    //
fprintf(stderr, "rivfs file chks:%s cinde:%d\n", file_name, cinde);
    make_cache_exist(cvalue, path, plive, file_name, 0x02)
    if(INVA_INDE_VALU == identi_read(&ident, plive->fpiden, cvalue.ainde))
        return 0x02;
fprintf(stderr, "ainde:%llu chks_hind:%llu chklen:%u file_name:%s\n", cvalue.ainde, cvalue.chks_hind, ident.chklen, ident.file_name);    
    uint64 ind_pos;
    cchks_length(statb.st_size)
    if (new_clen != ident.chklen) {
fprintf(stderr, "reset ckchks length, new_clen:%u old_clen:%u\n", new_clen, ident.chklen);
        ind_pos = reset_ckchks_length(plive->fpchks, cvalue.chks_hind, ident.chklen, new_clen);
        identi_clen_update(plive, ident, cvalue.ainde, new_clen)
        if (ind_pos != cvalue.chks_hind) {
            cvalue.chks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    // 0:pass 0x01:no content 0x02:not found 0x03:processing 0x04:server error
    uint32 uval = file_chunk_update(plive->fpchks, &ind_pos, path, cvalue.chks_hind, cinde, &statb);
fprintf(stderr, "ind_pos:%llu cinde:%u\n", ind_pos, cinde);
    if(!uval) file_rive_chks(fpriv, plive->fpchks, ind_pos, cinde);
// fprintf(stderr, "leave rivfs file\n");
    return uval;
}

// 0:pass 0x01:no content 0x02:not found 0x03:processing 0x04:server error
uint32 ren_file_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive) {
    struct cache_inde cvalue;
    char path[MAX_PATH * 3];
    struct file_id ident;
    //
fprintf(stderr, "renew file chks:%s cinde:%d\n", file_name, cinde);
    make_cache_exist(cvalue, path, plive, file_name, 0x02)
    if(INVA_INDE_VALU == identi_read(&ident, plive->fpiden, cvalue.ainde))
        return 0x02;
    uint64 ind_pos;
    cchks_length(statb.st_size)
    if (new_clen != ident.chklen) {
fprintf(stderr, "reset ckchks length, new_clen:%u old_clen:%u\n", new_clen, ident.chklen);
        ind_pos = reset_ckchks_length(plive->fpchks, cvalue.chks_hind, ident.chklen, new_clen);
        identi_clen_update(plive, ident, cvalue.ainde, new_clen)
        if (ind_pos != cvalue.chks_hind) {
            cvalue.chks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    // 0:pass 0x01:no content 0x02:not found 0x03:processing 0x04:server error
    uint32 uval = file_chunk_verify(plive->fpchks, &ind_pos, path, cvalue.chks_hind, cinde, &statb);
    if(!uval) file_rive_chks(fpriv, plive->fpchks, ind_pos, cinde);
// fprintf(stderr, "leave rivfs file\n");
    return uval;
}

// 0:pass 0x01:not found 0x02:processing 0x03:server error
uint32 riv_cuk_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive) {
    struct cache_inde cvalue;
    char path[MAX_PATH * 3];
    struct file_id ident;
    //
fprintf(stderr, "rivfs chunk chks:%s cinde:%u\n", file_name, cinde);
    make_cache_exist(cvalue, path, plive, file_name, 0x01)
    if(INVA_INDE_VALU == identi_read(&ident, plive->fpiden, cvalue.ainde))
        return 0x01;
    uint64 ind_pos;
    cchks_length(statb.st_size)
    if (new_clen != ident.chklen) {
fprintf(stderr, "reset ckchks length, new_clen:%u old_clen:%u\n", new_clen, ident.chklen);
        ind_pos = reset_ckchks_length(plive->fpchks, cvalue.chks_hind, ident.chklen, new_clen);
        identi_clen_update(plive, ident, cvalue.ainde, new_clen)
        if (ind_pos != cvalue.chks_hind) {
            cvalue.chks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    ind_pos = chunk_chks_update(plive->fpchks, path, cvalue.chks_hind, cinde, &statb);
    chunk_rive_chks(fpriv, plive->fpchks, ind_pos, cinde);
// fprintf(stderr, "leave rivfs chunk\n");
    return 0x00;
}

// 0:pass 0x01:not found 0x02:processing 0x03:server error
uint32 ren_cuk_chks(FILE *fpriv, char *file_name, uint32 cinde, struct riv_live *plive) {
    struct cache_inde cvalue;
    char path[MAX_PATH * 3];
    struct file_id ident;
    //
fprintf(stderr, "renew chunk chks:%s cinde:%u\n", file_name, cinde);
    make_cache_exist(cvalue, path, plive, file_name, 0x01)
    if(INVA_INDE_VALU == identi_read(&ident, plive->fpiden, cvalue.ainde)) 
        return 0x01;
    uint64 ind_pos;
    cchks_length(statb.st_size)
    if (new_clen != ident.chklen) {
fprintf(stderr, "reset ckchks length, new_clen:%u old_clen:%u\n", new_clen, ident.chklen);
        ind_pos = reset_ckchks_length(plive->fpchks, cvalue.chks_hind, ident.chklen, new_clen);
        identi_clen_update(plive, ident, cvalue.ainde, new_clen)
        if (ind_pos != cvalue.chks_hind) {
            cvalue.chks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    ind_pos = chunk_chks_verify(plive->fpchks, path, cvalue.chks_hind, cinde, &statb);
    chunk_rive_chks(fpriv, plive->fpchks, ind_pos, cinde);
// fprintf(stderr, "leave rivfs chunk\n");
    return 0x00;
}

//
uint32 river_insert(char *file_name, struct riv_live * plive) {
    struct cache_inde cvalue;
    //
    if (0 < cache_find(cvalue, plive->rivdb, file_name)) return 0x01;
    cvalue.ainde = init_identi(plive->fpiden, file_name);
    cvalue.chks_hind = INVA_INDE_VALU;
    insert_cache(plive->rivdb, file_name, cvalue);
    //
fprintf(stderr, "rivfs insert file:%s\n", file_name);
    return 0x00;
}

uint32 _river_insert_(struct cache_inde *pcval, char *file_name, struct riv_live * plive) {
    pcval->ainde = init_identi(plive->fpiden, file_name);
    pcval->chks_hind = INVA_INDE_VALU;
    insert_cache_ext(plive->rivdb, file_name, pcval);
    //
fprintf(stderr, "rivfs _insert_ file:%s\n", file_name);
    return 0x00;
}

uint32 river_copy(char *new_name, char *exis_name, struct riv_live * plive) {
    struct cache_inde cvalue;
    //
    if (0 < cache_find(cvalue, plive->rivdb, new_name)) return 0x01;
    if (-1 == cache_find(cvalue, plive->rivdb, exis_name)) return 0x01;
    cvalue.ainde = repod_identi(plive->fpiden, cvalue.ainde, new_name);
    cvalue.chks_hind = repod_ckchks(plive->fpchks, cvalue.chks_hind);
    insert_cache(plive->rivdb, new_name, cvalue);
    //
fprintf(stderr, "rivfs copy file:%s\n", new_name);
    return 0x00;
}

uint32 river_move(char *new_name, char *exis_name, struct riv_live * plive) {
    struct cache_inde cvalue;
    //
    if (0 < cache_find(cvalue, plive->rivdb, new_name)) return 0x01;
    if (-1 == cache_find(cvalue, plive->rivdb, exis_name)) return 0x01;
    cache_remove(plive->rivdb, exis_name);
    insert_cache(plive->rivdb, new_name, cvalue);
    identi_file_update(plive->fpiden, cvalue.ainde, new_name);
    //
fprintf(stderr, "rivfs move file:%s\n", new_name);
    return 0x00;
}

uint32 river_remove(char *file_name, struct riv_live * plive) {
    struct cache_inde cvalue;
    //
    if (-1 == cache_find(cvalue, plive->rivdb, file_name)) return 0x01;
    cache_remove(plive->rivdb, file_name);
    remove_identi(plive->fpiden, cvalue.ainde);
    remove_ckchks(plive->fpchks, cvalue.chks_hind);
    //
fprintf(stderr, "rivfs remove file:%s\n", file_name);
    return 0x00;
}