#ifndef ANCHOR_H_
#define ANCHOR_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ope_list.h"
#include "fasd_list.h"

typedef struct {
    char anch_idx[MAX_PATH * 3];
    char anch_acd[MAX_PATH * 3];
} anchor_files;

// #pragma pack(1)
struct idxent {
    uint32 anchor_number;
    uint64 data_oset;
    time_t time_stamp;
};
// #pragma pack()
extern anchor_files _anchor_files_;

//
inline int set_anchor_path(char *anch_idx, char *anch_acd);

//
uint32 load_last_anchor(char *location);
uint32 addi_anchor(char *location);
int flush_anch_list(char *location, struct ope_list *action_list);

//
#define SEARCH_FAILED       -1
#define SEARCH_FOUND        0x0000
#define SEARCH_NOFOND       0x0001
int search_anchor(struct idxent *anch_idx, char *idxname, uint32 anchor);
int get_alist_by_idxent(fsdo_lsth idx_lsth, char *anch_acd, struct idxent *ndxent);

#ifdef	__cplusplus
}
#endif

#endif /* ANCHOR_H_ */
