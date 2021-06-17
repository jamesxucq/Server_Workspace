/* 
 * File:   main.c
 * Author: Administrator
 *
 * Created on 2011年3月2日, 下午3:47
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
#include "AnchorBzl.h"
struct AnchOpt stAnchOpt;

int InitApplication() {
    AnchorBzl_Create("~/AnchorIndex.ndx", "~/AnchorData.dat");

    OpenAnchDataBzl(&stAnchOpt.anch_datfp, "/home/james/workspace/tmp");
}

int DestoryApplication() {
    AnchorBzl_Destroy();

    CloseAnchDataBzl(stAnchOpt.anch_datfp);
}

int TestFuncation();

int main(int argc, char** argv) {
    InitApplication();

    int uiAnchor = atoi(argv[1]);

    //TestFuncation();
    TestFuncationI(uiAnchor);

    DestoryApplication();
    return (EXIT_SUCCESS);
}

int TestFuncation() {


    AddNewAnchorBzl("/home/james/workspace/tmp", stAnchOpt.anch_datfp);

    AddNewChangeBzl(stAnchOpt.anch_datfp, "1.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "2.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "3.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "4.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "5.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "6.txt", 'A');

    AddNewAnchorBzl("/home/james/workspace/tmp", stAnchOpt.anch_datfp);

    AddNewChangeBzl(stAnchOpt.anch_datfp, "6.txt", 'M');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "7.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "8.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "9.txt", 'A');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "2.txt", 'M');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "3.txt", 'M');

    AddNewAnchorBzl("/home/james/workspace/tmp", stAnchOpt.anch_datfp);

    AddNewChangeBzl(stAnchOpt.anch_datfp, "2.txt", 'M');
    AddNewChangeBzl(stAnchOpt.anch_datfp, "3.txt", 'M');

    AddNewAnchorBzl("/home/james/workspace/tmp", stAnchOpt.anch_datfp);

    struct FilesAttList *pFilesAttList = NULL;
    int retValue;
    retValue = GetFlistByAnchorBzl((FilesAttListH) & pFilesAttList, &stAnchOpt, "/home/james/workspace/tmp", 1);
    if (retValue) {
        if (ERROR == retValue || ERR_SEARCH_ANCHOR == retValue)
            printf("ERROR || ERR_SEARCH_ANCHOR\n");
        else if (ERR_ANCHOR_NOT_FOUND == retValue)
            printf("ERR_ANCHOR_NOT_FOUND\n");
    } else printf("OK\n");

//#define FLIST_FOREACH(head,el)                                                  \
//    for(el=head;el;el=el->next)
struct FilesAttList *FlistItem;
    FLIST_FOREACH(pFilesAttList, FlistItem) {
        printf("filename:%s mod_type:%c\n", FlistItem->stFileAttrib.filename, FlistItem->stFileAttrib.mod_type);
    }


    DELETE_FATT_LIST(pFilesAttList)
    return 0;
}

int TestFuncationI(int uiAnchor) {
    struct FilesAttList *pFilesAttList = NULL;
    int retValue;
    retValue = GetFlistByAnchorBzl((FilesAttListH) & pFilesAttList, &stAnchOpt, "/home/james/workspace/tmp", uiAnchor);
    if (retValue) {
        if (ERROR == retValue || ERR_SEARCH_ANCHOR == retValue)
            printf("ERROR || ERR_SEARCH_ANCHOR\n");
        else if (ERR_ANCHOR_NOT_FOUND == retValue)
            printf("ERR_ANCHOR_NOT_FOUND\n");
    } else printf("OK\n");

//#define FLIST_FOREACH(head,el)                                                  \
//    for(el=head;el;el=el->next)
struct FilesAttList *FlistItem;
    FLIST_FOREACH(pFilesAttList, FlistItem) {
        printf("filename:%s mod_type:%c\n", FlistItem->stFileAttrib.filename, FlistItem->stFileAttrib.mod_type);
    }


    DELETE_FATT_LIST(pFilesAttList)
    return 0;
}
