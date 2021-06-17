/* 
 * File:   action.h
 * Author: Administrator
 *
 * Created on 2012
 */
/* the other backup mode */

#ifndef DELETE_LOG_H
#define	DELETE_LOG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "common_header.h"
#include "common_macro.h"
#include "fasd_list.h"
#include"ope_list.h"

#define ACTION_LOG_DEFAULT      "~/action_log.acd"
#define SECONDS_ONEDAY           86400
#define INVA_OFFSET_VALUE    ((uint64)-1)

    //

    struct daysec {
        uint64 today_length;
        unsigned int epoch_day;
        time_t time_stamp;
        unsigned int reserved; /* filehigh */
    };

    struct actent {
        unsigned int action_type; /* add mod del list recu */
        time_t act_time;
        uint64 file_size;
        char file_name[MAX_PATH * 3];
    };

    struct actsdo {
        uint64 entidx;
        time_t act_time;
        char *file_name;
    };

    //
    int set_action_log(char *action_log);
    void del_action(void *act);

    //
    int flush_action_list(char *location, struct ope_list *action_list);

#ifdef	__cplusplus
}
#endif
    
#endif	/* DELETE_LOG_H */

