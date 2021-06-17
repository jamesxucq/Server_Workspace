#include "common_header.h"

#include "versi_utility.h"

struct dele_bakup {
    time_t dele_time;
    char bakup_path[MAX_PATH * 3];
    int bakp_done; // 0:backup 0x01:finish
};

struct modi_bakup {
    time_t modi_time;
    char bakup_path[MAX_PATH * 3];
    int bakp_done; // 0:backup 0x01:finish
};

//
uint32 _modi_interv_;
uint32 _dele_interv_;

// debug
/*
void print_bakup(__off64_t offset, struct dele_bakup *dele_bakup) {
    _LOG_DEBUG("offset:%d", offset);
    _LOG_DEBUG("dele_bakup->modi_time:%d", dele_bakup->dele_time);
    _LOG_DEBUG("dele_bakup->bakup_path:%s", dele_bakup->bakup_path);
    _LOG_DEBUG("dele_bakup->bakp_done:%d", dele_bakup->bakp_done);
}
*/

int search_delete(struct dele_bakup *dele_bakup, __off64_t *offset, FILE *bakup_stre, char *file_name, time_t dele_time) { // 0:found 1:not found
    __off64_t dese;
    for (dese = -(__off64_t)sizeof (struct dele_bakup); ; dese -= sizeof (struct dele_bakup)) {
        fseeko64(bakup_stre, dese, SEEK_END);
        if (0 >= fread(dele_bakup, sizeof (struct dele_bakup), 1, bakup_stre))
            return 0x01;
        if (_dele_interv_ < (dele_time - dele_bakup->dele_time))
            return 0x01;
        if (!strcmp(file_name, dele_bakup->bakup_path)) {
// print_bakup(dese, dele_bakup);
            *offset = dese;
            break;
        }
    }
_LOG_DEBUG("--- search delete.");
    return 0x00;
}

int delete_logger(__off64_t *offset, FILE *bakup_stre, char *file_name, time_t dele_time) { // 0:backup 0x01:finish -1:error
    struct dele_bakup dele_bakup;
    int bakup_value = 0x01;
    //
    if (search_delete(&dele_bakup, offset, bakup_stre, file_name, dele_time)) {
        bakup_value = 0x00;
        *offset = -(__off64_t)sizeof (struct dele_bakup);
        fseeko64(bakup_stre, 0, SEEK_END);
        dele_bakup.dele_time = dele_time;
        strcpy(dele_bakup.bakup_path, file_name);
        dele_bakup.bakp_done = 0x00;
        if (0 >= fwrite(&dele_bakup, sizeof (struct dele_bakup), 1, bakup_stre))
            bakup_value = -1;
    } else bakup_value = dele_bakup.bakp_done;
    //
_LOG_DEBUG("delete logger bakup_value:%d", bakup_value);
    return bakup_value;
}

int bakup_done_dele(FILE *bakup_stre, __off64_t offset) {
    struct dele_bakup dele_bakup;
    fseeko64(bakup_stre, offset, SEEK_END);
    if (0 >= fread(&dele_bakup, sizeof (struct dele_bakup), 1, bakup_stre))
        return -1;
    dele_bakup.bakp_done = 0x01;
    fseeko64(bakup_stre, offset, SEEK_END);
    if (0 >= fwrite(&dele_bakup, sizeof (struct dele_bakup), 1, bakup_stre))
        return -1;
    //
_LOG_DEBUG("backup done.");
    return 0;
}

int delete_handler(char *location, char *file_name, time_t dele_time) { // 0:done 1:undone -1:error
    char bakup_path[MAX_PATH * 3], path_name[MAX_PATH * 3];
    POOL_PATH(path_name, location, file_name);
    LAST_BAKUP(bakup_path, location, file_name, dele_time);
    //
    int cpval = 0x00;
    if (rename(path_name, bakup_path) < 0) {
        if (EXDEV == errno) {
            if ((cpval = file_copy_ext(path_name, bakup_path)) < 0)
                _LOG_WARN("delete backup error! errno:%d path:%s", errno, path_name);
            if (!cpval) {
                if (unlink(path_name) < 0) {
                    cpval = -1;
                    _LOG_WARN("unlink file error! errno:%d path:%s", errno, path_name);
                }
            }
        }
    }
    // _LOG_DEBUG("delete handler cpval:%d", cpval);
    return cpval;
}

void delete_exceptio(char *location, char *file_name) {
    char path_name[MAX_PATH * 3];
    POOL_PATH(path_name, location, file_name);
    unlink(path_name);
_LOG_DEBUG("unlink file:%s", file_name);
}

FILE *creat_backup_file(char *location, time_t dele_time) {
    char bakup_path[MAX_PATH * 3];
    FILE *bakup_stre = NULL;
//
    BAKUP_PATH(bakup_path, location, dele_time, DELETE_BAKUP_DEFAULT);
    if (!(bakup_stre = fopen64(bakup_path, "rb+"))) {
        CREAT_BAKUP_DIRPATH(bakup_path, 0777);
        bakup_stre = fopen64(bakup_path, "wb+");
    }
    //
    return bakup_stre;
}

/*
int delete_file_exist(char *location, char *file_name) {
    static char file_path[MAX_PATH * 3];
    static int exist_value;
    char path_name[MAX_PATH * 3];
    //
    if(strcmp(file_path, file_name)) {
        strcpy(file_path, file_name);
        POOL_PATH(path_name, location, file_name);
        exist_value = FILE_EXIST(path_name);
    }
    //
    return exist_value;
}
*/

