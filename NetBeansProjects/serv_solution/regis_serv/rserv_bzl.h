/*
 * ServBzL.h
 *
 *  Created on: 2010-3-10
 *      Author: Divad
 */

#ifndef RSERVBZL_H_
#define RSERVBZL_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "rserv_object.h"

int servbzl_create(struct regis_config **config, char *xml_config);
int servbzl_destroy();

/*Anch operator section*/

/*Type operator section*/
struct regis_config *load_regis_config(const char *config_path);
inline unsigned int get_port_tatol();

inline void set_regis_config_file(const char *config_path);
inline char *get_regis_config_file();
inline void set_regis_config(struct regis_config *config);
inline struct regis_config *get_regis_config();

/*sync server aope*/

/* file list aope */

#ifdef	__cplusplus
}
#endif

#endif /* RSERVBZL_H_ */
