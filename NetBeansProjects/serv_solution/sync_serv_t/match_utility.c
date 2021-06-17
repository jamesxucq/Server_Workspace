#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "third_party.h"
#include "match_utility.h"

//
int file_chks_matcher(match_listh mlisth, hashtable *htable, struct dmap64 *buffer, uint64 file_size, uint64 foset) {
    struct local_match *alloc_local;
    struct file_matcher *matcher;
    int last_toke, cure_toke, end_toke;
    end_toke = (int)(file_size-foset) - BLOCK_SIZE;
    last_toke = cure_toke = 0x00;
    //
    seaenv senv;
    senv.mode = INITI_ADLER_CHECK;
    int find_inde = -1;
    while (end_toke >= cure_toke) {
        // bFine 1 find 0  not find
        find_inde = hash_search(htable, dmap_ptr64(buffer, foset + cure_toke, BLOCK_SIZE), &senv);
        if (0x00 <= find_inde) { // found the same block.
            if ((cure_toke - last_toke)) {
                uint32 molen = (uint32) (cure_toke - last_toke);
// _LOG_DEBUG("end_toke:%u, cure_toke:%u molen:%u", end_toke, cure_toke, molen);
                alloc_local = add_modify_data(mlisth, molen);
                if (!alloc_local) return ERR_FAULT;
                //
                alloc_local->matcher->offset = last_toke;
                alloc_local->matcher->inde_len = molen;
                memcpy(alloc_local->buffer, dmap_ptr64(buffer, foset + last_toke, molen), molen);
            }
            //           
            matcher = add_chks_match(mlisth);
            if (!matcher) return ERR_FAULT;
            matcher->offset = cure_toke;
            matcher->inde_len = find_inde / BLOCK_SIZE;
            //
            cure_toke += BLOCK_SIZE;
            last_toke = cure_toke;
            senv.mode = INITI_ADLER_CHECK;
        } else { // not found the same block.
            if ((BLOCK_SIZE-0x01) == (cure_toke - last_toke)) {
// _LOG_DEBUG("end_toke:%u, cure_toke:%u molen:%u", end_toke, cure_toke, BLOCK_SIZE);
                alloc_local = add_modify_data(mlisth, BLOCK_SIZE);
                if (!alloc_local) return ERR_FAULT;
                alloc_local->matcher->offset = last_toke;
                alloc_local->matcher->inde_len = BLOCK_SIZE;
                memcpy(alloc_local->buffer, dmap_ptr64(buffer, foset + last_toke, BLOCK_SIZE), BLOCK_SIZE);
                //
                cure_toke++;
                last_toke = cure_toke;
            } else { // not found the same block.
                cure_toke++;
                senv.mode = ROLLING_ADLER_CHECK;
            }
        }
    } // while
    // match the tail
    if((end_toke+BLOCK_SIZE) > cure_toke) {
// _LOG_DEBUG("the last not match data."); // disable by james 20120410
        uint32 molen = (end_toke+BLOCK_SIZE) - last_toke;
        alloc_local = add_modify_data(mlisth, molen);
        if (!alloc_local) return -1;
// _LOG_DEBUG("local ose:%u molen:%d", last_toke, molen);
        alloc_local->matcher->offset = last_toke;
        alloc_local->matcher->inde_len = molen;
        memcpy(alloc_local->buffer, dmap_ptr64(buffer, foset + last_toke, molen), molen);  
    }
    //
    return ERR_SUCCESS;
}

