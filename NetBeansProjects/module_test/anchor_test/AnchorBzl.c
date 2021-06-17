/*
 * AuchBzl.c
 *
 *  Created on: 2010-3-10
 *      Author: Administrator
 */

#include "AnchorBzl.h"

int AnchorBzl_Create(char *sAnchorIndex, char *sAnchorData) {
    /* set anchor file name */
    if (!SetAnchFilePath(sAnchorIndex, sAnchorData))
        LOG_INFO("------- Set Anch Conf OK -------");
    else {
        LOG_INFO("------- Set Anch Conf failed! -------");
        return ERROR;
    }

    return ERR_OK; //succ
}

int AnchorBzl_Destroy() {

    return ERR_OK; //succ
}

struct FilesAttList *FindFromFlist(struct FilesAttList *fattlist, struct FileAttrib *FileAttrib) {
    static char filename[MAX_PATH + 1];
    static struct FilesAttList *pos_item;

    if (!fattlist && !FileAttrib)
        pos_item = pos_item->next;
    else {
        pos_item = fattlist;
        strcpy(filename, FileAttrib->filename);
    }
    FLIST_FOREACH(pos_item, pos_item) {
        if (!strcmp(pos_item->stFileAttrib.filename, filename))
            break;
    }

    return pos_item;
}

struct FilesAttList *RenExistItem(struct FilesAttList *fattlist, struct FilesAttList *new_item) {
    struct FilesAttList *item_tmp;

    if (!fattlist || !new_item) return NULL;
    FLIST_FOREACH(fattlist, item_tmp){
    if (item_tmp->stFileAttrib.Reserved == new_item->stFileAttrib.Reserved &&
            item_tmp->stFileAttrib.mod_type == EXIST)
        break;
    }
    return item_tmp;
}

#define FlistNextItem(exist_item, nexn_item)                                    \
{ exist_item = nexn_item; nexn_item = nexn_item->next; }

#define HandAmdItem(ModifyFlistH, IndexFattlistH, ndx_item)                     \
{                                                                               \
    FlistDelete(IndexFattlistH, ndx_item);                                      \
    FlistAppend(ModifyFlistH, ndx_item);                                        \
}

#define HandExsnItem(ModifyFlistH, IndexFattlistH, ndx_item, nexn_item)         \
{                                                                               \
    FlistDelete(IndexFattlistH, ndx_item);                                      \
    FlistAppend(ModifyFlistH, ndx_item);                                        \
    ndx_item = nexn_item; nexn_item = nexn_item->next;                          \
    FlistDelete(IndexFattlistH, ndx_item);                                      \
    FlistAppend(ModifyFlistH, ndx_item);                                        \
}

