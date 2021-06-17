#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

// #include "file_utility.h"
// #include "string_utility.h"
// #include "fatt_type.h"
// #include "logger.h"
#include "recu_directory.h"

#define MAX_RECURSIVE_NUM       4096

int recursive_file(char *location, char *subdir) {
    FILE *recu_path, *recu_acd;
    creat_recuf_enviro(&recu_path, &recu_acd, location, subdir);
    int reval = hand_recursive_file(recu_path, recu_acd, MAX_RECURSIVE_NUM);
    clear_recuf_enviro(recu_path, location, reval, recu_acd);
    return reval;
}

int list_directory(char *location, char *subdir) {
    FILE *recu_acd;
    creat_listd_enviro(&recu_acd, location);
    int reval = hand_list_directory(recu_acd, location, subdir);
    clear_listd_enviro(recu_acd);
    return reval;
}

int recu_directory(char *location, char *subdir) {
    FILE *recu_path, *recu_acd;
    creat_recud_enviro(&recu_path, &recu_acd, location, subdir);
    int reval = hand_recu_directory(recu_path, recu_acd, MAX_RECURSIVE_NUM);
    clear_recud_enviro(recu_path, location, reval, recu_acd);
    return reval;
}
