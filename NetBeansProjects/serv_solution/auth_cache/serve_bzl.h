/* 
 * File:   serve_bzl.h
 * Author: Administrator
 */

#ifndef SERVE_BZL_H
#define	SERVE_BZL_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include "auth_ctrl.h"
    
    //
int sserve_create(char *cache_address, int bind_port);


#ifdef	__cplusplus
}
#endif

#endif	/* SERVE_BZL_H */

