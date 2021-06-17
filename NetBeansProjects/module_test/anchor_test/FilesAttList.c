#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "Logger.h"
#include "FileOpt.h"
#include "StringOpt.h"

#include "FilesAttList.h"

inline void DeleteFileAttrib(struct FileAttrib *file_attrib) {
    if (!file_attrib)return;
    free(file_attrib);
}

inline void DeleteFlistItem(struct FilesAttList *fattlist) {
    if (!fattlist) return;
    free(fattlist);
}

void DeleteFlist(struct FilesAttList *fattlist) {
    struct FilesAttList *pFattList = NULL, *pDeleteFlist = NULL;

    pFattList = fattlist;
    while (pFattList) {
        pDeleteFlist = pFattList;
        pFattList = pFattList->next;
        DeleteFlistItem(pDeleteFlist);
    }
}

inline void FlistPrepend(FilesAttListH head, struct FilesAttList *add) {
    add->next = *head;
    *head = add;
}

inline void FlistAppend(FilesAttListH head, struct FilesAttList *add) {
    struct FilesAttList *flist_tmp;

    add->next = NULL;
    if (*head) {
        flist_tmp = *head;
        while (flist_tmp->next)
            flist_tmp = flist_tmp->next;
        flist_tmp->next = add;
    } else *head = add;
}

inline void FlistDelete(FilesAttListH head, struct FilesAttList *del) {
    struct FilesAttList *flist_tmp;

    if (*head == del) *head = (*head)->next;
    else {
        flist_tmp = *head;
        while (flist_tmp->next && flist_tmp->next != del)
            flist_tmp = flist_tmp->next;
        if (flist_tmp->next) flist_tmp->next = del->next;
    }
}

inline void FlistInsert(FilesAttListH head, struct FilesAttList *ins, struct FilesAttList *pos) {
    struct FilesAttList *flist_tmp;

    if (*head == pos) {
        ins->next = *head;
        *head = ins;
    } else {
        flist_tmp = *head;
        while (flist_tmp->next && flist_tmp->next != pos)
            flist_tmp = flist_tmp->next;
        if (flist_tmp->next) {
            ins->next = flist_tmp->next;
            flist_tmp->next = ins;
        }
    }
}

struct FilesAttList *NewFlistItem(struct FileAttrib *file_attrib) {
    struct FilesAttList *new_item;

    if (!file_attrib) return NULL;
    new_item = (struct FilesAttList *) malloc(sizeof (struct FilesAttList));
    if (!new_item) return NULL;
    memset(new_item, '\0', sizeof (struct FilesAttList));
    memcpy(&new_item->stFileAttrib, file_attrib, sizeof (struct FileAttrib));

    return new_item;
}

struct FilesAttList *InsertNewFlistItem(FilesAttListH fattlisth, struct FileAttrib *file_attrib, struct FilesAttList *pos_item) {
    struct FilesAttList *new_item;

    if (!fattlisth || !file_attrib) return NULL;
    new_item = (struct FilesAttList *) malloc(sizeof (struct FilesAttList));
    if (!new_item) return NULL;
    memset(new_item, '\0', sizeof (struct FilesAttList));
    memcpy(&new_item->stFileAttrib, file_attrib, sizeof (struct FileAttrib));

    FlistInsert(fattlisth, new_item, pos_item);
    return new_item;
}

struct FilesAttList *AppendNewFlistItem(FilesAttListH fattlisth, struct FileAttrib *file_attrib) {
    struct FilesAttList *new_item;

    if (!fattlisth || !file_attrib) return NULL;
    new_item = (struct FilesAttList *) malloc(sizeof (struct FilesAttList));
    if (!new_item) return NULL;
    memset(new_item, '\0', sizeof (struct FilesAttList));
    memcpy(&new_item->stFileAttrib, file_attrib, sizeof (struct FileAttrib));

    FlistAppend(fattlisth, new_item);
    return new_item;
}

struct FilesAttList *AddNewFlistNode(FilesAttListH fattlisth,
        char *homedir,
        char *filename,
        char type) {
    struct FilesAttList *new_item;

    if (!fattlisth || !homedir || !filename) return NULL;
    new_item = (struct FilesAttList *) malloc(sizeof (struct FilesAttList));
    if (!new_item) return NULL;
    memset(new_item, '\0', sizeof (struct FilesAttList));

    LOG_INFO("filename:%s", filename);
    strcpy(new_item->stFileAttrib.filename, filename);
    new_item->stFileAttrib.mod_type = type;
    if (FileSizeTimeN(&new_item->stFileAttrib.filesize, &new_item->stFileAttrib.modtime, fullpath(homedir, filename))) {
        free(new_item);
        return NULL;
    }
    FlistPrepend(fattlisth, new_item);

    return new_item;
}

int GetFileAttribCount(struct FilesAttList *fattlist) {
    struct FilesAttList *item_tmp;
    int item_cnt = 0;

    if (!fattlist) return 0;
    //#define FLIST_FOREACH(head,el)
    //for(el=head;el;el=el->next)
    FLIST_FOREACH(fattlist, item_tmp)
    item_cnt++;

    return item_cnt;
}

inline struct FileAttrib *GetFileAttribByNum(struct FilesAttList *fattlist, int section, int index) {
    struct FilesAttList *item_tmp;
    int ndx = section + index;
    int item_cnt = -1;

    if (!fattlist) return 0;
    //#define FLIST_FOREACH(head,el)
    //for(el=head;el;el=el->next)

    FLIST_FOREACH(fattlist, item_tmp) {
        item_cnt++;
        if (ndx == item_cnt)
            break;
    }
    if (!item_tmp) return NULL;
    return &item_tmp->stFileAttrib;
}

inline struct FileAttrib *ReadFileAttribNext(struct FilesAttList *fattlist, int section) {
    struct FilesAttList *item_tmp;
    static struct FilesAttList *flpos;

    if (fattlist) {
        int item_cnt = -1;

        FLIST_FOREACH(fattlist, item_tmp) {
            item_cnt++;
            if (section == item_cnt) break;
        }
        flpos = item_tmp;
    } else flpos = flpos->next;
    if (!flpos) return NULL;

    return &flpos->stFileAttrib;
}

