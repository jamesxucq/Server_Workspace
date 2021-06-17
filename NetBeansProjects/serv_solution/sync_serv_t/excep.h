/* 
 * File:   excep.h
 * Author: Administrator
 *
 * Created on
 */

#ifndef EXCEP_H
#define	EXCEP_H

// #ifdef	__cplusplus
// extern "C" {
// #endif

#include "gseion.h"
#include "dseion.h"
#include "session.h"

    //
    void unused_excep(struct session *seion);
    void initial_excep(struct session *seion);
    void kalive_excep(struct session *seion);
    void head_excep(struct session *seion);
    void get_excep(struct session *seion);
    void post_excep(struct session *seion);
    void put_excep(struct session *seion);
    void copy_excep(struct session *seion);
    void move_excep(struct session *seion);
    void delete_excep(struct session *seion);
    void final_excep(struct session *seion);
    extern void(*exception_tab[])(struct session*);

// #ifdef	__cplusplus
// }
// #endif

#endif	/* EXCEP_H */

