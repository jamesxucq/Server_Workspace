#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "clear.h"
#include "utility.h"
#include "era_action.h"

#define CAPACITY_DEFAULT   "~/capacity.log"
#define ACTION_LOG_DEFAULT      "~/action.log"
#define ACTION_BACKUP_DEFAULT      "~/action.bak"

#define FAIL_EXIT(DATFP, RNO) { fclose(DATFP); return RNO; }
#define FAULT_EXIT(IDXFP, DATFP, RNO) { fclose(IDXFP); fclose(DATFP); return RNO; }

////////////////////////////////////////////////////////////////////////////////

struct daysec *read_daysec(struct daysec *read_sec, FILE *action_datfp, off_t offset) {
    if (fseek(action_datfp, offset, SEEK_SET)) return NULL;
    if (0 >= fread(read_sec, sizeof (struct daysec), 1, action_datfp)) return NULL;
    return read_sec;
}

struct actent *read_action(struct actent *read_act, FILE *action_datfp) {
    if (0 >= fread(read_act, sizeof (struct actent), 1, action_datfp))
        return NULL;
    return read_act;
}

off_t search_daysec(char *location, int backup_day, int epoch_day) {
    off_t dayset = 0;
    FILE *action_datfp = fopen(append_path(location, ACTION_LOG_DEFAULT), "rb");
    ////////////////////////////////////////////////////////////////////////////    
    struct daysec read_sec;
    int day_stamp = epoch_day - backup_day;
    LOG_DEBUG("day_stamp: %d epoch_day: %d backup_day: %d", day_stamp, epoch_day, backup_day);
    off_t offset = 0;
    while (read_daysec(&read_sec, action_datfp, offset)) {
        LOG_DEBUG("day_stamp: %d read_sec.epoch_day: %d offset: %d", day_stamp, read_sec.epoch_day, offset);
        if (day_stamp <= read_sec.epoch_day) {
            dayset = offset;
            break;
        }
        if (INVALID_OFFSET_VALUE == read_sec.today_length) {
            if (fseek(action_datfp, 0, SEEK_END)) FAIL_EXIT(action_datfp, INVALID_OFFSET_VALUE);
            dayset = ftell(action_datfp);
            break;
        }
        offset += read_sec.today_length + sizeof (struct daysec);
    }
    ////////////////////////////////////////////////////////////////////////////  
    fclose(action_datfp);
    LOG_DEBUG("dayset: %d", dayset);
    return dayset;
}

