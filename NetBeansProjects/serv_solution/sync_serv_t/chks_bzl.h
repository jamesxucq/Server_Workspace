/* 
 * File:   chks_bzl.h
 * Author: Administrator
 */

#ifndef CHKS_BZL_H
#define	CHKS_BZL_H

// #ifdef	__cplusplus
// extern "C" {
// #endif

#include "rivfs/rivfs.h"
#include "common_header.h"
//    
int chksbzl_create(int backet_size);
int chksbzl_destroy();
//
struct riv_live *get_chks_cache_bzl(int uid, char *location);
void erase_chks_cache_bzl(int uid);
//
#define riv_insert_bzl(file_name, plive)        river_insert(file_name, plive)
#define riv_copy_bzl(new_name, exis_name, plive)        river_copy(new_name, exis_name, plive)
#define riv_move_bzl(new_name, exis_name, plive)        river_move(new_name, exis_name, plive)
#define riv_remove_bzl(file_name, plive)        river_remove(file_name, plive)
//
#define RIVE_INVA       0x0000
#define RIVE_VERIFY     0x0001
#define RIVE_CACHE      0x0002
uint32 build_file_whole_bzl(FILE **fpchks, struct riv_live *plive, char *file_name, uint32 rive_verify);
uint32 build_file_simple_bzl(FILE **fpchks, struct riv_live *plive, char *file_name, uint64 offset, uint32 rive_verify);
uint32 build_chunk_simple_bzl(FILE **fpchks, struct riv_live *plive, char *file_name, uint64 offset, uint32 rive_verify);
#define RIV_CLOSE_BZL(fptr)     _RIV_CLOSE_(fptr)
//
#define build_file_complex_bzl  creat_file_complex_chks
#define build_chunk_complex_bzl creat_chunk_complex_chks
#define CHKS_CLOSE_BZL(fptr)    _CHKS_CLOSE_(fptr)
//
// #ifdef	__cplusplus
// }
// #endif

#endif	/* CHKS_BZL_H */

