#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>

#include "kyotocabinet/kchashdb.h"
#include "riv_common.h"
#include "river_utility.h"
#include "creat_river.h"

using namespace std;
using namespace kyotocabinet;

int create_river_envirn(char *location) {
    char file_name[MAX_PATH * 3];
    int rval = 0;
    RIVER_PATH(file_name, location, RIV_INDE_DEFAULT);
    unlink(file_name);
    //
    // create the database object
    TreeDB rivdb;
    // open the database
    if (!rivdb.open(file_name, TreeDB::OWRITER | TreeDB::OCREATE)) {
        cerr << "open error: " << rivdb.error().name() << endl;
        rval = 0x01;
    }
    // close the database
    if (!rivdb.close()) {
        cerr << "close error: " << rivdb.error().name() << endl;
    }
    //
    RIVER_PATH(file_name, location, RIV_ATTB_DEFAULT);
    FILE *fpattb = fopen64(file_name, "wb+");
    if (!fpattb) rval = 0x01;
    //
    RIVER_PATH(file_name, location, RIV_CHKS_DEFAULT);
    FILE *fpchks = fopen64(file_name, "wb+");
    if (!fpchks) rval = 0x01;
    if (!rval) fill_idle_entity(fpchks, fpattb);
    //
    /*
        struct fattb attb;
        attrib_read(&attb, fpattb, 0);
        printf("attb.file_name:%s attb.sibling:%llu attb.time_stamp:%d\n", attb.file_name, attb.sibling, attb.time_stamp);

        struct ckchks cchks;
        cchks_inde_read(&cchks, fpchks, 0, 0);
        printf("cchks.sibling:%llu cchks.time_stamp:%d\n", cchks.sibling, cchks.time_stamp);
     */
    if (fpchks) fclose(fpchks);
    if (fpattb) fclose(fpattb);
    //
    return rval;
}

int build_river_envirn(char *location) {
    char file_name[MAX_PATH * 3];
    int rval = 0;
    //
    RIVER_PATH(file_name, location, RIV_INDE_DEFAULT);
    if (FILE_EXISTS(file_name)) rval = create_river_envirn(location);
    //
    return rval;
}