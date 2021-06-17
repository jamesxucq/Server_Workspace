/*
 * File:   gseion.h
 * Author: Administrator
 *
 * Created on 7:40
 */

#ifndef GSEION_H
#define GSEION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_header.h"
#include "server_macro.h"
#include "session.h"    

//
int open_gnull_null(struct seion_ctrl *, struct list_data *, struct inte_value *);
extern int (*open_get_null[])(struct seion_ctrl *, struct list_data *, struct inte_value *);
//
int open_gchks_null(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_gchks_simple(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_gchks_complex(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_gchks_whole(struct seion_ctrl *, struct list_data *, struct inte_value *);
extern int (*open_get_chks[])(struct seion_ctrl *, struct list_data *, struct inte_value *);

//
int open_gfile_file(struct seion_ctrl *, struct list_data *, struct inte_value *);
extern int (*open_get_file[])(struct seion_ctrl *, struct list_data *, struct inte_value *);

//
int open_glist_null(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_glist_recu_files(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_glist_list_directory(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_glist_recu_dires(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_glist_anch_files(struct seion_ctrl *, struct list_data *, struct inte_value *);
extern int (*open_get_list[])(struct seion_ctrl *, struct list_data *, struct inte_value *);

//
int open_ganchor_null(struct seion_ctrl *, struct list_data *, struct inte_value *);
int open_ganchor_last(struct seion_ctrl *, struct list_data *, struct inte_value *);
extern int (*open_get_anchor[])(struct seion_ctrl *, struct list_data *, struct inte_value *);
extern int (**open_get_se[]) (struct seion_ctrl *, struct list_data *, struct inte_value *);

//
void close_gnull_null(struct seion_ctrl *);
extern void (*close_get_null[])(struct seion_ctrl *);

//
void close_gchks_null(struct seion_ctrl *);
void close_gchks_simple(struct seion_ctrl *);
void close_gchks_complex(struct seion_ctrl *);
void close_gchks_whole(struct seion_ctrl *);
extern void (*close_get_chks[])(struct seion_ctrl *);

void close_gfile_file(struct seion_ctrl *);
extern void (*close_get_file[])(struct seion_ctrl *);

void close_ganchor_null(struct seion_ctrl *);
void close_ganchor_last(struct seion_ctrl *);
extern void (*close_get_anchor[])(struct seion_ctrl *);

//
void close_glist_null(struct seion_ctrl *);
void close_glist_recu_files(struct seion_ctrl *);
void close_glist_list_directory(struct seion_ctrl *);
void close_glist_recu_dires(struct seion_ctrl *);
void close_glist_anch_files(struct seion_ctrl *);
extern void (*close_get_list[])(struct seion_ctrl *);
extern void (**close_get_se[]) (struct seion_ctrl *);

#endif /* GSEION_H */

