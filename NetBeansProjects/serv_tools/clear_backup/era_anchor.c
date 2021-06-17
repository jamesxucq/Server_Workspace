#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "clear.h"
#include "utility.h"
#include "era_anchor.h"

#define ANCHOR_DATA_DEFAULT      "~/anchor_data.dat"
#define ANCHOR_INDEX_DEFAULT      "~/anchor_index.ndx"
#define DATA_BACKUP_DEFAULT      "~/anchor_data.bak"
#define INDEX_BACKUP_DEFAULT      "~/anchor_index.bak"

#define FAIL_EXIT(DATFP, RNO) { fclose(DATFP); return RNO; }
#define FAULT_EXIT(IDXFP, DATFP, RNO) { fclose(IDXFP); fclose(DATFP); return RNO; }

////////////////////////////////////////////////////////////////////////////////

struct fattent *read_fattent(struct fattent *read_att, FILE *anchor_datfp) {
    if (0 >= fread(read_att, sizeof (struct fattent), 1, anchor_datfp)) return NULL;
    return read_att;
}

struct idxent *read_idxent(struct idxent *read_idx, FILE *anchor_idxfp, off_t offset) {
    if (fseek(anchor_idxfp, offset, SEEK_SET)) return NULL;
    if (0 >= fread(read_idx, sizeof (struct idxent), 1, anchor_idxfp))
        return NULL;
    return read_idx;
}

int write_idxent(struct idxent *write_idx, FILE *anchor_idxfp) {
    if (0 >= fwrite(write_idx, sizeof (struct idxent), 1, anchor_idxfp))
        return -1;
    return 0;
}

off_t search_idxent(struct idxent *ndxent, char *location, int anchor_day, int epoch_day) {
    off_t offset, ndxset = 0;
    time_t time_stamp = (epoch_day - anchor_day) * SECONDS_ONEDAY;
LOG_DEBUG("time_stamp: %d epoch_day: %d anchor_day: %d", time_stamp, epoch_day, anchor_day);
    FILE *anchor_idxfp = fopen(append_path(location, ANCHOR_INDEX_DEFAULT), "rb");
    if (!anchor_idxfp) return INVALID_OFFSET_VALUE;
    offset = 0;
    while (read_idxent(ndxent, anchor_idxfp, offset)) {
LOG_DEBUG("time_stamp: %d ndxent->time_stamp: %d offset: %d", time_stamp, ndxent->time_stamp, offset);
        if (time_stamp <= ndxent->time_stamp) {
            ndxset = offset;
            break;
        }
        offset += sizeof (struct idxent);
    }

    fclose(anchor_idxfp);
LOG_DEBUG("ndxset: %d", ndxset);
    return ndxset;
}

int handle_anchdat(char *location, off_t offset) {
    char anchor_data[MAX_PATH * 3], data_backup[MAX_PATH * 3];

    strcpy(anchor_data, append_path(location, ANCHOR_DATA_DEFAULT));
    FILE *anchor_datfp = fopen(anchor_data, "rb");
    if (!anchor_datfp) return -1;

    strcpy(data_backup, append_path(location, DATA_BACKUP_DEFAULT));
    FILE *data_bakfp = fopen(data_backup, "wb");
    if (!data_bakfp) FAIL_EXIT(anchor_datfp, -1);
// LOG_DEBUG("anchor_data: %s data_backup: %s offset: %d", anchor_data, data_backup, offset);
    if (clear_backup_file(data_bakfp, anchor_datfp, offset)) {
        LOG_INFO("backup data file error.");
        FAULT_EXIT(data_bakfp, anchor_datfp, -1)
    }

    fclose(data_bakfp);
    fclose(anchor_datfp);
    unlink(anchor_data);
    rename(data_backup, anchor_data);

    return 0;
}

int handle_anchidx(char *location, off_t idxset, off_t offset) {
    char anchor_index[MAX_PATH * 3], index_backup[MAX_PATH * 3];
    struct idxent ndxent;

    strcpy(anchor_index, append_path(location, ANCHOR_INDEX_DEFAULT));
    FILE *anchor_idxfp = fopen(anchor_index, "rb");
    if (!anchor_idxfp) return -1;

    strcpy(index_backup, append_path(location, INDEX_BACKUP_DEFAULT));
    FILE *index_bakfp = fopen(index_backup, "wb");
    if (!index_bakfp) return -1;
// LOG_DEBUG("anchor_index: %s index_backup: %s offset: %d", anchor_index, index_backup, offset);
    ////////////////////////////////////////////////////////////////////////////
    if (fseek(index_bakfp, 0, SEEK_SET)) return -1;
    off_t read_set = idxset;
    int index;
    for (index = 0; read_idxent(&ndxent, anchor_idxfp, read_set); index++) {
        ndxent.data_offset -= offset;
        if (write_idxent(&ndxent, index_bakfp)) return -1;
        read_set += sizeof (struct idxent);
    }
    ////////////////////////////////////////////////////////////////////////////
    fclose(index_bakfp);
    fclose(anchor_idxfp);
    unlink(anchor_index);
    rename(index_backup, anchor_index);
    ////////////////////////////////////////////////////////////////////////////
    return 0;
}

int anchor_exception(char *location) {
    LOG_INFO("handle clear anchor exception.");
    unlink(append_path(location, ANCHOR_INDEX_DEFAULT));
    unlink(append_path(location, ANCHOR_DATA_DEFAULT));
}

int erase_anchor(char *location, int anchor_day, int epoch_day) {
    struct idxent ndxent;
    ////////////////////////////////////////////////////////////////////////////
    off_t idxset = search_idxent(&ndxent, location, anchor_day, epoch_day);
    if (INVALID_OFFSET_VALUE == idxset) return -1;
    else if (!idxset) return 0;
    ////////////////////////////////////////////////////////////////////////////
    int hand_value = handle_anchidx(location, idxset, ndxent.data_offset);
    hand_value |= handle_anchdat(location, ndxent.data_offset);
    if (hand_value) {
        LOG_INFO("clear anchor index or data error.");
        anchor_exception(location);
    }
    ////////////////////////////////////////////////////////////////////////////
    return 0;
}
