#ifndef ANCHOR_H_
#define ANCHOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FilesAttList.h"

#define ADD		'A'
#define MOD		'M'
#define DEL		'D'

#define EXIST           'E'
#define NEW		'N'

#define RECU            'R'
#define LIST            'L'

struct AnchorFiles {
    char sAnchorIndex[PATH_MAX];
    char sAnchorData[PATH_MAX];
};

struct IndexrowStruct {
    uint32 AnchNumber;
    off_t FileOffset;
};

extern struct AnchorFiles _AnchorFiles_;

inline int SetAnchFilePath(char *sAnchorIndex, char *sAnchorData);
int OpenAnchData(FILE **anch_datfp, char *sHomePath);
void CloseAnchData(FILE *anch_datfp);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
uint32 AddNewAnchor(char *sHomePath, FILE *anch_datfp);
int AddNewChange(FILE *anch_datfp, char *sChangeName, char mod_status);
int AddNewRename(FILE *anch_datfp, char *sExistName, char *sNewName);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SEARCH_ERROR        -1
#define SEARCH_FOUND        0x0000
#define SEARCH_NOT_FOUND    0x0001
int SearchAnchor(struct IndexrowStruct *pAnchIndexrow, char *sAnchorIndex, uint32 uiAnchor);
//int GetFlistByAnchor(FilesAttListH fattlisth, char *sHomePath, uint32 Anch);

#endif /* ANCHOR_H_ */
