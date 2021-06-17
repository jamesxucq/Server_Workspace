#include "common_header.h"

#include "third_party.h"
#include "chks_cache.h"
#include "chks_bzl.h"

int chksbzl_create(int backet_size) {
    // _LOG_TRACE("set action log ok!"); // disable by james 20120410
    _rivlive_hm_ = create_hashmap(del_riv_live, backet_size);
    if (!_rivlive_hm_) return ERR_FAULT;
    //
    return ERR_SUCCESS; //succ
}

int chksbzl_destroy() {
    if (_rivlive_hm_) {
        hashm_del(_rivlive_hm_);
        _rivlive_hm_ = NULL;
    }
    return ERR_SUCCESS; //succ
}

struct riv_live *get_chks_cache_bzl(int uid, char *location) {
    struct riv_live *plive;
    if (!(plive = (struct riv_live *) hashm_value(uid, _rivlive_hm_))) {
        if ((plive = new_riv_live(location)))
            hashm_insert(uid, plive, _rivlive_hm_);
        _LOG_TRACE("cache new rivlive");
    }
    return plive;
}

void erase_chks_cache_bzl(int uid) {
    struct riv_live *plive;
    if ((plive = (struct riv_live *) hashm_value(uid, _rivlive_hm_))) {
        _LOG_TRACE("cache delete rivlive");
        destory_riv_live(plive);
        hashm_remove(uid, _rivlive_hm_);
    }
}

// #define RIVE_INVA       0x0000
// #define RIVE_VERIFY     0x0001
// #define RIVE_CACHE      0x0002
static uint32(*file_sha1_tab[])(FILE *, char *, struct riv_live *) = {
    NULL,
    ren_file_sha1,
    riv_file_sha1
};

uint32 build_file_whole_bzl(FILE **fpchks, struct riv_live *plive, char *file_name, uint32 rive_verify) {
    _LOG_TRACE("build file whole chks");
    FILE *fpriv;
    uint32 ibval = 0x00;
    //
    if (!(fpriv = tmpfile64())) return 0x03;
    if ((ibval = file_sha1_tab[rive_verify](fpriv, file_name, plive)))
        fclose(fpriv);
    else *fpchks = fpriv;
    //
    return ibval;
}

static uint32(*file_chks_tab[])(FILE *, char *, uint32, struct riv_live *) = {
    NULL,
    ren_file_chks,
    riv_file_chks
};

uint32 build_file_simple_bzl(FILE **fpchks, struct riv_live *plive, char *file_name, uint64 offset, uint32 rive_verify) {
    _LOG_TRACE("build file simple chks");
    FILE *fpriv;
    uint32 ibval = 0x00;
    //
    if (!(fpriv = tmpfile64())) return 0x03;
    if ((ibval = file_chks_tab[rive_verify](fpriv, file_name, offset >> 22, plive)))
        fclose(fpriv);
    else *fpchks = fpriv;
    //
    return ibval;
}

static uint32(*cuk_chks_tab[])(FILE *, char *, uint32, struct riv_live *) = {
    NULL,
    ren_cuk_chks,
    riv_cuk_chks
};

uint32 build_chunk_simple_bzl(FILE **fpchks, struct riv_live *plive, char *file_name, uint64 offset, uint32 rive_verify) {
    _LOG_TRACE("build chunk simple chks");
    FILE *fpriv;
    //
    if (!(fpriv = tmpfile64())) return 0x03;
    if (cuk_chks_tab[rive_verify](fpriv, file_name, offset >> 22, plive)) {
        fclose(fpriv);
        return 0x01;
    }
    *fpchks = fpriv;
    //
    return 0x00;
}