int block_match_file(match_listh mlisth, hashtable *htable, char *local_file, uint64 foset) {
    // _LOG_DEBUG("match file sums. new file is:%s", local_file); // disable by james 20120410
    int local_fide = open64(local_file, O_RDWR | O_CREAT, 0777);
    if (-1 == local_fide) {
        _LOG_ERROR("open file error! %s", local_file);
        return ERR_FAULT;
    }
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, local_fide)
    if(!file_size || (foset>=file_size)) {
        *mlisth = NULL;
        close(local_fide);
        return ERR_SUCCESS;
    }
    //
    struct dmap64 *buffer = (struct dmap64 *) dmap_file64(local_fide, file_size, MAX_MAP_SIZE, BLOCK_SIZE);
    if (!buffer) {
        close(local_fide);
        return ERR_FAULT;
    }
    //
    if (file_chks_matcher(mlisth, htable, buffer, file_size, foset)) {
        close(local_fide);
        return ERR_FAULT;
    }
    //
    // _LOG_DEBUG("match file data OK!");
    undmap_file64(buffer);
    close(local_fide);
    return ERR_SUCCESS;
}

//
int chunk_chks_matcher(match_listh mlisth, hashtable *htable, struct dmap64 *buffer, uint64 coset) {
    struct local_match * alloc_local;
    struct file_matcher * matcher;
    uint32 last_toke, cure_toke, end_toke;
    end_toke = CHUNK_SIZE - BLOCK_SIZE;
    last_toke = cure_toke = 0x00;
    //
    seaenv senv;
    senv.mode = INITI_ADLER_CHECK;
    int find_inde = -1;
    while (end_toke >= cure_toke) {
        // bFine 1 find 0  not find
        find_inde = hash_search(htable, dmap_ptr64(buffer, coset + cure_toke, BLOCK_SIZE), &senv);
        if (0x00 <= find_inde) { // found the same block.         
            if ((cure_toke - last_toke)) {
                uint32 molen = (uint32) (cure_toke - last_toke);
                alloc_local = add_modify_data(mlisth, molen);
                if (!alloc_local) return ERR_FAULT;
                //
                alloc_local->matcher->offset = last_toke;
                alloc_local->matcher->inde_len = molen;
                memcpy(alloc_local->buffer, dmap_ptr64(buffer, coset + last_toke, molen), molen);
            }
            //           
            matcher = add_chks_match(mlisth);
            if (!matcher) return ERR_FAULT;
            matcher->offset = cure_toke;
            matcher->inde_len = find_inde / BLOCK_SIZE;
            //
            cure_toke += BLOCK_SIZE;
            last_toke = cure_toke;
            senv.mode = INITI_ADLER_CHECK;
        } else { // not found the same block.
            if ((BLOCK_SIZE-0x01) == (cure_toke - last_toke)) {
                alloc_local = add_modify_data(mlisth, BLOCK_SIZE);
                if (!alloc_local) return ERR_FAULT;
                alloc_local->matcher->offset = last_toke;
                alloc_local->matcher->inde_len = BLOCK_SIZE;
                memcpy(alloc_local->buffer, dmap_ptr64(buffer, coset + last_toke, BLOCK_SIZE), BLOCK_SIZE);
                //
                cure_toke++;
                last_toke = cure_toke;
            } else { // not found the same block.
                cure_toke++;
                senv.mode = ROLLING_ADLER_CHECK;
            }
        }
        //
    }
    //
    if (CHUNK_SIZE > cure_toke) {
// _LOG_DEBUG("the last not match data.");
        unsigned int molen = CHUNK_SIZE - last_toke;
        alloc_local = add_modify_data(mlisth, molen);
        if (!alloc_local) return -1;
// _LOG_DEBUG("local ose:%u molen:%d", last_toke, molen);
        alloc_local->matcher->offset = last_toke;
        alloc_local->matcher->inde_len = molen;
        memcpy(alloc_local->buffer, dmap_ptr64(buffer, coset + last_toke, molen), molen);
    }
    //
    return ERR_SUCCESS;
}

