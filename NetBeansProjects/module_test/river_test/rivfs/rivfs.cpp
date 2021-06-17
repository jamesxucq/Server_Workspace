
#include <sys/stat.h>

#include "river_utility.h"
#include "river_cache.h"
#include "river_entity.h"
#include "creat_river.h"

#include "rivfs.h"

int riv_initial(struct riv_live *plive, char *location) {
    if (build_river_envirn(location)) return 0x01;
    strcpy(plive->location, location);
    //
    if (cache_initial(plive, location)) return 0x01;
    if (entity_initial(plive, location)) return 0x01;
    //
    /*
        struct fattb attb;
        attrib_read(&attb, plive->fpattb, 0);
        printf("attb.file_name:%s attb.sibling:%llu attb.time_stamp:%d\n", attb.file_name, attb.sibling, attb.time_stamp);
    
        struct ckchks cchks;
        cchks_inde_read(&cchks, plive->fpchks, 0, 0);
        printf("cchks.sibling:%llu cchks.time_stamp:%d\n", cchks.sibling, cchks.time_stamp);
     */
    //
    return 0x00;
}

int riv_final(struct riv_live *plive) {
    entity_final(plive);
    cache_final(plive);
    //
    return 0x00;
}

//
int _river_insert_(struct cache_index *pcval, char *file_name, struct riv_live * plive);
//
#define make_cache_exist(cvalue, path, plive, file_name) \
    POOL_PATH(path, plive->location, file_name); \
    struct stat64 statbuf; \
    if (stat64(path, &statbuf) < 0) return 0x01; \
    if (-1 == cache_find(cvalue, plive->rivdb, file_name)) \
        _river_insert_(&cvalue, file_name, plive); \
// printf("file_name:%s\n", file_name); 

#define ckchks_length_update(old_size, new_size) \
        uint32 old_clen = old_size / CHUNK_SIZE; \
        if (POW2N_MOD(old_size, CHUNK_SIZE)) old_clen++; \
        uint32 new_clen = new_size / CHUNK_SIZE; \
        if (POW2N_MOD(new_size, CHUNK_SIZE)) new_clen++;

