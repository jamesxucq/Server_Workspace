/*
 * Anch.c
 *
 *  Created on: 2010-3-10
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "CommonMacro.h"
#include "Logger.h"
#include "StringOpt.h"
#include "FileOpt.h"
#include "Anchor.h"


struct AnchorFiles _AnchorFiles_;

inline int SetAnchFilePath(char *sAnchorIndex, char *sAnchorData) {
    if (!sAnchorIndex || !sAnchorData) return -1;
    strcpy(_AnchorFiles_.sAnchorIndex, sAnchorIndex);
    strcpy(_AnchorFiles_.sAnchorData, sAnchorData);

    return 0;
}

int OpenAnchData(FILE **anch_datfp, char *sHomePath) {
    if (!anch_datfp || !sHomePath) return -1;

    *anch_datfp = fopen(appendpath(sHomePath, _AnchorFiles_.sAnchorData), "a+b");
    if (!*anch_datfp) return -1;

    return 0;
}

void CloseAnchData(FILE *anch_datfp) {
    if (!anch_datfp) return;

    fclose(anch_datfp);
    return;
}

uint32 AddNewAnchor(char *sHomePath, FILE *anch_datfp) {
    FILE *anch_ndxfp = NULL;
    struct IndexrowStruct stNewIndexrow, stLastIndexrow;

    memset(&stNewIndexrow, '\0', sizeof (struct IndexrowStruct));
    memset(&stLastIndexrow, '\0', sizeof (struct IndexrowStruct));
    anch_ndxfp = fopen(appendpath(sHomePath, _AnchorFiles_.sAnchorIndex), "a+b");
    if (!anch_ndxfp) return 0;
    if (0 < FileSizeP(anch_ndxfp)) {
        if (fseek(anch_ndxfp, -sizeof (struct IndexrowStruct), SEEK_END)) return 0;
        fread(&stLastIndexrow, sizeof (struct IndexrowStruct), 1, anch_ndxfp);
    }
    fflush(anch_datfp);
    stNewIndexrow.AnchNumber = stLastIndexrow.AnchNumber + 1;
    stNewIndexrow.FileOffset = FileSizeN(appendpath(sHomePath, _AnchorFiles_.sAnchorData));

    if (fseek(anch_ndxfp, 0, SEEK_END)) return 0;
    fwrite(&stNewIndexrow, sizeof (struct IndexrowStruct), 1, anch_ndxfp);
    fclose(anch_ndxfp);

    return stNewIndexrow.AnchNumber;
}

int AddNewChange(FILE *anch_datfp, char *sChangeName, char mod_status) {
    struct FileAttrib stFileAttrib = {0};

    if (!anch_datfp || !sChangeName) return -1;

    stFileAttrib.mod_type = mod_status;
    strcpy(stFileAttrib.filename, sChangeName);

    if (fseek(anch_datfp, 0, SEEK_END)) return -1;
    fwrite(&stFileAttrib, sizeof (struct FileAttrib), 1, anch_datfp);

    //fclose(anch_datfp);//modify by james 20101019
    return 0;
}

int AddNewRename(FILE *anch_datfp, char *sExistName, char *sNewName) {
    struct FileAttrib stFileAttrib = {0};

    if (!anch_datfp || !sExistName || !sNewName) return -1;

    if (fseek(anch_datfp, 0, SEEK_END)) return -1;
    long id = ftell(anch_datfp);
    stFileAttrib.mod_type = EXIST;
    strcpy(stFileAttrib.filename, sExistName);
    stFileAttrib.Reserved = id;
    fwrite(&stFileAttrib, sizeof (struct FileAttrib), 1, anch_datfp);

    stFileAttrib.mod_type = NEW;
    strcpy(stFileAttrib.filename, sNewName);
    stFileAttrib.Reserved = id;
    fwrite(&stFileAttrib, sizeof (struct FileAttrib), 1, anch_datfp);

    //fclose(anch_datfp);//modify by james 20101019
    return 0;
}

int SearchAnchor(struct IndexrowStruct *pAnchIndexrow, char *sAnchorIndex, uint32 uiAnchor) {
    struct IndexrowStruct *pIndexrow;
    struct IndexrowStruct *MidPosition, *MinPosition, *MaxPosition;
    int iAnchorFound = SEARCH_NOT_FOUND;

    if (!pAnchIndexrow || !sAnchorIndex) return SEARCH_ERROR;

    int iIndexrowSize = sizeof (struct IndexrowStruct);
    int anch_ndxfd = open(sAnchorIndex, O_RDWR | O_CREAT, 0777);
    off_t file_size = FileSizeD(anch_ndxfd);
    if (0x00 == file_size) {
        memset(pAnchIndexrow, '\0', iIndexrowSize);
        close(anch_ndxfd);
        if (0x00 == uiAnchor) return SEARCH_FOUND;
        else return SEARCH_NOT_FOUND;
    }

    pIndexrow = (struct IndexrowStruct *) mmap(0, file_size, PROT_READ, MAP_SHARED, anch_ndxfd, 0);
    if (pIndexrow == MAP_FAILED) {
        close(anch_ndxfd);
        return SEARCH_ERROR;
    }

    int iIndexNum = file_size / iIndexrowSize;
    MaxPosition = pIndexrow + iIndexNum - 1;
    MinPosition = pIndexrow;
    MidPosition = MinPosition + (iIndexNum >> 1);
    while (TRUE) {
        if (uiAnchor > MidPosition->AnchNumber) {
            MinPosition = MidPosition;
            iIndexNum = ((MaxPosition - MinPosition) / iIndexrowSize) >> 1;
            MidPosition = MinPosition + (iIndexNum?iIndexNum:1);
        } else if (uiAnchor < MidPosition->AnchNumber) {
            MaxPosition = MidPosition;
            iIndexNum = ((MaxPosition - MinPosition) / iIndexrowSize) >> 1;
            MidPosition = MinPosition + iIndexNum;
        } else if (uiAnchor == MidPosition->AnchNumber) {
            memcpy(pAnchIndexrow, MidPosition, iIndexrowSize);
            iAnchorFound = SEARCH_FOUND;
            break;
        }
        if (MinPosition == MaxPosition) {
            iAnchorFound = SEARCH_NOT_FOUND;
            break;
        }
    }
    //msync((void*)pIndexrowStart, file_siz, MS_ASYNC);
    munmap((void*) pIndexrow, file_size);
    close(anch_ndxfd);

    return iAnchorFound;
}

/*
int GetFlistByIndexrow(FilesAttListH IndexFattlistH, char *sAnchorData, struct IndexrowStruct *pIndexrow) {
    int fdandat;
    off_t file_siz;
    struct FileAttrib *pAmdFileAttrib = NULL;
    struct FileAttrib *pFileAttribPos = NULL;

    if (!sAnchorData || !pIndexrow) return -1;

    fdandat = open(sAnchorData, O_RDWR | O_CREAT, 0777);
    file_siz = FileSizeD(fdandat);
    int mlen = file_siz - pIndexrow->FileOffset;
    int AmdFileAttribNum = mlen / sizeof (struct FileAttrib);
    pAmdFileAttrib = (struct FileAttrib *) mmap(0, mlen, PROT_READ, MAP_SHARED, fdandat, pIndexrow->FileOffset);
    if (pAmdFileAttrib == MAP_FAILED) return -1;

    int iIndex;
    pFileAttribPos = pAmdFileAttrib;
    for (iIndex = 0; iIndex < AmdFileAttribNum; iIndex++) {
        if (!AppendNewFlistItem(IndexFattlistH, pFileAttribPos)) {
            DeleteFlist(*IndexFattlistH);
            return -1;
        }
        pFileAttribPos++;
    }
    munmap((void*) pAmdFileAttrib, mlen);
    close(fdandat);

    return 0;
}
 */
