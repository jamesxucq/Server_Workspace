#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "logger.h"
#include "file_utility.h"
#include "string_utility.h"

#include "dires_list.h"
/*

#define DLIST_APPEND(head, add) { \
  (add)->next = head; \
  head = add; \
}

struct dires_list *add_new_dlist(dires_listh dlisth, char *dirname) {
    struct dires_list *dlisttmp;

    dlisttmp = (struct dires_list *) malloc(sizeof (struct dires_list));
    if (!dlisttmp) return NULL;
    memset(dlisttmp, '\0', sizeof (struct dires_list));
    strcpy(dlisttmp->dir_name, dirname);
    
    DLIST_APPEND(*dlisth, dlisttmp);
    return dlisttmp;
}

*/
