#ifndef ANCHORBZL_H
#define	ANCHORBZL_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include "ServCom.h"
#include "Anchor.h"
#include "AnchorCache.h"
#include "Logger.h"
#include "StringOpt.h"

#define		ERR_OK						0
#define		ERROR						-1
/************************************************************************/
/*                    Mode Error Code Define                            */
/************************************************************************/
#define  MODE_BASE						0x5000
#define  ERR_CONF                                               MODE_BASE + 1
#define  ERR_LOAD_CONF						MODE_BASE + 3
#define  ERR_ANCHOR_NOT_FOUND					MODE_BASE + 5
#define  ERR_SEARCH_ANCHOR					MODE_BASE + 7

//#include "ParseConf.h"
//#include "Session.h"
struct AnchOpt {
    FILE *anch_datfp;
    off_t set_offset;
    off_t end_offset;
};


    int AnchorBzl_Create(char *sAnchorIndex, char *sAnchorData);
    int AnchorBzl_Destroy();

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
#define OpenAnchDataBzl     OpenAnchData
#define CloseAnchDataBzl    CloseAnchData

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
#define AddNewAnchorBzl             AddNewAnchor
#define AnchSyncingFailedBzl        FtruncateP

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
#define AddNewChangeBzl     AddNewChange
#define AddNewRenameBzl     AddNewRename


    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
int CreatModifyFlist(FilesAttListH ModifyFlistH, FilesAttListH IndexFattlistH);

#define GetFlistByAnchorBzl      InitAnchSync
    int InitAnchSync(OUT FilesAttListH fattlisth, OUT struct AnchOpt *pAnchOpt, char *sHomePath, uint32 uiAnchor);

#ifdef	__cplusplus
}
#endif

#endif	/* ANCHORBZL_H */