int CreatModifyFlist(FilesAttListH ModifyFlistH, FilesAttListH IndexFattlistH) {
    struct FilesAttList *ndx_item, *nexn_item;
    struct FilesAttList *exist_item, *mod_item;
    struct FileAttrib *IndexFileAttrib = NULL, *ModifyFileAttrib = NULL;

    if(!ModifyFlistH || !IndexFattlistH) return -1;
    *ModifyFlistH = NULL;
    if (!(*IndexFattlistH)) return 0;

    for (ndx_item = (*IndexFattlistH); ndx_item; ndx_item = nexn_item) {
        nexn_item = ndx_item->next;

        IndexFileAttrib = &ndx_item->stFileAttrib;
        mod_item = FindFromFlist(*ModifyFlistH, &ndx_item->stFileAttrib);
        if (!mod_item) {
            if (EXIST != IndexFileAttrib->mod_type)
                HandAmdItem(ModifyFlistH, IndexFattlistH, ndx_item)
            else HandExsnItem(ModifyFlistH, IndexFattlistH, ndx_item, nexn_item)
            continue;
        }
        while (mod_item) {
            ModifyFileAttrib = &mod_item->stFileAttrib;

            //if(ADD==ModifyFileAttrib->stat && ADD==IndexFileAttrib->stat){
            //} else
            //if (ADD == ModifyFileAttrib->mod_type && MOD == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            //} else
            if (ADD == ModifyFileAttrib->mod_type && DEL == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
                FlistDelete(ModifyFlistH, mod_item);
                DeleteFlistItem(mod_item);
            } else if (ADD == ModifyFileAttrib->mod_type && EXIST == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
                FlistNextItem(ndx_item, nexn_item);
                strcpy(ModifyFileAttrib->filename, ndx_item->stFileAttrib.filename);
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            }//else if(ADD==ModifyFileAttrib->stat && NEW==IndexFileAttrib->stat){
                //}
                //else if(MOD==ModifyFileAttrib->stat && ADD==IndexFileAttrib->stat){
                //}
            //else if (MOD == ModifyFileAttrib->mod_type && MOD == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            //}
            else if (MOD == ModifyFileAttrib->mod_type && DEL == IndexFileAttrib->mod_type) {
                ModifyFileAttrib->mod_type = DEL;
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            } else if (MOD == ModifyFileAttrib->mod_type && EXIST == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                InsertNewFlistItem(ModifyFlistH, &ndx_item->stFileAttrib, mod_item);
                FlistNextItem(ndx_item, nexn_item);
                //LL_DELETE(IndexFattlist, ndx_item);
                InsertNewFlistItem(ModifyFlistH, &ndx_item->stFileAttrib, mod_item);
                strcpy(ModifyFileAttrib->filename, ndx_item->stFileAttrib.filename);
            }//if(MOD==ModifyFileAttrib->mod_type && NEW==IndexFileAttrib->mod_type){
                //}
            else if (DEL == ModifyFileAttrib->mod_type && ADD == IndexFileAttrib->mod_type) {
                ModifyFileAttrib->mod_type = MOD;
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            }//else if(DEL==ModifyFileAttrib->mod_type && MOD==IndexFileAttrib->mod_type){
                //}
                //else if(DEL==ModifyFileAttrib->mod_type && DEL==IndexFileAttrib->mod_type){
                //}
                //else if(DEL==ModifyFileAttrib->mod_type && EXIST==IndexFileAttrib->mod_type){
                //}
            else if (DEL == ModifyFileAttrib->mod_type && NEW == IndexFileAttrib->mod_type) {
                ModifyFileAttrib->mod_type = MOD;
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            } else if (EXIST == ModifyFileAttrib->mod_type && ADD == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                AppendNewFlistItem(ModifyFlistH, &ndx_item->stFileAttrib);
            }//else if(EXIST==ModifyFileAttrib->mod_type && MOD==IndexFileAttrib->mod_type){
                //}
                //else if(EXIST==ModifyFileAttrib->mod_type && DEL==IndexFileAttrib->mod_type){
                //}
                //else if(EXIST==ModifyFileAttrib->mod_type && EXIST==IndexFileAttrib->mod_type){
                //}
            else if (EXIST == ModifyFileAttrib->mod_type && NEW == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                AppendNewFlistItem(ModifyFlistH, &ndx_item->stFileAttrib);
            }//else if(NEW==ModifyFileAttrib->mod_type && ADD==IndexFileAttrib->mod_type){
                //}
            else if (NEW == ModifyFileAttrib->mod_type && MOD == IndexFileAttrib->mod_type) {
                //LL_DELETE(IndexFattlist, ndx_item);
                AppendNewFlistItem(ModifyFlistH, &ndx_item->stFileAttrib);
            } else if (NEW == ModifyFileAttrib->mod_type && DEL == IndexFileAttrib->mod_type) {
                exist_item = RenExistItem(*ModifyFlistH, mod_item);
                ModifyFileAttrib->mod_type = DEL;
                ModifyFileAttrib->Reserved = 0;
                FlistDelete(ModifyFlistH, exist_item);
                DeleteFlistItem(exist_item);
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            } else if (NEW == ModifyFileAttrib->mod_type && EXIST == IndexFileAttrib->mod_type) {
                exist_item = RenExistItem((*IndexFattlistH), ndx_item);
                strcpy(ModifyFileAttrib->filename, exist_item->stFileAttrib.filename);
                FlistDelete(IndexFattlistH, exist_item);
                DeleteFlistItem(exist_item);
                //LL_DELETE(IndexFattlist, ndx_item);
                //DeleteFlistItem(ndx_item);
            }
            //else if(NEW==ModifyFileAttrib->mod_type && NEW==IndexFileAttrib->mod_type){
            //}
            //if(EXIST==ModifyFileAttrib->mod_type)

            mod_item = FindFromFlist(NULL, NULL);
        }
    }

    return 0;
}

void PrintFilesAttList(struct FilesAttList *fattlist) {
    struct FilesAttList *item;

    if (!fattlist) LOG_DEBUG("fattlist is null");
    else {

        FLIST_FOREACH(fattlist, item) {
            LOG_DEBUG("files->mod_type:%c", item->stFileAttrib.mod_type);
            LOG_DEBUG("files->filename:%s", item->stFileAttrib.filename);
            LOG_DEBUG("files->filesize:%d", item->stFileAttrib.filesize);
        }
    }
}

int InitAnchSync(OUT FilesAttListH fattlisth, OUT struct AnchOpt *pAnchOpt, char *sHomePath, uint32 uiAnchor) {
    struct IndexrowStruct stIndexrow;
    char fullname[(MAX_PATH + 1) * 2];
    struct FilesAttList *pIndexFattList = NULL;

    if (!fattlisth || !sHomePath || !pAnchOpt)
        return ERROR;
    *fattlisth = NULL;

    memset(&stIndexrow, '\0', sizeof (struct IndexrowStruct));
    strcpy(fullname, appendpath(sHomePath, _AnchorFiles_.sAnchorIndex));
    if (SearchAnchor(&stIndexrow, fullname, uiAnchor))
        return ERR_ANCHOR_NOT_FOUND;
    pAnchOpt->set_offset = stIndexrow.FileOffset;
    pAnchOpt->end_offset = FileSizeP(pAnchOpt->anch_datfp);

    //strcpy(fullname, appendpath(sHomePath, _AnchorFiles_.sAnchorData));
    if (GetFlistByIndexrow(&pIndexFattList, pAnchOpt->anch_datfp, &stIndexrow))
        return ERR_SEARCH_ANCHOR;
    if (pIndexFattList) {
        if (CreatModifyFlist(fattlisth, &pIndexFattList)) {
            DeleteFlist(pIndexFattList);
            return ERR_SEARCH_ANCHOR;
        }
        DeleteFlist(pIndexFattList);
    } else *fattlisth = NULL;

    return ERR_OK;
}

