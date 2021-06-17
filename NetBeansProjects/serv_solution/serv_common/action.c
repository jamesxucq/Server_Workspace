#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "iattb_type.h"
#include "action.h"

#define CACHE_BACKET_SIZE      1024 
#define FAIL_EXIT(DATA, RENO) { close(DATA); return RENO; }
char _action_log_[MAX_PATH * 3];

int set_action_log(char *action_log) {
    if (!action_log) return -1;
    strcpy(_action_log_, action_log);
    return 0;
}

void del_action(void *act) {
    if (act) {
        free(((struct actsdo*) act)->file_name);
        free(act);
    }
}

struct daysec *read_daysec(struct daysec *read_sec, int action_fide, uint64 offset) {
    if (-1 == lseek64(action_fide, offset, SEEK_SET)) return NULL;
    if (0 >= read(action_fide, read_sec, sizeof (struct daysec))) return NULL;
    return read_sec;
}

int creat_daysec(uint64 *last_set, int action_fide) {
    struct daysec day_sec, last_sec;
    int epoch_day = time(NULL) / SECONDS_ONEDAY;
    //
    uint64 day_set = 0;
    memset(&last_sec, '\0', sizeof (struct daysec));
    while (read_daysec(&last_sec, action_fide, day_set)) {
        if (INVA_OFFSET_VALUE == last_sec.today_length || !last_sec.today_length)
            break;
        day_set += last_sec.today_length + sizeof (struct daysec);
    }
    if (last_sec.epoch_day == epoch_day) {
        *last_set = day_set;
        return 0;
    }
    //
    day_sec.today_length = INVA_OFFSET_VALUE;
    day_sec.epoch_day = time(NULL) / SECONDS_ONEDAY;
    day_sec.time_stamp = day_sec.epoch_day * SECONDS_ONEDAY;
    //
    if (-1 == lseek64(action_fide, 0, SEEK_END)) return -1;
    uint64 file_size;
    FILE_SIZE_DESCRIP(file_size, action_fide)
    if (-1 == write(action_fide, &day_sec, sizeof (struct daysec))) return -1;
    //
    if (file_size) {
        last_sec.today_length = file_size - (day_set + sizeof (struct daysec));
        if (-1 == lseek64(action_fide, day_set, SEEK_SET)) return -1;
        if (-1 == write(action_fide, &last_sec, sizeof (struct daysec))) return -1;
    }
    //
    return 0x01;
}

struct actent *read_action(struct actent *read_act, int action_fide) {
    if (0 >= read(action_fide, read_act, sizeof (struct actent)))
        return NULL;
    return read_act;
}

#define ACTION_ENTSDO(SDO, ENT, ENT_IDX) { \
    SDO->entidx = ENT_IDX; \
    SDO->act_time = ENT.act_time; \
    SDO->file_name = strdup(ENT.file_name); \
}

int build_today_actmap(actionmap *today_action, int action_fide, uint64 last_set) {
    struct actent read_act;
    struct actsdo *sdotmp;
    uint64 entidx;
    if (-1 == lseek64(action_fide, last_set + sizeof (struct daysec), SEEK_SET)) return -1;
    for (entidx = 0; read_action(&read_act, action_fide); entidx += sizeof (struct actent)) {
        if (MODIFY & read_act.action_type) {
            sdotmp = (struct actsdo *) malloc(sizeof (struct actsdo));
            if(!sdotmp) return -1;
            ACTION_ENTSDO(sdotmp, read_act, entidx)
            action_insert(sdotmp->file_name, sdotmp, today_action);
        }
    }
    // 
    return 0;
}

#define MODIFY_OPEACT(ACT, AOPE) { \
    ACT.action_type = AOPE->action_type; \
    ACT.act_time = AOPE->act_time; \
    ACT.file_size = AOPE->reserved; \
    strcpy(ACT.file_name, AOPE->file_name); \
}

#define DELETE_OPEACT(ACT, AOPE) { \
    ACT.action_type = AOPE->action_type; \
    ACT.act_time = AOPE->act_time; \
    ACT.file_size = AOPE->file_size; \
    strcpy(ACT.file_name, AOPE->file_name); \
}

int flush_action(int action_fide, actionmap *today_action, struct ope_list *action_list, char *location, uint64 last_set) {
    struct actent entmp;
    struct ope_list *olist;
    struct actsdo *sdotmp;
    struct openod *opesdo;
    uint64 offset = last_set + sizeof (struct daysec);
    // #define OLIST_FOREACH(head,el) for(el=head;el;el=el->next)

    OLIST_FOREACH(action_list, olist) {
        opesdo = &olist->opesdo;
        if (MODIFY & opesdo->action_type) {
            if ((sdotmp = action_value(opesdo->file_name, today_action))) {
                if (1200 < opesdo->act_time - sdotmp->act_time) {
                    char last_bakup[MAX_PATH * 3]; // disable by james 20140515
                    LAST_BAKUP(last_bakup, location, opesdo->file_name, sdotmp->act_time);
                    unlink(last_bakup);
                    creat_action_bakup(location, opesdo->file_name, opesdo->act_time);
                    MODIFY_OPEACT(entmp, opesdo)
                    if (-1 == lseek64(action_fide, offset + sdotmp->entidx, SEEK_SET)) FAIL_EXIT(action_fide, -1)
                        if (-1 == write(action_fide, &entmp, sizeof (struct actent)))
                            FAIL_EXIT(action_fide, -1)
                        }
            } else {
                creat_action_bakup(location, opesdo->file_name, opesdo->act_time);
                MODIFY_OPEACT(entmp, opesdo)
                if (-1 == lseek64(action_fide, 0, SEEK_END)) FAIL_EXIT(action_fide, -1)
                    if (-1 == write(action_fide, &entmp, sizeof (struct actent)))
                        FAIL_EXIT(action_fide, -1)
                    }
        } else if (DELE & opesdo->action_type) {
            DELETE_OPEACT(entmp, opesdo)
            if (-1 == lseek64(action_fide, 0, SEEK_END)) FAIL_EXIT(action_fide, -1)
                if (-1 == write(action_fide, &entmp, sizeof (struct actent)))
                    FAIL_EXIT(action_fide, -1)
                }
    }
    //
    return 0;
}

int flush_action_list(char *location, struct ope_list *action_list) {
    char act_path[MAX_PATH * 3];
    APPEND_PATH(act_path, location, _action_log_);
    int action_fide = open64(act_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == action_fide) {
        _LOG_ERROR("open file error! %s", act_path);
        return -1;
    }
    //
    actionmap * today_action = create_action(del_action, CACHE_BACKET_SIZE);
    uint64 last_set;
    int creat_value = creat_daysec(&last_set, action_fide);
    if (!creat_value) {
        if (build_today_actmap(today_action, action_fide, last_set)) {
            _LOG_WARN("build today actmap error!");
            action_del(today_action);
            FAIL_EXIT(action_fide, -1)
        }
    } else if (-1 == creat_value) {
        action_del(today_action);
        FAIL_EXIT(action_fide, -1)
    }

    if (flush_action(action_fide, today_action, action_list, location, last_set)) {
        _LOG_WARN("flush action error!");
        action_del(today_action);
        FAIL_EXIT(action_fide, -1)
    }
    //  
    if (today_action) {
        action_del(today_action);
        today_action = NULL;
    }
    close(action_fide);
    //
    return 0;
}
