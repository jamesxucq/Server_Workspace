/* 
 * File:   action.h
 * Author: Administrator
 *
 * Created on 2012年9月14日, 上午8:52
 */

#ifndef ACTION_H
#define	ACTION_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "macro.h"

    struct daysec {
        off_t today_length;
        unsigned int epoch_day;
        time_t time_stamp;
        unsigned int reserved; /* filehigh */
    };

    struct actent {
        unsigned int action_type; /* add mod del list recu */
        time_t act_time;
        size_t file_size;
        char file_name[MAX_PATH * 3];
    };

    struct capacity {
        uint64 used_size;
        uint64 backup_size;
    };

#ifdef	__cplusplus
}
#endif

#endif	/* ACTION_H */

