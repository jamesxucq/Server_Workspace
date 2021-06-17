#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "utility.h"
#include "clear.h"
#include "era_action.h"
#include "era_anchor.h"
#include "auth_ctrl.h"

#define FAIL_EXIT(DATFP, RNO) { fclose(DATFP); return RNO; }
#define FAULT_EXIT(IDXFP, DATFP, RNO) { fclose(IDXFP); fclose(DATFP); return RNO; }


////////////////////////////////////////////////////////////////////////////////

int clear_handle(struct auth_value *avalue, struct erase_space *eraspa) {
    char location[MAX_PATH * 3];
    int epoch_day = time(NULL) / SECONDS_ONEDAY;

    strcpy(location, eraspa->local.path);
    strcat(location, strnrchr(avalue->location, '/', eraspa->local.delim_count));
    LOG_INFO("%s", location);

    if (-1 != eraspa->anchor_day) {
        if (erase_anchor(location, eraspa->anchor_day, epoch_day)) return -1;
    }
    if (erase_backup(location, &eraspa->backup, epoch_day)) return -1;

    return 0;
}

int clear_unlink(struct auth_value *avalue, struct erase_link *eralink, struct auth_config *config) {
    char linked_status[LARGE_TEXT_SIZE];

    if (get_linked_status(linked_status, config, avalue->uid)) {
        LOG_INFO("get linked status error, uid: %d", avalue->uid);
        return -1;
    }

    if (clear_linked_status(linked_status, eralink)) {
        CONVERT_SQLTEXT(linked_status)
        if (update_linked_status(config, avalue->uid, SQL_TEXT)) {
            LOG_INFO("get linked status error, uid: %d", avalue->uid);
            return -1;
        }
    }

    return 0;
}