int handle_reduce(uint64 *real_size, char *location, off_t dayset) {
    FILE *action_datfp = fopen(append_path(location, ACTION_LOG_DEFAULT), "rb+");
    if (!action_datfp) return -1;
    ////////////////////////////////////////////////////////////////////////////
    struct daysec read_sec;
    struct actent read_act;
    int idxset;
    for (idxset = 0; idxset < dayset; idxset += read_sec.today_length + sizeof (struct daysec)) {
        if (!read_daysec(&read_sec, action_datfp, idxset))
            break;
        ////////////////////////////////////////////////////////////////////////
        if (INVALID_OFFSET_VALUE != read_sec.today_length) {
            int index;
            int count = read_sec.today_length / sizeof (struct actent);
            for (index = 0; index < count; index++) {
                if (read_action(&read_act, action_datfp)) {
                    unlink(last_backup(location, read_act.file_name, read_act.act_time));
                    LOG_DEBUG("unlink: %s", last_backup(location, read_act.file_name, read_act.act_time));
                    *real_size += read_act.file_size;
                }
            }
        } else {
            while (read_action(&read_act, action_datfp)) {
                unlink(last_backup(location, read_act.file_name, read_act.act_time));
                LOG_DEBUG("unlink: %s", last_backup(location, read_act.file_name, read_act.act_time));
                *real_size += read_act.file_size;
            }
            break;
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    fclose(action_datfp);
    return 0;
}

int handle_actdat(char *location, off_t dayset) {
    char action_data[MAX_PATH * 3], data_backup[MAX_PATH * 3];

    strcpy(action_data, append_path(location, ACTION_LOG_DEFAULT));
    FILE *action_datfp = fopen(action_data, "rb+");
    if (!action_datfp) return -1;
    strcpy(data_backup, append_path(location, ACTION_BACKUP_DEFAULT));
    FILE *data_bakfp = fopen(data_backup, "wb+");
    if (!data_bakfp) FAIL_EXIT(action_datfp, -1);
    // LOG_DEBUG("action_data: %s data_backup: %s dayset: %d", action_data, data_backup, dayset);
    ////////////////////////////////////////////////////////////////////////////
    if (clear_backup_file(data_bakfp, action_datfp, dayset)) {
        LOG_INFO("backup data file error.");
        FAULT_EXIT(data_bakfp, action_datfp, -1)
    }
    ////////////////////////////////////////////////////////////////////////////
    fclose(data_bakfp);
    fclose(action_datfp);
    unlink(action_data);
    rename(data_backup, action_data);

    return 0;
}

int erase_action(uint64 *reduce_size, char *location, int backup_day, int epoch_day) {
    ////////////////////////////////////////////////////////////////////////////
    off_t dayset = search_daysec(location, backup_day, epoch_day);
    if (INVALID_OFFSET_VALUE == dayset) return -1;
    else if (!dayset) return 0;
    ////////////////////////////////////////////////////////////////////////////
    int hand_value = handle_reduce(reduce_size, location, dayset);
    hand_value |= handle_actdat(location, dayset);
    ////////////////////////////////////////////////////////////////////////////
    return hand_value;
}

////////////////////////////////////////////////////////////////////////////////

int read_capa(struct capacity *capa, FILE *capa_datfp) {
    if (fseek(capa_datfp, 0, SEEK_SET)) return -1;
    if (0 > fread(capa, sizeof (struct capacity), 1, capa_datfp))
        return -1;
    return 0;
}

int write_capa(struct capacity *capa, FILE *capa_datfp) {
    if (fseek(capa_datfp, 0, SEEK_SET)) return -1;
    if (0 > fwrite(capa, sizeof (struct capacity), 1, capa_datfp))
        return -1;
    return 0;
}

off_t second_daysec(char *location) {
    off_t dayset = INVALID_OFFSET_VALUE;
    FILE *action_datfp = fopen(append_path(location, ACTION_LOG_DEFAULT), "rb");
    ////////////////////////////////////////////////////////////////////////////    
    struct daysec read_sec;
    if (read_daysec(&read_sec, action_datfp, 0)) {
        if (INVALID_OFFSET_VALUE == read_sec.today_length) {
            if (fseek(action_datfp, 0, SEEK_END)) FAIL_EXIT(action_datfp, INVALID_OFFSET_VALUE);
            dayset = ftell(action_datfp);
        } else dayset = read_sec.today_length + sizeof (struct daysec);
    }
    ////////////////////////////////////////////////////////////////////////////  
    fclose(action_datfp);
    return dayset;
}

int capacity_expire(uint64 *real_size, char *location, uint64 reduce_size) {
    uint64 decrea_size = 0;
    ////////////////////////////////////////////////////////////////////////////
    while (reduce_size < decrea_size) {
        off_t dayset = second_daysec(location);
        if (INVALID_OFFSET_VALUE == dayset) return -1;
        ////////////////////////////////////////////////////////////////////////
        int hand_value = handle_reduce(&decrea_size, location, dayset);
        hand_value |= handle_actdat(location, dayset);
        if (hand_value) break;
        LOG_DEBUG("dayset: %d reduce_size: %ul decrea_size: %ul", dayset, reduce_size >> 30, decrea_size >> 30);
    }
    ////////////////////////////////////////////////////////////////////////////
    *real_size += decrea_size;

    return 0;
}

uint64 valid_backup_size(char *location, uint64 max_length) {
    struct capacity capa;
    memset(&capa, '\0', sizeof (struct capacity));

    FILE *capa_datfp = fopen(append_path(location, CAPACITY_DEFAULT), "rb+");
    if (!capa_datfp) FAIL_EXIT(capa_datfp, -1)
        if (read_capa(&capa, capa_datfp)) FAIL_EXIT(capa_datfp, -1)
            fclose(capa_datfp);
    LOG_DEBUG("capa.backup_size: %ul max_length: %ul", capa.backup_size >> 30, max_length);
    return capa.backup_size - max_length;
}

int reduce_capa(char *location, uint64 reduce_size) {
    struct capacity capa;
    FILE *capa_datfp = fopen(append_path(location, CAPACITY_DEFAULT), "rb+");
    if (!capa_datfp) return -1;
    //////////////////////////////////////////////////
    if (fseek(capa_datfp, 0, SEEK_SET)) FAIL_EXIT(capa_datfp, -1)
        if (0 > fread(&capa, sizeof (struct capacity), 1, capa_datfp))
            FAIL_EXIT(capa_datfp, -1)
            LOG_DEBUG("reduce_size: %ul", reduce_size >> 30);
    capa.backup_size -= reduce_size;
    if (fseek(capa_datfp, 0, SEEK_SET)) FAIL_EXIT(capa_datfp, -1)
        if (0 > fwrite(&capa, sizeof (struct capacity), 1, capa_datfp))
            FAIL_EXIT(capa_datfp, -1)
            //////////////////////////////////////////////////
            fclose(capa_datfp);
    return 0;
}

int erase_capacity(char *location, uint64 max_length) {
    ////////////////////////////////////////////////////////////////////////////       
    uint64 reduce_size = valid_backup_size(location, max_length);
    if (0 < reduce_size) {
        uint64 real_size;
        if (!capacity_expire(&real_size, location, reduce_size))
            reduce_capa(location, real_size);
    }
    ////////////////////////////////////////////////////////////////////////////  
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int action_exception(char *location) {
    LOG_INFO("handle clear anchor exception.");
    char bakpath[MAX_PATH * 3];
    sprintf(bakpath, "%s/backup", location);
    if (list_dirs_remove(bakpath))
        LOG_INFO("list remove backup directory error.");
    unlink(append_path(location, ACTION_LOG_DEFAULT));
    ////////////////////////////////////////////////////////////////////////////
    FILE *capa_datfp = fopen(append_path(location, CAPACITY_DEFAULT), "wb+");
    if (!capa_datfp) return -1;
    //////////////////////////////////////////////////
    uint64 used_size;
    if (recu_dirs_size(&used_size, location, "/")) {
        LOG_INFO("recu count pool used size error.");
        return -1;
    }
    //////////////////////////////////////////////////
    struct capacity capa;
    memset(&capa, '\0', sizeof (struct capacity));
    capa.used_size = used_size;
    if (write_capa(&capa, capa_datfp)) LOG_INFO("write capacity error.");
    //////////////////////////////////////////////////
    fclose(capa_datfp);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

int erase_backup(char *location, struct erabak *backup, int epoch_day) {
    uint64 reduce_size = 0;
    int erase_value = 0;
    ////////////////////////////////////////////////////////////////////////////
    if (-1 != backup->backup_day) {
        erase_value = erase_action(&reduce_size, location, backup->backup_day, epoch_day);
        if (!erase_value) reduce_capa(location, reduce_size);
    }
    if (-1 != backup->max_length) {
        erase_value |= erase_capacity(location, backup->max_length << 30);
        if (erase_value) {
            LOG_INFO("clear action or capacity error.");
            action_exception(location);
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    return 0;
}
