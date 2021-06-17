#ifndef FILESATTLIST_H_
#define FILESATTLIST_H_

//#include "utility/utlist.h"
#include "CommonMacro.h"

struct FileAttrib {
    char mod_type; /* add mod del list recu */
    char filename[MAX_PATH + 1];
    DWORD filesize;
    time_t modtime; /* When the item was last modified */
    DWORD Reserved; /* filehigh */
};

struct FilesAttList {
    struct FilesAttList *next;
    struct FileAttrib stFileAttrib;
};
//typedef struct FilesAttList* FilesAttList;
typedef struct FilesAttList** FilesAttListH;

inline void DeleteFlistItem(struct FilesAttList *fattlist);
void DeleteFlist(struct FilesAttList *fattlist);
#define DELETE_FATT_LIST(FILES_ATT_LIST)                                        \
    DeleteFlist((struct FilesAttList *)(FILES_ATT_LIST));                       \
    (FILES_ATT_LIST) = NULL;

inline void FlistPrepend(FilesAttListH head, struct FilesAttList *add);
inline void FlistAppend(FilesAttListH head, struct FilesAttList *add);
inline void FlistDelete(FilesAttListH head, struct FilesAttList *del);
inline void FlistInsert(FilesAttListH head, struct FilesAttList *fileattlist, struct FilesAttList *ins);
#define FLIST_FOREACH(head,el)                                                  \
    for(el=head;el;el=el->next)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int GetFileAttribCount(struct FilesAttList *fattlist);
inline struct FileAttrib *GetFileAttribByNum(struct FilesAttList *fattlist, int section, int index);
inline struct FileAttrib *ReadFileAttribNext(struct FilesAttList *fattlist, int section);

struct FilesAttList *NewFlistItem(struct FileAttrib *file_attrib);
struct FilesAttList *InsertNewFlistItem(FilesAttListH fattlisth, struct FileAttrib *file_attrib, struct FilesAttList *pos_item);
struct FilesAttList *AppendNewFlistItem(FilesAttListH fattlisth, struct FileAttrib *file_attrib);
struct FilesAttList *AddNewFlistNode(FilesAttListH fattlisth, char *homedir, char *filename, char type);
#define RecuAddNewFlist(fattlisth, homedir, filename) AddNewFlistNode(fattlisth, homedir, filename, 'R')
#define ListAddNewFlist(fattlisth, homedir, filename) AddNewFlistNode(fattlisth, homedir, filename, 'L')

#endif /* FILESATTLIST_H_ */

