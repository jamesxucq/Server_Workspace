/* 
 * File:   clear.h
 * Author: Administrator
 *
 * Created on 2012年9月12日, 上午10:31
 */

#ifndef CLEAR_H
#define	CLEAR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "macro.h"
#include "auth_ctrl.h"

    struct erabak {
        int backup_day;
        int max_length;
    };

    struct location {
        char path[MAX_PATH * 3];
        int delim_count;
    };

    struct erase_space {
        unsigned int start_uid;
        int number;
        int sleep;
        struct erabak backup;
        int anchor_day;
        struct location local;
    };

    struct erase_link {
        int unlink_day;
        int bomb_day;
    };

    int clear_handle(struct auth_value *avalue, struct erase_space *eraspa);
    int clear_unlink(struct auth_value *avalue, struct erase_link *eralink, struct auth_config *config);

#define SECONDS_ONEDAY           86400
#define INVALID_OFFSET_VALUE    ((off_t)-1)




#ifdef	__cplusplus
}
#endif

#endif	/* CLEAR_H */