int block_match_chunk(match_listh mlisth, hashtable *htable, char *local_file, uint64 coset) {
    // _LOG_DEBUG("match chunk sums. new file is:%s", local_file); // disable by james 20120410
    int local_fide = open64(local_file, O_RDWR | O_CREAT, 0777);
    if (-1 == local_fide) {
        _LOG_ERROR("open file error! %s", local_file);
        return ERR_FAULT;
    }
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, local_fide)
    if ((coset + CHUNK_SIZE) > file_size) {
        close(local_fide);
        return ERR_FAULT;    
    }
    //
    struct dmap64 *buffer = (struct dmap64 *) dmap_file64(local_fide, file_size, MAX_MAP_SIZE, BLOCK_SIZE);
    if (!buffer) {
        close(local_fide);
        return ERR_FAULT;
    }
    //
    if (chunk_chks_matcher(mlisth, htable, buffer, coset)) {
        close(local_fide);
        return ERR_FAULT;
    }
    //
    undmap_file64(buffer);
    close(local_fide);
    return ERR_SUCCESS;
}

static int (*block_match_table[])(match_listh, hashtable *, char *, uint64) = {
    NULL,
    block_match_file,
    block_match_chunk
};

int match_chks_by_memory(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    struct complex_chks *pcchks;
    struct match_list *malt = NULL;
    hashtable *htable = NULL;
    char file_name[MAX_PATH * 3];
    //
    POOL_PATH(file_name, ldata->location, ival->file_name);
    // _LOG_INFO("match sums file name:%s", file_name);
    pcchks = (struct complex_chks *) sctrl->recv_buffer;
    int block_tatol = ival->content_length / sizeof ( struct complex_chks);
    if (!(htable = create_none_ht(MATCH_BACKET_SIZE))) {
_LOG_WARN("create hash table failed.");
        return ERR_FAULT;
    }
    if (build_hash_table(htable, pcchks, block_tatol)) {
        hash_del(htable);
        return ERR_FAULT;
    }
    //
    if (block_match_table[ival->range_type](&malt, htable, file_name, ival->offset)) {
        delete_mlist(malt);
        hash_del(htable);
        return ERR_FAULT;
    }
    if (htable) hash_del(htable);
    sctrl->send_buffer = (BYTE *) malt;
    //
    return ERR_SUCCESS;
}

int match_zero_file(match_listh mlisth, char *local_file, uint64 foset) {
// _LOG_DEBUG("match zero file. new file is:%s", local_file);
    int local_fide = open64(local_file, O_RDWR | O_CREAT, 0777);
    if (-1 == local_fide) {
        _LOG_ERROR("open file error! %s", local_file);
        return ERR_FAULT;
    }
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, local_fide)
    if(!file_size) {
        *mlisth = NULL;
        close(local_fide);
        return ERR_SUCCESS;
    }
    //
    struct dmap64 *buffer = (struct dmap64 *) dmap_file64(local_fide, file_size, MAX_MAP_SIZE, BLOCK_SIZE);
    if (!buffer) {
        close(local_fide);
        return ERR_FAULT;
    }
    //
    struct local_match *alloc_local;
    uint32 vleng = (uint32) (file_size - foset);
    uint32 block_tatol = vleng / BLOCK_SIZE;
    uint32 posit = 0, inde;
// _LOG_DEBUG("vleng:%d block_tatol:%d", vleng, block_tatol);
    for (inde = 0; inde < block_tatol; inde++) {
        alloc_local = add_modify_data(mlisth, BLOCK_SIZE);
        if (!alloc_local) {
            undmap_file64(buffer);
            close(local_fide);
            return ERR_FAULT;
        }
// _LOG_DEBUG("molen:%d file_size:%d posit:%d", BLOCK_SIZE, file_size, posit);
        alloc_local->matcher->offset = posit;
        alloc_local->matcher->inde_len = BLOCK_SIZE;
        memcpy(alloc_local->buffer, dmap_ptr64(buffer, foset + posit, BLOCK_SIZE), BLOCK_SIZE);
        posit += BLOCK_SIZE;
    }
    uint32 tail_size = POW2N_MOD(vleng, BLOCK_SIZE);
    if (tail_size) {
        alloc_local = add_modify_data(mlisth, tail_size);
        if (!alloc_local) {
            undmap_file64(buffer);
            close(local_fide);
            return ERR_FAULT;
        }
// _LOG_DEBUG("molen:%d file_size:%d posit:%d", tail_size, file_size, posit);
        alloc_local->matcher->offset = posit;
        alloc_local->matcher->inde_len = tail_size;
        memcpy(alloc_local->buffer, dmap_ptr64(buffer, foset + posit, tail_size), tail_size);
    }
    //
    undmap_file64(buffer);
    close(local_fide);
    return ERR_SUCCESS;
}

