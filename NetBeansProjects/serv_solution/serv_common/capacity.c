#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>

#include "logger.h"
#include "string_utility.h"
#include "iattb_type.h" 
#include "capacity.h"

#define FAIL_EXIT(DATFD, RENO) { \
    _LOG_ERROR("capacity file error!"); \
    close(DATFD); return RENO; }

int reset_capacity(char *location, struct ope_list *action_list) {
    struct ope_list *olist;
    struct openod *opesdo;
    uint64 inc_size = 0, bakup_size = 0;
    // #define ALIST_FOREACH(head,el) for(el=head;el;el=el->next)
    OLIST_FOREACH(action_list, olist) {
        opesdo = &olist->opesdo;
        switch (opesdo->action_type) {
            case ADDI:
                inc_size += opesdo->file_size;
                break;
            case MODIFY:
                inc_size += (opesdo->file_size - opesdo->reserved);
                break;
            case DELE:
                inc_size -= opesdo->file_size;
                bakup_size += opesdo->file_size;
                break;
            case COPY:
                inc_size += opesdo->file_size;
                break;
        }
    }
    // _LOG_DEBUG("inc_size:%ul bakup_size:%ul ", inc_size>>30, bakup_size>>30); // disable by james 20130409
    char capa_data[MAX_PATH * 3];
    struct capacity capa;
    memset(&capa, '\0', sizeof (struct capacity));
    APPEND_PATH(capa_data, location, CAPACITY_DEFAULT);
    int capa_fide = open64(capa_data, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == capa_fide) {
        _LOG_ERROR("open file error. %s\n", capa_data);
        return -1;
    }
    if (-1 == lseek64(capa_fide, 0, SEEK_SET)) FAIL_EXIT(capa_fide, -1)
    if (-1 == read(capa_fide, &capa, sizeof (struct capacity))) FAIL_EXIT(capa_fide, -1)
    // _LOG_DEBUG("read capa.used_size:%ul capa.bakup_size:%ul", capa.used_size>>30, capa.bakup_size>>30); // disable by james 20130409
    capa.used_size += inc_size;
    capa.bakup_size += bakup_size;
    if (-1 == lseek64(capa_fide, 0, SEEK_SET)) FAIL_EXIT(capa_fide, -1)
    if (-1 == write(capa_fide, &capa, sizeof (struct capacity))) FAIL_EXIT(capa_fide, -1)
    // _LOG_DEBUG("capa.used_size:%ul capa.bakup_size:%ul", capa.used_size>>30, capa.bakup_size>>30); // disable by james 20130409
    close(capa_fide);
    return 0;
}
