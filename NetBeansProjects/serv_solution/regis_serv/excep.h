/* 
 * File:   excep.h
 * Author: Administrator
 *
 * Created on
 */

#ifndef EXCEP_H
#define EXCEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "event.h"
#include "session.h"

    //
    void unused_excep(struct session *seion);
    void regis_excep(struct session *seion);
    void settings_excep(struct session *seion);
    void link_excep(struct session *seion);
    void unlink_excep(struct session *seion);
    //
    extern void (*excep_null_code[])(struct session*);
    extern void (*excep_options_code[])(struct session*);
    extern void (**exception_tab[])(struct session*);

#ifdef __cplusplus
}
#endif

#endif /* EXCEP_H */