int match_zero_chunk(match_listh mlisth, char *local_file, uint64 coset) {
// _LOG_DEBUG("match zero chunk. new file is:%s", local_file);
    int local_fide = open64(local_file, O_RDWR | O_CREAT, 0777);
    if (-1 == local_fide) {
        _LOG_ERROR("open file error! %s", local_file);
        return ERR_FAULT;
    }
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, local_fide)
    if (file_size < (coset + CHUNK_SIZE)) {
        close(local_fide);
        return ERR_FAULT;
    }
    //
    struct dmap64 *buffer = (struct dmap64 *) dmap_file64(local_fide, file_size, MAX_MAP_SIZE, BLOCK_SIZE);
    if (!buffer) {
        close(local_fide);
        return ERR_FAULT;
    }
    //
    struct local_match *alloc_local;
    uint32 block_tatol = CHUNK_SIZE / BLOCK_SIZE;
    uint32 posit = 0, inde;
    for (inde = 0; inde < block_tatol; inde++) {
        alloc_local = add_modify_data(mlisth, BLOCK_SIZE);
        if (!alloc_local) {
            undmap_file64(buffer);
            close(local_fide);
            return ERR_FAULT;
        }
        alloc_local->matcher->offset = posit;
        alloc_local->matcher->inde_len = BLOCK_SIZE;
        memcpy(alloc_local->buffer, dmap_ptr64(buffer, coset + posit, BLOCK_SIZE), BLOCK_SIZE);
        posit += BLOCK_SIZE;
    }
    //
    undmap_file64(buffer);
    close(local_fide);
    return ERR_SUCCESS;
}

static int (*zero_match_table[])(match_listh, char *, uint64) = {
    NULL,
    match_zero_file,
    match_zero_chunk
};

int match_chks_zero(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
    struct match_list *malt = NULL;
    char file_name[MAX_PATH * 3];
    //
    POOL_PATH(file_name, ldata->location, ival->file_name);
// _LOG_DEBUG("match_chks_zero. file_name:%s offset:%lld", file_name, ival->offset);
    if (zero_match_table[ival->range_type](&malt, file_name, ival->offset)) {
        return ERR_FAULT;
    }
    sctrl->send_buffer = (BYTE *) malt;
    //
    return ERR_SUCCESS;
}

int match_chks_by_file(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival) {
// _LOG_DEBUG("ival->content_length:%d", ival->content_length);
    if (ival->content_length) {
        sctrl->recv_buffer = (void *) malloc(ival->content_length);
        if (!sctrl->recv_buffer) {
_LOG_ERROR("match file malloc error.");
            return ERR_FAULT;
        }
        if (fseeko64(sctrl->req_data, 0, SEEK_SET)) return ERR_FAULT;
        int rlen = fread(sctrl->recv_buffer, ival->content_length, 1, sctrl->req_data);
        if (1 != rlen) {
            free(sctrl->recv_buffer);
            return ERR_FAULT;
        }
        if (match_chks_by_memory(sctrl, ldata, ival)) {
            free(sctrl->recv_buffer);
_LOG_WARN("match chks by memory error.");
            return ERR_FAULT;
        }
        if (sctrl->recv_buffer) free(sctrl->recv_buffer);
    } else {
        if (match_chks_zero(sctrl, ldata, ival)) {
_LOG_WARN("match chks zero error.");
            return ERR_FAULT;
        }
    }
    //
    return ERR_SUCCESS;
}

