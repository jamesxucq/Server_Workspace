/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 2012年9月12日, 下午1:37
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 
 */
#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define SECONDS_ONEDAY           86400

#define ANCHOR_DATA_DEFAULT      "./AnchorData.dat"
#define ANCHOR_INDEX_DEFAULT      "./AnchorIndex.ndx"

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

#define FAIL_EXIT(DATFP, RNO) { fclose(DATFP); return RNO; }

#define PRINT_USAGE \
    printf("usage: anch_dump all/anchs/anch no.\n");

#define PRINT_FATTENT(ATTENT) \
    printf("anchor_type:%X file_size:%d last_write:%d reserved:%d file_name:%s\n", ATTENT.anchor_type, ATTENT.file_size, ATTENT.last_write, ATTENT.reserved, ATTENT.file_name);

#define PRINT_IDXENT(IDXENT) \
    printf("anchor_number:%d data_offset:%d time_stamp:%d, epoch_day:%d\n", IDXENT.anchor_number, IDXENT.data_offset, IDXENT.time_stamp, IDXENT.time_stamp/SECONDS_ONEDAY);

#define PRINT_IDXENT_EXT(IDXENT) \
    printf("anchor_number:%d data_offset:%d time_stamp:%d, epoch_day:%d\n", IDXENT->anchor_number, IDXENT->data_offset, IDXENT->time_stamp, IDXENT->time_stamp/SECONDS_ONEDAY);

struct fattent *read_fattent(struct fattent *read_att, FILE *anchor_datfp) {
    if (0 >= fread(read_att, sizeof (struct fattent), 1, anchor_datfp)) return NULL;
    return read_att;
}

struct idxent *read_idxent(struct idxent *read_idx, FILE *anchor_idxfp) {
    if (0 >= fread(read_idx, sizeof (struct idxent), 1, anchor_idxfp))
        return NULL;
    return read_idx;
}

#define SWAP_IDXENT(NDX1, NDX2) \
        __typeof(NDX1) _tmp = NDX2; NDX2 = NDX1; NDX1 = _tmp;

int dump_all(FILE *anchor_idxfp, FILE *anchor_datfp) {
    struct idxent read_inx1, read_inx2, *idxent1, *idxent2;
    struct fattent read_att;

    idxent1 = &read_inx1;
    idxent2 = &read_inx2;
    if (fseek(anchor_idxfp, 0, SEEK_SET)) return -1;
    if (!read_idxent(idxent2, anchor_idxfp))
        return -1;
    for (;;) {
        SWAP_IDXENT(idxent1, idxent2)
        PRINT_IDXENT_EXT(idxent1);
        if (read_idxent(idxent2, anchor_idxfp)) {
            int index;
            int count = (idxent2->data_offset - idxent1->data_offset) / sizeof (struct fattent);
            if (fseek(anchor_datfp, idxent1->data_offset, SEEK_SET)) return -1;
            for (index = 0; index < count; index++) {
                read_fattent(&read_att, anchor_datfp);
                PRINT_FATTENT(read_att);
            }
        } else {
            while (read_fattent(&read_att, anchor_datfp)) {
                PRINT_FATTENT(read_att);
            }
            break;
        }
    }

    return 0;
}

int dump_anchs(FILE *anchor_idxfp) {
    struct idxent read_inx;

    if (fseek(anchor_idxfp, 0, SEEK_SET)) return -1;
    for (;;) {
        if (!read_idxent(&read_inx, anchor_idxfp))
            break;
        PRINT_IDXENT(read_inx);
    }

    return 0;
}

int dump_anch(FILE *anchor_idxfp, FILE *anchor_datfp, int anchor) {
    struct idxent read_inx1, read_inx2;
    struct fattent read_att;

    if (fseek(anchor_idxfp, 0, SEEK_SET)) return -1;
    while (read_idxent(&read_inx1, anchor_idxfp)) {
        if (anchor == read_inx1.anchor_number) {
            PRINT_IDXENT(read_inx1);
            if (fseek(anchor_datfp, read_inx1.data_offset, SEEK_SET)) return -1;
            if (read_idxent(&read_inx2, anchor_idxfp)) {
                int index;
                int count = (read_inx2.data_offset - read_inx1.data_offset) / sizeof (struct fattent);
                for (index = 0; index < count; index++) {
                    read_fattent(&read_att, anchor_datfp);
                    PRINT_FATTENT(read_att);
                }
            } else {
                while (read_fattent(&read_att, anchor_datfp)) {
                    PRINT_FATTENT(read_att);
                }
            }
            break;
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    FILE *anchor_idxfp = fopen(ANCHOR_INDEX_DEFAULT, "rb");
    if (!anchor_idxfp) return -1;

    FILE *anchor_datfp = fopen(ANCHOR_DATA_DEFAULT, "rb");
    if (!anchor_datfp) FAIL_EXIT(anchor_idxfp, -1);

    if (2 > argc) PRINT_USAGE
    else {
        if (!strcmp(argv[1], "all")) dump_all(anchor_idxfp, anchor_datfp);
        else if (!strcmp(argv[1], "anchs"))dump_anchs(anchor_idxfp);
        else if (!strcmp(argv[1], "anch") && 2 < argc) dump_anch(anchor_idxfp, anchor_datfp, atoi(argv[2]));
        else PRINT_USAGE
        }

    fclose(anchor_datfp);
    fclose(anchor_idxfp);
    return (EXIT_SUCCESS);
}

