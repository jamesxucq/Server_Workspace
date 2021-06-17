#ifndef SERVBZL_H_
#define SERVBZL_H_

// #ifdef	__cplusplus
// extern "C" {
// #endif

#include "sserv_object.h"

struct serv_config *servbzl_create(char *xml_config);
int servbzl_destroy();

/*Anch operator section*/

/*Type operator section*/
struct serv_config *load_serv_config(const char *config_path);

inline void set_serv_config_file(const char *config_path) {
    strcpy(_config_path_, (char *) config_path);
}

inline char *get_serv_config_file() {
    return _config_path_;
}

inline struct serv_config *get_serv_config() {
    return &_serv_config_;
}

//inline unsigned int get_port_tatol() {
//    unsigned int port_tatol;
//    for (port_tatol = 0; 0 != _serv_config_.bind_port[port_tatol]; port_tatol++);
//    return port_tatol;
//}

/*sync server aope*/
#define SET_FATTRIB_BZL(BASE_NAME, IVAL) { \
    char file_name[MAX_PATH * 3]; \
    POOL_PATH(file_name, BASE_NAME, IVAL->file_name); \
    truncate64(file_name, IVAL->file_size); \
    SET_FILE_MODTIME(file_name, IVAL->last_write); \
}

#define SET_MODTIME_BZL(BASE_NAME, IVAL) \
    SET_FILE_MODTIME_EXT(BASE_NAME, (char *)IVAL->data_buffer, IVAL->last_write)

/* file list aope */

// #ifdef	__cplusplus
// }
// #endif

#endif /* SERVBZL_H_ */
