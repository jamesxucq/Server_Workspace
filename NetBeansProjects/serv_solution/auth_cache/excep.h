/* 
 * File:   excep.h
 * Author: Administrator
 *
 * Created on 
 */

#ifndef EXCEP_H
#define	EXCEP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "event.h"
#include "session.h"

    //
    void unused_excep(struct session *seion);
    void query_excep(struct session *seion);
    void add_excep(struct session *seion);
    void clear_excep(struct session *seion);
    void list_excep(struct session *seion);
    void status_excep(struct session *seion);
    void sanchor_excep(struct session *seion);
    void slocked_excep(struct session *seion);
    void kalive_excep(struct session *seion);

    //
    extern void(*excep_null_code[])(struct session*);
    extern void(*excep_options_code[])(struct session*);
    extern void(*excep_control_code[])(struct session*);
    extern void(*excep_kalive_code[])(struct session*);
    extern void(**exception_tab[])(struct session*);

#ifdef	__cplusplus
}
#endif

#endif	/* EXCEP_H */

