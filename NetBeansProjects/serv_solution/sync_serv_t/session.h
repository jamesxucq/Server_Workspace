#ifndef SESSION_H_
#define SESSION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_header.h"
#include "server_macro.h"
// #include "third_party.h"

struct inte_value {
    unsigned int uid;
    uint32 content_length;
    unsigned int content_type;
    time_t last_write;
    char seion_id[SESSION_LENGTH];
    // 
    char file_name[MAX_PATH * 3];
    uint64 file_size;
    int range_type;
    uint32 cache_verify;
    uint64 offset;
    unsigned char data_buffer[MAX_PATH * 3];
    //      
    unsigned int sub_entry;
    unsigned int svalue;
    //
    uint64 origin_length;
    time_t act_time;
    unsigned int is_dire;
};

struct seion_ctrl {
    BYTE *recv_buffer;
    FILE *req_data;
    //
    BYTE *send_buffer;
    FILE *res_data;
    //
    uint64 rwrite_tatol;
    struct {
        char sin_addr[SERVNAME_LENGTH];
        int sin_port;
    } saddr;
    // must end in struct, see BZERO_SCTRL
    enum STATUS_CODE status_code;
    char seion_id[SESSION_LENGTH];
};

struct session {
    struct list_data *ldata;
    struct inte_value ival;
    struct seion_ctrl sctrl;
    struct actno_ope *aope;
};

int initialize_seion(int seion_timeout);
int finalize_seion();

/* session ctrl */
//
#define RESET_PUT_CHKS_SE(SCTRL) { \
    FCLOSE_SAFETY(SCTRL->req_data); \
    SCTRL->rwrite_tatol = 0x0000; \
    FCLOSE_SAFETY(SCTRL->res_data) \
    SCTRL->res_data = tmpfile64(); \
}

#define BZERO_SCTRL(SCTRL) \
    memset(SCTRL , '\0', sizeof(struct seion_ctrl) - SESSION_LENGTH - sizeof(enum STATUS_CODE));

//
void open_initial_se(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival);
#define CLOSE_INITIAL_SE(SCTRL) { \
/*_LOG_TRACE("close initial session.");*/ \
    BZERO_SCTRL(SCTRL) \
}

//
void open_kalive_se(struct list_data *ldata);
#define CLOSE_KALIVE_SE(SCTRL) { \
/*_LOG_TRACE("close kalive session.");*/ \
    BZERO_SCTRL(SCTRL) \
}
//
int open_post_se(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
#define CLOSE_POST_SE(SCTRL) { \
/*_LOG_TRACE("close post session.");*/ \
    FCLOSE_SAFETY(SCTRL->req_data); \
    BZERO_SCTRL(SCTRL) \
}

//
int open_put_file(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
int open_put_chks(struct seion_ctrl *sctrl, struct list_data *ldata, struct inte_value *ival);
extern int (*open_put_se[])(struct seion_ctrl*, struct list_data*, struct inte_value*);

#define FILE_OFFSET_COPY(TO, FROM, OFFSET) { \
    unsigned int rlen; unsigned char buffer[0x2000] = ""; \
    rewind(FROM); \
    if (!fseeko64(TO, OFFSET, SEEK_SET)) { \
        while ((rlen = fread(buffer, 1, 0x2000, FROM))) { \
/* log_binary_code((BYTE *)buffer, rlen); */ \
            if (fwrite(buffer, 1, rlen, TO) != rlen) { \
                _LOG_WARN("copy write error! errno:%d", errno); \
                break; \
            } \
        } \
    } \
}
#define MAKE_FILE_EXIST(PATH_NAME) { \
    if(FILE_EXIST(PATH_NAME)) { \
        FILE *exisp = fopen64(PATH_NAME, "wb"); \
        if(exisp) fclose(exisp); \
    } \
}
#define TRUNCATE_MODTIME_FILENAME(FILE_NAME, IVALU) \
    if(IVALU->file_size == (IVALU->offset+IVALU->content_length)) \
        truncate64(FILE_NAME, IVALU->file_size); \
    SET_FILE_MODTIME(FILE_NAME, IVALU->last_write); \

#define CLOSE_PUT_FILE_SE(AOPE, SCTRL, LDATA, IVALU) { \
    char file_name[MAX_PATH * 3]; \
    POOL_PATH(file_name, LDATA->location, IVALU->file_name); \
    if(IVALU->is_dire) { \
        SET_FILE_MODTIME(file_name, IVALU->last_write); \
    } else { \
        MAKE_FILE_EXIST(file_name) \
        FILE *file_data; \
        if ((file_data = fopen64(file_name, "rb+"))) { \
            FILE_OFFSET_COPY(file_data, SCTRL->req_data, IVALU->offset) \
            FCLOSE_SAFETY(file_data); \
            TRUNCATE_MODTIME_FILENAME(file_name, IVALU) \
        } \
        FCLOSE_SAFETY(SCTRL->req_data); \
    } \
    ADD_ADDI_BZL(AOPE, IVALU->is_dire, IVALU->file_name, IVALU->file_size, IVALU->act_time); \
/*_LOG_TRACE("close put session.");*/ \
    BZERO_SCTRL(SCTRL) \
}

#define CLOSE_PUT_CHKS_SE(SCTRL) { \
/*_LOG_TRACE("close put sums session.");*/ \
    FCLOSE_SAFETY(SCTRL->res_data); \
    BZERO_SCTRL(SCTRL) \
}

//
int open_copy_se(struct list_data *ldata, struct inte_value *ival);
#define CLOSE_COPY_SE(SCTRL) { \
/*_LOG_TRACE("close copy session.");*/ \
    BZERO_SCTRL(SCTRL) \
}

//
int open_move_se(struct list_data *ldata, struct inte_value *ival);
#define CLOSE_MOVE_SE(SCTRL) { \
/*_LOG_TRACE("close move session.");*/ \
    BZERO_SCTRL(SCTRL) \
}

//
int open_delete_se(struct list_data *ldata, struct inte_value *ival);
#define CLOSE_DELETE_SE(SCTRL) { \
/*_LOG_TRACE("close delete session.");*/ \
    BZERO_SCTRL(SCTRL) \
}

//
void open_final_null(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival);
void open_final_end(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival);
void open_final_success(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival);
void open_final_finish(struct seion_ctrl *sctrl, struct session *seion, struct inte_value *ival);

extern void (*open_final_se[])(struct seion_ctrl *, struct session *, struct inte_value *);
#define CLOSE_FINAL_SE(SCTRL) { \
/*_LOG_TRACE("close final session.");*/ \
    BZERO_SCTRL(SCTRL) \
}

//
#define SESSION_ACTIVE 16
#define DELAY_SESSION_TIMEOUT(LDATA, LOOPSE, TIME_OUT) { \
    if (!POW2N_MOD(LOOPSE++, SESSION_ACTIVE)) { \
        long now = time(NULL);  /* session timeout check here */ \
        if (now - LDATA->last_active >= TIME_OUT) { \
            LDATA->last_active = now; \
            keep_alive_cache_bzl(LDATA->uid, LDATA->req_valid); \
        } \
    } \
}

#define DIRECT_SESSION_TIMEOUT(LDATA, TIME_OUT) { \
    long now = time(NULL);  /* session timeout check here */ \
    if (now - LDATA->last_active >= TIME_OUT) { \
        LDATA->last_active = now; \
        keep_alive_cache_bzl(LDATA->uid, LDATA->req_valid); \
    } \
}

//
extern int _seion_timeout_;
extern int _seion_loop_;

#endif /* SESSION_H_ */