struct file_matcher *add_chks_match(match_listh mlisth) {
    struct match_list *mlistmp = NULL;
    struct file_matcher *matcher = NULL;
    //
    mlistmp = (struct match_list *) malloc(sizeof (struct match_list));
    if (!mlistmp) return NULL;
    memset(mlistmp, '\0', sizeof (struct match_list));
    //
    matcher = (struct file_matcher *) malloc(sizeof (struct file_matcher));
    if (!matcher) {
        free(mlistmp);
        return NULL;
    }
    memset(matcher, '\0', sizeof (struct file_matcher));
    //
    matcher->match_type = CHECK_SUM;
    mlistmp->match.matcher = matcher;
    MLIST_APPEND(*mlisth, mlistmp);
    //
    return matcher;
}

struct local_match *add_modify_data(match_listh mlisth, int length) {
    struct match_list *mlistmp = NULL;
    struct file_matcher *matcher = NULL;
    //
    mlistmp = (struct match_list *) malloc(sizeof (struct match_list));
    if (!mlistmp) return NULL;
    memset(mlistmp, '\0', sizeof (struct match_list));
    //
    matcher = (struct file_matcher *) malloc(sizeof (struct file_matcher));
    if (!matcher) {
        free(mlistmp);
        return NULL;
    } 
    memset(matcher, '\0', sizeof (struct file_matcher));
    //
    unsigned char *buffer = (unsigned char *) malloc(length);
    if (!buffer) {
        free(mlistmp);
        free(matcher);
        return NULL;
    }
    memset(buffer, '\0', sizeof (length));
    //
    matcher->match_type = CONEN_MODI;
    mlistmp->match.matcher = matcher;
    mlistmp->match.buffer = buffer;
    MLIST_APPEND(*mlisth, mlistmp);
    //
    return &mlistmp->match;
}

#define DELETE_MATCH(pm) if ((pm)) free(pm); pm = NULL;

void delete_mlist(struct match_list *mlist) {
    struct match_list *pml = NULL, *del_mlist = NULL;
    //
    pml = mlist;
    while (pml) {
        del_mlist = pml;
        pml = pml->next;
        if (del_mlist) {
            if (del_mlist->match.matcher)
                free(del_mlist->match.matcher);
            if (del_mlist->match.buffer) free(del_mlist->match.buffer);
            free(del_mlist);
        }
    }
}

int flush_match_file(struct seion_ctrl *sctrl, struct inte_value *ival) {
    struct match_list *malt, *item;
    malt = (struct match_list *) sctrl->send_buffer;
    if (fseeko64(sctrl->res_data, 0, SEEK_SET))
        return ERR_FAULT;    
    //
    MLIST_FOREACH(malt, item) {
        fwrite(item->match.matcher, sizeof (struct file_matcher), 1, sctrl->res_data);
        if (CONEN_MODI & item->match.matcher->match_type)
            fwrite(item->match.buffer, item->match.matcher->inde_len, 1, sctrl->res_data);
// log_binary_code(item->match.buffer, item->match.matcher->inde_len);
// _LOG_DEBUG("match_type:%d offset:%u inde_len:%u", item->match.matcher->match_type, item->match.matcher->offset, item->match.matcher->inde_len);
    }
// exit(0);
    delete_mlist(malt);
    FILE_SIZE_STREAM(ival->content_length, sctrl->res_data)
// _LOG_DEBUG("ival->content_length:%d", ival->content_length);
    if (fseeko64(sctrl->res_data, 0, SEEK_SET))
        return ERR_FAULT;    
// _LOG_DEBUG("flush file data OK!"); // disable by james 20120410
    return ERR_SUCCESS;
}