int GetFlistByIndexrow(FilesAttListH IndexFattlistH, FILE *anch_datfp, struct IndexrowStruct *pIndexrow) {
    struct FileAttrib stFileAttrib;
    size_t iFileAttribSize = sizeof (struct FileAttrib);

    if (!IndexFattlistH || !anch_datfp || !pIndexrow) return -1;

    memset(&stFileAttrib, '\0', sizeof (struct FileAttrib));
    if (fseek(anch_datfp, pIndexrow->FileOffset, SEEK_SET)) return -1;
    while (!feof(anch_datfp)) {
        if (0x01 == fread(&stFileAttrib, iFileAttribSize, 1, anch_datfp)) {
            //do something
            if (!AppendNewFlistItem(IndexFattlistH, &stFileAttrib)) {
                DeleteFlist(*IndexFattlistH);
                return -1;
            }
        }
    }

    return 0;
}

/*
int GetFlistByAnchor(FilesAttListH fattlisth, char *sHomePath, uint32 Anch) {
    char fullname[(MAX_PATH+1) * 2];
    struct IndexrowStruct *pIndexrow = NULL;
    FilesAttListH IndexFattlistH = NULL;

    if (!sHomePath)  return -1;
    strcpy(fullname, appendpath(sHomePath, _AnchorFiles_.sAnchorIndex));
    pIndexrow = SearchAnchor(fullname, Anch);
    if (!pIndexrow) return -1;

    strcpy(fullname, appendpath(sHomePath, _AnchorFiles_.sAnchorData));
    if (GetFlistByIndexrow(IndexFattlistH, fullname, pIndexrow)) return -1;
    if (CreatModifyFlist(fattlisth, *IndexFattlistH)) return -1;

    DeleteFlist(*IndexFattlistH);
    free(pIndexrow);

    return 0;
}
 */