int delete_backup(char *location, char *file_name, time_t dele_time) { // 0:succ 1:exception -1:error
    // if(delete_file_exist(location, file_name)) return -1;
    FILE *bakup_stre = creat_backup_file(location, dele_time);
    if (!bakup_stre) return -1;
    //
    __off64_t offset;
    int deval = 0x00;
    if (!delete_logger(&offset, bakup_stre, file_name, dele_time)) {
        if (!(deval = delete_handler(location, file_name, dele_time)))
            bakup_done_dele(bakup_stre, offset);
    } else delete_exceptio(location, file_name);
    //
    fclose(bakup_stre);
_LOG_DEBUG("deval:%d", deval);
    return deval;
}

//
/* debug
void print_modbak(__off64_t offset, struct modi_bakup *mod_bakup) {
    _LOG_DEBUG("offset:%d", offset);
    _LOG_DEBUG("mod_bakup->modi_time:%d", mod_bakup->modi_time);
    _LOG_DEBUG("mod_bakup->bakup_path:%s", mod_bakup->bakup_path);
    _LOG_DEBUG("mod_bakup->bakp_done:%d", mod_bakup->bakp_done);
}
*/

int search_modified(struct modi_bakup *mod_bakup, __off64_t *offset, FILE *bakup_stre, char *file_name, time_t modi_time) { // 0:found 1:not found -1: error
    __off64_t mose;
    for (mose = -(__off64_t)sizeof (struct modi_bakup); ; mose -= sizeof (struct modi_bakup)) {
        fseeko64(bakup_stre, mose, SEEK_END);
        if (0 >= fread(mod_bakup, sizeof (struct modi_bakup), 1, bakup_stre))
            return 0x01;
// print_modbak(mose, mod_bakup);
        if (_modi_interv_ < (modi_time - mod_bakup->modi_time))
            return 0x01;
        else if (!strcmp(file_name, mod_bakup->bakup_path)) {
            *offset = mose;
            break;
        }
    }
    //
// _LOG_DEBUG("search delete.");
    return 0x00;
}

int modify_logger(__off64_t *offset, time_t *last_time, FILE *bakup_stre, char *file_name, time_t modi_time) { // 0:backup 0x01:finish -1:error
    struct modi_bakup mod_bakup;
    int bakup_value = 0x01;
    //
    if (search_modified(&mod_bakup, offset, bakup_stre, file_name, modi_time)) {
        bakup_value = 0x00;
        *offset = -(__off64_t)sizeof (struct modi_bakup);
        *last_time = modi_time;
        //
        fseeko64(bakup_stre, 0, SEEK_END);
        mod_bakup.modi_time = modi_time;
        strcpy(mod_bakup.bakup_path, file_name);
        mod_bakup.bakp_done = 0x00;
        if (0 >= fwrite(&mod_bakup, sizeof (struct modi_bakup), 1, bakup_stre))
            bakup_value = -1;
// _LOG_DEBUG("insert logger!");
    } else {
        *last_time = mod_bakup.modi_time;
        bakup_value = mod_bakup.bakp_done;
    }
    //
// _LOG_DEBUG("modify logger bakup_value:%d", bakup_value);
    return bakup_value;
}

int bakup_done_modi(FILE *bakup_stre, __off64_t offset) {
    struct modi_bakup mod_bakup;
    //
    fseeko64(bakup_stre, offset, SEEK_END);
    if (0 >= fread(&mod_bakup, sizeof (struct modi_bakup), 1, bakup_stre))
        return -1;
// print_modbak(offset, &mod_bakup);
    mod_bakup.bakp_done = 0x01;
    fseeko64(bakup_stre, offset, SEEK_END);
    if (0 >= fwrite(&mod_bakup, sizeof (struct modi_bakup), 1, bakup_stre))
        return -1;
    //
// _LOG_DEBUG("backup done.");
    return 0;
}

int modify_handler(char *location, char *file_name, time_t modi_time) { // 0:done 1:undone -1:error
    char bakup_path[MAX_PATH * 3], path_name[MAX_PATH * 3];
    int cpval = 0x00;
    //
    POOL_PATH(path_name, location, file_name);
    LAST_BAKUP(bakup_path, location, file_name, modi_time);
    if ((cpval = file_copy_ext(path_name, bakup_path)) < 0)
        _LOG_WARN("modify backup error! errno:%d path:%s", errno, path_name);
    //
// _LOG_DEBUG("modify handler cpval:%d", cpval);
    return cpval;
}

FILE *creat_modify_file(char *location, time_t modi_time) {
    char bakup_path[MAX_PATH * 3];
    FILE *bakup_stre = NULL;
//
    BAKUP_PATH(bakup_path, location, modi_time, MODIFY_VERSI_DEFAULT);
    if (!(bakup_stre = fopen64(bakup_path, "rb+"))) {
        CREAT_BAKUP_DIRPATH(bakup_path, 0777);
        bakup_stre = fopen64(bakup_path, "wb+");
    }
    //
    return bakup_stre;
}

int modify_verson(char *location, char *file_name,time_t modi_time) { // 0:succ 1:exception -1:error
    FILE *bakup_stre = creat_modify_file(location, modi_time);
    if (!bakup_stre) return -1;
    //
    __off64_t offset;
    time_t last_time;
    int moval = 0x00;
    if (!modify_logger(&offset, &last_time, bakup_stre, file_name, modi_time)) {
        if (!(moval = modify_handler(location, file_name, last_time))) {
            bakup_done_modi(bakup_stre, offset);
        }
    }
    //
    fclose(bakup_stre);
    return moval;
}