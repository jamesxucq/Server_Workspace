#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "file_utility.h"
#include "build_utility.h"

#define FAIL_EXIT(DATA, RENO) { fclose(DATA); return RENO; }
#define FAULT_EXIT(INDE, DATA, RENO) { fclose(INDE); fclose(DATA); return RENO; }

int build_file_content_memory(struct file_builder *builder, BYTE *ptrm) {
    FILE *local_stre = NULL;
    struct file_matcher *matcher;
    int match_tatol;
    //
    _LOG_DEBUG("build file content memoory.");
    if (!builder || !ptrm) return ERR_FAULT;
    uint64 foset = builder->offset;
    //
    if (!(local_stre = fopen64(builder->file_name, "rb+"))) {
        _LOG_ERROR("not open file:%s", builder->file_name);
        return ERR_FAULT;
    }
    FILE *bakup_stre = creat_bakup_file(local_stre, foset);
    if (!bakup_stre) FAIL_EXIT(local_stre, ERR_FAULT)
        //
        unsigned char buffer[BLOCK_SIZE * 0x02];
    int wlen, rlen;
    uint32 inde;
    for (inde = 0; builder->length > inde; inde += match_tatol) {
        match_tatol = sizeof (struct file_matcher);
        matcher = (struct file_matcher *) (ptrm + inde);
        if (CHECK_SUM & matcher->match_type) {
            // _LOG_DEBUG("check sum data! offset:%d inde:%d", matcher->offset, matcher->inde_len); // disable by james 20120410
            fseeko64(bakup_stre, matcher->inde_len * BLOCK_SIZE, SEEK_SET);
            rlen = fread(buffer, 1, BLOCK_SIZE, bakup_stre);
            if (rlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                //
                fseeko64(local_stre, matcher->offset + foset, SEEK_SET);
            wlen = fwrite(buffer, 1, BLOCK_SIZE, local_stre);
            if (wlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
            } else if (CONEN_MODI & matcher->match_type) {
            // _LOG_DEBUG("modify data! offset:%d length:%d", matcher->offset, matcher->inde_len); // disable by james 20120410
            fseeko64(local_stre, matcher->offset + foset, SEEK_SET);
            wlen = fwrite(matcher + MATCH_INFORMATION_END, 1, matcher->inde_len, local_stre);
            if (wlen != matcher->inde_len) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                // log_binary_code(matcher + MATCH_INFORMATION_END, matcher->inde_len);
                match_tatol += matcher->inde_len;
        }
    }
    fclose(bakup_stre);
    fclose(local_stre);
    //
    return ERR_SUCCESS;
}

int build_chunk_content_memory(struct file_builder *builder, BYTE *ptrm) {
    FILE *local_stre = NULL;
    struct file_matcher *matcher;
    int match_tatol;
    //
    _LOG_DEBUG("build chunk content memory.");
    if (!builder || !ptrm) return ERR_FAULT;
    if (!(local_stre = fopen64(builder->file_name, "rb+"))) {
        _LOG_ERROR("not open file:%s", builder->file_name);
        return ERR_FAULT;
    }
    FILE *bakup_stre = creat_bakup_chunk(local_stre, builder->offset);
    if (!bakup_stre) FAIL_EXIT(local_stre, ERR_FAULT)
        //
        uint64 coset = builder->offset;
    unsigned char buffer[BLOCK_SIZE * 0x02];
    int wlen, rlen;
    uint32 inde;
    for (inde = 0; inde < builder->length; inde += match_tatol) {
        match_tatol = sizeof (struct file_matcher);
        matcher = (struct file_matcher *) (ptrm + inde);
        if (CHECK_SUM & matcher->match_type) {
            //
            fseeko64(bakup_stre, matcher->inde_len * BLOCK_SIZE, SEEK_SET);
            rlen = fread(buffer, 1, BLOCK_SIZE, bakup_stre);
            if (rlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                //
                fseeko64(local_stre, matcher->offset + coset, SEEK_SET);
            wlen = fwrite(buffer, 1, BLOCK_SIZE, local_stre);
            if (wlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
            } else if (CONEN_MODI & matcher->match_type) {
            fseeko64(local_stre, matcher->offset + coset, SEEK_SET);
            wlen = fwrite(matcher + MATCH_INFORMATION_END, 1, matcher->inde_len, local_stre);
            if (wlen != matcher->inde_len) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                //
                match_tatol += matcher->inde_len;
        }
    }
    fclose(bakup_stre);
    fclose(local_stre);
    //
    return ERR_SUCCESS;
}

int (*build_content_by_memory[])(struct file_builder *, BYTE *) = {
    NULL,
    build_file_content_memory,
    build_chunk_content_memory
};

//

int build_file_content_file(struct file_builder *builder, FILE *build_stre) {
    FILE *local_stre = NULL;
    struct file_matcher matcher;
    int match_tatol;
    //
    _LOG_DEBUG("build file content file.");
    if (!builder || !build_stre) return ERR_FAULT;
    if (fseeko64(build_stre, 0, SEEK_SET)) return ERR_FAULT;
    //
    uint64 foset = builder->offset;
    if (!(local_stre = fopen64(builder->file_name, "rb+"))) {
        _LOG_ERROR("not open file:%s", builder->file_name);
        return ERR_FAULT;
    }
    FILE *bakup_stre = creat_bakup_file(local_stre, foset);
    if (!bakup_stre) FAIL_EXIT(local_stre, ERR_FAULT)
        //
        unsigned char buffer[BLOCK_SIZE * 0x02];
    int wlen, rlen;
    uint32 inde;
    for (inde = 0; inde < builder->length; inde += match_tatol) {
        match_tatol = sizeof (struct file_matcher);
        rlen = fread(&matcher, sizeof (struct file_matcher), 1, build_stre);
        if (1 != rlen) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
            if (CHECK_SUM & matcher.match_type) {
                // _LOG_DEBUG("check sum data! offset:%d inde:%d", matcher.offset, matcher.inde_len); // disable by james 20120410
                fseeko64(bakup_stre, matcher.inde_len * BLOCK_SIZE, SEEK_SET);
                rlen = fread(buffer, 1, BLOCK_SIZE, bakup_stre);
                if (rlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                    //
                    fseeko64(local_stre, matcher.offset + foset, SEEK_SET);
                wlen = fwrite(buffer, 1, BLOCK_SIZE, local_stre);
                if (wlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                } else if (CONEN_MODI & matcher.match_type) {
                rlen = fread(buffer, matcher.inde_len, 1, build_stre);
                if (1 != rlen) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                    // _LOG_DEBUG("modify data! offset:%d length:%d", matcher.offset, matcher.inde_len); // disable by james 20120410
                    fseeko64(local_stre, matcher.offset + foset, SEEK_SET);
                wlen = fwrite(buffer, 1, matcher.inde_len, local_stre);
                if (wlen != matcher.inde_len) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                    // log_binary_code(buffer, matcher.inde_len);
                    match_tatol += matcher.inde_len;
            }
    }
    fclose(bakup_stre);
    fclose(local_stre);
    //
    _LOG_DEBUG("build file content file finished.");
    return ERR_SUCCESS;
}

int (*build_content_by_file[])(struct file_builder *, FILE *) = {
    NULL,
    build_file_content_file,
    build_chunk_content_file
};

//

int build_chunk_content_file(struct file_builder *builder, FILE *build_stre) {
    FILE *local_stre = NULL;
    struct file_matcher matcher;
    int match_tatol;
    //
    _LOG_DEBUG("build chunk content file.");
    if (!builder || !build_stre) return ERR_FAULT;
    if (fseeko64(build_stre, 0, SEEK_SET)) return ERR_FAULT;
    //
    if (!(local_stre = fopen64(builder->file_name, "rb+"))) {
        _LOG_ERROR("not open file:%s", builder->file_name);
        return ERR_FAULT;
    }
    FILE *bakup_stre = creat_bakup_chunk(local_stre, builder->offset);
    if (!bakup_stre) FAIL_EXIT(local_stre, ERR_FAULT)
        //
        uint64 coset = builder->offset;
    unsigned char buffer[BLOCK_SIZE * 0x02];
    int wlen, rlen;
    uint32 inde;
    for (inde = 0; inde < builder->length; inde += match_tatol) {
        match_tatol = sizeof (struct file_matcher);
        rlen = fread(&matcher, sizeof (struct file_matcher), 1, build_stre);
        if (1 != rlen) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
            if (CHECK_SUM & matcher.match_type) {
                fseeko64(bakup_stre, matcher.inde_len * BLOCK_SIZE, SEEK_SET);
                rlen = fread(buffer, 1, BLOCK_SIZE, bakup_stre);
                if (rlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                    //
                    fseeko64(local_stre, matcher.offset + coset, SEEK_SET);
                wlen = fwrite(buffer, 1, BLOCK_SIZE, local_stre);
                if (wlen != BLOCK_SIZE) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                } else if (CONEN_MODI & matcher.match_type) {
                rlen = fread(buffer, matcher.inde_len, 1, build_stre);
                if (1 != rlen) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                    //
                    fseeko64(local_stre, matcher.offset + coset, SEEK_SET);
                wlen = fwrite(buffer, 1, matcher.inde_len, local_stre);
                if (wlen != matcher.inde_len) FAULT_EXIT(bakup_stre, local_stre, ERR_FAULT)
                    //
                    match_tatol += matcher.inde_len;
            }
    }
    fclose(bakup_stre);
    fclose(local_stre);
    //
    _LOG_DEBUG("build chunk content file finished.");
    return ERR_SUCCESS;
}
