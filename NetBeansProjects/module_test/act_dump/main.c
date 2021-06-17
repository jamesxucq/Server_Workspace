/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 2012年9月6日, 下午3:29
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stddef.h>
/*
 * 
 */
#define ACTION_LOG_DEFAULT      "./action.log"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#define INVALID_OFFSET_VALUE    ((off_t)-1)

#ifndef uint64
#define uint64 unsigned long long
#endif

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

#define PRINT_USAGE \
    printf("usage: act_dump all/capa/days/day no.\n");

#define PRINT_DAYSEC(DAYSEC) \
    printf("today_length:%d epoch_day:%d time_stamp:%d reserved:%d\n", DAYSEC.today_length, DAYSEC.epoch_day, DAYSEC.time_stamp, DAYSEC.reserved);

#define PRINT_ACTION(ACTION) \
    printf("action_type:%X act_time:%d file_size:%d file_name:%s\n", ACTION.action_type, ACTION.act_time, ACTION.file_size, ACTION.file_name);

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

int dump_all(FILE *action_datfp) {
    struct daysec read_sec;
    struct actent read_act;
    
    off_t offset = 0;
    for (;;) {
        if (!read_daysec(&read_sec, action_datfp, offset)) {
            printf("read day section error, exit.\n");
            return (EXIT_SUCCESS);
        }
        PRINT_DAYSEC(read_sec);

        if (INVALID_OFFSET_VALUE != read_sec.today_length) {
            int index;
            int count = read_sec.today_length / sizeof (struct actent);
            for (index = 0; index < count; index++) {
                read_action(&read_act, action_datfp);
                PRINT_ACTION(read_act);
            }
        } else break;
        offset += read_sec.today_length + sizeof (struct daysec);
    }
    while (read_action(&read_act, action_datfp)) {
        PRINT_ACTION(read_act);
    }

    return 0;
}

int dump_days(FILE *action_datfp) {
    struct daysec read_sec;
    off_t offset = 0;

    for (;;) {
        if (!read_daysec(&read_sec, action_datfp, offset)) {
            return (EXIT_SUCCESS);
        }
        PRINT_DAYSEC(read_sec);

        if (INVALID_OFFSET_VALUE == read_sec.today_length) break;
        offset += read_sec.today_length + sizeof (struct daysec);
    }

    return 0;
}

int dump_day(FILE *action_datfp, int epoch_day) {
    struct daysec read_sec;
    struct actent read_act;
    off_t offset = 0;

    while (read_daysec(&read_sec, action_datfp, offset)) {
        if (epoch_day == read_sec.epoch_day) {
            PRINT_DAYSEC(read_sec);
            if (INVALID_OFFSET_VALUE != read_sec.today_length) {
                int index;
                int count = read_sec.today_length / sizeof (struct actent);
                for (index = 0; index < count; index++) {
                    read_action(&read_act, action_datfp);
                    PRINT_ACTION(read_act);
                }
            } else {
                while (read_action(&read_act, action_datfp)) {
                    PRINT_ACTION(read_act);
                }
            }
            break;
        }
        offset += read_sec.today_length + sizeof (struct daysec);
        if (INVALID_OFFSET_VALUE != read_sec.today_length) printf("not have such day\n");
    }

    return 0;
}

#define FAIL_EXIT(DATFD, RNO) { close(DATFD); return RNO; }

struct capacity {
    uint64 used_size;
    uint64 backup_size;
};

#define CAPACITY_DEFAULT   "./capacity.log"    

int dump_capa() {
    struct capacity capa;
    int capa_fildes = open(CAPACITY_DEFAULT, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == capa_fildes) return -1;
    if (-1 == lseek(capa_fildes, 0, SEEK_SET)) FAIL_EXIT(capa_fildes, -1)
        if (-1 == read(capa_fildes, &capa, sizeof (struct capacity))) FAIL_EXIT(capa_fildes, -1)
            printf("capa.used_size:%ul capa.backup_size:%ul\n", capa.used_size>>30, capa.backup_size>>30);
    close(capa_fildes);
}

int main(int argc, char** argv) {
    FILE *action_datfp = fopen(ACTION_LOG_DEFAULT, "rb+");
    if (!action_datfp) return -1;

    if (2 > argc) PRINT_USAGE
    else {
        if (!strcmp(argv[1], "all")) {
            dump_capa();
            dump_all(action_datfp);
        } else if (!strcmp(argv[1], "capa"))dump_capa();
        else if (!strcmp(argv[1], "days"))dump_days(action_datfp);
        else if (!strcmp(argv[1], "day") && 2 < argc) dump_day(action_datfp, atoi(argv[2]));
        else PRINT_USAGE
        }

    fclose(action_datfp);
    return (EXIT_SUCCESS);
}

