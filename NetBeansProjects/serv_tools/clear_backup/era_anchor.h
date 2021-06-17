/* 
 * File:   anchor.h
 * Author: Administrator
 *
 * Created on 2012年9月14日, 上午8:52
 */

#ifndef ANCHOR_H
#define	ANCHOR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "macro.h"

    struct fattent {
        unsigned int anchor_type; /* add mod del list recu */
        char file_name[MAX_PATH * 3];
        size_t file_size;
        time_t last_write; /* When the item was last modified */
        unsigned int reserved; /* filehigh */
    };

    // #pragma pack(1)

    struct idxent {
        uint32 anchor_number;
        off_t data_offset;
        time_t time_stamp;
    };
    // #pragma pack()

#ifdef	__cplusplus
}
#endif

#endif	/* ANCHOR_H */