int riv_file_sha1(unsigned char *pchks, char *file_name, struct riv_live *plive) {
    struct cache_index cvalue;
    char path[MAX_PATH * 3];
    struct fattb attb;

    make_cache_exist(cvalue, path, plive, file_name)
    attrib_read(&attb, plive->fpattb, cvalue.ainde);
    ckchks_length_update(attb.file_size, statbuf.st_size)
    if (new_clen != old_clen) {
        uint64 ind_pos = reset_ckchks_length(plive->fpchks, cvalue.cuks_hind, old_clen, new_clen);
        attrib_size_update(plive, attb, cvalue.ainde, statbuf.st_size)
        if (ind_pos != cvalue.cuks_hind) {
            cvalue.cuks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    if (statbuf.st_mtime != attb.time_stamp) {
        // printf("statbuf.st_mtime:%ld attb.time_stamp:%ld\n", statbuf.st_mtime, attb.time_stamp);
        file_chks_update(plive->fpchks, path, cvalue.cuks_hind, &statbuf);
        file_sha1_chks(attb.sha1_chks, plive->fpchks, cvalue.cuks_hind);
        attrib_stamp_update(plive, attb, cvalue.ainde, statbuf.st_mtime)
    }
    memcpy(pchks, attb.sha1_chks, SHA1_DIGEST_LEN);
    //
    return 0x00;
}

int riv_file_chks(struct riv_chks *pchks, char *file_name, uint32 cinde, struct riv_live *plive) {
    struct cache_index cvalue;
    char path[MAX_PATH * 3];
    struct fattb attb;
    //
    make_cache_exist(cvalue, path, plive, file_name)
    attrib_read(&attb, plive->fpattb, cvalue.ainde);
    uint64 ind_pos;
    ckchks_length_update(attb.file_size, statbuf.st_size)
    if (new_clen != old_clen) {
        ind_pos = reset_ckchks_length(plive->fpchks, cvalue.cuks_hind, old_clen, new_clen);
        attrib_size_update(plive, attb, cvalue.ainde, statbuf.st_size)
        if (ind_pos != cvalue.cuks_hind) {
            cvalue.cuks_hind = ind_pos;
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    ind_pos = file_chunk_update(plive->fpchks, path, cvalue.cuks_hind, cinde, &statbuf);
    file_rive_chks(pchks, plive->fpchks, ind_pos);
    //
    return 0x00;
}

int riv_cuk_chks(struct riv_chks *pchks, char *file_name, uint32 cinde, struct riv_live *plive) {
    struct cache_index cvalue;
    char path[MAX_PATH * 3];
    struct fattb attb;
    //
    make_cache_exist(cvalue, path, plive, file_name)
    // printf("cvalue.ainde:%llu cvalue.cuks_hind:%llu\n", cvalue.ainde, cvalue.cuks_hind);
    attrib_read(&attb, plive->fpattb, cvalue.ainde);
    uint64 ind_pos;
    // printf("path:%s statbuf.st_size:%llu statbuf.st_mtime:%d\n", path, statbuf.st_size, statbuf.st_mtime);
    // printf("attb.file_name:%s attb.file_size:%llu attb.time_stamp:%d\n", attb.file_name, attb.file_size, attb.time_stamp);
    ckchks_length_update(attb.file_size, statbuf.st_size)
    if (new_clen != old_clen) {
        // printf("statbuf.st_size:%llu attb.file_size:%llu\n", statbuf.st_size, attb.file_size);
        ind_pos = reset_ckchks_length(plive->fpchks, cvalue.cuks_hind, old_clen, new_clen);
        // printf("cchks inde numbe:%d ind_pos:%llu\n", cchks_inde_numbe(plive->fpchks, ind_pos));
        attrib_size_update(plive, attb, cvalue.ainde, statbuf.st_size)
        if (ind_pos != cvalue.cuks_hind) {
            cvalue.cuks_hind = ind_pos;
            // printf("new cvalue.cuks_hind:%llu\n", cvalue.cuks_hind);
            cache_update(plive->rivdb, file_name, cvalue);
        }
    }
    ind_pos = chunk_chks_update(plive->fpchks, path, cvalue.cuks_hind, cinde, &statbuf);
    // printf("ind_pos:%d\n", ind_pos);
    chunk_rive_chks(pchks, plive->fpchks, ind_pos, cinde);
    //
    return 0x00;
}

int river_insert(char *file_name, struct riv_live * plive) {
    struct cache_index cvalue;
    //
    if (0 < cache_find(cvalue, plive->rivdb, file_name)) return 0x01;
    // printf("in river insert! file_name:%s\n", file_name);
    cvalue.ainde = init_attrib(plive->fpattb, file_name);
    cvalue.cuks_hind = INVALID_INDEX_VALUE; // init_cchks(plive->fpchks);
    // printf("cvalue.ainde:%llu cvalue.cuks_hind:%llu\n", cvalue.ainde, cvalue.cuks_hind);
    insert_cache(plive->rivdb, file_name, cvalue);
    // for test
    //    memset(&cvalue, 0, sizeof (struct cache_index));
    //    cache_find(cvalue, plive->rivdb, file_name);
    //    printf("cvalue.ainde:%llu cvalue.cuks_hind:%llu\n", cvalue.ainde, cvalue.cuks_hind);
    /*
        struct fattb attb;
        attrib_read(&attb, plive->fpattb, cvalue.ainde);
        printf("attb.file_name:%s attb.file_size:%llu attb.time_stamp:%d\n", attb.file_name, attb.file_size, attb.time_stamp);
     */
    //
    return 0x00;
}

int _river_insert_(struct cache_index *pcval, char *file_name, struct riv_live * plive) {
    // printf("_ in river insert! _ file_name:%s\n", file_name);
    pcval->ainde = init_attrib(plive->fpattb, file_name);
    pcval->cuks_hind = INVALID_INDEX_VALUE; // init_cchks(plive->fpchks);
    // printf("pcval->ainde:%llu pcval->cuks_hind:%llu\n", pcval->ainde, pcval->cuks_hind);
    insert_cache_ext(plive->rivdb, file_name, pcval);
    return 0x00;
}

int river_copy(char *new_name, char *exis_name, struct riv_live * plive) {
    struct cache_index cvalue;
    //
    if (0 < cache_find(cvalue, plive->rivdb, new_name)) return 0x01;
    if (-1 == cache_find(cvalue, plive->rivdb, exis_name)) return 0x01;
    //    uint64 x = cvalue.ainde; // for test
    //    uint64 y = cvalue.cuks_hind;
    cvalue.ainde = repod_attrib(plive->fpattb, cvalue.ainde, new_name);
    cvalue.cuks_hind = repod_ckchks(plive->fpchks, cvalue.cuks_hind);
    //    printf("cvalue.ainde:%llu cvalue.cuks_hind:%llu\n", x, y);
    //    printf("cvalue.ainde:%llu cvalue.cuks_hind:%llu\n", cvalue.ainde, cvalue.cuks_hind);
    insert_cache(plive->rivdb, new_name, cvalue);
    // for test
    /*
        cache_find(cvalue, plive->rivdb, cval, exis_name)
        printf("cval:%d\n", cval);
        cache_find(cvalue, plive->rivdb, cval, new_name)
        printf("cval:%d\n", cval);
        struct fattb attb;
        attrib_read(&attb, plive->fpattb, x);
        printf("attb.file_name:%s attb.file_size:%lld attb.time_stamp:%d\n", attb.file_name, attb.file_size, attb.time_stamp);
        attrib_read(&attb, plive->fpattb, cvalue.ainde);
        printf("attb.file_name:%s attb.file_size:%lld attb.time_stamp:%d\n", attb.file_name, attb.file_size, attb.time_stamp);
        cchks_inde_numbe(plive->fpchks, y);
        cchks_inde_numbe(plive->fpchks, cvalue.cuks_hind);
     */
    //
    return 0x00;
}

int river_move(char *new_name, char *exis_name, struct riv_live * plive) {
    struct cache_index cvalue;
    //
    if (0 < cache_find(cvalue, plive->rivdb, new_name)) return 0x01;
    if (-1 == cache_find(cvalue, plive->rivdb, exis_name)) return 0x01;
    cache_remove(plive->rivdb, exis_name);
    insert_cache(plive->rivdb, new_name, cvalue);
    attrib_file_update(plive->fpattb, cvalue.ainde, new_name);
    // for test
    /*
        cache_find(cvalue, plive->rivdb, cval, exis_name)
        printf("cval:%d\n", cval);
        cache_find(cvalue, plive->rivdb, cval, new_name)
        printf("cval:%d\n", cval);
        struct fattb attb;
        attrib_read(&attb, plive->fpattb, cvalue.ainde);
        printf("attb.file_name:%s attb.file_size:%lld attb.time_stamp:%d\n", attb.file_name, attb.file_size, attb.time_stamp);
     */
    //
    return 0x00;
}

int river_remove(char *file_name, struct riv_live * plive) {
    struct cache_index cvalue;
    //
    if (-1 == cache_find(cvalue, plive->rivdb, file_name)) return 0x01;
    cache_remove(plive->rivdb, file_name);
    remove_attrib(plive->fpattb, cvalue.ainde);
    remove_ckchks(plive->fpchks, cvalue.cuks_hind);
    // for test
    /*
        cache_find(cvalue, plive->rivdb, cval, file_name)
        printf("cval:%d\n", cval);
        attrib_idle_link(plive->fpattb);
        cchks_idle_link(plive->fpchks);
     */
    //
    return 0x00;
}